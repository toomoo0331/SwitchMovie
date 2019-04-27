//
// Created by kei666 on 18/11/14.
//

#include <iostream>
#include "SwitchMovie.h"

using namespace sw;

SwitchMovie::SwitchMovie(int video_num, float alpha) : video_num_(video_num), alpha_(alpha), graph_(Graph(video_num))
{
    desc_[0] = graph_.addVertex();
    weights_.resize(video_num_);
    for (int i = 0; i < video_num_; i++) {
        weights_[i].resize(video_num_);
    }
    add = std::bind(&sw::SwitchMovie::add_weight, this, std::placeholders::_1);
    optimisation = std::bind(&sw::SwitchMovie::optimisation_weight, this);
}

SwitchMovie::SwitchMovie(int video_num, int frame_min) : video_num_(video_num), frame_min_(frame_min), graph_(Graph(video_num))
{
    desc_[0] = graph_.addVertex();
    weights_.resize(video_num_);
    for (int i = 0; i < video_num_; i++) {
        weights_[i].resize(video_num_);
    }
    add = std::bind(&sw::SwitchMovie::add_mintime, this, std::placeholders::_1);
    optimisation = std::bind(&sw::SwitchMovie::optimisation_mintime, this);
}

void SwitchMovie::add_weight(const std::vector<float> &values)
{
    //
    bool flag = false;
    int size = desc_.size();
    if (size > 1) {
        flag = true;
    }
    for (int i = 0; i < values.size(); i++) {
        desc_[desc_.size()] = graph_.addVertex();//頂点を増やす
        if (flag == true) {
            for (int j = 0; j < values.size(); j++) {
                //すべてのノードに自身のノードを重み付きでつなげる
                float weight = alpha_ * values[i];
                if (j != i) {
                    weight += (1.0 - alpha_);
                }
                graph_.addEdge(desc_[size - values.size() + j],  desc_[desc_.size() - 1], weight);
            }
        } else {
            graph_.addEdge(desc_[0], desc_[desc_.size() - 1], alpha_ * values[i]);
        }
    }
}

void SwitchMovie::add_mintime(const std::vector<float> &values)
{
    //最小フレーム先にエッジをつなげる処理
    // つなげる先のカメラ番号のその前最小フレーム分のスコアの合計値をエッジの重みとしている．
    //weight_は３重のvector，自身のカメラ番号，接続先のカメラ番号，重み
    //自身と最小フレーム数前のカメラノードをつなげるイメージ
    bool flag = false;
    int size = desc_.size();
    if (size > 1) {
        flag = true;
    }
    for (int i = 0; i < values.size(); i++) {
        desc_[desc_.size()] = graph_.addVertex();
        float weight = values[i];
        if (flag == true) {
            for (int j = 0; j < values.size(); j++) {
                if (j == i) {
                    //addedge_の引数は，　１フレーム前のカメラ番号，現在（重みを持っている）のカメラ番号，重み　である．
                    graph_.addEdge(desc_[size - values.size() + j], desc_[desc_.size() - 1], weight);
                } else {
                    //毎フレーム重みを足していく
                    //最小フレーム分足し終わったらエッジに加えていく
                    //最初フレーム分のサイズがweights_[i][j]にあり，それぞれが最小フレーム数〜１まで重みを足した数となっている．
                    for (std::deque<float>::iterator it = weights_[i][j].begin(); it != weights_[i][j].end(); it++) {
                        //for(int k = 0;k < weights_[i][j].size();k++)
                            //weights[i][j][k] += weight;
                        *it += weight;
                    }

                    weights_[i][j].push_back(weight);
                    if (weights_[i][j].size() >= frame_min_) {//はじめの状態から最小フレーム数を超えたら
                        if (size > video_num_ * frame_min_ * 2 - video_num_) {//
                            //desc_のイメージは，pythonの辞書的なもので，毎フレームvalue.size()分（５つ分）増えていく
                            //例えば，13ー>3，14->4的な感じ．１０フレーム前の3のカメラ番号を知りたかった場合，sizeからその分だけ引けばいい
                            graph_.addEdge(desc_[size - values.size() + j - (frame_min_ - 1) * video_num_],
                                            desc_[desc_.size() - 1], weights_[i][j].front());
                        }
                        weights_[i][j].pop_front();
                    }
                }
            }
        } else {
            //初期フレームのときの処理
            graph_.addEdge(desc_[0], desc_[desc_.size() - 1], weight);
        }
    }
}

std::vector<int> SwitchMovie::optimisation_weight()
{
    desc_[desc_.size()] = graph_.addVertex();
    for (int i = 0; i < video_num_; i++) {
        //最後のフレームをendにつなげる
        graph_.addEdge(desc_[desc_.size() - 1 - video_num_ + i],  desc_[desc_.size() - 1], 0.0);
    }


    const int from = 0; // start
    const int to = desc_.size() - 1; // end

    desc_.clear();

    std::vector<int> route = graph_.shortestPath(from, to);

    return route;
}

std::vector<int> SwitchMovie::optimisation_mintime()
{
    //最適化を行う
    //．最小フレームを足す
    std::vector<int> result = optimisation_weight();
    int tmp = result[0];
    for (std::vector<int>::iterator it = result.begin(); it != result.end(); it++) {
        if (tmp != *it) {
            it = result.insert(it, frame_min_ - 1, *it);
            it++;
        }
        tmp = *it;
    }

    return result;
}