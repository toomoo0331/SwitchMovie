//
// Created by kei666 on 18/08/01.
//

#ifndef PROJECT_PLANEDISTANCE_H
#define PROJECT_PLANEDISTANCE_H

#include <vector>
#include <map>
#include<memory>

#include <opencv2/opencv.hpp>

#include "difference.h"

namespace sw {

struct Camera
{
    cv::Mat camera_matrix;
    cv::Mat dist_coeffes;
    cv::Mat rmat;
    cv::Mat tvec;
    cv::Size size;
    Camera(cv::Mat &_camera_matrix, cv::Mat &_dist_coeffes, cv::Mat &_rmat, cv::Mat &_tvec, cv::Size _size) :
            camera_matrix(_camera_matrix), dist_coeffes(_dist_coeffes), rmat(_rmat), tvec(_tvec), size(_size){}
};

enum Base {
    STANDARDIMAGE,
    PLANE,
};

class PlaneDistance {
public:
    PlaneDistance(std::vector<Camera> &cameras)
            : cameras_(cameras) {};
    PlaneDistance(const int standard, std::vector<Camera> &cameras)
            : standard_(standard), cameras_(cameras) {};
    PlaneDistance(std::vector<Camera> &cameras, const std::vector<cv::Mat> &inputs, double min, double max, double quantization_width, int param = STANDARDIMAGE)
            : cameras_(cameras) {ini(inputs, min, max, quantization_width, param);};
    PlaneDistance(const int standard, std::vector<Camera> &cameras, const std::vector<cv::Mat> &inputs, double min, double max, double quantization_width, int param = STANDARDIMAGE)
            : standard_(standard), cameras_(cameras) {ini(inputs, min, max, quantization_width, param);};
    ~PlaneDistance(){};

    bool changeStandard(const int standard) {
        if (0 <= standard && standard < cameras_.size()) {
            standard_ = standard;
            return true;
        }
        return false;
    }

    void ini(const std::vector<cv::Mat> &inputs, double min, double max, double quantization_width, int param = STANDARDIMAGE);
    cv::Mat hographyImage(const cv::Mat &input, const int video_num);
    std::vector<double> calculateDistance(const std::vector<cv::Mat> &images, int vi);
    std::vector<double> calculateDistance(const std::vector<cv::Mat> &images, std::vector<int> *ranks, int vi);


private:
    void sumDistance(std::vector<std::vector<cv::Mat>> &images, const cv::Size &size, int param = SSD);

    template <class T>
    void hash(const std::vector<cv::Mat> &homograpy_images, double* values);

    cv::Mat estimatePlaneBasedHomography(int camera, double length);
    cv::Mat estimateStandardImageBasedHomography(int camera, double length);

    int standard_ = 0;
    cv::Mat mask_;

    const std::vector<Camera> cameras_;
    std::vector<cv::Mat> homographies_;
};

}

#endif //PROJECT_PLANEDISTANCE_H
