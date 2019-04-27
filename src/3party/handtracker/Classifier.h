#ifndef Define_LcClassifier
#define Define_LcClassifier

#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/flann/config.h>

#include <opencv2/legacy/legacy.hpp>		// EM
#include <opencv2/contrib/contrib.hpp>		// colormap
#include <opencv2/nonfree/nonfree.hpp>		// SIFT

#include "LcBasic.h"

class LcValidator
{
public:
	
	void work( cv::Mat & res, cv::Mat & lab);
	float fp,tp;
	float fn,tn;

	LcValidator(){fp = tp = fn = tn = 0;}

	LcValidator( cv::Mat & res, cv::Mat & lab);

	LcValidator( float _tp, float _fp, float _fn , float _tn);

	void display();

	float getPrecision(int i=1);

	float getRecall(int i=1);

	float getF1(int i=1);

	float getZeroOne();

	LcValidator operator+( const LcValidator & a);

	


private:
	static void count( cv::Mat & res, cv::Mat & lab, float th, float & tp, float & fp, float & tn, float & fn);
};

class LcClassifier
{
public:
	int veb;

	virtual void train(cv::Mat & feature, cv::Mat & label){;}

	virtual LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label){return LcValidator();}
	
	virtual LcValidator predict(cv::Mat & feature, cv::Mat & res){return LcValidator();}

	virtual void save( std::string filename_prefix ){;}
	virtual void load( std::string filename_prefix ){;}
	virtual void load_full( std::string filename_prefix ){;}

	virtual LcClassifier* clone() const{ return new LcClassifier(*this);}

	virtual void release(){;}
};

//=================================

class LcRandomTreesR : public LcClassifier
{
public:
	
	CvRTParams _params;
	CvRTrees _random_tree;

	void train(cv::Mat & feature, cv::Mat & label);

	LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label);
	
	LcValidator predict(cv::Mat & feature, cv::Mat & res);
	void save( std::string filename_prefix );
	void load( std::string filename_prefix );
	void load_full( std::string filename_prefix );

	void release(){_random_tree.clear();}

	LcRandomTreesR* clone() const{ return new LcRandomTreesR(*this);}


};

//=================================

class LcRandomTreesC : public LcClassifier
{
public:
	
	CvRTParams _params;
	CvRTrees _random_tree;
	
	void train(cv::Mat & feature, cv::Mat & label);
	
	LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label);
	
	LcValidator predict(cv::Mat & feature, cv::Mat & res);
	
	void save( std::string filename_prefix );
	void load( std::string filename_prefix );
	
	void release(){_random_tree.clear();}
	
	LcRandomTreesC* clone() const{ return new LcRandomTreesC(*this);}
	
	
};

//=================================

class LcDecisionTree : public LcClassifier
{
public:

	CvDTreeParams _params;
	CvDTree _tree;

	void train(cv::Mat & feature, cv::Mat & label);

	LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label);

	void save( std::string filename_prefix );
	void load( std::string filename_prefix );

	LcDecisionTree* clone() const{ return new LcDecisionTree(*this);}
};

//=================================

class LcAdaBoosting : public LcClassifier
{
public:

	CvBoostParams _params;
	CvBoost _boost;

	void train(cv::Mat & feature, cv::Mat & label);

	LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label);

	void save( std::string filename_prefix );
	void load( std::string filename_prefix );

	LcAdaBoosting* clone() const{ return new LcAdaBoosting(*this);}
};

//=================================

class LcKNN : public LcClassifier
{
public:

	void train(cv::Mat & feature, cv::Mat & label);

	LcValidator predict(cv::Mat & feature, cv::Mat & res, cv::Mat & label);

	void setKernel( cv::Mat & kernel);

	int knn;

	LcKNN();

	void save( std::string filename_prefix );
	void load( std::string filename_prefix );


private:

	cv::Mat rotation_kernel;

	cv::Mat _lab;

	cv::Mat _feat;

	void rotate_data( cv::Mat & src, cv::Mat & dst);
};



//=================================


//#include "FeatureComputer.h"
//#include "VideoRead.h"
//
//void testClassifiers();
//
//#include "PostProcessing.h"
//
//void testPredictOnFrame();

//=================================

#endif