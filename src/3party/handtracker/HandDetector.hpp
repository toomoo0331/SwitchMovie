/*
 *  HandDetector.h
 *  TrainHandModels
 *
 *  Created by Kris Kitani on 4/1/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "FeatureComputer.hpp"
#include "Classifier.h"
#include "LcBasic.h"


class HandDetector
{
public:
	
	void loadMaskFilenames(std::string msk_prefix);
	std::vector<std::string> _filenames;
	
	void trainModels(std::string basename, std::string img_prefix,std::string msk_prefix,std::string model_prefix,std::string feat_prefix, std::string feature_set, int max_models, int img_width);
	std::string _feature_set;
	
	void testInitialize(std::string model_prefix,std::string feat_prefix, std::string feature_set, int knn, int width);
	
	std::vector<LcRandomTreesR>		_classifier;
	std::vector<int>					_indices;
	std::vector<float>				_dists;
	int							_knn;
	cv::flann::Index				_searchtree;
	cv::flann::IndexParams			_indexParams;
	LcFeatureExtractor			_extractor;
	cv::Mat							_hist_all;				// do not destroy!
	
	float						_img_width;
	float						_img_height;
	cv::Size					_img_size;
	
	void test(cv::Mat &img);
	void test(cv::Mat &img,cv::Mat &dsp);
	void test(cv::Mat &img,int num_models);
	void test(cv::Mat &img,cv::Mat &dsp,int num_models);
	void test(cv::Mat &img,int num_models, int step_size);
	void test(cv::Mat &img,cv::Mat &dsp,int num_models, int step_size);
	
	cv::Mat							_descriptors;
	cv::Mat							_response_vec;
	cv::Mat							_response_img;
	std::vector<cv::KeyPoint>			_kp;
	cv::Mat							_dsp;
	cv::Size					_sz;
	int							_bs;
	cv::Mat							_response_avg;
	
	cv::Mat							_raw;				// raw response
	cv::Mat							_blu;				// blurred image
	cv::Mat							_ppr;				// post processed
	
	cv::Mat postprocess(cv::Mat &img,std::vector<cv::Point2f> &pt);
	cv::Mat postprocess(cv::Mat &img);
	
	void computeColorHist_HSV(cv::Mat &src, cv::Mat &hist);
	void colormap(cv::Mat &src, cv::Mat &dst, int do_norm);
	void rasterizeResVec(cv::Mat &img, cv::Mat&res,std::vector<cv::KeyPoint> &keypts, cv::Size s, int bs);
	
private:
	
};