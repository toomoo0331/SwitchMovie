//
// Created by kei666 on 19/07/09.
//

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <opencv2/core/core.hpp>

int main(int argc, char **argv)
{
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("video,v", po::value<std::string>(), "video name.")
            ("video_num,n",po::value<int>(), "the number of video.")
            ("video_extension,e", po::value<std::string>(), "video extension.");

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
    if (vm.count("video") && vm.count("video_num") && vm.count("video_extension")) {
        video_name = vm["video"].as<std::string>();
        video_num = vm["video_num"].as<int>();
        video_extension = vm["video_extension"].as<std::string>();

        std::cout << "video_name      = " << video_name      << std::endl
                  << "video_num       = " << video_num       << std::endl
                  << "video_extension = " << video_extension << std::endl;
    } else {
        std::cout << opt << std::endl;
        return EXIT_FAILURE;
    }

    cv::VideoCapture pcaps0("/home/kei666/CLionProjects/SwitchMovie/input/20190911/0.mp4");
    cv::VideoCapture pcaps1("/home/kei666/CLionProjects/SwitchMovie/input/20190911/1.mp4");
    cv::VideoCapture pcaps2("/home/kei666/CLionProjects/SwitchMovie/input/20190911/2.mp4");
    cv::VideoCapture pcaps3("/home/kei666/CLionProjects/SwitchMovie/input/20190911/3.mp4");
    cv::VideoCapture pcaps4("/home/kei666/CLionProjects/SwitchMovie/input/20190911/4.mp4");
    std::vector<std::unique_ptr<cv::VideoWriter>> pwriter;
    int video_len = pcaps0.get(CV_CAP_PROP_FRAME_COUNT);
    int w = pcaps0.get(CV_CAP_PROP_FRAME_WIDTH);
    int h = pcaps0.get(CV_CAP_PROP_FRAME_HEIGHT);
    std::cout<<"asdafaf"<<std::endl;
    for (int i = 0; i < video_num; i++) {
        std::string save_name =  video_name+"rotate_"+std::to_string(i)+"." + video_extension;

        pwriter.push_back(std::unique_ptr<cv::VideoWriter>(new cv::VideoWriter(save_name,
                                                                               cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                                                                               30,
                                                                               cv::Size(540, 540),
                                                                               true)));
        if (!pcaps0.isOpened()) {
            return EXIT_FAILURE;
        }
    }
    double degree[5]={165,235,300,20,90};
    double scale[5] ={1,1,1,1,1};
//
//    double degree[5]={0,0,5,0,0};
//    double scale[5] ={1,1,1,1,1};
    double cols[5]={-4,-13,17,78,-10};
    double rows[5]={-20,35,28,7,-60};

    cv::Point2f center = cv::Point2f(
            static_cast<float>(w / 2),
            static_cast<float>(h / 2));

    cv::Mat frame0,frame1,frame2,frame3,frame4;
    std::vector<cv::Mat> frame;

    std::cout << std::endl << "save movie" << std::endl;
    std::cout << "0%        50%        100%" << std::endl;
    std::cout << "+-----------+-----------+" << std::endl;
    int percent = 0;
    for(int k=0;k<video_len;k++) {
        percent = k * 100 / video_len + 1;
        std::string progress = "";
        for (int j = 0; j < k * 100 / video_len / 4; j++) {
            progress += "#";
        }
        std::cout << std::setfill(' ') << std::setw(25) << std::left << progress
                  << std::setfill(' ') << std::setw(5) << std::right << percent << "%";
        std::cout << "\r";
        pcaps0 >> frame0;
        frame.push_back(frame0);
        pcaps1 >> frame1;
        frame.push_back(frame1);
        pcaps2 >> frame2;
        frame.push_back(frame2);
        pcaps3 >> frame3;
        frame.push_back(frame3);
        pcaps4 >> frame4;
        frame.push_back(frame4);

//        std::cout << std::setfill(' ') << std::setw(5) << std::left << int(k/1800)
//                  << std::setfill(' ') << std::setw(5) << std::right << int(k/30)-int(k/1800)*60<< " time";
//        std::cout << "\r";

        for (int i = 0; i < 5; i++) {
            cv::Mat affine,dest;
            std::string name = std::to_string(i);
            //cv::imshow(name,frame[i]);
            name += "affine";

            cv::Mat m2(cv::Size(3, 2), CV_32F, cv::Scalar::all(0));
            m2.at<float>(0,0)=1;
            m2.at<float>(0,1)=0;
            m2.at<float>(0,2)=cols[i];
            m2.at<float>(1,0)=0;
            m2.at<float>(1,1)=1;
            m2.at<float>(1,2)=rows[i];

            cv::getRotationMatrix2D(center, degree[i], scale[i]).copyTo(affine);

            cv::warpAffine(frame[i], dest, affine, frame[i].size(), cv::INTER_CUBIC);
            cv::warpAffine(dest, dest, m2, frame[i].size(), cv::INTER_CUBIC);
            *pwriter[i] << dest;
            //cv::imshow(name, dest);
        }


        int key = cv::waitKey(1);

        if(key == 'a') {
            cvWaitKey(0);
        }

        if(key == 'q') {
            break;
        }
        frame.clear();
    }

    return true;
}