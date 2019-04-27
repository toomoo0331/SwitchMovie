//
// Created by kei666 on 18/08/01.
//

#include <algorithm>

#include <opencv2/img_hash.hpp>

#include "PlaneDistance.h"

using namespace sw;

void PlaneDistance::ini(const std::vector<cv::Mat> &inputs, double min, double max, double quantization_width, int param)
{
    double minimized_sum = 1000 * cameras_.size() * cameras_.size();
    for (double j = (int)min; j < max; j+= quantization_width) {
        cv::Mat mask = cv::Mat::ones(cameras_[0].size, CV_8U);
        std::vector<cv::Mat> homo_images;
        std::vector<cv::Mat> homographies;
        for (int i = 0; i < cameras_.size(); i++) {
            cv::Mat homography;
            switch (param) {
                case STANDARDIMAGE:
                    homography = estimateStandardImageBasedHomography(i, j).clone();
                    break;
                case PLANE:
                    homography = estimatePlaneBasedHomography(i, min + j).clone();
                    break;
                default:
                    homography = estimateStandardImageBasedHomography(i, j).clone();
            }
            cv::Mat homo_image = cv::Mat::zeros(inputs[standard_].rows, inputs[standard_].cols, CV_8UC3);
            cv::warpPerspective(inputs[i],
                                homo_image,
                                homography,
                                homo_image.size());
            homo_images.push_back(homo_image);
            homographies.push_back(homography);

            cv::Mat tmp_mask = cv::Mat::zeros(cameras_[i].size, CV_8U);
            cv::Mat tmp = cv::Mat::ones(cameras_[i].size, CV_8U);
            cv::warpPerspective(tmp,
                                tmp_mask,
                                homography,
                                tmp_mask.size());
            mask &= tmp_mask;
        }

        std::vector<cv::Mat> homo_mask_images(homo_images.size());
        for (int i = 0; i < cameras_.size(); i++) {
            cv::Mat homo_mask_image;
            homo_images[i].copyTo(homo_mask_image, mask);
            homo_mask_images[i] = homo_mask_image;
        }

        double* values = new double[cameras_.size() * cameras_.size()];
        hash<cv::img_hash::PHash>(homo_mask_images, values);
        double sum = 0;
        for (int i = 0; i < cameras_.size(); i++) {
            for (int k = 0; k < i; k++) {
                sum += values[cameras_.size() * i + k];
            }
        }

        if (sum < minimized_sum) {
            minimized_sum = sum;
            mask_ = mask.clone();
            homographies_.clear();
            homographies_.resize(cameras_.size());
            for (int i = 0; i < cameras_.size(); i++) {
                homographies_[i] = homographies[i].clone();
            }
        }
        delete[] values;
    }
}

cv::Mat PlaneDistance::hographyImage(const cv::Mat &input, const int video_num)
{
    cv::Mat homo_image = cv::Mat::zeros(cameras_[standard_].size, CV_8UC3);
    cv::warpPerspective(input,
                        homo_image,
                        homographies_[video_num],
                        homo_image.size());
    return homo_image;
}

std::vector<double> PlaneDistance::calculateDistance(const std::vector<cv::Mat> &images, int vi)
{
    std::vector<cv::Mat> homography_images(cameras_.size());

    for (int i = 0; i < images.size(); i++) {
        cv::Mat homo_image = hographyImage(images[i], i);

        cv::Mat homo_mask_image;
        homo_image.copyTo(homo_mask_image, mask_);
        homography_images[i] = homo_mask_image.clone();
        cv::imwrite("../../output/homography_images/camera" + std::to_string(i) + "/" + std::to_string(vi) + ".png", homo_mask_image);
    }
    double* values = new double[cameras_.size() * cameras_.size()];
    hash<cv::img_hash::PHash>(homography_images, values);

    std::vector<double> sum_values(cameras_.size(), 0.0);
    for (int i = 0; i < cameras_.size(); i++) {
        for (int j = 0; j < i; j++) {
            sum_values[i] += values[cameras_.size() * i + j];
            sum_values[j] += values[cameras_.size() * i + j];
        }
    }
    delete[] values;
    return sum_values;
}

std::vector<double> PlaneDistance::calculateDistance(const std::vector<cv::Mat> &images, std::vector<int> *ranks, int vi)
{
    std::vector<double> sum_values = calculateDistance(images, vi);

    ranks->resize(images.size());
    std::iota(ranks->begin(), ranks->end(), 0);
    std::sort(ranks->begin(), ranks->end(), [&sum_values](int a, int b) -> bool {return sum_values[a] < sum_values[b];});

    return sum_values;
}

//void PlaneDistance::sumDistance(std::vector<std::vector<cv::Mat>> &images, const cv::Size &size, int param)
//{
//    std::vector<std::vector<cv::Mat>> differences(cameras_.size());
//    for (int i = 0; i < images.size(); i++) {
//        for (int j = 0; j < images[i].size(); j++) {
//            cv::Mat result = difference(images[0][0], images[i][j], size, layers_[j].mask, param);
//            result.convertTo(result, CV_8U, 255);
//            cv::imshow(std::to_string(i) + std::to_string(j), result);
//        }
//    }
//    cv::waitKey(0);
//}

template <class T>
void PlaneDistance::hash(const std::vector<cv::Mat> &homograpy_images, double* values)
{
    cv::Mat hashA, hashB;
    cv::Ptr<cv::img_hash::ImgHashBase> func = T::create();
    for (int i = 0; i < homograpy_images.size(); i++) {
        for (int j = 0; j < i; j++) {
            cv::Mat hashA, hashB;
            func->compute(homograpy_images[i], hashA);
            func->compute(homograpy_images[j], hashB);
            values[homograpy_images.size() * i + j] = func->compare(hashA, hashB);
            values[homograpy_images.size() * j + i] = func->compare(hashA, hashB);
        }
    }
}

cv::Mat PlaneDistance::estimatePlaneBasedHomography(int camera, double length)
{
    cv::Mat Rt_standard = cv::Mat::eye(4, 4, CV_64F);
    cv::Rect Rroi(0, 0, 3, 3);
    cv::Rect troi(3, 0, 1, 3);
    cameras_[standard_].rmat.copyTo(Rt_standard(Rroi));
    cameras_[standard_].tvec.copyTo(Rt_standard(troi));

    cv::Rect roi(2, 0, 1, 3);
    cv::Mat n = -cameras_[camera].rmat(roi);
    cv::Mat Rt_i = cv::Mat::eye(4, 4, CV_64F);
    cameras_[camera].rmat.copyTo(Rt_i(Rroi));
    cameras_[camera].tvec.copyTo(Rt_i(troi));
    cv::Mat Rt = Rt_standard * Rt_i.inv();

    double h = cameras_[camera].tvec.dot(n) + length;

    return cameras_[standard_].camera_matrix *
                         (Rt(Rroi) + ((Rt(troi) * n.t()) / h)) * cameras_[camera].camera_matrix.inv();

}

cv::Mat PlaneDistance::estimateStandardImageBasedHomography(int camera, double length)
{
    cv::Mat Rt_standard = cv::Mat::eye(4, 4, CV_64F);
    cv::Rect Rroi(0, 0, 3, 3);
    cv::Rect troi(3, 0, 1, 3);
    cameras_[standard_].rmat.copyTo(Rt_standard(Rroi));
    cameras_[standard_].tvec.copyTo(Rt_standard(troi));

    cv::Rect roi(2, 0, 1, 3);
    cv::Mat Rt_i = cv::Mat::eye(4, 4, CV_64F);
    cameras_[camera].rmat.copyTo(Rt_i(Rroi));
    cameras_[camera].tvec.copyTo(Rt_i(troi));
    cv::Mat Rt = Rt_standard * Rt_i.inv();
    cv::Mat Rt_inv = Rt.inv();

    cv::Mat n_i = Rt_inv(Rroi)(roi);

    double h = length + Rt_inv(troi).dot(n_i);

    return cameras_[standard_].camera_matrix * (Rt(Rroi) + ((Rt(troi) * n_i.t()) / h)) * cameras_[camera].camera_matrix.inv();
}