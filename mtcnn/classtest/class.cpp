#include "classification.h"
#include "addLayer.h"
#include <vector>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv\cv.h>
#include <algorithm>
#include "General\FileProc.h"
#include "General\MyString.h"
#include "General\draw_3D.h"

using namespace std;
using namespace cv;

struct candidate{
	float conf;
	int ltx, lty, rbx, rby, width, height, area, cls;
	float pts[8];
	bool use;
};
//nms算法，包含两个主要函数，分别是nms和计算iou
#pragma region nms

bool comp(candidate _c1, candidate _c2){
	return _c1.conf > _c2.conf;
}

//计算iou
float calculateIOU(candidate _c1, candidate _c2){
	int ltxMax = _c1.ltx > _c2.ltx ? _c1.ltx : _c2.ltx;
	int rbxMin = _c1.rbx < _c2.rbx ? _c1.rbx : _c2.rbx;
	int ltyMax = _c1.lty > _c2.lty ? _c1.lty : _c2.lty;
	int rbyMin = _c1.rby < _c2.rby ? _c1.rby : _c2.rby;
	int overlayWidth = (rbxMin - ltxMax)>0 ? (rbxMin - ltxMax) : 0;
	int overlayHeight = (rbyMin - ltyMax) > 0 ? (rbyMin - ltyMax) : 0;
	int overlayArea = overlayWidth*overlayHeight;
	float iou = float(overlayArea) / (_c1.area + _c2.area - overlayArea);
	return iou;
}

//被nms掉的框，use属性被标记为false，否则为true
void nms(Vector<candidate> &_canOfScale, float _nmsThreshold){
	float nmsThreshold = _nmsThreshold;
	//根据conf从大到小排序
	sort(_canOfScale.begin(), _canOfScale.end(), comp);
	candidate can, can1;
	for (int i = 0; i < _canOfScale.size(); i++){
		can = _canOfScale[i];
		if (!can.use)
			continue;
		for (int j = i + 1; j < _canOfScale.size(); j++){
			can1 = _canOfScale[j];
			if (!can1.use)
				continue;
			if (can.cls == can1.cls&&calculateIOU(can, can1) > nmsThreshold)
				_canOfScale[j].use = false;
		}
	}
}
#pragma endregion

//获取多尺度
vector<float> getScales(int _width, int _height, int _minCandidate = 80){
	vector<float> scales;
	int min = _width < _height ? _width : _height;
	float factor = 0.709, scale = (float)12 / _minCandidate;

	//必须保证min{宽,高} * scale >12 ,否则forward时会出错
	while (min*scale > 12){
		scales.push_back(scale);
		scale *= 0.709;
	}
	return scales;
}
//多类mtcnn算法
Vector<candidate> mtcnn_multi(Mat img, Vector<Classifier> &mtcnn_net, int class_num){
	int img_size[2] = { img.cols, img.rows };
	vector<float> scales = getScales(img_size[0], img_size[1]);  //获取图像金字塔中的各种缩放系数
	cv::Mat img1;
	img.copyTo(img1);
	float scale;
	Vector<candidate> canOfAll;
	//pnet
#pragma region pnet
	for (int i = 0; i < scales.size(); i++){
		scale = scales[i];
		cv::resize(img, img, cv::Size(img_size[0] * scale, img_size[1] * scale), 0, 0, cv::INTER_LINEAR);
		mtcnn_net[0].Forward(img);
		Blob<float>* b0 = mtcnn_net[0].GetBlob("prob1");  //最终的“prob1”的形式：img_num * class_num * height * width，其中特征图上的每一个点的就存储了每一个类别的置信度
		vector<int> shape = b0->shape();
		vector<cv::Mat> cls;
		for (int n = 0; n<class_num; n++)  //cls容器，用class_num个feature map存储每一类
			cls.push_back(*new cv::Mat(shape[2], shape[3], CV_32F, b0->mutable_cpu_data() + n*shape[2] * shape[3]));
		Vector<candidate> cansOfScale;
		for (int row = 0; row < shape[2]; row++){
			for (int col = 0; col < shape[3]; col++){
				vector<float> conf(class_num, 0);//存储每个类别的置信度
				int conf_cls = 0, flag = 0;  //conf_cls中存储最大置信度所在类别
				float max_conf = 0; //最大的置信度
				for (int n = 1; n < class_num; n++){
					conf[n] = cls[n].at<float>(row, col);  //找到最终的prob1上的每个点的属于每个类别的置信度
					if (max_conf < conf[n]){
						max_conf = conf[n];
						conf_cls = n;
					}
				}
				if (max_conf>0.3){
					candidate can;
					int ltx = max(col * 2 / scale, float(0));
					int lty = max(row * 2 / scale, float(0));
					int rbx = min(ltx + 12 / scale, float(img1.cols - 1));
					int rby = min(lty + 12 / scale, float(img1.rows - 1));
					//特征点每个坐标还原到原图上：*2是因为forward过程中存在一个池化层，2个卷积核为3*3的卷积层，可以直接*2还原到
					//forward之前，resize之后的图像的对应坐标，不用管卷积层带来的坐标变换，因为两个卷积层也仅仅
					//减了几个像素而已。/scale是还原到resize之前也就是原图的对应坐标
					can.conf = conf[conf_cls];
					can.ltx = ltx;
					can.lty = lty;
					can.rbx = rbx;
					can.rby = rby;
					can.width = can.rbx - can.ltx + 1;
					can.height = can.rby - can.lty + 1;
					can.area = can.width*can.height;
					can.use = true;
					can.cls = conf_cls;
					cansOfScale.push_back(can);
				}
			}
		}
		nms(cansOfScale, 0.4);
		for (int n = 0; n < cansOfScale.size(); n++){
			candidate can = cansOfScale[n];
			if (!can.use)
				continue;
			canOfAll.push_back(can);  //canOfAll中存储所有经过nms后的can对象（同一尺度下）
		}
	}
	nms(canOfAll, 0.4);  //原图在不同尺度下经过nms后的proposal，在这里再进行一次nms（row86-row89行，使它们又回到了同一尺度）
	/*
	Mat img2;
	img1.copyTo(img2);
	   
	for (int i = 0; i < canOfAll.size(); ++i){
	candidate can = canOfAll[i];
	if (!can.use)
	continue;
	int cls = can.cls;
	switch (cls)
	{
	case 0:
	rectangle(img2, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(255, 0, 0), 2);
	break;
	case 1:
	rectangle(img2, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 255, 0), 2);
	break;
	case 2:
	rectangle(img2, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 255), 2);
	break;
	case 3:
	rectangle(img2, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 0), 2);
	break;
	default:
	break;
	}
	}
	imshow("a", img2);
	waitKey(0);
	*/
#pragma endregion
	//rnet
#pragma region rnet
	for (int i = 0; i < canOfAll.size(); ++i){
		candidate *can = &canOfAll[i];
		if (!can->use)
			continue;
		Mat cropImg = img1(Range(can->lty, can->rby), Range(can->ltx, can->rbx));
		resize(cropImg, cropImg, Size(24, 24));
		float conf = 0;
		//根据can的类别选择测试模型
		Blob<float> *prob = NULL, *coord = NULL;
		for (int n = 0; n < class_num; n++){  //在做lable时，-1时part类，0是背景类，1到后面是其他类（class_num中是加上背景类的）
			if (can->cls == n + 1){  //can->cls的类别即物体的类别（从1到后面，不包括背景类）
				mtcnn_net[2 * n + 1].Forward(cropImg);  //当n = 1即物体的类别can->cls为2时，使用mtcnn_net[3]进行前向传播，即第二类物体的rnet网络
				prob = mtcnn_net[2 * n + 1].GetBlob("prob1");  //物体类别的概率
				coord = mtcnn_net[2 * n + 1].GetBlob("conv5-2");  //bbox的回归量
				break;
			}
		}
		conf = *(prob->mutable_cpu_data() + 1);
		can->use = false;
		if (conf > 0.5){
			//回归坐标
			//解算坐标，保证坐标在正确的范围内；改变了坐标之后，相应的宽高，面积也应该改变,conf也需要改
			const float *data = coord->cpu_data();
			int can_ltx = can->ltx, can_lty = can->lty;
			can->ltx = data[0] * can->width + can_ltx;  //因为在训练的时候用的是偏移量，所以这里要这么操作
			can->ltx = canOfAll[i].ltx > 0 ? canOfAll[i].ltx : 0;  //边界判断
			can->lty = data[1] * can->height + can_lty;
			can->lty = canOfAll[i].lty > 0 ? canOfAll[i].lty : 0;
			can->rbx = data[2] * can->width + can_ltx;
			can->rbx = canOfAll[i].rbx < img1.cols ? canOfAll[i].rbx : img1.cols;
			can->rby = data[3] * can->height + can_lty;
			can->rby = canOfAll[i].rby < img1.rows ? canOfAll[i].rby : img1.rows;

			can->width = canOfAll[i].rbx - canOfAll[i].ltx + 1;
			can->height = canOfAll[i].rby - canOfAll[i].lty + 1;
			can->area = canOfAll[i].width*canOfAll[i].height;
			can->conf = *(prob->mutable_cpu_data() + 1);
			can->use = true;
		}
	}
	nms(canOfAll, 0.5);
	/*
	Mat img3;
	img1.copyTo(img3);
	for (int i = 0; i < canOfAll.size(); ++i){
	candidate can = canOfAll[i];
	if (!can.use)
	continue;
	switch (can.cls){
	case 0:
	rectangle(img3, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(255, 0, 0), 2);
	break;
	case 1:
	rectangle(img3, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 255, 0), 2);
	break;
	case 2:
	rectangle(img3, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 255), 2);
	break;
	case 3:
	rectangle(img3, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 0), 2);
	break;
	default:
	break;
	}
	}
	imshow("rnt", img3);
	waitKey(0);
	*/

#pragma endregion
	//onet
#pragma region onet
	for (int i = 0; i < canOfAll.size(); ++i){
		candidate *can = &canOfAll[i];
		if (!can->use)
			continue;
		int size = max(can->width, can->height), cx = (can->ltx + can->rbx) / 2, cy = (can->lty + can->rby) / 2;
		int nx1 = cx - size / 2, nx2 = cx + size / 2, ny1 = cy - size / 2, ny2 = cy + size / 2;
		nx1 = max(nx1, 0);
		nx2 = min(nx2, img1.cols - 1);
		ny1 = max(ny1, 0);
		ny2 = min(ny2, img1.rows - 1);  //因为之前的rnet有对bbox做了nms，所以这里上面一部分加入了边界判断
		Mat cropImg = img1(Range(ny1, ny2), Range(nx1, nx2));
		resize(cropImg, cropImg, Size(48, 48));
		Blob<float> *prob = NULL, *coord = NULL, *pts = NULL;
		for (int n = 0; n < class_num; n++){
			if (can->cls == n + 1){
				mtcnn_net[2 * n + 2].Forward(cropImg);
				prob = mtcnn_net[2 * n + 2].GetBlob("prob1");
				coord = mtcnn_net[2 * n + 2].GetBlob("conv6-2");
				pts = mtcnn_net[2 * n + 2].GetBlob("conv6-3");
				break;
			}
		}
		const float conf = *(prob->cpu_data() + 1);
		const float *coord_data = coord->cpu_data();
		const float *pts_data = pts->cpu_data();
		can->use = false;
		if (conf > 0.8){
			int can_ltx = can->ltx, can_lty = can->lty;
			can->use = true;
			can->ltx = coord_data[0] * size + nx1;
			can->ltx = max(can->ltx, 0);
			can->lty = coord_data[1] * size + ny1;
			can->lty = max(can->lty, 0);
			can->rbx = coord_data[2] * size + nx1;
			can->rbx = min(can->rbx, img1.cols - 1);
			can->rby = coord_data[3] * size + ny1;
			can->rby = min(can->rby, img1.cols - 1);
			//pts
			float *can_pts = can->pts;
			for (int i = 0; i < 8; i += 2){
				can_pts[i] = pts_data[i] * size + nx1;
				can_pts[i] = min(can_pts[i], float(img1.cols - 1));
				can_pts[i] = max(can_pts[i], float(0));
				can_pts[i + 1] = pts_data[i + 1] * size + ny1;
				can_pts[i + 1] = min(can_pts[i + 1], float(img1.rows - 1));
				can_pts[i + 1] = max(can_pts[i + 1], float(0));
			}

			can->width = can->rbx - can->ltx + 1;
			can->height - can->rby - can->lty + 1;
			can->area = can->width*can->height;
		}
	}
	nms(canOfAll, 0.5);
	return canOfAll;
	/*
	Mat img4;
	img1.copyTo(img4);
	for (int i = 0; i < canOfAll.size(); ++i){
	candidate can = canOfAll[i];
	if (!can.use)
	continue;
	switch (can.cls){
	case 0:
	rectangle(img4, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(255, 0, 0), 2);
	break;
	case 1:
	rectangle(img4, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 255, 0), 2);
	break;
	case 2:
	rectangle(img4, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 255), 2);
	break;
	case 3:
	rectangle(img4, Point(can.ltx, can.lty), Point(can.rbx, can.rby), Scalar(0, 0, 0), 2);
	break;
	default:
	break;
	}
	for (int i = 0; i < 8; i += 2)
	circle(img4, Point(can.pts[i], can.pts[i + 1]), 2, Scalar(0, 0, 0), 2);
	for (int i = 0; i < 8; i += 2)
	line(img4, Point(can.pts[i % 8], can.pts[(i + 1) % 8]),
	Point(can.pts[(i + 2) % 8], can.pts[(i + 3) % 8]), Scalar(0, 0, 0), 2);
	}
	imshow("onet", img4);
	if (cv::waitKey(1) == 'q')
	break;
	time = (double)1000 * (getTickCount() - time) / getTickFrequency();
	printf("%.2f\n", time);
	*/
#pragma endregion
}

void main(){
	cv::VideoCapture cap("video/Mathbook.mp4");  //cap中存储这个视频对象
	cv::Mat img;
	float mean[3] = { 127.5, 127.5, 127.5 }, scale = 0.0078125;
	vector<Scalar> colors(4);
	colors[0] = Scalar(0, 0, 255);
	colors[1] = Scalar(255, 0, 0);
	colors[2] = Scalar(0, 255, 0);
	colors[3] = Scalar(0, 0, 0);
	Vector<Classifier> mtcnn_net(1);
	vector<FileVector> class_dir;
	vector<FileVector> pnet_model;
	vector<FileVector> class_model;
	vector<FileVector> proto_name;
	char *models_path = "../../train/models/";
	string proto_path = "../../prototxt/test/";
	char pnet_model_name[64];
	int class_num = BrowPathDirectory(models_path, class_dir);  //寻找文件夹路径下的子文件夹并存在std::vector<FileVector>,子文件的数目对应的就是类别的数目
	//载入pnet
	char pnet_dir[64];
	sprintf(pnet_dir, "%spnet/", models_path);  //这里的路径即为“D:\mtcnn\train\models\pnet\”
	BrowPathFiles(pnet_dir, ".caffemodel", pnet_model);  //将pnet_dir路径下后缀名为.caffemodel的文件名存储在pnet_model中，只存储文件名不包括路径部分
	std::sprintf(pnet_model_name, "%spnet/%s", models_path, pnet_model[pnet_model.size() - 1].name);  //选择最后一个模型文件，也就是最终训练的50000.caffemodel文件
	mtcnn_net[0] = Classifier(proto_path + "det1.prototxt", pnet_model_name, scale, mean, 1);  //存储第一个分类器，即PNet
	for (int i = 0; i < class_dir.size() - 1; i++){
		//载入rnet
		char class_dir_name[64], ro_model_name[64]; 
		std::sprintf(class_dir_name, "%s%s/rnet/", models_path, class_dir[i].name);  //class_dir_name形式为：“D:\mtcnn\train\models\0_book\rnet\”
		BrowPathFiles(class_dir_name, ".caffemodel", class_model);  //class_model中存储所有的rnet模型文件名
		std::sprintf(ro_model_name, "%s%s", class_dir_name, class_model[class_model.size() - 1].name);
		mtcnn_net.push_back(Classifier(proto_path + "det2.prototxt", ro_model_name, scale, mean, 1));
		//载入onet
		std::sprintf(class_dir_name, "%s%s/onet/", models_path, class_dir[i].name);
		BrowPathFiles(class_dir_name, ".caffemodel", class_model);
		std::sprintf(ro_model_name, "%s%s", class_dir_name, class_model[class_model.size() - 1].name);
		mtcnn_net.push_back(Classifier(proto_path + "det3.prototxt", ro_model_name, scale, mean, 1));
	}
	//Classifier pose("../../test_z.prototxt", "../../pose_z_iter_50000.caffemodel", scale, mean, 1);
	int camera_params[4] = { 850, 850, 640, 360 };
	while (cap.isOpened()){
		double time = getTickCount();  //用于计时
		cap >> img;
		if (img.empty()) break;
		Mat img1;
		img.copyTo(img1);
		Vector<candidate> canOfAll = mtcnn_multi(img, mtcnn_net, class_num);
		img.copyTo(img1);
		for (int i = 0; i < canOfAll.size(); ++i){
			candidate can = canOfAll[i];
			if (!can.use) continue;
			rectangle(img1, Point(can.ltx, can.lty), Point(can.rbx, can.rby), colors[can.cls - 1], 2);
			cv::putText(img1, class_dir[0].name, cv::Point(can.ltx + 5, can.lty + 18),
				0, 0.8, cv::Scalar(0, 0, 0), 2, 1);
			for (int i = 0; i < 8; i += 2)
				circle(img1, Point(can.pts[i], can.pts[i + 1]), 2, Scalar(0, 255, 255), 2);  //每个特征点上画一个圆点
			for (int i = 0; i < 8; i += 2)
				line(img1, Point(can.pts[i % 8], can.pts[(i + 1) % 8]),
				Point(can.pts[(i + 2) % 8], can.pts[(i + 3) % 8]), Scalar(0, 255, 255), 2);
			/*
			//显示pose
			int size = max(can.width, can.height), cx = (can.ltx + can.rbx) / 2, cy = (can.lty + can.rby) / 2;
			int nx1 = cx - size / 2, nx2 = cx + size / 2, ny1 = cy - size / 2, ny2 = cy + size / 2;
			nx1 = max(nx1, 0);
			nx2 = min(nx2, img1.cols - 1);
			ny1 = max(ny1, 0);
			ny2 = min(ny2, img1.rows - 1);
			Mat cropImg = img(Range(ny1, ny2), Range(nx1, nx2));
			resize(cropImg, cropImg, Size(48, 48));
			//计算box_info
			float box_info[4] = { 0 }, pts_ave[2] = { 0 };
			pts_ave[0] = (2 * can.ltx + 2 * can.rbx + 2 * can.width) / 4;
			pts_ave[1] = (2 * can.lty + 2 * can.rby + 2 * can.height) / 4;
			box_info[0] = (float)(pts_ave[0] - camera_params[2]) / camera_params[0];
			box_info[1] = (float)(pts_ave[1] - camera_params[3]) / camera_params[1];
			box_info[2] = (float)can.width / camera_params[0];
			box_info[3] = (float)can.height / camera_params[1];
			
			//cropImg = imread("../../5.jpg");//-0.048472 -0.014390 0.846720 0.478511
			//0.078445 -0.137162 0.729141 0.723968   17.742194
			box_info[0] = 0;
			box_info[1] = 0;
			box_info[2] = 0;
			box_info[3] = 0;
			
			pose.Forward(cropImg, box_info);
			//Blob<float> *xyz_blob = pose.GetBlob("conv9-1");
			Blob<float> *z_blob = pose.GetBlob("conv9-1");
			Blob<float> *q_blob = pose.GetBlob("conv9-2");
			//const float *xyz_data = xyz_blob->cpu_data();
			const float *z_data = z_blob->cpu_data();
			const float *q_data = q_blob->cpu_data();
			//double xyz[3] = { xyz_data[0], xyz_data[1], xyz_data[2] };
			double Q[4] = { q_data[0], q_data[1], q_data[2], q_data[3] };
			
			double temp = 0;
			for (int j = 0; j < 4; j++){
				temp += Q[j] * Q[j];
			}
			temp = sqrt(temp);
			for (int j = 0; j < 4; j++){
				Q[j] /= temp;
			}
			
			double pts[2] = { can.pts[0], can.pts[1] };
			double z = z_data[0];
			//cout << z << endl;

			//
			double box[6] = { 0, 14, 0, 20, -2, 0 };
			cv::Mat K(3, 3, CV_64FC1);
			cv::Mat RR(3, 3, CV_64FC1);

			double *p4 = K.ptr<double>();
			p4[0] = camera_params[0]; p4[1] = 0;  p4[2] = camera_params[2];
			p4[3] = 0;  p4[4] = camera_params[1]; p4[5] = camera_params[3];
			p4[6] = 0;  p4[7] = 0;  p4[8] = 1;
			double r[9];
			mtQ2R(Q, r);
			double *p2 = RR.ptr<double>();
			for (int i = 0; i < 3 * 3; i++)
				p2[i] = r[i];

			cv::Rect rect(can.ltx, can.lty, abs(can.rbx - can.ltx), abs(can.rby - can.lty));
			double XYZ[3];
			BoxTo3d(K, box, RR, rect, XYZ, nullptr);

			//draw_Proj_Obj3d1(img, Q, pts);
			//draw_Proj_Obj3d2(img1, Q, pts, z);
			//for (int i = 0; i < 4; i++) cout << Q[i] << "  ";
			//cout << "total:" << Q[0] * Q[0] + Q[1] * Q[1] + Q[2] * Q[2] + Q[3] * Q[3] << endl;
			draw_Proj_Obj3d3(img, Q, XYZ);
			*/
		}
		imshow("onet", img1);
		if (cv::waitKey(1) == 'q') break;
		time = (double)1000 * (getTickCount() - time) / getTickFrequency();
		std::printf("%.2fms\n", time);
	}
}