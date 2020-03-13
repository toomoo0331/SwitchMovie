//
// Created by kei666 on 20/1/28.
//

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

int main(int argc, char **argv) {

    cv::VideoCapture pcaps("/home/kei666/CLionProjects/SwitchMovie/data/20190505.mp4");
    std::vector<std::unique_ptr<cv::VideoWriter>> pwriter;
    cv::Mat tomo;
    pcaps >> tomo;
    int w = tomo.cols;
    int h = tomo.rows;

    std::string folder_name="/home/kei666/tomo/lstm-cnn/data/20190505-0128/";
    int video_num=5;
    for (int i = 0; i < video_num; i++) {
        std::string save_name = folder_name + std::to_string(i) + ".mp4";

        pwriter.push_back(std::unique_ptr<cv::VideoWriter>(new cv::VideoWriter(save_name,
                                                                               cv::VideoWriter::fourcc('M', 'P', '4',
                                                                                                       'V'),
                                                                               30,
                                                                               cv::Size(640, 540),
                                                                               true)));
        if (!pcaps.isOpened()) {
            return EXIT_FAILURE;
        }
    }

    int a[5] = {0, 640, 1280, 0, 640};
    int b[5] = {0, 0, 0, 540, 540};
    int k = 0;

    std::ofstream csv_file;
    csv_file.open(folder_name+ "head.csv");

    while (true) {
        cv::Mat frame;
        pcaps >> frame;
        cv::imshow("all", frame);
        std::cout << std::setfill(' ') << std::setw(5) << std::left << int(k / 1800)
                  << std::setfill(' ') << std::setw(5) << std::right << int(k / 30) - int(k / 1800) * 60 << " time";
        std::cout << "\r";
        if (frame.cols == w && frame.rows == h) {
            for (int i = 0; i < video_num; i++) {
                cv::Mat f0(frame, cv::Rect(a[i], b[i], 640, 540));
                *pwriter[i] << f0;
            }
        }
        k++;
        int key = cv::waitKey(0);
        if (key == 'q'||frame.empty()) {
            break;
        }
        key=key-49;
        csv_file << 0<<","<<key<<std::endl;
        cv::Mat f0(frame, cv::Rect(a[key], b[key], 640, 540));
        cv::imshow(std::to_string(key), f0);
        //cv::imshow("movie",frame);
    }


    return false;
}