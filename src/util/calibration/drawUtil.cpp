//
// Created by kei666 on 18/07/17.
//

#include "drawUtil.h"

using namespace calibration;

void DrawUtil::zoomImage(const int x, const int y, const cv::Size &window, const float rate)
{
    cv::Mat image = *draw_image_;
    cv::Mat tmp_window = cv::Mat::zeros(window, CV_8UC3);

    cv::Rect tmp_window_roi = windowRoi(x, y, window);
    cv::Mat tmp_window_image = tmp_window(tmp_window_roi);

    cv::Rect image_roi = imageRoi(x, y, window);
    cv::Mat roi_image = image(image_roi);
    roi_image.copyTo(tmp_window_image);

    cv::Mat resize_tmp_window;
    cv::Size out_window(int(window.width * rate), int(window.height * rate));
    cv::resize(tmp_window, resize_tmp_window, out_window);

    cv::Rect resize_tmp_window_roi = windowRoi(x, y, out_window);
    cv::Mat resize_tmp_window_image = resize_tmp_window(resize_tmp_window_roi);
    cv::Rect resize_image_roi = imageRoi(x, y, out_window);
    cv::Mat resize_roi_image = image(resize_image_roi);
    resize_tmp_window_image.copyTo(resize_roi_image);
}

void DrawUtil::zoomImage(const cv::Point2i &point, const cv::Size &window, const float rate)
{
    zoomImage(point.x, point.y, window, rate);
}

void DrawUtil::drawNumber(const Points &points, const int radius, const float font_scale) {
    *draw_image_ = image_.clone();
    typename Points::const_iterator it;
    for (it = points.begin(); it != points.end(); ++it) {
        cv::Point2f point(it->second.x + (image_.cols / one_side_) * (it->first.first % one_side_),
                          it->second.y + (image_.rows / one_side_) * (it->first.first / one_side_));
        cv::circle(*draw_image_, point, radius, cv::Scalar(181, 203, 128), -1, cv::LINE_AA);
        cv::Point2f tp = point;
        cv::putText(*draw_image_, std::to_string(it->first.second), tp, cv::FONT_HERSHEY_PLAIN, font_scale,
                    cv::Scalar(181, 203, 128),
                    2, cv::LINE_AA);
    }
}

cv::Rect DrawUtil::windowRoi(const int x, const int y, const cv::Size &window)
{
    int roi_x = 0;
    int roi_y = 0;
    int roi_width = window.width;
    int roi_height = window.height;
    if ((x - window.width / 2) < 0) {
        roi_x = window.width / 2 - x;
        roi_width = window.width - window.width/ 2 + x;
    }
    if ((y - window.height / 2) < 0) {
        roi_y = window.height / 2 - y;
        roi_height = window.height - window.height/ 2 + y;
    }
    if ((image_.cols - x - window.width + window.width / 2)  < 0) {
        roi_width = window.width / 2 + (image_.cols - x);
    }
    if ((image_.rows - y - window.height + window.height / 2)  < 0) {
        roi_height = window.height / 2 + (image_.rows - y);
    }
    return cv::Rect(roi_x, roi_y, roi_width, roi_height);
}

cv::Rect DrawUtil::imageRoi(const int x, const int y, const cv::Size &window)
{
    int roi_x = x - window.width / 2;
    int roi_y = y - window.height / 2;
    int roi_width = window.width;
    int roi_height = window.height;
    if ((x - window.width / 2) < 0) {
        roi_x = 0;
        roi_width = window.width - window.width/ 2 + x;
    }
    if ((y - window.height / 2) < 0) {
        roi_y = 0;
        roi_height = window.height - window.height/ 2 + y;
    }
    if ((image_.cols - x - window.width + window.width / 2)  < 0) {
        roi_width = window.width / 2 + (image_.cols - x);
    }
    if ((image_.rows - y - window.height + window.height / 2)  < 0) {
        roi_height = window.height / 2 + (image_.rows - y );
    }
    return cv::Rect(roi_x, roi_y, roi_width, roi_height);
}
