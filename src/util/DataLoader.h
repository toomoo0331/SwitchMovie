//
// Created by kei666 on 18/04/12.
//

#ifndef PROJECT_DATALEADER_H
#define PROJECT_DATALEADER_H

#include <iostream>
#include <string>
#include <vector>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/optional.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

namespace util {

namespace pt = boost::property_tree;

class DataLoader {
public:
    DataLoader(std::string &ini_file_name);
    ~DataLoader();

    //target
    std::string target_;

    //path
    std::string root_;
    std::vector<std::string> models_directories_;
    std::vector<std::string> globfeat_directories_;
    std::vector<std::string> movie_pathes_;

    //headdetector configuration
    int step_size_;
    int num_models_to_average_;
    int target_width_;
    bool positive_region_;

    //switch movie
    std::vector<float> alphas_;
    std::vector<int> mintimes_;
    int mode_;

    //movie configuration
    int movie_num_;
    std::string extension_;

    //output
    std::string output_directory_;

private:
    pt::ptree pt_;
};

}
#endif //PROJECT_DATALEADER_H
