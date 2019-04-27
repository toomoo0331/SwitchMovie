//
// Created by kei666 on 18/04/12.
//
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "DataLoader.h"

namespace util {

namespace fs = boost::filesystem;

DataLoader::DataLoader(std::string &ini_file_name)
{
    pt::read_ini(ini_file_name, this->pt_);

    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("target")) {
        target_ = tmp_str.get();
        std::cout << "target = " << target_ << std::endl;
    } else {
        std::cerr << "target is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("root")) {
        root_ = tmp_str.get();
        std::cout << "root = " << root_ << std::endl;
    } else {
        std::cerr << "root is nothing." << std::endl;
        exit(1);
    }

    if (!fs::exists(root_ + "output/")) {
        fs::create_directories(root_ + "output/");
    }
    output_directory_ = root_ + "output/" + target_ + "/";
    if (!fs::exists(output_directory_)) {
        fs::create_directories(output_directory_);
    }
    std::cout << "output directory = " << output_directory_ << std::endl;

    std::cout << std::endl;
    std::cout << "[movie]" << std::endl;

    std::cout << "movie directory = " << root_ + "input/" + target_ + "/" << std::endl;
    if (boost::optional<int> tmp_int = pt_.get_optional<int>("movie.num")) {
        movie_num_ = tmp_int.get();
        std::cout << "num = " << movie_num_ << std::endl;
    } else {
        std::cerr << "num is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("movie.extension")) {
        extension_ = tmp_str.get();
        std::cout << "extension = " << extension_ << std::endl;
    } else {
        std::cerr << "extension is nothing." << std::endl;
        exit(1);
    }

    for (int i = 0; i < movie_num_; i++) {
        movie_pathes_.push_back(root_ + "input/" + target_ + "/" + std::to_string(i) + "." + extension_);
    }

    std::cout << std::endl;
    std::cout << "[headditector]" << std::endl;

    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("headdetector.model_names")) {
        std::vector<std::string> models_names;
        std::stringstream ss(tmp_str.get());
        std::string buffer;
        while(std::getline(ss, buffer, ',')) {
            models_names.push_back(buffer);
        }
        std::cout << "model name = ";
        for (const auto &m : models_names) {
            std::cout << m << " ";
            models_directories_.push_back(root_ + "models/" + m + "/");
            globfeat_directories_.push_back(root_ + "globfeat/" + m + "/");
        }
        std::cout << std::endl;
    } else {
        std::cerr << "model_name is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<int> tmp_int = pt_.get_optional<int>("headdetector.step_size")) {
        step_size_ = tmp_int.get();
        std::cout << "step_size = " << step_size_ << std::endl;
    } else {
        std::cerr << "step_size is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<int> tmp_int = pt_.get_optional<int>("headdetector.num_models_to_average")) {
        num_models_to_average_ = tmp_int.get();
        std::cout << "num_models_to_average = " << num_models_to_average_ << std::endl;
    } else {
        std::cerr << "num_models_to_average is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<int> tmp_int = pt_.get_optional<int>("headdetector.target_width")) {
        target_width_ = tmp_int.get();
        std::cout << "target_width = " << target_width_ << std::endl;
    } else {
        std::cerr << "target_width is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<bool> tmp_bool = pt_.get_optional<bool>("headdetector.positive_region")) {
        positive_region_ = tmp_bool.get();
        std::cout << "positive_region = " << positive_region_ << std::endl;
    } else {
        std::cerr << "positive_region is nothing." << std::endl;
        exit(1);
    }

    std::cout << std::endl;
    std::cout << "[switch_movie]" << std::endl;
    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("switch_movie.alpha")) {
        std::vector<std::string> alphas;
        std::stringstream ss(tmp_str.get());
        std::string buffer;
        while(std::getline(ss, buffer, ',')) {
            alphas.push_back(buffer);
        }
        std::cout << "alpha = ";
        for (const auto &a : alphas) {
            std::cout << a << " ";
            alphas_.push_back((float)atof(a.c_str()));
        }
        std::cout << std::endl;
    } else {
        std::cerr << "alpha is nothing." << std::endl;
        exit(1);
    }
    if (boost::optional<std::string> tmp_str = pt_.get_optional<std::string>("switch_movie.mintime")) {
        std::vector<std::string> mintimes;
        std::stringstream ss(tmp_str.get());
        std::string buffer;
        while(std::getline(ss, buffer, ',')) {
            mintimes.push_back(buffer);
        }
        std::cout << "mintime = ";
        for (const auto &m : mintimes) {
            std::cout << m << " ";
            mintimes_.push_back(atoi(m.c_str()));
        }
        std::cout << std::endl;
    } else {
        std::cerr << "mintime is nothing." << std::endl;
        exit(1);
    }

    if (boost::optional<int> tmp_int = pt_.get_optional<int>("switch_movie.mode")) {
        mode_= tmp_int.get();
        std::cout << "mode = " << ((mode_ == 0) ? "WEIGHT" : "MIN_TIME") << std::endl;
    } else {
        std::cerr << "mode is nothing." << std::endl;
        exit(1);
    }

}

DataLoader::~DataLoader() {
}

}