//
// Created by kei666 on 18/07/17.
//

#ifndef PROJECT_DRAWUTIL_H
#define PROJECT_DRAWUTIL_H

#include <opencv2/opencv.hpp>

#include "points.h"

namespace calibration {

class DrawUtil {
public:
    DrawUtil(cv::Mat &image) : draw_image_(&image), image_(image.clone()){};
    DrawUtil(cv::Mat &image, int one_side) : draw_image_(&image), image_(image.clone()), one_side_(one_side){};
    ~DrawUtil(){};

    void zoomImage(const int x, const int y, const cv::Size &window, const float rate = 5.0);
    void zoomImage(const cv::Point2i &point, const cv::Size &window, const float rate = 5.0);
    void drawNumber(const Points &points, const int radius = 2, const float font_scale = 1.2);
    cv::Size imageSize()const {
        return cv::Size(image_.cols, image_.rows);
    };
private:
    cv::Rect windowRoi(const int x, const int y, const cv::Size &window);
    cv::Rect imageRoi(const int x, const int y, const cv::Size &window);

    cv::Mat* draw_image_;
    cv::Mat image_;
    int one_side_ = 1;
};

}


#endif //PROJECT_DRAWUTIL_H
