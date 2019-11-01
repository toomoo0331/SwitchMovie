//
// Created by kei666 on 18/10/19.
//

#ifndef PROJECT_HEADDETECTION_H
#define PROJECT_HEADDETECTION_H

#include <opencv2/opencv.hpp>

#include "handtracker/HandDetector.hpp"

namespace sw {

    class HeadDetectors {
    public:
        HeadDetectors(const std::vector<std::string> &model_prefixes,
                      const std::vector<std::string> &globfeat_prefixes,
                      const int camera_num,
                      const bool positive_region = true,
                      const int num_models_to_average = 50,
                      const int step_size = 3,

                      const int target_width = 300);

        void detectHead(std::vector<cv::Mat> &images, std::vector<cv::Mat> *head_mask);

        void calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights);

        void calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights, std::vector<int> *ranks);

    private:
        std::vector<HandDetector> hds_;
        int num_models_to_average_;
        int step_size_;
        bool positive_region_ = true;
        int model_num_;
    };

}


#endif //PROJECT_HEADDETECTION_H
