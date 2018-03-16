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
	net_.reset(new Net<float>(model_file, TEST));  //�洢����Ľṹ
	net_->CopyTrainedLayersFrom(trained_file);  //net��ʼ�����
	Blob<float>* input_layer = net_->input_blobs()[0];  //��������ݲ��֣�label��������Ϊ1��
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
	WrapInputLayer(&input_channels);  //����һ��caffe���ݽṹģ��

	Preprocess(img, &input_channels);  //�������ͼƬת��Ϊģ���ʽ

	net_->Forward();  //����ǰ�򴫲�
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
	img.convertTo(sample_float, CV_32FC3);  //CV_32FC3���͵�ȡֵ��Ϊ0.0��1.0
	mean_ = cv::Mat(cv::Size(img.cols, img.rows), CV_32FC3, mean_value_);
	cv::subtract(sample_float, mean_, sample_float);
	if (scale_ != 1)
		sample_float *= scale_;
	cv::split(sample_float, *input_channels);  //ת����caffe���ݲ�����Ҫ�ĸ�ʽ��img_num*channel*height*width������ͨ��ͼƬ�洢��mat�е������ʽΪheight*width*channel,������Ҫ����ת��
}
Blob<float>* Classifier::GetBlob(string blob_name)
{
	return net_->blob_by_name(blob_name).get();
}