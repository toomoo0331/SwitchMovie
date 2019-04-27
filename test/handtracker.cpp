//
// Created by kei666 on 18/10/12.
//

#include <boost/program_options.hpp>

#include <opencv2/opencv.hpp>

#include "handtracker/HandDetector.hpp"

int main(int argc, char* const argv[])
{
    namespace po = boost::program_options;

    po::options_description opt("");

    opt.add_options()
            ("help,h", "HELP")
            ("train,t", po::value<int>(), "train")
            ("root,r", po::value<std::string>()->default_value("/home/kei666/CLionProjects/SwitchMovie/"), "root path")
            ("basename,b", po::value<std::string>()->default_value("Mouth"), "base name")
            ("image_width,w", po::value<int>()->default_value(300), "for resizing the input (small is faster)")
            ("num_models_to_average,a", po::value<int>()->default_value(20), "number of models used to compute a single pixel response. must be less than the number of training models. only used at test time")
            ("step_size,s", po::value<int>()->default_value(3), "runs detector on every 'step_size' pixels. bigger means faster but you lose resolution. you need post-processing to get contours. only used at test time.")
            ("input_video,v", po::value<std::string>()->default_value("/home/kei666/CLionProjects/SwitchMovie/input/20190423/0.mp4"), "input video path");

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

    std::string feature_set = "rvl";

    if (vm.count("train")) {
        std::string root, basename;
        int num_models_to_train, target_width;
        if (vm.count("root") && vm.count("basename") && vm.count("image_width")) {
            root = vm["root"].as<std::string>();
            basename = vm["basename"].as<std::string>();
            num_models_to_train = vm["train"].as<int>();
            target_width = vm["image_width"].as<int>();
        } else {
            std::cout << opt << std::endl;
            return EXIT_FAILURE;
        }
        std::string img_prefix		= root + "data/img"	+ basename + "/";			// color images
        std::string msk_prefix		= root + "data/mask"     + basename + "/";			// binary masks
        std::string model_prefix		= root + "models/"	+ basename + "/";			// output path for learned models
        std::string globfeat_prefix  = root + "globfeat/" + basename + "/";

        std::cout << "Training...\n";
        HandDetector hd;
        hd.loadMaskFilenames(msk_prefix);
        hd.trainModels(basename, img_prefix, msk_prefix,model_prefix,globfeat_prefix,feature_set,num_models_to_train,target_width);
        std::cout << "Done Training...\n";
    } else {
        std::string root, basename, day;
        int num_models_to_average, target_width, step_size;
        if (vm.count("root") && vm.count("basename") && vm.count("num_models_to_average") && vm.count("image_width") && vm.count("step_size")) {
            root = vm["root"].as<std::string>();
            basename = vm["basename"].as<std::string>();
            num_models_to_average = vm["num_models_to_average"].as<int>();
            target_width = vm["image_width"].as<int>();
            step_size = vm["step_size"].as<int>();
            day="0423";
        } else {
            std::cout << opt << std::endl;
            return EXIT_FAILURE;
        }
        std::string img_prefix		= root + "data/img"	+ basename + "/";			// color images
        std::string msk_prefix		= root + "data/mask"     + basename + "/";			// binary masks
        std::string model_prefix		= root + "models/"	+ basename + "/";			// output path for learned models
        std::string globfeat_prefix  = root + "globfeat/" + basename + "/";

        std::cout << "Testing...\n";
        std::string vid_filename		= root + "output/vid/"		+ basename + ".mov";

        HandDetector hd;
        hd.testInitialize(model_prefix,globfeat_prefix,feature_set,num_models_to_average,target_width);

        cv::VideoCapture cap;
        if (vm.count("input_video")) {
            cap = cv::VideoCapture(vm["input_video"].as<std::string>());
        } else {
            cap = cv::VideoCapture(0);
        }
        cv::Mat im;
        cv::Mat ppr;

        cv::VideoWriter avi;
        cv::VideoWriter avi2;

        int fps = (int)cap.get(CV_CAP_PROP_FPS);
        std::cout << fps << std::endl;
        while(1)
        {
            cap >>(im);

            if(!im.data) break;
            //cap >> im; if(!im.data) break; // skip frames with these
            //cap >> im; if(!im.data) break;
            //cap >> im; if(!im.data) break;

            hd.test(im,num_models_to_average,step_size);


            // Different ways to visualize the results
            // hd._response_img (float probabilities in a matrix)
            // hd._blur (blurred version of _response_img)


            int SHOW_RAW_PROBABILITY = 1;
            if(SHOW_RAW_PROBABILITY)
            {
                cv::Mat raw_prob;
                hd.colormap(hd._response_img,raw_prob,0);
                cv::imshow("probability",raw_prob);	// color map of probability
            }

            int SHOW_BLUR_PROBABILITY = 1;
            if(SHOW_BLUR_PROBABILITY)
            {
                cv::Mat pp_res;
                hd.postprocess(hd._response_img);
                imshow("blurred",hd._blu);		// colormap of blurred probability
            }

            int SHOW_BINARY_CONTOUR = 1;
            if(SHOW_BINARY_CONTOUR)
            {
                cv::Mat pp_contour = hd.postprocess(hd._response_img);// binary contour
                hd.colormap(pp_contour,pp_contour,0);					// colormap of contour
                cv::imshow("contour",pp_contour);

                if(!avi2.isOpened())
                {
                    std::stringstream ss;
                    ss.str("");
                    ss << root + "/output/vis/" +day+"_"+ basename + "0.mov";
                    int fourcc = CV_FOURCC('m','p','4','v');
                    avi2.open(ss.str(),fourcc,cap.get(CV_CAP_PROP_FPS),pp_contour.size(),true);
                }
                avi2 << pp_contour;
            }

            int SHOW_RES_ALPHA_BLEND = 1;
            if(SHOW_RES_ALPHA_BLEND)
            {
                cv::Mat pp_res = hd.postprocess(hd._response_img);
                hd.colormap(pp_res,pp_res,0);
                resize(pp_res,pp_res,im.size(),0,0,cv::INTER_LINEAR);
                addWeighted(im,0.7,pp_res,0.3,0,pp_res);				// alpha blend of image and binary contour
                imshow("alpha_res",pp_res);
                ppr = pp_res.clone();
            }

             if(!avi.isOpened())
             {
             std::stringstream ss;
             ss.str("");
             ss << root + "/output/vis/"+day+"_" + basename + ".mov";
//                 ss << root + "/output/vis/" + "20190213_4" + ".mov";
             int fourcc = CV_FOURCC('m','p','4','v');
             avi.open(ss.str(),fourcc,cap.get(CV_CAP_PROP_FPS),ppr.size(),true);
             }
             avi << ppr;


            int k = cv::waitKey(1);
            if (k == 'q') {
                break;
            }
            if (k == 's') {
                int current_frame = (int)cap.get(CV_CAP_PROP_POS_FRAMES);
                cap.set(CV_CAP_PROP_POS_FRAMES, current_frame + fps * 60 * 5);
            }
        }
    }

    return EXIT_SUCCESS;
}