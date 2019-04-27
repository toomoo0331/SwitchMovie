//
// Created by kei666 on 18/07/16.
//
#include <math.h>

#include "detectPoints.h"

using namespace calibration;

void DetectPoints::setParam(detectPointsParam param)
{
    param_ = param;
}

void DetectPoints::setChessBoard(const ChessBoard &chess)
{
    chess_board_ = &chess;
}

bool DetectPoints::runUsingChessboard(const std::vector<cv::Mat> &images)
{
    if (!chess_board_) {
        return false;
    }

    int chessBoardFlags = cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE;
    for (int i = 0; i < images.size(); i++) {
        cv::Mat gray;
        cv::cvtColor(images[i], gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point2f> corners;
        bool is_find = cv::findChessboardCorners(gray, chess_board_->pattern_size, corners, chessBoardFlags);

        if (is_find) {
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
            for (int j = 0; j < corners.size(); j++) {
                points_.insert(i, j, corners[j]);
            }
        }
    }
    return true;
}

bool DetectPoints::runSelectPoints(const std::vector<cv::Mat> &images)
{
    const char* window_name = "images";
    cv::namedWindow(window_name);

    cv::setWindowProperty(window_name, cv::WND_PROP_AUTOSIZE, cv::WINDOW_AUTOSIZE);

    one_side_ = (int)sqrt(images.size()) + ((sqrt(images.size()) > (int)sqrt(images.size())) ? 1 : 0);
    cv::Size image_size = images[0].size();
    cv::Mat all_image(image_size.height * one_side_, image_size.width * one_side_, CV_8UC3);
    for (int i = 0; i < images.size(); i++) {
        cv::Rect roi(image_size.width * (i % one_side_),
                     image_size.height * (i / one_side_),
                     image_size.width,
                     image_size.height);
        cv::Mat roi_image = all_image(roi);
        images[i].copyTo(roi_image);
    }

    du_ = new DrawUtil(all_image, one_side_);
    cv::setMouseCallback(window_name, DetectPoints::mouse, this);

    for (;;) {
        cv::imshow(window_name, all_image);
        int k = cv::waitKey(1);
        if (k == 'q') {
            break;
        }
    }

    return true;
}

int DetectPoints::run(const std::vector<cv::Mat> &images)
{
    if (images.size() < 2) {
        printf("images.size < 2\n");
        return -1;
    }

    switch(param_) {
        case USING_CHESSBOARD:
            if (!runUsingChessboard(images)) {
                printf("Please set chess-board param.\n");
                return -1;
            }
            break;
        case SELECT_POINTS:
            runSelectPoints(images);
            break;
        default:
            printf("Please set detect-points-param.\n");
            return -1;
    }
    return points_.trackNum();
}

void DetectPoints::mouse(int event, int x, int y, int flags, void *param)
{
    DetectPoints *dp = static_cast<DetectPoints*>(param);
    switch(event){
        case cv::EVENT_RBUTTONUP: {
            int image_id = y / (dp->du_->imageSize().height / dp->one_side_) * dp->one_side_
                           + x / (dp->du_->imageSize().width / dp->one_side_);
            int track_id = dp->points_.getImageTrackNum(image_id) - 1;
            dp->points_.remove(image_id, track_id);
            break;
        }
        case cv::EVENT_LBUTTONUP: {
            int image_id = y / (dp->du_->imageSize().height / dp->one_side_) * dp->one_side_
                           + x / (dp->du_->imageSize().width / dp->one_side_);
            int track_id = dp->points_.getImageTrackNum(image_id);
            cv::Point2f point(x - (dp->du_->imageSize().width / dp->one_side_) * (image_id % dp->one_side_),
                              y - (dp->du_->imageSize().height / dp->one_side_) * (image_id / dp->one_side_));
            dp->points_.insert(image_id, track_id, point);
            break;
        }
        default:
            break;
    }
    switch (flags) {
        case cv::EVENT_FLAG_LBUTTON:
            dp->du_->drawNumber(dp->points_);
            dp->du_->zoomImage(x, y, cv::Size(23, 23));
            break;
        default:
            dp->du_->drawNumber(dp->points_);
            break;
    }
}