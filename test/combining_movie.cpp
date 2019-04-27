//
// Created by kei666 on 18/09/21.
//
#include <iostream>
#include <memory>

#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {
    if (argc < 3) {
        return EXIT_FAILURE;
    }

    int video_num = argc - 2;

    std::vector<std::unique_ptr<cv::VideoCapture>> pcaps;

    std::cout << "input_movie = ";
    for (int i = 1; i <= video_num; i++) {
        std::string input_path = argv[i];
        std::cout << input_path << ", ";
        pcaps.push_back(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(input_path)));
    }
    std::cout << std::endl;

    int s_num = (int)sqrt(video_num) + ((sqrt(video_num) > (int)sqrt(video_num)) ? 1 : 0);
    int v_num = (int)sqrt(video_num) + ((sqrt(video_num) > (int)sqrt(video_num)) ? 1 : 0);

    if (s_num * (v_num - 1) > video_num) v_num--;

    int video_len = pcaps[0]->get(CV_CAP_PROP_FRAME_COUNT);
    int video_height = pcaps[0]->get(CV_CAP_PROP_FRAME_HEIGHT);
    int video_width = pcaps[0]->get(CV_CAP_PROP_FRAME_WIDTH);
    std::string w_input = argv[argc - 1];
    std::cout << "save_path = " << w_input << std::endl;
    cv::VideoWriter writer(argv[argc - 1],
                           CV_FOURCC('m', 'p', '4', 'v'),
                           pcaps[0]->get(CV_CAP_PROP_FPS),
                           cv::Size(video_width * s_num,
                                    video_height * v_num),
                           true);

//    cv::VideoWriter writer2("difference_scene" + std::string(argv[argc - 1]),
//                           CV_FOURCC('m', 'p', '4', 'v'),
//                           pcaps[0]->get(CV_CAP_PROP_FPS),
//                           cv::Size(video_width * s_num,
//                                    video_height * v_num),
//                           true);

    cv::Mat all = cv::Mat::zeros(video_height * v_num, video_width * s_num, CV_8UC3);
    for (int i = 0; i < video_len; i++) {
        std::vector<cv::Mat> images;
        for (int i = 0; i < video_num; i++) {
            cv::Mat image;
            *pcaps[i] >> image;
            images.push_back(image.clone());
            cv::Rect roi(video_width * (i % s_num), video_height * (i / v_num), video_width, video_height);
            cv::Mat roi_image = all(roi);
            image.copyTo(roi_image);
        }
//        int x = 0;
//        for (int p = 0; p < video_num; p++) {
//            for (int q = 0; q < p; q++) {
//                cv::Mat tmp = images[p] - images[q];
//                x += (int)cv::sum(tmp)[0];
//            }
//        }
//        if (x != 0) {
//            writer2 << all;
//        }

        writer << all;
    }

    return EXIT_SUCCESS;
}