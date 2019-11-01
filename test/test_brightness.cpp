//
// Created by kei666 on 19/05/08.
//

//
// Created by kei666 on 18/08/08
//
//

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, char **argv)
{



    std::vector<std::unique_ptr<cv::VideoCapture>> pcaps;

    for (int i = 0; i < 1; i++) {
        pcaps.push_back(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture("/home/kei666/CLionProjects/SwitchMovie/input/20190505_001/0.mp4")));


        if (!pcaps[i]->isOpened()) {
            return EXIT_FAILURE;
        }
    }

    for (;;)
    {
        int key;
        for (int i = 0; i < 13000; i++) {
            cv::Mat frame, hsv;
            float score;
            *pcaps[0] >> frame;

            key = cv::waitKey(1);
            float denominator = frame.rows * frame.cols * 255;
            cvtColor(frame, hsv, CV_BGR2HSV);
            score = (float) cv::sum(hsv)[2] / denominator;
            std::cout << "asdfa; " << score << std::endl;

            imshow("sow", frame);
        }
        if(key == 'q') {
            break;
        }
    }
    return false;
}