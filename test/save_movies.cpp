//
// Created by kei666 on 18/08/08
//
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

    int _fourcc = fourcc(video_extension);
    if (_fourcc < 0) {
        return EXIT_FAILURE;
    }

    std::vector<std::unique_ptr<cv::VideoCapture>> pcaps;
    std::vector<std::unique_ptr<cv::VideoWriter>> pwriter;

    for (int i = 0; i < video_num; i++) {
        pcaps.push_back(std::unique_ptr<cv::VideoCapture>(new cv::VideoCapture(i)));
        std::string save_name =  video_name + std::to_string(i) + "." + video_extension;

        pwriter.push_back(std::unique_ptr<cv::VideoWriter>(new cv::VideoWriter(save_name,
                                                                               cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                                                               pcaps[0]->get(CV_CAP_PROP_FPS),
                                                                               cv::Size(pcaps[i]->get(CV_CAP_PROP_FRAME_WIDTH), pcaps[i]->get(CV_CAP_PROP_FRAME_HEIGHT)),
                                                                               true)));
        if (!pcaps[i]->isOpened()) {
            return EXIT_FAILURE;
        }
    }

    for (;;)
    {
        for (int i = 0; i < video_num; i++) {
            cv::Mat frame;
            *pcaps[i] >> frame;
            *pwriter[i] << frame;
            cv::imshow(std::to_string(i), frame);
        }
        int key = cv::waitKey(1);
        if(key == 'q') {
            break;
        }
    }
    return false;
}