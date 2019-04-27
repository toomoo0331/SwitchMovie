#ifndef Define_LcFeatureComputer
#define Define_LcFeatureComputer


#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/flann/config.h>

#include <opencv2/legacy/legacy.hpp>		// EM
#include <opencv2/contrib/contrib.hpp>		// colormap
#include <opencv2/nonfree/nonfree.hpp>		// SIFT

#include "LcBasic.h"



//a father class
class LcFeatureComputer
{
public:
	int dim;
	int bound;
	int veb;
	bool use_motion;
	virtual void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc){;}
};



class LcFeatureExtractor
{
public:

	LcFeatureExtractor();

	int veb;

	int bound_setting;

	void img2keypts( cv::Mat & img, std::vector<cv::KeyPoint> & keypts,cv::Mat & img_ext, std::vector<cv::KeyPoint> & keypts_ext, int step_size);

	void work(cv::Mat & img , cv::Mat & desc, int step_size, std::vector<cv::KeyPoint> * p_keypoint = NULL);
	
	void work(cv::Mat & img , cv::Mat & desc, std::vector<cv::KeyPoint> * p_keypoint = NULL);
	// That's the main interface member which return a descriptor matrix
	// with an image input

	void work(cv::Mat & img, cv::Mat & desc, cv::Mat & img_gt, cv::Mat & lab, std::vector<cv::KeyPoint> * p_keypoint = NULL);
	//with ground truth image output at same time

	void work(cv::Mat & img, cv::Mat & desc, cv::Mat & img_gt, cv::Mat & lab, int step_size, std::vector<cv::KeyPoint> * p_keypoint = NULL);
	
	
	void set_extractor( std::string setting_string );

private:

	std::vector < LcFeatureComputer * > computers;

	int get_dim();

	int get_maximal_bound();

	void allocate_memory(cv::Mat & desc,int dims,int data_n);

	void extract_feature( cv::Mat & img,std::vector<cv::KeyPoint> & keypts,
						cv::Mat & img_ext, std::vector<cv::KeyPoint> & keypts_ext,
						cv::Mat & desc);

	void Groundtruth2Label( cv::Mat & img_gt, cv::Size _size , std::vector< cv::KeyPoint> , cv::Mat & lab);

};

//================

#ifndef Define_LcColorSpaceType
#define Define_LcColorSpaceType

enum ColorSpaceType{
	LC_RGB,LC_LAB,LC_HSV
};
#endif

template< ColorSpaceType color_type, int win_size>
class LcColorComputer: public LcFeatureComputer
{
public:
	LcColorComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};


//================

class LcHoGComputer: public LcFeatureComputer
{
public:
	LcHoGComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};

//================

class LcBRIEFComputer: public LcFeatureComputer
{
public:
	LcBRIEFComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};

//===================

class LcSIFTComputer: public LcFeatureComputer
{
public:
	LcSIFTComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};

//===================

class LcSURFComputer: public LcFeatureComputer
{
public:
	LcSURFComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};


//====================

class LcOrbComputer: public LcFeatureComputer
{
public:
	LcOrbComputer();
	void compute( cv::Mat & img, std::vector<cv::KeyPoint> & keypts, cv::Mat & desc);
};

#endif