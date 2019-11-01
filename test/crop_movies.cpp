//
// Created by kei666 on 19/04/23.
//

#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

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

    cv::VideoCapture pcaps("/home/kei666/CLionProjects/SwitchMovie/data/20190505b.mp4");
    std::vector<std::unique_ptr<cv::VideoWriter>> pwriter;
    cv::Mat tomo;
    pcaps>>tomo;
    int w = tomo.cols;
    int h = tomo.rows;

    for (int i = 0; i < video_num; i++) {
        std::string save_name =  video_name + std::to_string(i) + "." + video_extension;

        pwriter.push_back(std::unique_ptr<cv::VideoWriter>(new cv::VideoWriter(save_name,
                                                                               cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                                                                               30,
                                                                               cv::Size(640, 540),
                                                                               true)));
        if (!pcaps.isOpened()) {
            return EXIT_FAILURE;
        }
    }
    int a[5]={0,640,1280,0,640};
    int b[5]={0,0,0,540,540};
    int k =0;
    while(true) {
        cv::Mat frame;
        pcaps >> frame;
        std::cout << std::setfill(' ') << std::setw(5) << std::left << int(k/1800)
                  << std::setfill(' ') << std::setw(5) << std::right << int(k/30)-int(k/1800)*60<< " time";
        std::cout << "\r";
        if (1800<k&&k<18000)
        {
            if(frame.cols==w && frame.rows == h) {
                for (int i = 0; i < video_num; i++) {
                    cv::Mat f0(frame, cv::Rect(a[i], b[i], 640, 540));
                    *pwriter[i] << f0;
                    //cv::imshow(std::to_string(i), f0);
                }
//        if (1800<k&&k<3600)
//        {
//            if (k%5==0) {
//                if (frame.cols == w && frame.rows == h) {
//                    for (int i = 0; i < video_num; i++) {
//                        cv::Mat f0(frame, cv::Rect(a[i], b[i], 525, 525));
//                        *pwriter[i] << f0;
//                        //cv::imshow(std::to_string(i), f0);
//                    }
//                }
//            }
//        }

//        if(1500<k&&k<16500) {
//            if (frame.cols == w && frame.rows == h) {
//                for (int i = 0; i < video_num; i++) {
//                    cv::Mat f0(frame, cv::Rect(a[i], b[i], 525, 525));
//                    *pwriter[i] << f0;
//                    //cv::imshow(std::to_string(i), f0);
//                }
//            }
//        }
        }
        k++;
        int key = cv::waitKey(1);
        //cv::imshow("movie",frame);
        if(key == 'q' || k > 18000) {
            break;
        }
    }

    return false;
}