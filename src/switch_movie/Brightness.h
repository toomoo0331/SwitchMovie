//
// Created by kei666 on 19/11/01.
//

#ifndef PROJECT_BRIGHTNESS_H
#define PROJECT_BRIGHTNESS_H

#include <opencv2/opencv.hpp>


namespace sw {

    class Brightness {
    public:
        Brightness(const std::vector<std::string> &model_prefixes,
                   const std::vector<std::string> &globfeat_prefixes,
                   const int camera_num,
                   const bool positive_region = true,
                   const int num_models_to_average = 50,
                   const int step_size = 3,

                   const int target_width = 300);

        void calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights);

        void calculateWeights(std::vector<cv::Mat> &images, std::vector<float> *weights, std::vector<int> *ranks);

    private:
        int num_models_to_average_;
        int step_size_;
        bool positive_region_ = true;
        int model_num_;
    };

}


#endif //PROJECT_BRIGHTNESS_H
