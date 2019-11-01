#include <stdio.h>
#include <stdlib.h>


#include <iomanip>
#include <iostream>

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <tbb/parallel_for.h>
#include <switch_movie/Brightness.h>

#include "util/DataLoader.h"
#include "util/visualizer.h"
#include "switch_movie/HeadDetectors.h"
#include "switch_movie/SwitchMovie.h"

template <typename T>
bool makeCsv(const std::string csv_name, const std::vector<T> &tops, const std::vector<std::vector<int>> &top_ranks, const std::vector<std::vector<int>> &optimizes)
{
    if (top_ranks.size() != optimizes[0].size()) {
        return false;
    }
    std::ofstream csv_file;
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

int main(int argc, char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("config,c", po::value<std::string>()->default_value("/home/kei666/CLionProjects/SwitchMovie/config.ini"), "config ini file");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, opt), vm);
    } catch (const po::error_with_option_name &e) {
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help")) {
        std::cout << opt << std::endl;
        return EXIT_SUCCESS;
    }

    std::string config;
    if(vm.count("config")) {
        config = vm["config"].as<std::string>();
    } else {
        std::cout << opt << std::endl;
        return EXIT_FAILURE;
    }

    //configファイルを読みこむ
    util::DataLoader dl(config);

    //動画を読み込み，vectorに格納する
    std::vector<cv::VideoCapture> caps;
    for (const auto &str : dl.movie_pathes_) {
        caps.push_back(cv::VideoCapture(str));
    }

    //動画をスイッチングするためのクラスに格納する．グラフのノードを作るイメージ
    //paramater_sizeとmovie_numは同じサイズ1
    int paramater_size;
    std::vector<sw::SwitchMovie> sms;
    if (dl.mode_ == 0) {
        paramater_size = dl.alphas_.size();
        for (const float a : dl.alphas_) {
            sms.push_back(sw::SwitchMovie(dl.movie_num_, a));
        }
    } else if (dl.mode_ == 1){
        paramater_size = dl.mintimes_.size();
        for (const int m : dl.mintimes_) {
            sms.push_back(sw::SwitchMovie(dl.movie_num_, m * (int)caps[0].get(CV_CAP_PROP_FPS)));
        }
    }else {
        return EXIT_FAILURE;
    }

    //スイッチングの際にダイクストラ法を考慮し，スイッチングを行うVideoWriter
    std::vector<cv::VideoWriter> optimize_mov(paramater_size);
    //スイッチングの際に何も考慮せず，ただスコアの高い順にするためのVideoWriter
    cv::VideoWriter toprank_mov;

    //すべての画像を表示し，スコアが最もいいもの(ダイクストラ法)以外を赤色にして動画に保存するためのVideoWriter
    std::vector<cv::VideoWriter> optimize_red_mov(paramater_size);
    //すべての画像を表示し，スコアが最もいいもの以外を赤色にして動画に保存するためのVideoWriter
    cv::VideoWriter toprank_red_mov;

//    //フレームのスコアを計算するためのHeadDetectorのインスタンスを生成する．
//    sw::HeadDetectors hd(dl.models_directories_, dl.globfeat_directories_, dl.movie_num_, dl.positive_region_,
//                         dl.num_models_to_average_, dl.step_size_, dl.target_width_);


    sw::Brightness bn(dl.models_directories_, dl.globfeat_directories_, dl.movie_num_, dl.positive_region_,
                         dl.num_models_to_average_, dl.step_size_, dl.target_width_);

    int video_len = caps[0].get(CV_CAP_PROP_FRAME_COUNT);
    //全フレームのランクを格納しておくためのvector
    std::vector<std::vector<int>> rank_vecs;
    std::cout << "0%        50%        100%" << std::endl;
    std::cout << "+-----------+-----------+" << std::endl;
    int percent = 0;
    std::ofstream csv_file;
    csv_file.open(dl.output_directory_ + "head.csv");
    //グラフを作成する
    for (int i = 0; i < video_len; i++) {
        percent = i * 100 / video_len + 1;
        std::string progress = "";
        for (int j = 0; j < i * 100 / video_len / 4; j++) {
            progress += "#";
        }
        std::cout <<std::setfill(' ') << std::setw(25) << std::left << progress
                  << std::setfill(' ') << std::setw(5) << std::right << percent << "%";
        std::cout << "\r";
        std::vector<cv::Mat> images(dl.movie_num_);
        //すべてのフレーム（５つ分）をimagesに格納する
        for (int c = 0; c < dl.movie_num_; c++) {
            cv::Mat image;
            caps[c] >> image;
            images[c] = image.clone();
        }
        std::vector<int> ranks;
        std::vector<float> weights;
//        hd.calculateWeights(images, &weights, &ranks);//それぞれの重みを計算し，ランク付けも行う
        bn.calculateWeights(images, &weights, &ranks);//それぞれの重みを計算し，ランク付けも行う
        rank_vecs.push_back(ranks);//現在のフレームのランクを格納する
        for (int j = 0; j < paramater_size; j++) {
            //グラフを作成する（ノードをつなげる）
            if (dl.mode_ == 0)
                sms[j].add_weight(weights);
            else if (dl.mode_ == 1)
                sms[j].add_mintime(weights);
        }
        csv_file << weights[0];
        for (int c = 1; c < dl.movie_num_; c++) {
            csv_file << "," << weights[c];
        }
        csv_file << std::endl;
//        //visualization
//        std::vector<cv::Mat> masks;
//        hd.detectHead(images, &masks);
//        for (int c = 0; c < dl.movie_num_; c++) {
//            masks[c] *= 255;
//            cv::cvtColor(masks[c], masks[c], CV_GRAY2BGR);
//            cv::resize(masks[c], masks[c], images[c].size(), 0, 0, cv::INTER_LINEAR);
//            cv::addWeighted(images[c], 0.7, masks[c], 0.3, 0, masks[c]);
//            cv::imshow(std::to_string(c), masks[c]);
//        }
//        cv::waitKey(1);
    }

    //グラフのダイクストラ法を用いた最適化を行う
    csv_file.close();
    std::cout << std::endl;
    std::vector<std::vector<int>> movie_nums;
    std::cout << "start optimizer" << std::endl;
    for (int i = 0; i < paramater_size; i++) {
        if (dl.mode_ == 0){
            movie_nums.push_back(sms[i].optimisation_weight());
        }else if (dl.mode_ == 1) {
            movie_nums.push_back(sms[i].optimisation_mintime());
        }
    }

    std::cout << "end optimizer" << std::endl;

    if (dl.mode_ == 0) {
        makeCsv<float>(dl.output_directory_ + "weight_optimize.csv", dl.alphas_, rank_vecs, movie_nums);
    } else if (dl.mode_ == 1) {
        makeCsv<int>(dl.output_directory_ + "mintime_optimize.csv", dl.mintimes_, rank_vecs, movie_nums);
    }

    for (int i = 0; i < dl.movie_num_; i++) {
        caps[i].set(CV_CAP_PROP_POS_AVI_RATIO, 0);
    }

    std::cout << std::endl << "save movie" << std::endl;
    std::cout << "0%        50%        100%" << std::endl;
    std::cout << "+-----------+-----------+" << std::endl;
    percent = 0;
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
        for (int c = 0; c < dl.movie_num_; c++) {
            cv::Mat image;
            caps[c] >> image;
            images.push_back(image);
        }

        //rank_vecsはスコア順にカメラ番号が並んでいるため，0が最も高い最適なカメラ番号となる．
        //save top_rank
        if(!toprank_mov.isOpened())
        {
            std::stringstream ss;
            ss.str("");
            ss << dl.output_directory_ + "top_ranking" + "_mouth.mov";
            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
            toprank_mov.open(ss.str(),fourcc, caps[0].get(CV_CAP_PROP_FPS),images[0].size(),true);
        }
        toprank_mov << images[rank_vecs[i][0]];

        //save top_rank_red
        cv::Mat all_top = util::makeRedImageWithNumber(images, rank_vecs[i], rank_vecs[i][0]);
        if(!toprank_red_mov.isOpened())
        {
            std::stringstream ss;
            ss.str("");
            ss << dl.output_directory_ + "top_ranking_red" + "_mouth.mov";
            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
            toprank_red_mov.open(ss.str(),fourcc, caps[0].get(CV_CAP_PROP_FPS),all_top.size(),true);
        }
        toprank_red_mov << all_top;

        if (dl.mode_ == 0) {
            //並列処理のためにpararell_forで書いているが，イメージはfor(i=0;i<alpha.size():i++)を同時にやっているイメージ
            tbb::parallel_for(tbb::blocked_range<int>(0, dl.alphas_.size()),
                [&dl, &images, &rank_vecs, &movie_nums, i, &optimize_mov, &optimize_red_mov, &caps]
                        (const tbb::blocked_range<int>& range)
                {
                    for (int s = range.begin(); s != range.end(); s++) {
                        if (!optimize_mov[s].isOpened()) {
                            std::stringstream ss;
                            ss.str("");
                            ss << dl.output_directory_ + "optimize" + std::to_string(dl.alphas_[s]) + ".mov";
                            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                            optimize_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS),
                                                 images[movie_nums[s][i]].size(), true);
                        }
                        optimize_mov[s] << images[movie_nums[s][i]];

                        cv::Mat all = util::makeRedImageWithNumber(images, rank_vecs[i], movie_nums[s][i]);
                        if (!optimize_red_mov[s].isOpened()) {
                            std::stringstream ss;
                            ss.str("");
                            ss << dl.output_directory_ + "optimize" + std::to_string(dl.alphas_[s]) + "_red.mov";
                            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                            optimize_red_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS), all.size(), true);
                        }
                        optimize_red_mov[s] << all;
                    }
                });
        } else if (dl.mode_ == 1) {
            tbb::parallel_for(tbb::blocked_range<int>(0, dl.mintimes_.size()),
                [&dl, &images, &rank_vecs, &movie_nums, i, &optimize_mov, &optimize_red_mov, &caps]
                        (const tbb::blocked_range<int>& range)
                {
                    for (int s = range.begin(); s != range.end(); s++) {
                        if (!optimize_mov[s].isOpened()) {
                            std::stringstream ss;
                            ss.str("");
                            ss << dl.output_directory_ + "mintime_optimize" + std::to_string(dl.mintimes_[s]) + "_mouth.mov";
                            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                            optimize_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS),
                                                 images[movie_nums[s][i]].size(), true);
                        }
                        optimize_mov[s] << images[movie_nums[s][i]];

                        cv::Mat all = util::makeRedImageWithNumber(images, rank_vecs[i], movie_nums[s][i]);
                        if (!optimize_red_mov[s].isOpened()) {
                            std::stringstream ss;
                            ss.str("");
                            ss << dl.output_directory_ + "mintime_optimize" + std::to_string(dl.mintimes_[s]) + "_red_mouth.mov";
                            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                            optimize_red_mov[s].open(ss.str(), fourcc, caps[0].get(CV_CAP_PROP_FPS), all.size(), true);
                        }
                        optimize_red_mov[s] << all;
                    }
                });
        }
    }

    return EXIT_SUCCESS;
}