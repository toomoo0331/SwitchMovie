//
// Created by kei666 on 18/10/26.
//

#ifndef PROJECT_VISUALIZER_H
#define PROJECT_VISUALIZER_H

#include "opencv2/opencv.hpp"

namespace util {

cv::Mat makeRedImage(const std::vector<cv::Mat> &images, int draw_num);

cv::Mat makeRedImageWithNumber(const std::vector<cv::Mat> &images, const std::vector<int> &ranks, int draw_num);

}

#endif //PROJECT_VISUALIZER_H
