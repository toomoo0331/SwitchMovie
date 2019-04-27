//
// Created by kei666 on 18/07/16.
//

#ifndef PROJECT_DETECTPOINTS_H
#define PROJECT_DETECTPOINTS_H

#include <vector>

#include <opencv2/opencv.hpp>

#include "points.h"
#include "drawUtil.h"

namespace calibration {

enum detectPointsParam {
    USING_CHESSBOARD,
    SELECT_POINTS
};

struct ChessBoard{
    cv::Size pattern_size;
    cv::Size chess_size;
};

class DetectPoints {
public:
    DetectPoints(){};
    DetectPoints(detectPointsParam param) : param_(param) {};
    DetectPoints(detectPointsParam param, const ChessBoard &chess) : param_(param), chess_board_(&chess) {};

    ~DetectPoints() {
        delete(du_);
    };

    void setParam(detectPointsParam param);
    void setChessBoard(const ChessBoard &chess);

    bool runUsingChessboard(const std::vector<cv::Mat> &images);
    bool runSelectPoints(const std::vector<cv::Mat> &images);
    int run(const std::vector<cv::Mat> &images);

    int detectSuccessImageNum() {
        return points_.imageNum();
    };

    const Points & getPoints() const {
        return points_;
    }

private:
    static void mouse(int event, int x, int y, int flags, void *param);

    detectPointsParam param_ = USING_CHESSBOARD;
    Points points_;
    const ChessBoard* chess_board_;
    DrawUtil *du_;
    int one_side_ = 1;
};

}


#endif //PROJECT_DETECTPOINTS_H
