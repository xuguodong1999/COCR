#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2//imgproc.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

std::string obj365_dir = "/media/xgd/hjyy-ext4/obj365";
std::string raw_annotation_path = obj365_dir + "/zhiyuan_objv2_train.json";
std::string train_annotations_path = obj365_dir + "/train_annotations.bin";
std::string label_path = obj365_dir + "/obj365.labels";
std::string darknet_label_dir = obj365_dir + "/labels";
std::string train_patch_dir = obj365_dir + "/JPEGImages";
std::string indices_train_file = obj365_dir + "/train.list";
std::string indices_test_file = obj365_dir + "/test.list";

class ObjDataBlob {
    struct Image {
        /** images {
        "height": 512,    #图片高度
        "id": 420917,    #图片id
        "license": 5,
        "width": 769,    #图片宽度
        "file_name": "images/v1/patch8/objects365_v1_00420917.jpg",    #图片文件名称
        "url": ""
        } */
        std::string filepath;
        int w, h;
    } image;

    /** annotations {
      "id": 51,            #框体id
      "iscrowd": 0,
      "isfake": 0,
      "area": 1584.6324365356109,
      "isreflected": 0,
      "bbox": [            #框体位置信息：[ x, y, width, height ]
        491.3955078011,
        88.1856689664,
        35.65588379410002,
        44.442382796800004
      ],
      "image_id": 420917,    #框体所在图片的id
      "category_id": 84        #框体标注物体的类别
    } */
    struct Annotation {
        float x, y, w, h;
        int label;

        Annotation(const float &_x, const float &_y, const float &_w, const float &_h, const int &_label)
                : x(_x), y(_y), w(_w), h(_h), label(_label) {}
    };

    std::vector<Annotation> annotations;
    /** categories {
      "name": "Person",     #类别名称
      "id": 1    #类别id
    } */
    inline static std::unordered_map<int, std::string> categories;
    inline static std::unordered_map<int, std::shared_ptr<ObjDataBlob>> sDataBlob;
public:

    void dumpAsDarknet() {
        auto &fakePath = image.filepath;
        auto pos = fakePath.find_first_of("p");
        auto pos2 = fakePath.find_first_of(".");
        auto darknet_label_path = darknet_label_dir + "/" + fakePath.substr(pos, pos2 - pos) + ".txt";
        auto posdir = darknet_label_path.find_last_of("/");
        auto dir = darknet_label_path.substr(0, posdir);
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }
        std::ofstream ofsm(darknet_label_path, std::ios::out);
        if (!ofsm.is_open()) {
            std::cerr << "fail to open " << darknet_label_path << std::endl;
            exit(-1);
        }
        for (auto &annotation:annotations) {
            float x = (annotation.x + annotation.w / 2.0) / image.w;
            float y = (annotation.y + annotation.h / 2.0) / image.h;
            float w = annotation.w / image.w;
            float h = annotation.h / image.h;
            ofsm << annotation.label - 1 << " " << x << " " << y << " "
                 << w << " " << h << std::endl;
        }
        ofsm.close();
    }

    friend std::ostream &operator<<(std::ostream &_ost, const ObjDataBlob &_dataBlob) {
        int path_length = _dataBlob.image.filepath.length();
        _ost.write((char *) &(_dataBlob.image.w), sizeof(int));
        _ost.write((char *) &(_dataBlob.image.h), sizeof(int));
        _ost.write((char *) &path_length, sizeof(int));
        _ost.write(_dataBlob.image.filepath.data(), path_length * sizeof(char));
        int annotation_num = _dataBlob.annotations.size();
        _ost.write((char *) &annotation_num, sizeof(int));
        for (auto &annotation:_dataBlob.annotations) {
            _ost.write((char *) &(annotation.label), sizeof(int));
            _ost.write((char *) &(annotation.x), sizeof(float));
            _ost.write((char *) &(annotation.y), sizeof(float));
            _ost.write((char *) &(annotation.w), sizeof(float));
            _ost.write((char *) &(annotation.h), sizeof(float));
        }
        return _ost;
    }

    friend std::istream &operator>>(std::istream &_ist, ObjDataBlob &_dataBlob) {
        _dataBlob.image.filepath.clear();
        _dataBlob.annotations.clear();
        int path_length, annotation_num;
        _ist.read((char *) &_dataBlob.image.w, sizeof(int));
        _ist.read((char *) &_dataBlob.image.h, sizeof(int));
        _ist.read((char *) &path_length, sizeof(int));
        _dataBlob.image.filepath.resize(path_length);
        _ist.read(_dataBlob.image.filepath.data(), path_length * sizeof(char));
        _ist.read((char *) &annotation_num, sizeof(int));
        for (int i = 0; i < annotation_num; i++) {
            float x, y, w, h;
            int label;
            _ist.read((char *) &label, sizeof(int));
            _ist.read((char *) &x, sizeof(float));
            _ist.read((char *) &y, sizeof(float));
            _ist.read((char *) &w, sizeof(float));
            _ist.read((char *) &h, sizeof(float));
            _dataBlob.annotations.emplace_back(x, y, w, h, label);
        }
        return _ist;
    }

    static void addCategory(const int &_label, std::string _labelName) {
        categories[_label] = std::move(_labelName);
    }

    ObjDataBlob() {}

    ObjDataBlob(const int &_width, const int &_height, std::string _path) {
        image.w = _width;
        image.h = _height;
        image.filepath = std::move(_path);
    }

    bool isValid() const {
        auto &fakePath = image.filepath;
        if ("images/v2/patch21/objects365_v2_01118660.jpg" == fakePath ||
            "images/v2/patch44/objects365_v2_02024631.jpg" == fakePath)
            return false;
        auto truePath = getTruePath();
        if (!std::filesystem::exists(truePath)) {
            std::cerr << "unknown file " << fakePath << " @ " << truePath << std::endl;
            return false;
        }
        return true;
    }

    bool isValidSize() const {
        auto mat = cv::imread(getTruePath());
        return mat.cols == image.w && mat.rows == image.h && mat.channels() == 3;
    }

    void addBoundingBox(const float &_x, const float &_y, const float &_w, const float &_h, const int &_id) {
        annotations.emplace_back(_x, _y, _w, _h, _id);
    }

    static std::shared_ptr<ObjDataBlob> GetImgBlobById(const int &_id) {
        auto itr = sDataBlob.find(_id);
        if (sDataBlob.end() != itr) {
            return itr->second;
        } else {
            return nullptr;
        }
    }

    static void AddImageBlob(const int &_id, const int &_width, const int &_height, std::string _path) {
        sDataBlob[_id] = std::make_shared<ObjDataBlob>(_width, _height, std::move(_path));
    }

    static void DumpAsBin() {
        std::ofstream ofs(train_annotations_path, std::ios::out | std::ios::binary);
        if (!ofs.is_open()) {
            std::cerr << "fail to open " << train_annotations_path << std::endl;
            exit(-1);
        }
        for (auto &imgBlob:ObjDataBlob::sDataBlob) {
            if (imgBlob.second->isValid())
                ofs << *(imgBlob.second);
            else
                std::cerr << "invalid img @ " << imgBlob.second->image.filepath << std::endl;
        }
        ofs.close();
        ofs.open(label_path, std::ios::out);
        if (!ofs.is_open()) {
            std::cerr << "fail to open " << label_path << std::endl;
            exit(-1);
        }
        std::vector<std::pair<int, std::string>> labels;
        for (auto&[id, name]:categories) {
            labels.push_back({id, name});
        }
        std::sort(labels.begin(), labels.end(),
                  [](const std::pair<int, std::string> &a, const std::pair<int, std::string> &b) {
                      return a.first < b.first;
                  });
        for (auto&[id, name]:labels) {
            ofs << name << std::endl;
        }
        ofs.close();
    }

    static void ReadRawAnnotations(const std::string &_filepath) {
        using namespace rapidjson;
        std::ifstream ifs(_filepath);
        IStreamWrapper isw(ifs);
        Document d;
        d.ParseStream(isw);
        for (auto &obj:d.GetObject()) {
            std::string tag = obj.name.GetString();
            if ("images" == tag) {
                for (auto &image:obj.value.GetArray()) {
                    ObjDataBlob::AddImageBlob(image["id"].GetInt(), image["width"].GetInt(),
                                              image["height"].GetInt(), image["file_name"].GetString());
                }
            } else if ("annotations" == tag) {
                for (auto &annotation:obj.value.GetArray()) {
                    float pos[4];
                    size_t i = 0;
                    for (auto &num:annotation["bbox"].GetArray()) {
                        pos[i++] = num.GetFloat();
                    }
                    auto imgDataPtr = ObjDataBlob::GetImgBlobById(annotation["image_id"].GetInt());
                    if (!imgDataPtr) {
                        std::cerr << "unknown image_id=" << annotation["image_id"].GetInt() << std::endl;
                    } else {
                        imgDataPtr->addBoundingBox(pos[0], pos[1], pos[2], pos[3],
                                                   annotation["category_id"].GetInt());
                    }
                }
            } else if ("categories" == tag) {
                for (auto &category:obj.value.GetArray()) {
                    ObjDataBlob::addCategory(category["id"].GetInt(), category["name"].GetString());
                }
            }
            // ignore licenses
        }
        ifs.close();
        d.Clear();
    }

    const std::string &getPath() const {
        return image.filepath;
    }

    std::string getTruePath() const {
        auto &fakePath = image.filepath;
        auto pos = fakePath.find_first_of("p");
        if (pos != std::string::npos)
            return train_patch_dir + "/" + fakePath.substr(pos, fakePath.length());
        return getPath();
    }
};

std::vector<ObjDataBlob> LoadFromBin(const std::string &_filepath) {
    std::ifstream ifs(_filepath, std::ios::in | std::ios::binary);
    std::vector<ObjDataBlob> dataBlobs;
    ObjDataBlob blob;
    while (ifs >> blob) {
        dataBlobs.push_back(std::move(blob));
    }
    ifs.close();
    return std::move(dataBlobs);
}

int main() {
//    ObjDataBlob::ReadRawAnnotations(raw_annotation_path);
//    ObjDataBlob::DumpAsBin();
//    return 0;

    auto dataBlobs = LoadFromBin(train_annotations_path);
    std::cout << dataBlobs.size() << std::endl;
//#pragma omp parallel for num_threads(8)
    std::ofstream ofs(indices_test_file, std::ios::out);
    std::vector<std::string> all;
    for (size_t i = 0; i < dataBlobs.size(); i++) {
//        dataBlobs[i].dumpAsDarknet();
        all.push_back(dataBlobs[i].getTruePath());
//        ofs << dataBlobs[i].getTruePath() << std::endl;
//        if (!dataBlobs[i].isValidSize()) {
//            std::cout << dataBlobs[i].getTruePath();
//        }
    }
    std::random_shuffle(all.begin(), all.end());
    size_t j = 0;
    for (; j < 10000; j++)ofs << all[j] << std::endl;
    ofs.close();
    ofs.open(indices_train_file, std::ios::out);
    for (; j < all.size(); j++)ofs << all[j] << std::endl;
    ofs.close();
    return 0;
}