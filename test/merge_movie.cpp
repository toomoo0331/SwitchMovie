//
// Created by kei666 on 19/05/08.
//


#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

class graph{
public:
    vector<vector<double>> data;
    vector<vector<int>> rank;
    double min;
    cv::Mat base;
    int video_num;
    int framerate;
    int video_len;
    int graph_h;

    graph(int day, int num, int frame, int len, int h){
        csv_read(day);
        video_num=num;
        framerate=frame;
        video_len = len;
        graph_h = h;
    };


    void csv_read(int day){
        std::string score_name = "/home/kei666/CLionProjects/SwitchMovie/output/"+std::to_string(day)+"/head.csv";
        std::string rank_name = "/home/kei666/CLionProjects/SwitchMovie/output/"+std::to_string(day)+"/mintime_optimize.csv";
        std::ifstream ifs1(score_name);
        string buf;                 // 1行分バッファ
        min=1;
        while (getline(ifs1, buf)) {
            vector<double> rec;     // 1行分ベクタ
            istringstream iss(buf); // 文字列ストリーム
            string field;           // 1列分文字列
            // 1列分文字列を1行分ベクタに追加
            while (getline(iss, field, ',')) {
                rec.push_back(std::stod(field));
                if(min>std::stod(field)) min = std::stod(field);
            }// １行分ベクタを data ベクタに追加
            if (rec.size() != 0) data.push_back(rec);
        }
        std::ifstream ifs2(rank_name);
        int i=0;
        while (getline(ifs2, buf)) {
            if(i!=0) {
                vector<int> rec;     // 1行分ベクタ
                istringstream iss(buf); // 文字列ストリーム
                string field1;           // 1列分文字列
                // 1列分文字列を1行分ベクタに追加
                while (getline(iss, field1, ',')) {
                    rec.push_back(std::stoi(field1));
                }// １行分ベクタを data ベクタに追加
                if (rec.size() != 0)rank.push_back(rec);
            }
            i++;
        }

    }

    cv::Mat getGraph(int k, int width){
        base = cv::Mat::zeros(graph_h, 3*width, CV_8UC3);
        double tmp1,tmp2;
        for(int i = 0 ; i < 2*width ; i++){
            for(int j=0;j<video_num;j++) {
                tmp1 = getScore(j,k+width-i);
                tmp2 = getScore(j,k+width-i-1);
                cv::line(base, cv::Point((int)(width*2.5)-i, (int)(0.85*graph_h-(0.7*graph_h*tmp1))), cv::Point((int)(width*2.5)-i-1, (int)(0.85*graph_h-(0.7*graph_h*tmp2))), getColor(j), 1, CV_AA);
            }
        }
        cv::putText(base, "|", cv::Point(width * 1.5 + framerate*10, (int)(0.875*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.3,
                    cv::Scalar(255, 255, 255), 1, CV_AA);
        std::string time_name = "before "+ std::to_string(10)+"sec";
        cv::putText(base, time_name, cv::Point(width * 1.5 + framerate*10 - 30, (int)(0.95*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.4,
                    cv::Scalar(255, 255, 255), 1, CV_AA);

        cv::line(base, cv::Point((int)width * 1.5, 0), cv::Point((int)width * 1.5, (int)(0.875*graph_h)), getColor(6), 1, CV_AA);
        time_name = "now";
        cv::putText(base, time_name, cv::Point(width * 1.5-10, (int)(0.95*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.4,
                    cv::Scalar(255, 255, 255), 1, CV_AA);

        cv::putText(base, "|", cv::Point(width * 1.5 + framerate*10*(-1), (int)(0.875*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.3,
                    cv::Scalar(255, 255, 255), 1, CV_AA);
        time_name = "after "+ std::to_string(10)+"sec";
        cv::putText(base, time_name, cv::Point(width * 1.5 + framerate*10*(-1)-30, (int)(0.95*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.4,
                    cv::Scalar(255, 255, 255), 1, CV_AA);

        cv::putText(base, std::to_string(1), cv::Point((int)width*0.4, (int)(0.15*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(255, 255, 255), 1, CV_AA);
        cv::putText(base, std::to_string(min), cv::Point((int)width*0.4-30, (int)(0.85*graph_h)), cv::FONT_HERSHEY_SIMPLEX, 0.5,
                    cv::Scalar(255, 255, 255), 1, CV_AA);
        return base;
    }

    double getScore(int num, int time){
        if(time<0){
            return 1;
        }
        if(time>=video_len){
            return 1;
        }
        double tmp = data[time][num];
        return (tmp-min)/(1-min);
    }

    int getRank(int time){
        return rank[time][1];
    }

    cv::Scalar getColor(int num){
        if(num==0)return cv::Scalar(255,0,255);
        if(num==1)return cv::Scalar(0,255,255);
        if(num==2)return cv::Scalar(0,255,0);
        if(num==3)return cv::Scalar(255,255,0);
        if(num==4)return cv::Scalar(0,0,255);
        if(num==5)return cv::Scalar(255,0,0);
        if(num==6)return cv::Scalar(255,255,255);
    }


};

void mergeMovie1(int video_num, std::string video_extension){
    cv::VideoCapture pcaps0("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/Mouth.mov");
    cv::VideoCapture pcaps1("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/InternalBody.mov");
    cv::VideoCapture pcaps2("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/Skin.mov");
    cv::VideoCapture pcaps3("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/Gloves.mov");
    cv::VideoCapture pcaps4("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/Specimen.mov");
    cv::VideoCapture pcaps5("/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/unet_iron.mp4");
    std::vector<std::unique_ptr<cv::VideoWriter>> pwriter;
    int video_len = pcaps0.get(CV_CAP_PROP_FRAME_COUNT);
    int w = pcaps0.get(CV_CAP_PROP_FRAME_WIDTH);
    int h = pcaps0.get(CV_CAP_PROP_FRAME_HEIGHT);
    std::cout<<"asdafaf"<<std::endl;
    for (int i = 0; i < video_num; i++) {
        std::string save_name =  "/home/kei666/CLionProjects/SwitchMovie/output/vis/20190712/all." + video_extension;

        pwriter.push_back(std::unique_ptr<cv::VideoWriter>(new cv::VideoWriter(save_name,
                                                                               cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                                                                               25,
                                                                               cv::Size(900, 600),
                                                                               true)));

    }
    int a[6]={0,300,600,0,300,600};
    int b[6]={0,0,0,300,300,300};
    cv::Mat image = cv::Mat::zeros(600,900,CV_8UC3);

    for(int k=0;k<video_len;k++){
        cv::Mat frame0,frame1,frame2,frame3,frame4,frame5;
        pcaps0 >> frame0;
        pcaps1 >> frame1;
        pcaps2 >> frame2;
        pcaps3 >> frame3;
        pcaps4 >> frame4;
        pcaps5 >> frame5;
        cv::resize(frame5,frame5,cv::Size(300,300));

        std::cout << std::setfill(' ') << std::setw(5) << std::left << int(k/1800)
                  << std::setfill(' ') << std::setw(5) << std::right << int(k/30)-int(k/1800)*60<< " time";
        std::cout << "\r";

        cv::Mat roi0 = image(cv::Rect(a[0],b[0],300,300));
        frame0.copyTo(roi0);
        cv::putText(image, "Mouth",cv::Point(0,250), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);

        cv::Mat roi1 = image(cv::Rect(a[1],b[1],300,300));
        frame1.copyTo(roi1);
        cv::putText(image, "InternalBody",cv::Point(300,250), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);

        cv::Mat roi2 = image(cv::Rect(a[2],b[2],300,300));
        frame2.copyTo(roi2);
        cv::putText(image, "Skin",cv::Point(600,250), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);

        cv::Mat roi3 = image(cv::Rect(a[3],b[3],300,300));
        frame3.copyTo(roi3);
        cv::putText(image, "Gloves",cv::Point(0,550), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);

        cv::Mat roi4 = image(cv::Rect(a[4],b[4],300,300));
        frame4.copyTo(roi4);
        cv::putText(image, "Specimen",cv::Point(300,550), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);

        cv::Mat roi5 = image(cv::Rect(a[5],b[5],300,300));
        frame5.copyTo(roi5);
        cv::putText(image, "unet_iron_for_MouthOpener",cv::Point(600,550), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0,0,200), 1, CV_AA);
//        cv::Mat roi3 = image(cv::Rect(a[3],b[3],300,252));
//        frame3.copyTo(roi3);
//        cv::putText(image, "InternalBody",cv::Point(426,612), cv::FONT_HERSHEY_SIMPLEX, 1.2, cv::Scalar(0,0,200), 2, CV_AA);
//        std::cout<<"asd2"<<std::endl;

        imshow("all", image);
        *pwriter[0] << image;
        int key = cv::waitKey(1);
        //cv::imshow("movie",frame);
        if(key == 'q') {
            break;
        }
    }
}

void mergeMovie2(int video_num, std::string video_extension, int day, int framerate, int graph_h){
    std::vector<cv::VideoCapture> caps;
    std::vector<std::string> movie_pathes;
    std::string csv_name;
    for (int i = 0; i < video_num; i++) {
        movie_pathes.push_back("/home/kei666/CLionProjects/SwitchMovie/input/" + std::to_string(day) + "/" + std::to_string(i) + ".mp4");
    }
    for (const auto &str : movie_pathes) {
        caps.push_back(cv::VideoCapture(str));
        std::cout << str << std::endl;
    }
    std::string save_name =  "/home/kei666/CLionProjects/SwitchMovie/output/"+std::to_string(day)+"/score." + video_extension;
    int video_len = caps[0].get(CV_CAP_PROP_FRAME_COUNT);
    int w = caps[0].get(CV_CAP_PROP_FRAME_WIDTH);
    int h = caps[0].get(CV_CAP_PROP_FRAME_HEIGHT);
    int a[6]={0,w,2*w,0,w,2*w};
    int b[6]={0,0,0,h,h,h};
    cv::VideoWriter pwriter(save_name, cv::VideoWriter::fourcc('M', 'P', '4', 'V'),framerate,cv::Size(3*w, 2*h+graph_h),true);
    cv::Mat image = cv::Mat::zeros(2*h+graph_h,3*w,CV_8UC3);
    graph score_graph(day, video_num, framerate, video_len,graph_h);


    for(int k=0;k<video_len;k++){
        std::vector<cv::Mat> frame;
        cv::Mat tmp_frame;
        for(int i=0; i < video_num; i++) {
            caps[i] >> tmp_frame;
            cv::Mat roi0 = image(cv::Rect(a[i], b[i], w, h));
            tmp_frame.copyTo(roi0);
            cv::putText(image, std::to_string(i), cv::Point(a[i] + 30, b[i] + 30), cv::FONT_HERSHEY_SIMPLEX, 1,
                        score_graph.getColor(i), 2, CV_AA);
        }
        cv::rectangle(image, cv::Point(a[score_graph.getRank(k)]+2,b[score_graph.getRank(k)]+2), cv::Point(a[score_graph.getRank(k)]+w-2, b[score_graph.getRank(k)]+h-2), cv::Scalar(255,255,255), 2);

        std::cout << std::setfill(' ') << std::setw(5) << std::left << int(k/1800)
                  << std::setfill(' ') << std::setw(5) << std::right << int(k/30)-int(k/1800)*60<< " time";
        std::cout << "\r";

        cv::Mat roi = image(cv::Rect(0,2*h,3*w,graph_h));
        score_graph.getGraph(k,w).copyTo(roi);
        imshow("all", image);
        pwriter << image;
        int key = cv::waitKey(1);
        if(key == 'q') {
            break;
        }
    }
}

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

    //mergeMovie1(video_num, video_extension);
    mergeMovie2(5, video_extension,20190825, 30,300);

    return true;
}