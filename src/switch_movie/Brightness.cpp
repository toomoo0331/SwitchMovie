//
// Created by kei666 on 19/11/01.
//

#include <tbb/parallel_for.h>
#include <tbb/combinable.h>

#include "Brightness.h"

using namespace sw;

Brightness::Brightness(const std::vector<std::string> &model_prefixes,
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


}

void Brightness::calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights)
{
    //
    weights->resize(images.size());

    std::vector<float> &weights_r = *weights;

    float denominator = images[0].rows * images[0].cols*255;
    for (int i = 0; i < images.size(); i++) {
        cv::Mat hsv;
        cvtColor(images[i], hsv,CV_BGR2HSV);
        if (positive_region_)
            weights_r[i] = 1 - (float)cv::sum(hsv)[2] / denominator;
        else {
            weights_r[i] = (float)cv::sum(hsv)[2] / denominator;
        }
    }
}

void Brightness::calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights, std::vector<int> *ranks)
{
    calculateWeights(images, weights);

    std::vector<float> &weights_r = *weights;

    ranks->resize(images.size());
    //一度数字を照準に並べる（５つ分）
    std::iota(ranks->begin(), ranks->end(), 0);
    //スコア順に並べ替える（index番号を）
    std::sort(ranks->begin(), ranks->end(), [&weights_r](int a, int b) -> bool {return weights_r[a] < weights_r[b];});
}