#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <iostream>

inline std::string helloRapidJson() {
    using namespace rapidjson;
    using namespace std;
    // README 里面的样例
    // 1. 把 JSON 解析至 DOM。
    const char *json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);
    // 2. 利用 DOM 作出修改。
    Value &s = d["stars"];
    s.SetInt(s.GetInt() + 1);
    // 3. 把 DOM 转换（stringify）成 JSON。
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    // Output {"project":"rapidjson","stars":11}
    return buffer.GetString();
}

#include <QStringList>

inline QString helloQt() {
    return QString::number(100.1).split(".").at(0);
}

#include <openbabel/mol.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>
#include <iostream>

inline std::string helloOpenBabel() {
    using namespace OpenBabel;
    using namespace std;
    OBConversion conv;
    auto format_in = conv.FindFormat("smiles");
    auto format_out = conv.FindFormat("pdb");
    stringstream ism("c1ccccc1");
    OBMol mol;
    conv.SetInFormat(format_in);
    conv.SetOutFormat(format_out);
    conv.Read(&mol, &ism);
    OBForceField *pFF = OBForceField::FindForceField("uff");
    OBBuilder builder;
    builder.Build(mol);
    mol.AddHydrogens(false, true);
    pFF->Setup(mol);
    pFF->UpdateCoordinates(mol);
    stringstream osm;
    conv.Write(&mol, &osm);
    return osm.str();
}

#include <Eigen/Eigen>
#include <iostream>

inline Eigen::Vector2d helloEigen() {
    using namespace Eigen;
    using namespace std;
    Vector2d p1(1, 2);
    return p1 + p1 * 1.2;
}

#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <vector>

inline float helloOpenCV() {
    using namespace cv;
    using namespace dnn;
    using namespace std;
    using namespace chrono;
    const char *cfg = "../../weights/enetb0.cfg";
    const char *weights = "../../weights/enetb0.weights";
    auto net = dnn::readNetFromDarknet(cfg, weights);
    net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
    net.setPreferableTarget(dnn::DNN_TARGET_CPU);
    auto outLayerIndices = net.getUnconnectedOutLayers();
    auto layerNames = net.getLayerNames();
    vector<String> outBlobNames(outLayerIndices.size());
    for (auto i = 0; i < outLayerIndices.size(); i++) {
        outBlobNames.at(i) = layerNames.at(outLayerIndices.at(i) - 1);
    }
    auto img = imread("../../test/1a0n.jpg", IMREAD_COLOR);
    Size inputSize(img.rows - img.rows % 32, img.cols - img.cols % 32);
    Mat vec4d;
    blobFromImage(img, vec4d, 1 / 255.0, inputSize);
    net.setInput(vec4d);
    auto start = system_clock::now();
    vector<Mat> outs;
    net.forward(outs, outBlobNames);
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    return duration.count() * milliseconds::period::num / milliseconds::period::den;
}
