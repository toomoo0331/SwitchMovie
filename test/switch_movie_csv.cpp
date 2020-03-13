//
// Created by kei666 on 19/11/17.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

#include <tbb/parallel_for.h>
#include <boost/tokenizer.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

#include "switch_movie/SwitchMovie.h"
#include "util/visualizer.h"

namespace fs = boost::filesystem;

template <typename T>
bool makeCsv(const std::string csv_name, const std::vector<T> &tops, const std::vector<std::vector<int>> &top_ranks, const std::vector<std::vector<int>> &optimizes)
{
    if (top_ranks.size() != optimizes[0].size()) {
        return false;
    }
    std::ofstream csv_file;
    std::cout << csv_name << std::endl;
    csv_file.open(csv_name);

    csv_file << "topranks";
    for (int i = 0; i < tops.size(); i++) {
        csv_file << "," << tops[i];
    }
    csv_file << std::endl;

    for (int i = 0; i < top_ranks.size(); i++) {
        csv_file << top_ranks[i][0];
        for (int j = 0; j < optimizes.size(); j++) {
            csv_file << "," << optimizes[j][i];
        }
        csv_file << std::endl;
    }
    csv_file.close();

    return true;
}

void rotationImage(const cv::Mat &input, cv::Mat &output, int r) {
    cv::Point2f center(input.cols / 2, input.rows / 2);
    cv::Mat trans = cv::getRotationMatrix2D(center, r, 1);
    cv::warpAffine(input, output, trans, input.size());
}

int main()
{
    std::vector<int> rotation = {0,68,141,217,287};
    std::vector<int> times = {5};
    std::string root = "/home/kei666/CLionProjects/SwitchMovie/";
    std::string directory = "20190213/";
    int video_num = 5;
    std::string extension = "mp4";

    std::ifstream ifs(root + "output/" + directory + "head.csv");

    std::vector<cv::VideoCapture> caps;
    for (int i = 0; i < video_num; i++) {
        std::string str = root + "input/" + directory + std::to_string(i) + "." + extension;
        caps.push_back(cv::VideoCapture(str));
    }

//    std::vector<cv::VideoWriter> optimize_mov(times.size());
//    cv::VideoWriter toprank_mov;

    std::vector<cv::VideoWriter> optimize_red_mov(times.size());
    cv::VideoWriter toprank_red_mov;

    std::vector<sw::SwitchMovie> sm;
    for (int i = 0; i < times.size(); i++) {
        sm.push_back(sw::SwitchMovie(video_num, (int)(caps[0].get(CV_CAP_PROP_FPS) * times[i] + 0.5)));
    }
    std::string line;
    std::vector<std::vector<int>> rank_vecs;
    while (std::getline(ifs, line)) {
        std::vector<float> data;
        boost::tokenizer<boost::escaped_list_separator<char>> tokens(line);
        for (const std::string& token : tokens) {
            data.push_back((float)atof(token.c_str()));
        }
        for (int i = 0; i < times.size(); i++) {
            sm[i].add_mintime(data);
        }
        std::vector<int> ranks;
        ranks.resize(video_num);
        std::iota(ranks.begin(), ranks.end(), 0);
        std::sort(ranks.begin(), ranks.end(), [&data](int a, int b) -> bool {return data[a] < data[b];});
        rank_vecs.push_back(ranks);
    }
    ifs.close();

    std::vector<std::vector<int>> movie_nums;
    std::cout << "start optimizer" << std::endl;
    for (int i = 0; i < times.size(); i++) {
        movie_nums.push_back(sm[i].optimisation_mintime());
    }
    makeCsv<int>(root + "output/" + directory + "mintime_optimize.csv", times, rank_vecs, movie_nums);
    std::cout << "end optimizer" << std::endl;
    int video_len = caps[0].get(CV_CAP_PROP_FRAME_COUNT);

    std::cout << "0%        50%        100%" << std::endl;
    std::cout << "+-----------+-----------+" << std::endl;
    int percent = 0;

    for (int i = 0; i < video_len; i++) {
        percent = i * 100 / video_len + 1;
        std::string progress = "";
        for (int j = 0; j < i * 100 / video_len / 4; j++) {
            progress += "#";
        }
        std::cout << std::setfill(' ') << std::setw(25) << std::left << progress
                  << std::setfill(' ') << std::setw(5) << std::right << percent << "%";
        std::cout << "\r";

        std::vector<cv::Mat> images;
        for (int c = 0; c < video_num; c++) {
            cv::Mat image;
            caps[c] >> image;
            cv::Mat top_dst;
            rotationImage(image, top_dst, rotation[c]);
//            images.push_back(image);
            images.push_back(top_dst);
        }

//        if(!toprank_mov.isOpened())
//        {
//            std::stringstream ss;
//            ss.str("");
//            ss << root + "output/" + directory + "top_ranking" + ".mov";
//            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
//            toprank_mov.open(ss.str(),fourcc, caps[0].get(CV_CAP_PROP_FPS),images[0].size(),true);
//        }
//        cv::Mat top_dst;
//        rotationImage(images[rank_vecs[i][0]], top_dst, rotation[rank_vecs[i][0]]);
//        toprank_mov << images[rank_vecs[i][0]];
//        toprank_mov << top_dst;

        cv::Mat all_top = util::makeRedImageWithNumber(images, rank_vecs[i], rank_vecs[i][0]);
        if(!toprank_red_mov.isOpened())
        {
            std::stringstream ss;
            ss.str("");
            ss << root + "output/" + directory + "top_ranking_red" + ".mov";
            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
            toprank_red_mov.open(ss.str(),fourcc, caps[0].get(CV_CAP_PROP_FPS),all_top.size(),true);
        }
        toprank_red_mov << all_top;

        tbb::parallel_for(tbb::blocked_range<int>(0, times.size()),
                          [&root, &directory, &times, &images, &rank_vecs, &movie_nums, i, /*&optimize_mov,*/ &optimize_red_mov, &caps, &rotation]
                                  (const tbb::blocked_range<int>& range)
                          {
                              for (int s = range.begin(); s != range.end(); s++) {
//                                  if (!optimize_mov[s].isOpened()) {
//                                      std::stringstream ss;
//                                      ss.str("");
//                                      ss << root + "output/" + directory + "mintime_optimize" + std::to_string(times[s]) + ".mov";
//                                      int fourcc = CV_FOURCC('m', 'p', '4', 'v');
//                                      optimize_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS),
//                                                           images[movie_nums[s][i]].size(), true);
//                                  }
//                                  cv::Mat optimize_dst;
//                                  rotationImage(images[movie_nums[s][i]], optimize_dst, rotation[movie_nums[s][i]]);
//                                  optimize_mov[s] << images[movie_nums[s][i]];
//                                  optimize_mov[s] << optimize_dst;
                                  cv::Mat all = util::makeRedImageWithNumber(images, rank_vecs[i], movie_nums[s][i]);
                                  if (!optimize_red_mov[s].isOpened()) {
                                      std::stringstream ss;
                                      ss.str("");
                                      ss << root + "output/" + directory + "mintime_optimize" + std::to_string(times[s]) + "_red.mov";
                                      int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                                      optimize_red_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS), all.size(), true);
                                  }
                                  optimize_red_mov[s] << all;
                              }
                          });
    }


    return EXIT_SUCCESS;
}