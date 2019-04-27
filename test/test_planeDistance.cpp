//
// Created by kei666 on 18/08/01.
//

#include <iostream>

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include "switch_movie/PlaneDistance.h"

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("video,v", po::value<std::string>(), "video name.")
            ("intrinsic,i", po::value<std::string>(), "intrinsic yml file name.")
            ("extrinsic,x", po::value<std::string>(), "extrinsic yml file name.")
            ("video_num,n",po::value<int>(), "the number of video.")
            ("video_extension,e", po::value<std::string>(), "video extension.")
            ("save_video_path,s", po::value<std::string>(), "save video path")
            ("save_homography_video_path,p", po::value<std::string>(), "save homography video path");

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

    std::string video_name;
    int video_num;
    std::string video_extension;
    std::string intrinsic_name;
    std::string extrinsic_name;
    std::string save_video_path;
    std::string save_homography_video_path;
    if (vm.count("video") && vm.count("video_num") && vm.count("video_extension") && vm.count("intrinsic")
        && vm.count("extrinsic") && vm.count("save_video_path") && vm.count("save_homography_video_path")) {
        video_name                 = vm["video"].as<std::string>();
        video_num                  = vm["video_num"].as<int>();
        video_extension            = vm["video_extension"].as<std::string>();
        intrinsic_name             = vm["intrinsic"].as<std::string>();
        extrinsic_name             = vm["extrinsic"].as<std::string>();
        save_video_path            = vm["save_video_path"].as<std::string>();
        save_homography_video_path = vm["save_homography_video_path"].as<std::string>();

        std::cout << "video_name                 = " << video_name      << std::endl
                  << "video_num                  = " << video_num       << std::endl
                  << "video_extension            = " << video_extension << std::endl
                  << "intrinsic_name             = " << intrinsic_name  << std::endl
                  << "extrinsic_name             = " << extrinsic_name  << std::endl
                  << "save_video_path            = " << save_video_path << std::endl
                  << "save_homography_video_path = " << save_homography_video_path << std::endl;
    } else {
        std::cout << opt << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::unique_ptr<cv::VideoCapture>> pcaps;

    for (int i = 0; i < video_num; i++) {
        std::string input_path = video_name + std::to_string(i) + "." + video_extension;
        pcaps.push_back(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(input_path)));
    }

    cv::VideoWriter writer(save_video_path,
                           cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                           29.97,
                           cv::Size(pcaps[0]->get(cv::CAP_PROP_FRAME_WIDTH), pcaps[0]->get(cv::CAP_PROP_FRAME_HEIGHT)),
                           true);
    
    cv::VideoWriter homography_writer(save_homography_video_path,
                                      cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                      29.97,
                                      cv::Size(pcaps[0]->get(cv::CAP_PROP_FRAME_WIDTH), pcaps[0]->get(cv::CAP_PROP_FRAME_HEIGHT)),
                                      true);

    int video_len = pcaps[0]->get(cv::CAP_PROP_FRAME_COUNT);
    int video_height = pcaps[0]->get(cv::CAP_PROP_FRAME_HEIGHT);
    int video_width = pcaps[0]->get(cv::CAP_PROP_FRAME_WIDTH);
    int one_side = (int)sqrt(video_num) + ((sqrt(video_num) > (int)sqrt(video_num)) ? 1 : 0);

    std::vector<sw::Camera> cameras;
    for (int i = 0 ; i < video_num; i++) {
        std::string intrinsic_yml_name = intrinsic_name + std::to_string(i) + ".yml";
        cv::FileStorage ifs(intrinsic_yml_name, cv::FileStorage::READ);
        cv::Mat _camera_mtx, _camera_dist;
        ifs["mtx"] >> _camera_mtx;
        ifs["dist"] >> _camera_dist;
        ifs.release();
        std::string extrinsic_yml_name = extrinsic_name + std::to_string(i) + ".yml";
        cv::FileStorage efs(extrinsic_yml_name, cv::FileStorage::READ);
        cv::Mat _rmat, _tvec;
        efs["rmat"] >> _rmat;
        efs["tvec"] >> _tvec;
        efs.release();
        cameras.push_back(sw::Camera(_camera_mtx, _camera_dist, _rmat, _tvec, cv::Size(video_width, video_height)));
    }

    std::string window_name = "all images";
    cv::namedWindow(window_name);
    cv::Mat all_image = cv::Mat::zeros(video_height * one_side, video_width * one_side, CV_8UC3);

    bool status = false;
    sw::PlaneDistance pd(3, cameras);
    for (int vi = 0; vi < video_len; vi++) {
        std::vector<cv::Mat> images;
        for (int i = 0; i < video_num; i++) {
            cv::Mat image;
            *pcaps[i] >> image;
            images.push_back(image);
        }
        if (status == false) {
            for (int i = 0; i < video_num; i++) {
                cv::Rect roi(video_width * (i % one_side), video_height * (i / one_side), video_width, video_height);
                cv::Mat roi_image = all_image(roi);
                images[i].copyTo(roi_image);
            }
        } else {
            std::vector<int> idx;
            pd.calculateDistance(images, &idx);
            cv::imshow("result", images[idx[0]]);
            writer << images[idx[0]];
            homography_writer << pd.hographyImage(images[idx[0]], idx[0]);
            for (int i = 0; i < video_num; i++) {
                cv::Rect roi(video_width * (i % one_side), video_height * (i / one_side), video_width, video_height);
                cv::Mat roi_image = all_image(roi);
                images[idx[i]].copyTo(roi_image);
            }
        }
        cv::imshow(window_name, all_image);
        int k = cv::waitKey(100);
        if (vi == 0) {
            pd.ini(images, -150.0, 150.0, 5.0, sw::PLANE);
            std::vector<int> idx;
            pd.calculateDistance(images, &idx);
            writer << images[idx[0]];
            status = true;
        }

    }

    return EXIT_SUCCESS;
}