//
// Created by kei666 on 18/08/27.
//

#ifndef PROJECT_DISTANCE_H
#define PROJECT_DISTANCE_H

#include <opencv2/opencv.hpp>

namespace sw {

enum Distance {
    SSD,
    SAD,
    NCC,
    ZNCC,
};

cv::Mat difference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask, int param = SSD);

cv::Mat sumSquaredDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask);

/**
 * 計算量が少ない（メリット）
 * 外れ値の影響を受けにくい（メリット）
 * 証明の影響をかなり受けやすい（デメリット）
 * 最小値が最も類似する部分
 **/
cv::Mat sumAbsoluteDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask);

/**
 * 証明の影響を受けにくい（メリット）
 * 計算量が多い（デメリット）
 * -1.0~1.0の値をとり,最大値が最も類似する部分
 * SSD,SADに合わせ最小値が最も類似する部分にする予定
 **/
cv::Mat normalizedCrossCorrelation(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask);

/**
 * NCCよりも照明変化に対してロバスト
 **/
cv::Mat zeroMeansNormalizedCrossCorrelation(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask);

cv::Mat sumSquaredAverageDifference(const cv::Mat &img1, const cv::Mat &img2, const cv::Size &size, const cv::Mat &mask);

}

#endif //PROJECT_DISTANCE_H
