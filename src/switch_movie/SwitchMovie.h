//
// Created by kei666 on 18/11/14.
//

#ifndef PROJECT_SWITCHMOVIEV2_H
#define PROJECT_SWITCHMOVIEV2_H

#include <vector>
#include <map>
#include <functional>

#include "Graph.h"

namespace sw {

class SwitchMovie {
public:
    SwitchMovie(int video_num, float alpha = 0.5);
    SwitchMovie(int video_num, int frame_min = 30);
    
    std::function<void(const std::vector<float> &)> add;
    std::function<std::vector<int>()> optimisation;

    void add_weight(const std::vector<float> &values);
    void add_mintime(const std::vector<float> &values);

    std::vector<int> optimisation_weight();
    std::vector<int> optimisation_mintime();

    const int numVertex() const {
        return (int)desc_.size();
    }

private:
    Graph graph_;

    std::vector<std::vector<std::deque<float>>> weights_;//3重配列のイメージ

    float alpha_;
    int video_num_;
    int frame_min_;
    std::map<int, int> desc_;//２重配列
};

}


#endif //PROJECT_SWITCHMOVIEV2_H
