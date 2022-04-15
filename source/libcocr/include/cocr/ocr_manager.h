#pragma once

#include "els_cocr_export.h"
#include "ocv/mat.h"
#include "cocr/text_corrector.h"
#include "cocr/object_detector.h"
#include "cocr/text_recognizer.h"
#include "cocr/graph_composer.h"

class ELS_COCR_EXPORT OCRManager {
    Mat image;
    inline static const int MAX_WIDTH = 960;
    TextRecognizer &recognizer;
    ObjectDetector &detector;
    GraphComposer &composer;
    TextCorrector &corrector;

    void display(const std::vector<OCRItem> &_items, const Mat &_input);

    std::vector<OCRItem> convert(const std::vector<DetectorObject> &_objects, const Mat &_input);

public:
    OCRManager(ObjectDetector &_detector, TextRecognizer &_recognizer, TextCorrector &_corrector,
               GraphComposer &_composer);

    std::shared_ptr<GuiMol> ocr(Mat &_originInput, bool _debug);

    Mat &getImage();

    void setImage(const QList<QList<QPointF>> &_script, const int&screenWidth);

    void setImage(const QImage &_image);

    void setImage(const QPixmap &_pixmap);

    void setImage(const Mat &_cvMat);

    void clearImage();
};