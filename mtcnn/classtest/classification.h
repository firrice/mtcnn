#ifndef CLASSIFICATION_H_
#define CLASSIFICATION_H_

#include <caffe/caffe.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <algorithm>
#include <iosfwd>
#include <memory>
#include <string>
#include <utility>
#include <vector>
//#include "addLayer.h"

using namespace caffe;  // NOLINT(build/namespaces)
using std::string;
class Classifier {
public:
	Classifier(const string& model_file,
		const string& trained_file,
		const float scale,
		const float *mean,
		const int mode); //0--CPU,1--GPU
	Classifier();

	void Forward(const cv::Mat& img);
	void Forward(const cv::Mat& img,const float* box_info);
	Blob<float>* GetBlob(string blob_name);
private:

	std::vector<float> Predict(const cv::Mat& img);

	void WrapInputLayer(std::vector<cv::Mat>* input_channels);

	void Preprocess(const cv::Mat& img,
		std::vector<cv::Mat>* input_channels);

private:
	boost::shared_ptr<Net<float>> net_;
	int num_channels_;
	cv::Mat mean_;//默认图像为三通道,则均值也为三通道
	cv::Scalar mean_value_;
	float scale_;
};

#endif