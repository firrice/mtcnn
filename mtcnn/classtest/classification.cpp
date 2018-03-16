#include "classification.h"


Classifier::Classifier(const string& model_file,
	const string& trained_file,
	const float scale,
	const float *mean,
	const int mode)
{
	if (mode == 0)
		Caffe::set_mode(Caffe::CPU);
	else
		Caffe::set_mode(Caffe::GPU);
	net_.reset(new Net<float>(model_file, TEST));  //存储网络的结构
	net_->CopyTrainedLayersFrom(trained_file);  //net初始化完毕
	Blob<float>* input_layer = net_->input_blobs()[0];  //输入的数据部分（label部分索引为1）
	num_channels_ = input_layer->channels();
	scale_ = scale;
	for (int i = 0; i < 3; ++i)
		mean_value_[i] = mean[i];
}
Classifier::Classifier(){}
void Classifier::Forward(const cv::Mat& img)
{
	Blob<float>* input_layer = net_->input_blobs()[0];
	input_layer->Reshape(1, num_channels_, img.rows, img.cols);
	net_->Reshape();

	std::vector<cv::Mat> input_channels;
	WrapInputLayer(&input_channels);  //构建一个caffe数据结构模板

	Preprocess(img, &input_channels);  //将输入的图片转换为模板格式

	net_->Forward();  //进行前向传播
}
void Classifier::Forward(const cv::Mat& img, const float* box_info){
	//box_info
	Blob<float>* box_info_blob = net_->input_blobs()[1];//blob_by_name("convb-1").get();
	box_info_blob->Reshape(1, 4, 1, 1);
	float* box_info_data = box_info_blob->mutable_cpu_data();
	for (int i = 0; i < 4; i++){
		box_info_data[i] = box_info[i];
	}
	Forward(img);
}
void Classifier::WrapInputLayer(std::vector<cv::Mat>* input_channels)
{
	Blob<float>* input_layer = net_->input_blobs()[0];

	int width = input_layer->width();
	int height = input_layer->height();
	float* input_data = input_layer->mutable_cpu_data();
	for (int i = 0; i < input_layer->channels(); ++i) {
		cv::Mat channel(height, width, CV_32FC1, input_data);
		input_channels->push_back(channel);
		input_data += width * height;
	}
}
void Classifier::Preprocess(const cv::Mat& img,
	std::vector<cv::Mat>* input_channels)
{
	cv::Mat sample_float;
	img.convertTo(sample_float, CV_32FC3);  //CV_32FC3类型的取值仅为0.0到1.0
	mean_ = cv::Mat(cv::Size(img.cols, img.rows), CV_32FC3, mean_value_);
	cv::subtract(sample_float, mean_, sample_float);
	if (scale_ != 1)
		sample_float *= scale_;
	cv::split(sample_float, *input_channels);  //转换成caffe数据层所需要的格式：img_num*channel*height*width，而普通的图片存储的mat中的数组格式为height*width*channel,所以需要进行转换
}
Blob<float>* Classifier::GetBlob(string blob_name)
{
	return net_->blob_by_name(blob_name).get();
}