#pragma once
#include <ref_torch_public_export.h>
#include <vector>
#include <utility>
#include <memory>
// For External Library
#include <torch/types.h>
#include <opencv2/opencv.hpp>

#define CV_MAT false
#define TORCH_TENSOR true


// -----------------------
// namespace{transforms}
// -----------------------
namespace transforms {


    // --------------------------------------------
    // namespace{transforms} -> class{ComposeImpl}
    // --------------------------------------------
#define transforms_Compose std::shared_ptr<transforms::ComposeImpl>

    class REF_TORCH_PUBLIC_EXPORT ComposeImpl {
    public:
        ComposeImpl() {}

        virtual bool type() = 0;

        virtual void forward(cv::Mat &data_in, cv::Mat &data_out) {}

        virtual void forward(cv::Mat &data_in, torch::Tensor &data_out) {}

        virtual void forward(torch::Tensor &data_in, cv::Mat &data_out) {}

        virtual void forward(torch::Tensor &data_in, torch::Tensor &data_out) {}

        virtual void forward(cv::Mat &data_in1, std::tuple<torch::Tensor, torch::Tensor> &data_in2, cv::Mat &data_out1,
                             std::tuple<torch::Tensor, torch::Tensor> &data_out2) {}

        virtual ~ComposeImpl() {}
    };

    // Function Prototype
    REF_TORCH_PUBLIC_EXPORT
    torch::Tensor apply(std::vector<transforms_Compose > &transform, cv::Mat &data_in);

    REF_TORCH_PUBLIC_EXPORT
    torch::Tensor applyT(std::vector<transforms_Compose > &transform, torch::Tensor &data_in);

    template<typename T_in, typename T_out>
    void forward(std::vector<transforms_Compose > &transform_, T_in &data_in, T_out &data_out, const int count);



    /*******************************************************************************/
    /*                                   Data 1d                                   */
    /*******************************************************************************/


    // -----------------------------------------------------------
    // namespace{transforms} -> class{Normalize1dImpl}(ComposeImpl)
    // -----------------------------------------------------------
#define transforms_Normalize1d std::make_shared<transforms::Normalize1dImpl>

    class REF_TORCH_PUBLIC_EXPORT Normalize1dImpl : public ComposeImpl {
    private:
        torch::Tensor mean, std;
    public:
        Normalize1dImpl() {}

        Normalize1dImpl(const float mean_, const float std_);

        Normalize1dImpl(const float mean_, const std::vector<float> std_);

        Normalize1dImpl(const std::vector<float> mean_, const float std_);

        Normalize1dImpl(const std::vector<float> mean_, const std::vector<float> std_);

        bool type() override { return TORCH_TENSOR; }

        void forward(torch::Tensor &data_in, torch::Tensor &data_out) override;
    };



    /*******************************************************************************/
    /*                                   Data 2d                                   */
    /*******************************************************************************/

    // -----------------------------------------------------------
    // namespace{transforms} -> class{GrayscaleImpl}(ComposeImpl)
    // -----------------------------------------------------------
#define transforms_Grayscale std::make_shared<transforms::GrayscaleImpl>

    class REF_TORCH_PUBLIC_EXPORT GrayscaleImpl : public ComposeImpl {
    private:
        int channels;
    public:
        GrayscaleImpl() {}

        GrayscaleImpl(const int channels_ = 1);

        bool type() override { return CV_MAT; }

        void forward(cv::Mat &data_in, cv::Mat &data_out) override;
    };


    // --------------------------------------------------------
    // namespace{transforms} -> class{ResizeImpl}(ComposeImpl)
    // --------------------------------------------------------
#define transforms_Resize std::make_shared<transforms::ResizeImpl>

    class REF_TORCH_PUBLIC_EXPORT ResizeImpl : public ComposeImpl {
    private:
        cv::Size size;
        int interpolation;
    public:
        ResizeImpl() {}

        ResizeImpl(const cv::Size size_, const int interpolation_ = cv::INTER_LINEAR);

        bool type() override { return CV_MAT; }

        void forward(cv::Mat &data_in, cv::Mat &data_out) override;
    };


    // --------------------------------------------------------------
    // namespace{transforms} -> class{ConvertIndexImpl}(ComposeImpl)
    // --------------------------------------------------------------
#define transforms_ConvertIndex std::make_shared<transforms::ConvertIndexImpl>

    class REF_TORCH_PUBLIC_EXPORT ConvertIndexImpl : public ComposeImpl {
    private:
        int before, after;
    public:
        ConvertIndexImpl() {}

        ConvertIndexImpl(const int before_, const int after_);

        bool type() override { return CV_MAT; }

        void forward(cv::Mat &data_in, cv::Mat &data_out) override;
    };


    // -----------------------------------------------------------
    // namespace{transforms} -> class{ToTensorImpl}(ComposeImpl)
    // -----------------------------------------------------------
#define transforms_ToTensor std::make_shared<transforms::ToTensorImpl>

    class REF_TORCH_PUBLIC_EXPORT ToTensorImpl : public ComposeImpl {
    public:
        ToTensorImpl() {}

        bool type() override { return TORCH_TENSOR; }

        void forward(cv::Mat &data_in, torch::Tensor &data_out) override;
    };


    // ---------------------------------------------------------------
    // namespace{transforms} -> class{ToTensorLabelImpl}(ComposeImpl)
    // ---------------------------------------------------------------
#define transforms_ToTensorLabel std::make_shared<transforms::ToTensorLabelImpl>

    class REF_TORCH_PUBLIC_EXPORT ToTensorLabelImpl : public ComposeImpl {
    public:
        ToTensorLabelImpl() {}

        bool type() override { return TORCH_TENSOR; }

        void forward(cv::Mat &data_in, torch::Tensor &data_out) override;
    };


    // -------------------------------------------------------------
    // namespace{transforms} -> class{AddRVINoiseImpl}(ComposeImpl)
    // -------------------------------------------------------------
#define transforms_AddRVINoise std::make_shared<transforms::AddRVINoiseImpl>

    class REF_TORCH_PUBLIC_EXPORT AddRVINoiseImpl : public ComposeImpl {
    private:
        float occur_prob;
        std::pair<float, float> range;
    public:
        AddRVINoiseImpl() {}

        AddRVINoiseImpl(const float occur_prob_ = 0.01, const std::pair<float, float> range_ = {0.0, 1.0});

        bool type() override { return TORCH_TENSOR; }

        void forward(torch::Tensor &data_in, torch::Tensor &data_out) override;
    };


    // ------------------------------------------------------------
    // namespace{transforms} -> class{AddSPNoiseImpl}(ComposeImpl)
    // ------------------------------------------------------------
#define transforms_AddSPNoise std::make_shared<transforms::AddSPNoiseImpl>

    class REF_TORCH_PUBLIC_EXPORT AddSPNoiseImpl : public ComposeImpl {
    private:
        float occur_prob;
        float salt_rate;
        std::pair<float, float> range;
    public:
        AddSPNoiseImpl() {}

        AddSPNoiseImpl(const float occur_prob_ = 0.01, const float salt_rate_ = 0.5,
                       const std::pair<float, float> range_ = {0.0, 1.0});

        bool type() override { return TORCH_TENSOR; }

        void forward(torch::Tensor &data_in, torch::Tensor &data_out) override;
    };


    // ---------------------------------------------------------------
    // namespace{transforms} -> class{AddGaussNoiseImpl}(ComposeImpl)
    // ---------------------------------------------------------------
#define transforms_AddGaussNoise std::make_shared<transforms::AddGaussNoiseImpl>

    class REF_TORCH_PUBLIC_EXPORT AddGaussNoiseImpl : public ComposeImpl {
    private:
        float occur_prob;
        float mean, std;
        std::pair<float, float> range;
    public:
        AddGaussNoiseImpl() {}

        AddGaussNoiseImpl(const float occur_prob_ = 1.0, const float mean_ = 0.0, const float std_ = 0.01,
                          const std::pair<float, float> range_ = {0.0, 1.0});

        bool type() override { return TORCH_TENSOR; }

        void forward(torch::Tensor &data_in, torch::Tensor &data_out) override;
    };


    // -----------------------------------------------------------
    // namespace{transforms} -> class{NormalizeImpl}(ComposeImpl)
    // -----------------------------------------------------------
#define transforms_Normalize std::make_shared<transforms::NormalizeImpl>

    class REF_TORCH_PUBLIC_EXPORT NormalizeImpl : public ComposeImpl {
    private:
        torch::Tensor mean, std;
    public:
        NormalizeImpl() {}

        NormalizeImpl(const float mean_, const float std_);

        NormalizeImpl(const float mean_, const std::vector<float> std_);

        NormalizeImpl(const std::vector<float> mean_, const float std_);

        NormalizeImpl(const std::vector<float> mean_, const std::vector<float> std_);

        bool type() override { return TORCH_TENSOR; }

        void forward(torch::Tensor &data_in, torch::Tensor &data_out) override;
    };


}