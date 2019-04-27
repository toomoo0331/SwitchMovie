//
// Created by kei666 on 18/10/26.
//

#include "visualizer.h"

cv::Mat util::makeRedImage(const std::vector <cv::Mat> &images, int draw_num) {
    //対象画像以外を赤色にした上で，すべての画像を並べてallにする．
    int s_num = (int)sqrt(images.size()) + ((sqrt(images.size())) > (int)sqrt(images.size()) ? 1 : 0);
    int v_num = s_num;
    if (v_num * (s_num - 1) >= images.size()) s_num--;

    cv::Mat all = cv::Mat::zeros(images[0].rows * s_num, images[0].cols * v_num, CV_8UC3);
    for (int i = 0; i < images.size(); i++) {
        cv::Mat image;
        if (i == draw_num) {
            image = images[i].clone();
        } else {
            std::vector<cv::Mat> planes;
            cv::split(images[i], planes);
            std::vector<cv::Mat> mats;
            cv::Mat black = cv::Mat::zeros(cv::Size(images[i].cols, images[i].rows), CV_8U);
            mats.push_back(black.clone());
            mats.push_back(black.clone());
            mats.push_back(planes[2]);
            cv::merge(mats, image);
        }

        cv::Rect roi(images[i].cols * (i % v_num), images[i].rows * (i / v_num), images[i].cols, images[i].rows);
        image.copyTo(all(roi));
    }

    return all;
}

cv::Mat util::makeRedImageWithNumber(const std::vector<cv::Mat> &images, const std::vector<int> &ranks, int draw_num)
{
    //画像を並べて一つのMaｔにする
    int s_num = (int)sqrt(images.size()) + ((sqrt(images.size())) > (int)sqrt(images.size()) ? 1 : 0);
    int v_num = s_num;
    if (v_num * (s_num - 1) >= images.size()) s_num--;
    cv::Mat all = makeRedImage(images, draw_num);

    for (int i = 0; i < ranks.size(); i++) {
        cv::putText(all, std::to_string(i + 1),
                    cv::Point(images[ranks[i]].cols * (ranks[i] % v_num),
                              images[ranks[i]].rows * (ranks[i] / v_num) + images[ranks[i]].rows),
                    cv::FONT_HERSHEY_PLAIN, 2,
                    cv::Scalar(181, 203, 128),
                    2, CV_AA);
    }

    return all;
}