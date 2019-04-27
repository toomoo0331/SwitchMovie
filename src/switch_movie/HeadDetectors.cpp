//
// Created by kei666 on 18/10/19.
//

#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "HeadDetectors.h"

using namespace sw;

HeadDetectors::HeadDetectors(const std::vector<std::string> &model_prefixes,
                             const std::vector<std::string> &globfeat_prefixes,
                             const int camera_num,
                             const bool positive_region,
                             const int num_models_to_average,
                             const int step_size,
                             const int target_width)
        : num_models_to_average_(num_models_to_average), step_size_(step_size), positive_region_(positive_region)
{
    if ((model_prefixes.size() != globfeat_prefixes.size()) && model_prefixes.size() < 1 && globfeat_prefixes.size() < 1) {
        std::cerr << "model_prefixes.size() == globfeat_prefixes.size() "
                "and model_prefixes.size() > 0 and globfeat_prefixes.size() > 0" << std::endl;
        exit(1);
    }

    model_num_ = model_prefixes.size();

    std::string feature_set = "rvl";

    hds_.reserve(camera_num * model_prefixes.size());
    for (int j = 0; j < camera_num; j++) {
        for (int i = 0; i < model_prefixes.size(); i++) {
            hds_.push_back(HandDetector());
            hds_[j * model_prefixes.size() + i].testInitialize(model_prefixes[i], globfeat_prefixes[i], feature_set, num_models_to_average_,
                                   target_width);
        }
    }
}

void HeadDetectors::detectHead(std::vector<cv::Mat> &images, std::vector<cv::Mat> *head_mask)
{
    //実際に頭の大きさを計算する関数
    head_mask->resize(images.size());
    std::vector<cv::Mat> &head_mask_r = *head_mask;
    head_mask_r.resize(images.size());
    std::vector<cv::Mat> masks(images.size() * model_num_);
    
    tbb::parallel_for(tbb::blocked_range<int>(0, images.size() * model_num_),
                      [&images, &masks, this](const tbb::blocked_range<int>& range) {
        for (int i = range.begin(); i != range.end(); i++) {
            cv::Mat image = images[i / model_num_].clone();
            hds_[i].test(image, num_models_to_average_, step_size_);
            masks[i] = hds_[i].postprocess(hds_[i]._response_img) / 255;
        }
    });
//    tbb::parallel_for(tbb::blocked_range<int>(0, images.size()),
//                      [&images, &images_cp, &masks, &head_mask_r, this](const tbb::blocked_range<int>& range) {
//        for (int i = range.begin(); i != range.end(); i++) {
//            head_mask_r[i] = masks[i * model_num_];
//            for (int j = 1; j < model_num_; j++) {
//                head_mask_r[i] |= masks[i * model_num_ + j];
//            }
//        }
//    });
    for (int i = 0; i < images.size(); i++) {
        head_mask_r[i] = masks[i * model_num_];
        for (int j = 1; j < model_num_; j++) {
            head_mask_r[i] |= masks[i * model_num_ + j];
        }
    }
}

void HeadDetectors::calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights)
{
    //
    weights->resize(images.size());
    std::vector<cv::Mat> head_masks;
    detectHead(images, &head_masks);

    std::vector<float> &weights_r = *weights;

    float denominator = head_masks[0].rows * head_masks[0].cols;
    for (int i = 0; i < head_masks.size(); i++) {
        cv::Mat dst;
        if (positive_region_)
            weights_r[i] = 1 - (float)cv::sum(head_masks[i])[0] / denominator;
        else {
            weights_r[i] = (float)cv::sum(head_masks[i])[0] / denominator;
        }
    }
}

void HeadDetectors::calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights, std::vector<int> *ranks)
{
    calculateWeights(images, weights);

    std::vector<float> &weights_r = *weights;

    ranks->resize(images.size());
    //一度数字を照準に並べる（５つ分）
    std::iota(ranks->begin(), ranks->end(), 0);
    //スコア順に並べ替える（index番号を）
    std::sort(ranks->begin(), ranks->end(), [&weights_r](int a, int b) -> bool {return weights_r[a] < weights_r[b];});
}