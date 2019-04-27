//
// Created by kei666 on 8/16/18.
//

#include <iostream>
#include <algorithm>

#include <tbb/parallel_for.h>
#include <tbb/combinable.h>
#include <tbb/blocked_range.h>

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>


void reduceSize(std::vector<std::vector<cv::Point3f>> *points3f, std::vector<std::vector<cv::Point2f>> *points2f, int size = 100)
{
     std::vector<std::vector<cv::Point3f>> rp3;
     std::vector<std::vector<cv::Point2f>> rp2;

     int step = ((points3f->size() / size) >= 1.0) ? (int)points3f->size() / size: 1;

     for (int i = 0; i < points3f->size(); i += step) {
         rp3.push_back(points3f->at(i));
         rp2.push_back(points2f->at(i));
     }

     points3f->clear();
     points2f->clear();
     std::copy(rp3.begin(), rp3.end(), std::back_inserter(*points3f));
     std::copy(rp2.begin(), rp2.end(), std::back_inserter(*points2f));
}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("video,v", po::value<std::string>(), "video name.")
            ("video_num,n", po::value<int>(), "the number of video.")
            ("video_extension,e", po::value<std::string>(), "video extension.")
            ("save,s", po::value<std::string>(), "save file name.");

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
    std::string save_file_name;
    if (vm.count("video") && vm.count("video_num") && vm.count("video_extension") && vm.count("save")) {
        video_name = vm["video"].as<std::string>();
        video_num = vm["video_num"].as<int>();
        video_extension = vm["video_extension"].as<std::string>();
        save_file_name = vm["save"].as<std::string>();

        std::cout << "video_name      = " << video_name << std::endl
                  << "video_num       = " << video_num << std::endl
                  << "video_extension = " << video_extension << std::endl
                  << "save file name  = " << save_file_name << std::endl;
    } else {
        std::cout << opt << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<std::unique_ptr<cv::VideoCapture>> pcaps;

    for (int i = 0; i < video_num; i++) {
        std::string input_path = video_name + std::to_string(i) + "." + video_extension;
        std::cout << input_path << std::endl;
        pcaps.push_back(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(input_path)));
    }

    int video_len = pcaps[0]->get(CV_CAP_PROP_FRAME_COUNT);
    int video_height = pcaps[0]->get(CV_CAP_PROP_FRAME_HEIGHT);
    int video_width = pcaps[0]->get(CV_CAP_PROP_FRAME_WIDTH);

    int one_side = (int)sqrt(video_num) + ((sqrt(video_num) > (int)sqrt(video_num)) ? 1 : 0);
    std::string window_name = "all images";
    cv::namedWindow(window_name);
    cv::Mat all_image = cv::Mat::zeros(video_height * one_side, video_width * one_side, CV_8UC3);

    int horizonalCrossCount = 9;
    int verticalCrossCount = 6;
    float chessSize = 26.0;

    std::vector<std::vector<std::vector<cv::Point2f>>> points2f;
    std::vector<std::vector<std::vector<cv::Point3f>>> points3f;
    points2f.resize(video_num);
    points3f.resize(video_num);
    std::vector<cv::Point3f> obj;
    for(int j=0;j< horizonalCrossCount * verticalCrossCount;j++) {
        obj.push_back(cv::Point3f(j / horizonalCrossCount * chessSize, j % horizonalCrossCount * chessSize, 0.0f));
    }
    cv::Size chessboardPatterns(horizonalCrossCount, verticalCrossCount);
    for (int vi = 0; vi < video_len; vi++) {
        tbb::parallel_for(tbb::blocked_range<int>(0, video_num),
                [&pcaps, &all_image, &points3f, &points2f, &obj, &chessboardPatterns, video_width, video_height, one_side]
                        (const tbb::blocked_range<int>& range) {
                    for (int i = range.begin(); i != range.end(); i++) {
                        cv::Mat image;
                        *pcaps[i] >> image;
                        cv::Mat gray;

                        cv::cvtColor(image, gray, CV_RGB2GRAY);
                        std::vector<cv::Point2f> centers;
                        bool found = findChessboardCorners(gray, chessboardPatterns, centers,
                                                           cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
                        if (found) {
                            cv::cornerSubPix(gray, centers, cv::Size(11, 11), cv::Size(-1, -1),
                                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
                            points3f[i].push_back(obj);
                            points2f[i].push_back(centers);

                            // draw
                            cv::drawChessboardCorners(image, chessboardPatterns, cv::Mat(centers), true);
                        }
                        cv::Rect roi(video_width * (i % one_side), video_height * (i / one_side), video_width, video_height);
                        cv::Mat roi_image = all_image(roi);
                        image.copyTo(roi_image);
                    }
                });
//        for (int i = 0; i < video_num; i++) {
//            cv::Mat image;
//            *pcaps[i] >> image;
//            cv::Mat gray;
//
//            cv::cvtColor(image, gray, CV_RGB2GRAY);
//            std::vector<cv::Point2f> centers;
//            bool found = findChessboardCorners(gray, chessboardPatterns, centers,
//                    cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
//            if (found) {
//                cv::cornerSubPix(gray, centers, cv::Size(11, 11), cv::Size(-1, -1),
//                             cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
//                points3f[i].push_back(obj);
//                points2f[i].push_back(centers);
//
//                // draw
//                cv::drawChessboardCorners(image, chessboardPatterns, cv::Mat(centers), true);
//            }
//            cv::Rect roi(video_width * (i % one_side), video_height * (i / one_side), video_width, video_height);
//            cv::Mat roi_image = all_image(roi);
//            image.copyTo(roi_image);
//        }
        cv::Mat r_all;
        cv::resize(all_image, r_all, cv::Size(), 0.5, 0.5);
        cv::imshow(window_name, r_all);
        cv::waitKey(1);
    }

    tbb::parallel_for(tbb::blocked_range<int>(0, video_num),
                      [&points3f, &points2f, &save_file_name, video_width, video_height](const tbb::blocked_range<int>& range) {
                          for (int i = range.begin(); i != range.end(); i++) {
                              std::vector<cv::Mat> rvecs, tvecs;
                              cv::Mat mtx(3,3, CV_64F);
                              cv::Mat dist(8,1, CV_64F);
                              reduceSize(&points3f[i], &points2f[i], 100);
                              std::cout << points3f[i].size() << std::endl;
                              cv::calibrateCamera(points3f[i], points2f[i], cv::Size(video_width, video_height), mtx, dist, rvecs, tvecs);
                              std::string save_name = save_file_name + std::to_string(i) + ".yml";
                              cv::FileStorage fs(save_name, cv::FileStorage::WRITE);
                              fs << "mtx" << mtx;
                              fs << "dist" << dist;
                              fs.release();
                          }
                      });
//    for (int i = 0; i < video_num; i++) {
//        std::vector<cv::Mat> rvecs, tvecs;
//        cv::Mat mtx(3,3, CV_64F);
//        cv::Mat dist(8,1, CV_64F);
//        std::cout << points3f[i].size() << std::endl;
//        reduceSize(&points3f[i], &points2f[i], 100);
//        std::cout << points3f[i].size() << std::endl;
//        cv::calibrateCamera(points3f[i], points2f[i], cv::Size(video_width, video_height), mtx, dist, rvecs, tvecs);
//        std::string save_name = save_file_name + std::to_string(i) + ".yml";
//        cv::FileStorage fs(save_name, cv::FileStorage::WRITE);
//        fs << "mtx" << mtx;
//        fs << "dist" << dist;
//        fs.release();
//    }

    return EXIT_SUCCESS;
}