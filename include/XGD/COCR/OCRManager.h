#pragma once

#include "xgd_cocr_export.h"
#include "XGD/Vision/Mat.h"
#include "XGD/COCR/TextRecognizer.h"
#include "XGD/COCR/TextCorrector.h"
#include "XGD/COCR/ObjectDetector.h"
#include "XGD/COCR/GraphComposer.h"

class XGD_COCR_EXPORT OCRManager {
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

    void setImage(const QList<QList<QPointF>> &_script, const int &screenWidth);

    void setImage(const QImage &_image);

    void setImage(const QPixmap &_pixmap);

    void setImage(const Mat &_cvMat);

    void clearImage();
};