#ifndef _XGD_OCR_MANAGER_HPP_
#define _XGD_OCR_MANAGER_HPP_

#include "text_recognizer.hpp"
#include "object_detector.hpp"
#include "graph_composer.hpp"
#include "text_corrector.hpp"

namespace xgd {
    class OCRManager {
        TextRecognizer &recognizer;
        ObjectDetector &detector;
        GraphComposer &composer;
        TextCorrector &corrector;

        void display(const std::vector<OCRItem> &_items, const cv::Mat &_input);

        std::vector<OCRItem> convert(const std::vector<DetectorObject> &_objects, const cv::Mat &_input);

    public:
        OCRManager(ObjectDetector &_detector, TextRecognizer &_recognizer, TextCorrector &_corrector,
                   GraphComposer &_composer);

        std::shared_ptr<JMol> ocr(cv::Mat &_originInput, bool _debug = false);
    };
}
#endif//_XGD_OCR_MANAGER_HPP_
