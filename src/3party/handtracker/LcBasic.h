#ifndef Define_LcBasic
#define Define_LcBasic


#include <cstdio>
#include <cstdlib>
#include <iostream>

#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/flann/config.h>

#include <opencv2/legacy/legacy.hpp>		// EM
#include <opencv2/contrib/contrib.hpp>		// colormap
#include <opencv2/nonfree/nonfree.hpp>		// SIFT

#include <algorithm>

namespace lc
{
	
	void LcMat2Bin( const char * file_name, cv::Mat & src);

	void LcBin2Mat( const char * file_name, cv::Mat & ans);

	class LcSortElement
	{
	public:
		float element;
		int id;
		
		LcSortElement(){;}
		

		//static bool compare( LcSortElement & a, LcSortElement & b);
	};
	
	bool LcSortcompare( const LcSortElement & a, const LcSortElement & b);
	
	
	void argsort( std::vector<float> & data, std::vector<int> & id);

	void swapMatrix( cv::Mat & src, cv::Mat & dst);

	void swapMatrix( cv::Mat & src, cv::Mat & dst, cv::Mat & src_lab, cv::Mat & dst_lab);
};

#endif