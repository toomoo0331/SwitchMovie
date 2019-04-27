//
// Created by kei666 on 18/08/27.
//

#include "difference.h"

cv::Mat sw::difference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask, int param)
{
    cv::Mat (*fpFunc)(const cv::Mat&, const cv::Mat&, const cv::Size&, const cv::Mat &);
    switch (param) {
        case SSD:
            fpFunc = sumSquaredDifference;
            break;
        case SAD:
            fpFunc = sumAbsoluteDifference;
            break;
        case NCC:
            fpFunc = normalizedCrossCorrelation;
            break;
        case ZNCC:
            fpFunc = zeroMeansNormalizedCrossCorrelation;
            break;
        default:
            fpFunc = sumSquaredAverageDifference;
            break;
    }
    return (*fpFunc)(img1, img2, size, mask);
}

cv::Mat sw::sumSquaredDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask)
{
    cv::Mat result = cv::Mat::ones(img1.rows, img1.cols, CV_64F);
    result.forEach<double>([&img1, &img2, &size](double &p, const int position[2]) -> void {
        if (position[0] > (size.height / 2 - 1) && position[1] > (size.width / 2 - 1) &&
                position[0] < (img1.rows - size.height / 2) && position[1] < (img1.cols - size.width / 2)) {
            p = 0;
            for (int y = 0; y < size.height; y++) {
                for (int x = 0; x < size.width; x++) {
                    p += pow(double(img1.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x] -
                                         img2.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x]), 2.0)
                         / (255 * 255 * size.width * size.height);
                }
            }
        }
    });
    return result;
}

cv::Mat sw::sumAbsoluteDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask)
{
    cv::Mat result = cv::Mat::ones(img1.rows, img1.cols, CV_64F);
    result.forEach<double>([&img1, &img2, &mask, &size](double &p, const int position[2]) -> void {
        if (position[0] > (size.height / 2 - 1) && position[1] > (size.width / 2 - 1) &&
            position[0] < (img1.rows - size.height / 2) && position[1] < (img1.cols - size.width / 2) &&
            mask.ptr<uchar>(position[0])[position[1]] > 0) {
            p = 0;
            for (int y = 0; y < size.height; y++) {
                for (int x = 0; x < size.width; x++) {
                    p += fabs(double(img1.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x] -
                                    img2.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x]))
                            / (255 * size.width * size.height);
                }
            }
        }
    });
    return result;
}

cv::Mat sw::normalizedCrossCorrelation(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask)
{
    cv::Mat result = cv::Mat::ones(img1.rows, img1.cols, CV_64F);
    result.forEach<double>([&img1, &img2, &size](double &p, const int position[2]) -> void {
        if (position[0] > (size.height / 2 - 1) && position[1] > (size.width / 2 - 1) &&
            position[0] < (img1.rows - size.height / 2) && position[1] < (img1.cols - size.width / 2)) {
            double num = 0.0, den1 = 0.0, den2 = 0.0;
            for (int y = 0; y < size.height; y++) {
                for (int x = 0; x < size.width; x++) {
                    num += img1.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x] *
                           img2.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x];
                    den1 += pow(double(img1.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x]), 2.0);
                    den2 += pow(double(img2.ptr<uchar>(position[0] - size.height / 2 + y)[position[1] - size.width / 2 + x]), 2.0);
                }
            }
            p = (-num / (sqrt(den1) * sqrt(den2)) + 1) / 2;
        }
    });
    return result;
}

cv::Mat sw::zeroMeansNormalizedCrossCorrelation(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask)
{
    cv::Mat result = cv::Mat::ones(img1.rows, img1.cols, CV_64F);
    return result;
}

cv::Mat sw::sumSquaredAverageDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask)
{
    cv::Mat result = cv::Mat::ones(img1.rows, img1.cols, CV_16S);
    for (int y = 0; y < result.rows; y += size.height) {
        for (int x = 0; x < result.cols; x += size.width) {
            uchar img1_sum = 0, img2_sum = 0;
            for (int i = 0; i < size.height; i++) {
                for (int j = 0; j < size.width; j++) {
                    img1_sum += (int)img1.ptr<uchar>(y + i)[x + j];
                    img2_sum += (int)img2.ptr<uchar>(y + i)[x + j];
                }
            }
            img1_sum /= (size.height * size.width);
            img2_sum /= (size.height * size.width);
            cv::Rect roi(x, y, size.width, size.height);
            result(roi) = cv::Mat::ones(size.height, size.width, CV_16S) * abs(img1_sum - img2_sum);
        }
    }
    return result;
}