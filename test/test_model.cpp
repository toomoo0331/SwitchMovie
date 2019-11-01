//
// Created by kei666 on 18/10/12.
//

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include "handtracker/HandDetector.hpp"

#include "util/DataLoader.h"

int main(int argc, char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
             ("config,c", po::value<std::string>()->default_value("/home/kei666/CLionProjects/SwitchMovie/config_test.ini"), "config ini file");

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

    std::string feature_set = "rvl";

    std::string model_prefix		= dl.models_directories_[0];			// output path for learned models
    std::string globfeat_prefix  = dl.globfeat_directories_[0];

    std::cout << "Testing...\n";

    HandDetector hd;
    hd.testInitialize(model_prefix,globfeat_prefix,feature_set,dl.num_models_to_average_,dl.target_width_);

    cv::VideoCapture cap;
    cap = cv::VideoCapture(dl.movie_pathes_[0]);

    cv::Mat im;
    cv::Mat ppr;

    cv::VideoWriter avi;
    cv::VideoWriter avi2;

    int fps = (int)cap.get(CV_CAP_PROP_FPS);
    std::cout << fps << std::endl;
    while(1) {
        cap >> (im);

        if (!im.data) break;
        //cap >> im; if(!im.data) break; // skip frames with these
        //cap >> im; if(!im.data) break;
        //cap >> im; if(!im.data) break;

        hd.test(im, dl.num_models_to_average_, dl.step_size_);


        // Different ways to visualize the results
        // hd._response_img (float probabilities in a matrix)
        // hd._blur (blurred version of _response_img)


        int SHOW_RAW_PROBABILITY = 1;
        if (SHOW_RAW_PROBABILITY) {
            cv::Mat raw_prob;
            hd.colormap(hd._response_img, raw_prob, 0);
            cv::imshow("probability", raw_prob);    // color map of probability
        }

        int SHOW_BLUR_PROBABILITY = 1;
        if (SHOW_BLUR_PROBABILITY) {
            cv::Mat pp_res;
            hd.postprocess(hd._response_img);
            imshow("blurred", hd._blu);        // colormap of blurred probability
        }

        int SHOW_BINARY_CONTOUR = 1;
        if (SHOW_BINARY_CONTOUR) {
            cv::Mat pp_contour = hd.postprocess(hd._response_img);// binary contour
            hd.colormap(pp_contour, pp_contour, 0);                    // colormap of contour
            cv::imshow("contour", pp_contour);

            if (!avi2.isOpened()) {
                std::stringstream ss;
                ss.str("");
                ss << dl.root_ + "/output/vis/" + dl.target_ + "/" + dl.basename_[0] + "0.mov";
                int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                //avi2.open(ss.str(),fourcc,cap.get(CV_CAP_PROP_FPS),pp_contour.size(),true);
            }
            //avi2 << pp_contour;
        }

        int SHOW_RES_ALPHA_BLEND = 1;
        if (SHOW_RES_ALPHA_BLEND) {
            cv::Mat pp_res = hd.postprocess(hd._response_img);
            hd.colormap(pp_res, pp_res, 0);
            resize(pp_res, pp_res, im.size(), 0, 0, cv::INTER_LINEAR);
            addWeighted(im, 0.7, pp_res, 0.3, 0, pp_res);                // alpha blend of image and binary contour
            imshow("alpha_res", pp_res);
            ppr = pp_res.clone();
        }

        if (!avi.isOpened()) {
            std::stringstream ss;
            ss.str("");
            ss << dl.root_ + "/output/vis/" + dl.target_ + "/" + dl.basename_[0] + ".mov";
//                ss << root + "/output/vis/" + "20190213_4" + ".mov";
            int fourcc = CV_FOURCC('m', 'p', '4', 'v');
                //avi.open(ss.str(),fourcc,cap.get(CV_CAP_PROP_FPS),ppr.size(),true);
        }
            //avi << ppr;

        int k = cv::waitKey(1);
        if (k == 'q') {
            break;
        }
        if (k == 's') {
            int current_frame = (int) cap.get(CV_CAP_PROP_POS_FRAMES);
            cap.set(CV_CAP_PROP_POS_FRAMES, current_frame + fps * 60 * 5);
        }

    }
    return EXIT_SUCCESS;
}