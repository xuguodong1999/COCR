// copy from https://www.cnblogs.com/yanghailin/p/12901586.html
#pragma once

#include <torch/cuda.h>
#include <torch/nn/init.h>
#include <iostream>
#include <vector>
#include <opencv2/imgcodecs.hpp>

using std::cout;
using std::endl;
using std::vector;

static const torch::Device DEVICE(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);

void helloworld() {
    auto ones = torch::ones({4, 9}).to(DEVICE).to(torch::kFloat32);
    ones[3][8] = 4;
    std::cout << ones << std::endl;
}

void debug() {
    torch::Tensor box_1 = torch::rand({5, 4});
    std::cout << box_1 << std::endl;  // 可以打印出数值
}

void full_like() {
    torch::Tensor tmp_1 = torch::rand({2, 3});
    torch::Tensor tmp_2 = torch::full_like(tmp_1, 1);
    cout << tmp_1 << endl;
    cout << tmp_2 << endl;
}

void init() {
    {
        torch::Tensor input = torch::rand({1, 3, 2, 3});
        cout << input << endl;
    }
    {
        torch::Tensor a = torch::empty({2, 4});
        std::cout << a << std::endl;
    }
    {
        torch::Tensor a = torch::ones({2, 4});
        std::cout << a << std::endl;
    }
    {
        torch::Tensor scores = torch::ones({1, 2, 3}).to(DEVICE);
        torch::Tensor keep = torch::zeros({scores.size(0)}).to(torch::kLong).to(scores.device());
    }
    {
        torch::Tensor num_out = torch::full({2, 3}, -2, torch::dtype(torch::kLong));
        std::cout << num_out << std::endl;
    }
    {
        torch::Tensor a = torch::ones({3, 2}).fill_(-8).to(torch::kCUDA);
        std::cout << a << std::endl;
    }
    {//按列拼接
        torch::Tensor a = torch::rand({2, 3});
        torch::Tensor b = torch::rand({2, 1});
        torch::Tensor cat_1 = torch::cat({a, b}, 1);  // 按列拼接--》》前提是行数需要一样

        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << cat_1 << std::endl;
    }
    {//按行拼接
        torch::Tensor a = torch::rand({2, 3});
        torch::Tensor b = torch::rand({1, 3});
        torch::Tensor cat_1 = torch::cat({a, b}, 0);

        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << cat_1 << std::endl;
    }
    {
        torch::Tensor box_1 = torch::rand({5, 4});
        torch::Tensor score_1 = torch::rand({5, 1});
        torch::Tensor label_1 = torch::rand({5, 1});
        torch::Tensor result_1 = torch::cat({box_1, score_1, label_1}, 1);
    }
    {
        // vector 和cat的融合操作
        torch::Tensor xs, ys, wh_0, wh_1;
        xs = ys = wh_0 = wh_1 = torch::ones({1, 2, 99});
        torch::Tensor xs_t0 = xs - wh_0 / 2;
        torch::Tensor ys_t0 = ys - wh_1 / 2;
        torch::Tensor xs_t1 = xs + wh_0 / 2;
        torch::Tensor ys_t1 = ys + wh_1 / 2;
        vector<torch::Tensor> abce = {xs_t0, ys_t0, xs_t1, ys_t1};
        torch::Tensor bboxes = torch::cat(abce, 2);
        std::cout << "-----cat   shape---" << std::endl;
        torch::Tensor bboxes2 = torch::cat({xs_t0, ys_t0, xs_t1, ys_t1}, 2);
        torch::Tensor x1 = bboxes.select(1, 0).clone();
    }
    {
        // select//按行取
        torch::Tensor a = torch::rand({2, 3});
        std::cout << a << std::endl;
        torch::Tensor b = a.select(0, 1);//按行取
        std::cout << b << std::endl;
    }
    {
        // select//按列取
        torch::Tensor a = torch::rand({2, 3});
        std::cout << a << std::endl;
        torch::Tensor b = a.select(1, 1);
        std::cout << b << std::endl;
    }
    {
        // select浅拷贝
        torch::Tensor a = torch::rand({2, 3});
        std::cout << a << std::endl;
        torch::Tensor b = a.select(1, 1);
        std::cout << b << std::endl;
        b[0] = 0.0;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
    }
    {
        // index一定要是toType(torch::kLong)这种类型的
        // 用数组导入tensor的，发现idx全是0，原因未知

        torch::Tensor a = torch::rand({2, 6});
        std::cout << a << std::endl;

        torch::Tensor idx = torch::empty({4}).toType(torch::kLong);
        idx[0] = 0;
        idx[1] = 2;
        idx[2] = 4;
        idx[3] = 1;

        // int idx_data[4] = {1,3,2,4};
        // torch::Tensor idx = torch::from_blob(idx_data,{4}).toType(torch::kLong);//idx全是0  ?????????????????

        std::cout << idx << std::endl;

        torch::Tensor b = a.index_select(1, idx);
        std::cout << b << std::endl;
    }
    {
        // index_select深拷贝

        torch::Tensor a = torch::rand({2, 6});
        std::cout << a << std::endl;


        torch::Tensor idx = torch::empty({4}).toType(torch::kLong);
        idx[0] = 0;
        idx[1] = 2;
        idx[2] = 4;
        idx[3] = 1;

        // int idx_data[4] = {1,3,2,4};
        // torch::Tensor idx = torch::from_blob(idx_data,{4}).toType(torch::kLong);

        std::cout << idx << std::endl;

        torch::Tensor b = a.index_select(1, idx);
        std::cout << b << std::endl;

        b[0][0] = 0.0;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
    }
    {
        // index inline Tensor Tensor::index(TensorList indices)
        // 这个函数实验下来，只能按行取，且是深拷贝
        torch::Tensor a = torch::rand({2, 6});
        std::cout << a << std::endl;

        torch::Tensor idx_1 = torch::empty({2}).toType(torch::kLong);
        idx_1[0] = 0;
        idx_1[1] = 1;

        torch::Tensor bb = a.index({idx_1});
        bb[0][0] = 0;

        std::cout << bb << std::endl;
        std::cout << a << std::endl;
    }
    {
        // slice inline Tensor Tensor::slice(int64_t dim, int64_t start, int64_t end, int64_t step) //dim0表示按行取，1表示按列取，从start开始，到end（不含）结束
        // 可以看到结果，是浅拷贝！！！
        torch::Tensor a = torch::rand({2, 6});
        std::cout << a << std::endl;
        torch::Tensor b = a.slice(0, 0, 1);
        torch::Tensor c = a.slice(1, 0, 3);
        b[0][0] = 0.0;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << a << std::endl;
    }
    {
//narrow narrow_copy
//inline Tensor Tensor::narrow(int64_t dim, int64_t start, int64_t length) const
//inline Tensor Tensor::narrow_copy(int64_t dim, int64_t start, int64_t length) const
        torch::Tensor a = torch::rand({4, 6});
        torch::Tensor b = a.narrow(0, 1, 2);
        torch::Tensor c = a.narrow_copy(0, 1, 2);
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
    }
    {
//inline Tensor Tensor::squeeze() const//不加参数的，把所有为1的维度都压缩
//inline Tensor Tensor::squeeze(int64_t dim)const//加参数的，指定哪个维度压缩
//inline Tensor & Tensor::squeeze_() const //暂时不知道啥区别
//inline Tensor & Tensor::squeeze_(int64_t dim) const //暂时不知道啥区别
//squeeze(int64_t dim) 指定压缩哪个维度

        torch::Tensor a = torch::rand({1, 1, 3});
        std::cout << a << std::endl;

        torch::Tensor b = a.squeeze();
        std::cout << b << std::endl;

        torch::Tensor c = a.squeeze(0);
        std::cout << c << std::endl;

        torch::Tensor d = a.squeeze(1);
        std::cout << d << std::endl;

        torch::Tensor e = a.squeeze(2);
        std::cout << e << std::endl;
    }
    {
        //unsqueeze
        torch::Tensor a = torch::rand({2, 3});
        std::cout << a << std::endl;

        torch::Tensor b = a.unsqueeze(0);
        std::cout << b << std::endl;

        torch::Tensor bb = a.unsqueeze(1);
        std::cout << bb << std::endl;

        torch::Tensor bbb = a.unsqueeze(2);
        std::cout << bbb << std::endl;
    }
    {
//torch::nonzero 输出非0的坐标
        torch::Tensor a = torch::rand({2, 3});
        a[0][1] = 0;
        a[1][2] = 0;
        std::cout << a << std::endl;
        torch::Tensor b = torch::nonzero(a);
        std::cout << b << std::endl;
    }
    {
//取出tensor的某个值 为int或者float ===》》》auto bbb = a[1][1].item().toFloat();
//一般情况下取出tensor某个值可以直接下标索引即可。比如a[0][1],但是这个值还是tensor类型的，要想为c++的int或者float的，如下：
        torch::Tensor a = torch::rand({2, 3});
        std::cout << a << std::endl;
        auto bbb = a[1][1].item().toFloat();
        std::cout << bbb << std::endl;
    }
    {
        torch::Tensor scores = torch::rand({10});
        std::tuple<torch::Tensor, torch::Tensor> sort_ret = torch::sort(scores.unsqueeze(1), 0, 1);
        torch::Tensor v = std::get<0>(sort_ret).squeeze(1).to(scores.device());
        torch::Tensor idx = std::get<1>(sort_ret).squeeze(1).to(scores.device());
        std::cout << scores << std::endl;
        std::cout << v << std::endl;
        std::cout << idx << std::endl;

        for (int i = 0; i < 10; i++) {
            int idx_1 = idx[i].item<int>();
            float s = v[i].item<float>();

            std::cout << idx_1 << "  " << s << std::endl;
        }
        {
            const size_t m_SIZE_IMAGE = 64;
            const char *path = "/home/xgd/Pictures/home.png";
            cv::Mat m_out = cv::imread(path);
            //[320,320,3]
            auto input_tensor = torch::from_blob(
                    m_out.data, {m_SIZE_IMAGE, m_SIZE_IMAGE, 3}).toType(torch::kFloat32);
            //如果torch::kByte这种格式，会把负数变成正数，所以需要torch::kFloat32类型的
            //[3,320,320]
            input_tensor = input_tensor.permute({2, 0, 1});
            input_tensor = input_tensor.unsqueeze(0);
            input_tensor = input_tensor.to(torch::kFloat).to(DEVICE);
        }
        {
            const int w = 6345, h = 4;
            std::vector<float> region_priors(w * h, 0);
            torch::Tensor m_prior = torch::from_blob(region_priors.data(), {w, h}).cuda();
        }
        {
            torch::Tensor a = torch::rand({2, 3});
            std::cout << a << std::endl;

            auto aa = a.size(0);
            auto bb = a.size(1);
            auto a_size = a.sizes();
            std::cout << aa << std::endl;
            std::cout << bb << std::endl;
            std::cout << a_size << std::endl;

            int num_ = a.numel();
            std::cout << num_ << std::endl;

            torch::Tensor aaa;
            int numa_ = aaa.numel();
            std::cout << numa_ << std::endl;
        }
        {
            // 获取维度大小，比如[1,5,8,2],我需要得到维度4
            auto img_poly = torch::ones({1, 5, 8, 2});
            auto aaa = img_poly.sizes();
            int len_ = aaa.size();
        }
        {
            // torch::sort
            // static inline std::tuple<Tensor,Tensor> sort(const Tensor & self, Dimname dim, bool descending)
            // dim0表示按行，1表示按列
            // descending=false表示升序，true表示降序
            // 返回的是元组，第一个表示排序后的值，第二个表示排序之后对应之前的索引。

            torch::Tensor scores = torch::rand({10});
            std::tuple<torch::Tensor, torch::Tensor> sort_ret = torch::sort(scores.unsqueeze(1), 0, 1);
            torch::Tensor v = std::get<0>(sort_ret).squeeze(1).to(scores.device());
            torch::Tensor idx = std::get<1>(sort_ret).squeeze(1).to(scores.device());
            std::cout << scores << std::endl;
            std::cout << v << std::endl;
            std::cout << idx << std::endl;
        }
        {
            // clamp 把数值控制在min max之间，小于min的就为min，大于max的就为max
            // inline Tensor Tensor::clamp(c10::optional min, c10::optional max) const

            torch::Tensor a = torch::rand({2, 3});
            a[0][0] = 20;
            a[0][1] = 21;
            a[0][2] = 22;
            a[1][0] = 23;
            a[1][1] = 24;
            std::cout << a << std::endl;

            torch::Tensor b = a.clamp(21, 22);
            std::cout << b << std::endl;
            //在工程中，一般要取tensor里面的值，还有有时候就只限制一边，比如只限制min，如下：
            // xx1 = xx1.clamp(x1[i].item().toFloat(),INT_MAX*1.0);
        }
        {
            // 大于> 小于< 运算
            torch::Tensor a = torch::rand({2, 3});
            std::cout << a << std::endl;
            torch::Tensor b = a > 0.5;
            std::cout << b << std::endl;
        }
        {
            //.转置Tensor::transpose
            //inline Tensor Tensor::transpose(Dimname dim0, Dimname dim1) const

            torch::Tensor a = torch::rand({2, 3});
            std::cout << a << std::endl;

            torch::Tensor b = a.transpose(1, 0);
            std::cout << b << std::endl;
        }
        {
            //expand_as
            //inline Tensor Tensor::expand_as(const Tensor & other) const

            torch::Tensor a = torch::rand({2, 3});;
            // torch::Tensor b = torch::ones({2,2});
            torch::Tensor b = torch::ones({2, 1});
            torch::Tensor c = b.expand_as(a);

            std::cout << a << std::endl;
            std::cout << b << std::endl;
            std::cout << c << std::endl;
        }
        {
            //乘 mul_ 除div 减sub_
            const int width = 64, height = 128;
            torch::Tensor boxes_my, prediction, input_tensor;
            boxes_my = prediction = input_tensor = torch::ones({256, 5, 1});
            boxes_my.select(1, 0).mul_(width);
            boxes_my.select(1, 1).mul_(height);
            boxes_my.select(1, 2).mul_(width);
            boxes_my.select(1, 3).mul_(height);
            prediction.select(2, 3).div(2);
            input_tensor[0][0] = input_tensor[0][0].sub_(0.485).div_(0.229);
            input_tensor[0][1] = input_tensor[0][1].sub_(0.456).div_(0.224);
            input_tensor[0][2] = input_tensor[0][2].sub_(0.406).div_(0.225);
        }
        {
            // torch::jit::script::Module m_model = torch::jit::load(path_pt);
            // m_model.to(DEVICE);
            // m_model.eval();
        }
        {
            // auto output = m_model.forward({input_tensor});
            // auto tpl = output.toTuple();
            // auto arm_loc = tpl->elements()[0].toTensor();
            //  // arm_loc.print();
            //  // std::cout<<arm_loc[0]<<std::endl;
            // auto arm_conf = tpl->elements()[1].toTensor();
            // //arm_conf.print();
            // auto odm_loc = tpl->elements()[2].toTensor();
            // //odm_loc.print();
            //  // std::cout<<odm_loc[0]<<std::endl;
            // auto odm_conf = tpl->elements()[3].toTensor();
            //  // odm_conf.print();
        }
        {
            //resize_ zero_
            //Tensor & resize_(IntArrayRef size) const;
            //Tensor & zero_() const;
            //
            torch::Tensor a = torch::rand({1, 3, 2, 2});

            const int batch_size = a.size(0);
            const int depth = a.size(1);
            const int image_height = a.size(2);
            const int image_width = a.size(3);

            torch::Tensor crops = torch::rand({1, 3, 2, 2});
            // torch::Tensor crops;
            crops.resize_({batch_size, depth, image_height, image_width});
            crops.zero_();

            std::cout << a << std::endl;
            std::cout << crops << std::endl;
        }
        {
            //meshgrid 把tens变成方阵
            //static inline std::vector meshgrid(TensorList tensors)

            torch::Tensor scales = torch::ones({2});
            torch::Tensor ratios = torch::ones({2});
            ratios += 2;

            std::cout << scales << std::endl;
            std::cout << ratios << std::endl;

            std::vector<torch::Tensor> mesh = torch::meshgrid({scales, ratios});

            torch::Tensor scales_1 = mesh[0];
            torch::Tensor ratios_1 = mesh[1];

            std::cout << scales_1 << std::endl;
            std::cout << ratios_1 << std::endl;
        }
        {
            //flatten 展平tensor
            //Tensor flatten(int64_t start_dim=0, int64_t end_dim=-1) const;
            //Tensor flatten(int64_t start_dim, int64_t end_dim, Dimname out_dim) const;
            //Tensor flatten(Dimname start_dim, Dimname end_dim, Dimname out_dim) const;
            //Tensor flatten(DimnameList dims, Dimname out_dim) const;

            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor b = a.flatten();
            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            // fill_ tensor填充某个值 就地操作，填充当前tensor
            // Tensor & fill_(Scalar value) const;
            // Tensor & fill_(const Tensor & value) const;

            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor b = a.fill_(4);

            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            // .torch::stack
            // static inline Tensor stack(TensorList tensors, int64_t dim)

            torch::Tensor a = torch::rand({3});
            torch::Tensor b = torch::rand({3});
            torch::Tensor c = torch::stack({a, b}, 1);

            std::cout << a << std::endl;
            std::cout << b << std::endl;
            std::cout << c << std::endl;
        }
        {
            //reshape
            //inline Tensor Tensor::reshape(IntArrayRef shape) const

            torch::Tensor a = torch::rand({2, 4});
            torch::Tensor b = a.reshape({-1, 2});
            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            //view
            //inline Tensor Tensor::view(IntArrayRef size) const
            //
            //需要先contiguous
            //a.contiguous().view({-1, 4});
            //
            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor b = a.contiguous().view({-1, 6});
            torch::Tensor c = a.contiguous().view({3, 2});

            std::cout << a << std::endl;
            std::cout << b << std::endl;
            std::cout << c << std::endl;
        }
        {
            //argmax argmin
            //static inline Tensor argmax(const Tensor & self, c10::optional<int64_t> dim=c10::nullopt, bool keepdim=false);
            //static inline Tensor argmin(const Tensor & self, c10::optional<int64_t> dim=c10::nullopt, bool keepdim=false);

            torch::Tensor a = torch::rand({2, 3});
            auto b = torch::argmax(a, 0);

            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            torch::Tensor a = torch::rand({2, 3});
            auto b = torch::argmax(a, 1);

            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            // where
            // static inline Tensor where(const Tensor & condition, const Tensor & self, const Tensor & other);
            // static inline std::vector where(const Tensor & condition);
//
            // torch::Tensor d = torch::where(a>0.5,b,c);
            // 说明：在a大于0.5的位置设为pos，d的pos位置上用b的pos位置上面值填充，其余的位置上值是c的值
            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor b = torch::ones({2, 3});
            torch::Tensor c = torch::zeros({2, 3});
            torch::Tensor d = torch::where(a > 0.5, b, c);
            std::cout << a << std::endl;
            std::cout << b << std::endl;
            std::cout << c << std::endl;
            std::cout << d << std::endl;
        }
        {
            torch::Tensor a = torch::rand({2, 3});
            auto b = torch::where(a > 0.5);
            std::cout << a << std::endl;
            std::cout << b << std::endl;
        }
        {
            //accessor
            //TensorAccessor<T,N> accessor() const&
            //auto result_data = result.accessor<float, 2>(); //2代表二维
            //示例1：
            //
            torch::Tensor one = torch::randn({9, 6});
            auto foo_one = one.accessor<float, 2>();
            for (int i = 0, sum = 0; i < foo_one.size(0); i++)
                for (int j = 0; j < foo_one.size(1); j++)
                    sum += foo_one[i][j];
            //    torch::Tensor result;
            //    for(int i=1;i<m_num_class;i++)
            //    {
            //        //...
            //        if(0 == result.numel())
            //        {
            //            result = result_.clone();
            //        }else
            //        {
            //            result = torch::cat({result,result_},0);//按行拼接
            //        }
            //    }
            //    result =result.cpu();
            //    auto result_data = result.accessor<float, 2>();
            //
            //    cv::Mat img_draw = img.clone();
            //    for(int i=0;i<result_data.size(0);i++)
            //    {
            //        float score = result_data[i][4];
            //        if(score < 0.4) { continue;}
            //        int x1 = result_data[i][0];
            //        int y1 = result_data[i][1];
            //        int x2 = result_data[i][2];
            //        int y2 = result_data[i][3];
            //        int id_label = result_data[i][5];
            //
            //        cv::rectangle(img_draw,cv::Point(x1,y1),cv::Point(x2,y2),cv::Scalar(255,0,0),3);
            //        cv::putText(img_draw,label_map[id_label],cv::Point(x1,y2),CV_FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,55));
            //    }
        }
        {
            // min max
            torch::Tensor a = torch::rand({4, 2});
            std::tuple<torch::Tensor, torch::Tensor> max_test = torch::max(a, 1);

            auto max_val = std::get<0>(max_test);
            // index
            auto index = std::get<1>(max_test);

            std::cout << a << std::endl;
            std::cout << max_val << std::endl;
            std::cout << index << std::endl;
        }
        {
            torch::Tensor a = torch::rand({4, 2});
            torch::Tensor max_test = torch::max(a);

            std::cout << a << std::endl;
            std::cout << max_test << std::endl;
        }
        {
            // masked_select 与 masked_fill
            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor c = (a > 0.25);
            torch::Tensor d = a.masked_select(c);

            std::cout << a << std::endl;
            std::cout << c << std::endl;
            std::cout << d << std::endl;
        }
        {
            torch::Tensor a = torch::rand({2, 3});
            torch::Tensor aa = a.clone();
            aa.masked_fill_(aa > 0.5, -2);

            std::cout << a << std::endl;
            std::cout << aa << std::endl;
        }
        {
            // masked_fill_ 带下划线的都是就地操作
            // 有个需求是Tensor score表示得分，Tensor label表示标签，他们都是同大小的。
            // 后处理就是当label=26并且label=26的分数小于0.5，那么就把label相应位置置1

            float index[] = {3, 2, 3, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
            float score[] = {0.1, 0.1, 0.9, 0.9, 0.9, 0.1, 0.1, 0.1, 0.1, 0.1, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8};

            torch::Tensor aa = torch::from_blob(index, {4, 4}).toType(torch::kFloat32);
            torch::Tensor bb = torch::from_blob(score, {4, 4}).toType(torch::kFloat32);
            std::cout << aa << std::endl;
            std::cout << bb << std::endl;

            torch::Tensor tmp = (aa == 3);
            torch::Tensor tmp_2 = (bb >= 0.9);
            std::cout << tmp << std::endl;
            std::cout << tmp_2 << std::endl;
            torch::Tensor condition_111 = tmp * tmp_2;

            std::cout << condition_111 << std::endl;
            aa.masked_fill_(condition_111, -1);
        }
        {
//   torch::jit::script::Module module = torch::jit::load(argv[1]);
//    std::cout << "== Switch to GPU mode" << std::endl;
//    // to GPU
//    module.to(at::kCUDA);
//
//    if (LoadImage(file_name, image)) {
//            auto input_tensor = torch::from_blob(
//                    image.data, {1, kIMAGE_SIZE, kIMAGE_SIZE, kCHANNELS});
//            input_tensor = input_tensor.permute({0, 3, 1, 2});
//            input_tensor[0][0] = input_tensor[0][0].sub_(0.485).div_(0.229);
//            input_tensor[0][1] = input_tensor[0][1].sub_(0.456).div_(0.224);
//            input_tensor[0][2] = input_tensor[0][2].sub_(0.406).div_(0.225);
//
//            // to GPU
//            input_tensor = input_tensor.to(at::kCUDA);
//
//            torch::Tensor out_tensor = module.forward({input_tensor}).toTensor();
//
//            auto results = out_tensor.sort(-1, true);
//            auto softmaxs = std::get<0>(results)[0].softmax(0);
//            auto indexs = std::get<1>(results)[0];
//
//            for (int i = 0; i < kTOP_K; ++i) {
//                auto idx = indexs[i].item<int>();
//                std::cout << "    ============= Top-" << i + 1
//                          << " =============" << std::endl;
//                std::cout << "    Label:  " << labels[idx] << std::endl;
//                std::cout << "    With Probability:  "
//                          << softmaxs[i].item<float>() * 100.0f << "%" << std::endl;
//            }
//
//        }
        }
        {
//    //[128,512]
//    torch::Tensor b = torch::argmax(output_1, 2).cpu();
//    //    std::cout<<b<<std::endl;
//    b.print();
//
//    cv::Mat mask(T_height, T_width, CV_8UC1, (uchar*)b.data_ptr());
//    imshow("mask",mask*255);
//    waitKey(0);
        }
        {
            cv::Mat grayMat;
            cv::Mat m_tmp = grayMat.clone();
            m_tmp.convertTo(m_tmp, CV_32FC1);/////又是个大坑 图片要先转float32啊
            torch::Tensor label_deal = torch::from_blob(
                    m_tmp.data, {grayMat.rows, grayMat.cols}).toType(torch::kByte).to(DEVICE);
        }
        {
//            torch::Tensor output = m_model->forward({input_tensor}).toTensor()[0];
//            torch::Tensor output_cpu = output.cpu();
//            //output_cpu     Variable[CPUFloatType] [26, 480, 480]]
//            output_cpu.print();
////            void *ptr = output_cpu.data_ptr();
////            std::cout<<(float*)ptr<<std::endl;
//            void *ptr = output_cpu.data_ptr();
////        std::cout<<*((float*)ptr[0][0][0])<<std::endl;
////        std::cout<<(float*)ptr[0][0][0]<<std::endl;
//
//            std::cout<<*((float*)(ptr+2))<<std::endl;
//            const float* result = reinterpret_cast<const float *>(output_cpu.data_ptr());
////            void *ptr = output_cpu.data_ptr();
////            const float* result = (float*)ptr;

        }
        {
////            pytorch端的输出：
//
////            def forward(self, x, batch=None):
////            output, cnn_feature = self.dla(x)
////            return (output['ct_hm'],output['wh'],cnn_feature)
////            对应的libtorch端
//
//            auto out = m_model->forward({input_tensor});
//            auto tpl = out.toTuple();
//            auto out_ct_hm = tpl->elements()[0].toTensor();
//            out_ct_hm.print();
//            auto out_wh = tpl->elements()[1].toTensor();
//            out_wh.print();
//            auto out_cnn_feature = tpl->elements()[2].toTensor();
//            out_cnn_feature.print();
////            如果输出单个tensor，就是
//
//            at::Tensor output = module->forward(inputs).toTensor();
        }
        {
            const int m_height = 64, m_width = 64, m_color_cnt = 3;
            /////////////////////////////////////////////////////////////////////////////////////////////////
            auto t1 = std::chrono::steady_clock::now();
//    static torch::Tensor tensor_m0 = torch::zeros({m_height,m_width}).to(torch::kByte).to(torch::kCPU);
//    static torch::Tensor tensor_m1 = torch::zeros({m_height,m_width}).to(torch::kByte).to(torch::kCPU);
//    static torch::Tensor tensor_m2 = torch::zeros({m_height,m_width}).to(torch::kByte).to(torch::kCPU);

            static torch::Tensor tensor_m0 = torch::zeros({m_height, m_width}).to(torch::kByte);
            static torch::Tensor tensor_m1 = torch::zeros({m_height, m_width}).to(torch::kByte);
            static torch::Tensor tensor_m2 = torch::zeros({m_height, m_width}).to(torch::kByte);
            tensor_m0 = tensor_m0.to(torch::kCUDA);
            tensor_m1 = tensor_m1.to(torch::kCUDA);
            tensor_m2 = tensor_m2.to(torch::kCUDA);
//            for(int i=1;i<m_color_cnt;i++)
//            {
//                tensor_m0.masked_fill_(index==i,colormap[i * 3]);
//                tensor_m1.masked_fill_(index==i,colormap[i * 3 + 1]);
//                tensor_m2.masked_fill_(index==i,colormap[i * 3 + 2]);
//            }
            torch::Tensor tensor_m00 = tensor_m0.cpu();
            torch::Tensor tensor_m11 = tensor_m1.cpu();
            torch::Tensor tensor_m22 = tensor_m2.cpu();
            cv::Mat m0 = cv::Mat(m_height, m_width, CV_8UC1, (uchar *) tensor_m00.data_ptr());
            cv::Mat m1 = cv::Mat(m_height, m_width, CV_8UC1, (uchar *) tensor_m11.data_ptr());
            cv::Mat m2 = cv::Mat(m_height, m_width, CV_8UC1, (uchar *) tensor_m22.data_ptr());
            std::vector<cv::Mat> channels = {m0, m1, m2};
            cv::Mat mergeImg;
            cv::merge(channels, mergeImg);
            mergeImg = mergeImg.clone();
            auto ttt1 = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::steady_clock::now() - t1).count();
            std::cout << "merge time=" << ttt1 << "ms" << std::endl;
            /////////////////////////////////////////////////////////////////////////////////////////////

        }
        {
//            auto t0 = std::chrono::steady_clock::now();
//            for (int i = 0; i<labelMat.rows; i++)
//            {
//                for (int j = 0; j<labelMat.cols; j++)
//                {
//                    int id = labelMat.at<uchar>(i,j);
//                    if(0 == id)
//                    {
//                        continue;
//                    }
//                    colorMat.at<cv::Vec3b>(i, j)[0] = colormap[id * 3];
//                    colorMat.at<cv::Vec3b>(i, j)[1] = colormap[id * 3 + 1];
//                    colorMat.at<cv::Vec3b>(i, j)[2] = colormap[id * 3 + 2];
//                }
//            }
//            auto ttt = std::chrono::duration_cast<std::chrono::milliseconds>
//                    (std::chrono::steady_clock::now() - t0).count();
//            std::cout << "consume time="<<ttt<<"ms"<<std::endl;
        }
        {
            torch::Tensor a = torch::rand({5, 3, 2});
            torch::Tensor idx = torch::zeros({5}).toType(torch::kLong);
            idx[3] = 1;
            idx[1] = 1;

            torch::Tensor abc = torch::nonzero(idx);
            torch::Tensor b = a.index_select(0, abc.squeeze());

            std::cout << a << std::endl;
            std::cout << abc << std::endl;
            std::cout << b << std::endl;

        }
    }
}

bool nms(const torch::Tensor &boxes, const torch::Tensor &scores, torch::Tensor &keep, int &count, float overlap,
         int top_k) {
    count = 0;
    keep = torch::zeros({scores.size(0)}).to(torch::kLong).to(scores.device());
    if (0 == boxes.numel()) {
        return false;
    }

    torch::Tensor x1 = boxes.select(1, 0).clone();
    torch::Tensor y1 = boxes.select(1, 1).clone();
    torch::Tensor x2 = boxes.select(1, 2).clone();
    torch::Tensor y2 = boxes.select(1, 3).clone();
    torch::Tensor area = (x2 - x1) * (y2 - y1);
    //    std::cout<<area<<std::endl;

    std::tuple<torch::Tensor, torch::Tensor> sort_ret = torch::sort(scores.unsqueeze(1), 0, 0);
    torch::Tensor v = std::get<0>(sort_ret).squeeze(1).to(scores.device());
    torch::Tensor idx = std::get<1>(sort_ret).squeeze(1).to(scores.device());

    int num_ = idx.size(0);
    if (num_ > top_k) //python:idx = idx[-top_k:]
    {
        idx = idx.slice(0, num_ - top_k, num_).clone();
    }
    torch::Tensor xx1, yy1, xx2, yy2, w, h;
    while (idx.numel() > 0) {
        auto i = idx[-1];
        keep[count] = i;
        count += 1;
        if (1 == idx.size(0)) {
            break;
        }
        idx = idx.slice(0, 0, idx.size(0) - 1).clone();

        xx1 = x1.index_select(0, idx);
        yy1 = y1.index_select(0, idx);
        xx2 = x2.index_select(0, idx);
        yy2 = y2.index_select(0, idx);

        xx1 = xx1.clamp(x1[i].item().toFloat(), INT_MAX * 1.0);
        yy1 = yy1.clamp(y1[i].item().toFloat(), INT_MAX * 1.0);
        xx2 = xx2.clamp(INT_MIN * 1.0, x2[i].item().toFloat());
        yy2 = yy2.clamp(INT_MIN * 1.0, y2[i].item().toFloat());

        w = xx2 - xx1;
        h = yy2 - yy1;

        w = w.clamp(0, INT_MAX);
        h = h.clamp(0, INT_MAX);

        torch::Tensor inter = w * h;
        torch::Tensor rem_areas = area.index_select(0, idx);

        torch::Tensor union_ = (rem_areas - inter) + area[i];
        torch::Tensor Iou = inter * 1.0 / union_;
        torch::Tensor index_small = Iou < overlap;
        auto mask_idx = torch::nonzero(index_small).squeeze();
        idx = idx.index_select(0, mask_idx);//pthon: idx = idx[IoU.le(overlap)]
    }
    return true;
}

//ex shape[1,5,4,2]      ex[..., 0, 1] -->>[1,5]
torch::Tensor index_tensor_3(const torch::Tensor &ex, const int &idx1, const int &idx2) {
    const int dim0 = ex.size(0);
    const int dim1 = ex.size(1);
    const int dim2 = ex.size(2);
    const int dim3 = ex.size(3);

    std::vector<int> v_index(ex.numel());//初始化：ex.numel() 个0
    int offset = dim2 * dim3;
    for (int i = 0; i < dim1; i++) {
        int index_ = idx1 * dim3 + idx2;
        v_index[i * offset + index_] = 1;
    }

    torch::Tensor index = torch::tensor(v_index).to(ex.device());
    index = index.reshape(ex.sizes()).toType(torch::kByte);//这里需要kByte类型
//    std::cout<<index<<std::endl;

    torch::Tensor selete = ex.masked_select(index).unsqueeze(0);
    return selete;
}


// aim [1,10,2,2]   ind_mask_ [1,10] 比如前5个是1余都是0  得到的结果形状是[5,40,2]  即pytorch里面的操作 aim = aim[ind_mask]
torch::Tensor deal_mask_index22(torch::Tensor aim_, torch::Tensor ind_mask_) {
    torch::Tensor aim = aim_.clone().squeeze(0);//[1,100,40,2]  -->> [100,40,2]
    torch::Tensor ind_mask = ind_mask_.clone().squeeze(0);////[1,100]  -->> [100]
    int row = ind_mask.size(0);
    int cnt = 0;
    for (int i = 0; i < row; i++) {
        if (ind_mask[i].item().toInt()) {
            cnt += 1;
        }
    }
    torch::Tensor out = torch::zeros({cnt, aim.size(1), aim.size(2)});
    int index_ = 0;
    for (int i = 0; i < row; i++) {
        if (ind_mask[i].item().toInt()) {
            out[index_++] = aim[i];
//            std::cout<<i<<std::endl;
        }
    }

    std::cout << "##############################################" << std::endl;
    std::cout << out << std::endl;

    return out;
}

void test_deal_mask_index22() {
    torch::Tensor ind_mask = torch::ones({1, 10});
    ind_mask[0][0] = 0;
    ind_mask[0][1] = 0;
    ind_mask[0][2] = 0;
    ind_mask[0][4] = 0;

    torch::Tensor aim = torch::rand({1, 10, 2, 2});
    std::cout << aim << std::endl;

    deal_mask_index22(aim, ind_mask);
}
