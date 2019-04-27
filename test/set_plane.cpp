//
// Created by kei666 on 8/17/18.
//

#include <iostream>
#include <algorithm>

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgproc/types_c.h>

void drawAxis(cv::Mat &image, const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs,
              const cv::Mat &rvec, const cv::Mat &tvec, const int size = 100)
{
    std::vector<cv::Point3f> point3f;
    point3f.push_back(cv::Point3f(0, 0, 0));
    point3f.push_back(cv::Point3f(size, 0, 0));
    point3f.push_back(cv::Point3f(0, size, 0));
    point3f.push_back(cv::Point3f(0, 0, size));

    std::vector<cv::Point2f> point2f;
    cv::projectPoints(point3f, rvec, tvec, camera_matrix, dist_coeffs, point2f);

    cv::line(image, point2f[0], point2f[1], cv::Scalar(0, 0, 255), 3, cv::LINE_AA);
    cv::line(image, point2f[0], point2f[2], cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
    cv::line(image, point2f[0], point2f[3], cv::Scalar(255, 0, 0), 3, cv::LINE_AA);
}

int main(int argc, char **argv) {
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("video,v", po::value<std::string>(), "video name.")
            ("yml,y", po::value<std::string>(), "intrinsic yml file name.")
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
    std::string yml_file_name;
    int video_num;
    std::string video_extension;
    std::string save_file_name;
    if (vm.count("video") && vm.count("yml") && vm.count("video_num") && vm.count("video_extension") && vm.count("save")) {
        video_name = vm["video"].as<std::string>();
        yml_file_name = vm["yml"].as<std::string>();
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

    std::vector<cv::Point3f> obj;
    for(int j=0;j< horizonalCrossCount * verticalCrossCount;j++) {
        obj.push_back(cv::Point3f(j / horizonalCrossCount * chessSize, j % horizonalCrossCount * chessSize, 0.0f));
    }
    cv::Size chessboardPatterns(horizonalCrossCount, verticalCrossCount);

    std::vector<cv::Mat> camera_matrixes, dist_coeffes, rvecs, tvecs;
    for (int i = 0; i < video_num; i++) {
        std::string yml_name = yml_file_name + std::to_string(i) + ".yml";
        cv::FileStorage rfs(yml_name, cv::FileStorage::READ);
        cv::Mat _camera_mtx, _camera_dist;
        rfs["mtx"] >> _camera_mtx;
        rfs["dist"] >> _camera_dist;
        rfs.release();
        camera_matrixes.push_back(_camera_mtx);
        dist_coeffes.push_back(_camera_dist);
    }

    rvecs.resize(video_num);
    tvecs.resize(video_num);
    for (int vi = 0; vi < video_len; vi++) {
        bool check = true;
        for (int i = 0; i < video_num; i++) {
            cv::Mat image;
            *pcaps[i] >> image;
            cv::Mat gray;

            cv::cvtColor(image, gray, CV_RGB2GRAY);
            std::vector<cv::Point2f> centers;
            bool found = findChessboardCorners(gray, chessboardPatterns, centers,
                                               cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            check &= found;
            if (found) {
                cv::cornerSubPix(gray, centers, cv::Size(11, 11), cv::Size(-1, -1),
                                 cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
                cv::Mat rvec, tvec;
                cv::solvePnP(obj, centers, camera_matrixes[i], dist_coeffes[i], rvec, tvec);
                rvecs[i] = rvec;
                tvecs[i] = tvec;
                // draw
                cv::drawChessboardCorners(image, chessboardPatterns, cv::Mat(centers), true);
                drawAxis(image, camera_matrixes[i], dist_coeffes[i], rvec, tvec, 100);
            }
            cv::Rect roi(video_width * (i % one_side), video_height * (i / one_side), video_width, video_height);
            cv::Mat roi_image = all_image(roi);
            image.copyTo(roi_image);
        }
        cv::Mat r_all;
        cv::resize(all_image, r_all, cv::Size(), 0.5, 0.5);
        cv::imshow(window_name, r_all);
        int k = cv::waitKey(1);
        if (k == 'q' && check) {
            for (int i = 0; i < video_num; i++) {
                std::string save_name = save_file_name + std::to_string(i) + ".yml";
                cv::FileStorage fs(save_name, cv::FileStorage::WRITE);
                cv::Mat rmat;
                cv::Rodrigues(rvecs[i], rmat);
                fs << "rvec" << rvecs[i];
                fs << "rmat" << rmat;
                fs << "tvec" << tvecs[i];
                 fs.release();
            }
             break;
        }
    }

    return EXIT_SUCCESS;
}

