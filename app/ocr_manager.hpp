#ifndef _XGD_OCR_MANAGER_HPP_
#define _XGD_OCR_MANAGER_HPP_

#include "text_recognition.hpp"
#include "object_detection.hpp"
#include "graph_composer.hpp"
#include "text_correction.hpp"
namespace xgd {


    class OCRManager {
        TextRecognition &recognizer;
        ObjectDetector &detector;
        GraphComposer &composer;
        TextCorrector&corrector;
        void display(const std::vector<OCRItem>&_items, const cv::Mat&_input);
        std::vector<OCRItem> convert(const std::vector<DetectorObject>&_objects,const cv::Mat&_input);
    public:
        OCRManager(ObjectDetector &_detector, TextRecognition &_recognizer, TextCorrector&_corrector,GraphComposer &_composer);
        std::shared_ptr<Molecule> ocr(cv::Mat&_originInput,bool _debug=false);
    };
}
#endif//_XGD_OCR_MANAGER_HPP_
