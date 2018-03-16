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
//nms�㷨������������Ҫ�������ֱ���nms�ͼ���iou
#pragma region nms

bool comp(candidate _c1, candidate _c2){
	return _c1.conf > _c2.conf;
}

//����iou
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

//��nms���Ŀ�use���Ա����Ϊfalse������Ϊtrue
void nms(Vector<candidate> &_canOfScale, float _nmsThreshold){
	float nmsThreshold = _nmsThreshold;
	//����conf�Ӵ�С����
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

//��ȡ��߶�
vector<float> getScales(int _width, int _height, int _minCandidate = 80){
	vector<float> scales;
	int min = _width < _height ? _width : _height;
	float factor = 0.709, scale = (float)12 / _minCandidate;

	//���뱣֤min{��,��} * scale >12 ,����forwardʱ�����
	while (min*scale > 12){
		scales.push_back(scale);
		scale *= 0.709;
	}
	return scales;
}
//����mtcnn�㷨
Vector<candidate> mtcnn_multi(Mat img, Vector<Classifier> &mtcnn_net, int class_num){
	int img_size[2] = { img.cols, img.rows };
	vector<float> scales = getScales(img_size[0], img_size[1]);  //��ȡͼ��������еĸ�������ϵ��
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
		Blob<float>* b0 = mtcnn_net[0].GetBlob("prob1");  //���յġ�prob1������ʽ��img_num * class_num * height * width����������ͼ�ϵ�ÿһ����ľʹ洢��ÿһ���������Ŷ�
		vector<int> shape = b0->shape();
		vector<cv::Mat> cls;
		for (int n = 0; n<class_num; n++)  //cls��������class_num��feature map�洢ÿһ��
			cls.push_back(*new cv::Mat(shape[2], shape[3], CV_32F, b0->mutable_cpu_data() + n*shape[2] * shape[3]));
		Vector<candidate> cansOfScale;
		for (int row = 0; row < shape[2]; row++){
			for (int col = 0; col < shape[3]; col++){
				vector<float> conf(class_num, 0);//�洢ÿ���������Ŷ�
				int conf_cls = 0, flag = 0;  //conf_cls�д洢������Ŷ��������
				float max_conf = 0; //�������Ŷ�
				for (int n = 1; n < class_num; n++){
					conf[n] = cls[n].at<float>(row, col);  //�ҵ����յ�prob1�ϵ�ÿ���������ÿ���������Ŷ�
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
					//������ÿ�����껹ԭ��ԭͼ�ϣ�*2����Ϊforward�����д���һ���ػ��㣬2�������Ϊ3*3�ľ���㣬����ֱ��*2��ԭ��
					//forward֮ǰ��resize֮���ͼ��Ķ�Ӧ���꣬���ùܾ�������������任����Ϊ���������Ҳ����
					//���˼������ض��ѡ�/scale�ǻ�ԭ��resize֮ǰҲ����ԭͼ�Ķ�Ӧ����
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
			canOfAll.push_back(can);  //canOfAll�д洢���о���nms���can����ͬһ�߶��£�
		}
	}
	nms(canOfAll, 0.4);  //ԭͼ�ڲ�ͬ�߶��¾���nms���proposal���������ٽ���һ��nms��row86-row89�У�ʹ�����ֻص���ͬһ�߶ȣ�
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
		//����can�����ѡ�����ģ��
		Blob<float> *prob = NULL, *coord = NULL;
		for (int n = 0; n < class_num; n++){  //����lableʱ��-1ʱpart�࣬0�Ǳ����࣬1�������������ࣨclass_num���Ǽ��ϱ�����ģ�
			if (can->cls == n + 1){  //can->cls������������𣨴�1�����棬�����������ࣩ
				mtcnn_net[2 * n + 1].Forward(cropImg);  //��n = 1����������can->clsΪ2ʱ��ʹ��mtcnn_net[3]����ǰ�򴫲������ڶ��������rnet����
				prob = mtcnn_net[2 * n + 1].GetBlob("prob1");  //�������ĸ���
				coord = mtcnn_net[2 * n + 1].GetBlob("conv5-2");  //bbox�Ļع���
				break;
			}
		}
		conf = *(prob->mutable_cpu_data() + 1);
		can->use = false;
		if (conf > 0.5){
			//�ع�����
			//�������꣬��֤��������ȷ�ķ�Χ�ڣ��ı�������֮����Ӧ�Ŀ�ߣ����ҲӦ�øı�,confҲ��Ҫ��
			const float *data = coord->cpu_data();
			int can_ltx = can->ltx, can_lty = can->lty;
			can->ltx = data[0] * can->width + can_ltx;  //��Ϊ��ѵ����ʱ���õ���ƫ��������������Ҫ��ô����
			can->ltx = canOfAll[i].ltx > 0 ? canOfAll[i].ltx : 0;  //�߽��ж�
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
		ny2 = min(ny2, img1.rows - 1);  //��Ϊ֮ǰ��rnet�ж�bbox����nms��������������һ���ּ����˱߽��ж�
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
	cv::VideoCapture cap("video/Mathbook.mp4");  //cap�д洢�����Ƶ����
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
	int class_num = BrowPathDirectory(models_path, class_dir);  //Ѱ���ļ���·���µ����ļ��в�����std::vector<FileVector>,���ļ�����Ŀ��Ӧ�ľ���������Ŀ
	//����pnet
	char pnet_dir[64];
	sprintf(pnet_dir, "%spnet/", models_path);  //�����·����Ϊ��D:\mtcnn\train\models\pnet\��
	BrowPathFiles(pnet_dir, ".caffemodel", pnet_model);  //��pnet_dir·���º�׺��Ϊ.caffemodel���ļ����洢��pnet_model�У�ֻ�洢�ļ���������·������
	std::sprintf(pnet_model_name, "%spnet/%s", models_path, pnet_model[pnet_model.size() - 1].name);  //ѡ�����һ��ģ���ļ���Ҳ��������ѵ����50000.caffemodel�ļ�
	mtcnn_net[0] = Classifier(proto_path + "det1.prototxt", pnet_model_name, scale, mean, 1);  //�洢��һ������������PNet
	for (int i = 0; i < class_dir.size() - 1; i++){
		//����rnet
		char class_dir_name[64], ro_model_name[64]; 
		std::sprintf(class_dir_name, "%s%s/rnet/", models_path, class_dir[i].name);  //class_dir_name��ʽΪ����D:\mtcnn\train\models\0_book\rnet\��
		BrowPathFiles(class_dir_name, ".caffemodel", class_model);  //class_model�д洢���е�rnetģ���ļ���
		std::sprintf(ro_model_name, "%s%s", class_dir_name, class_model[class_model.size() - 1].name);
		mtcnn_net.push_back(Classifier(proto_path + "det2.prototxt", ro_model_name, scale, mean, 1));
		//����onet
		std::sprintf(class_dir_name, "%s%s/onet/", models_path, class_dir[i].name);
		BrowPathFiles(class_dir_name, ".caffemodel", class_model);
		std::sprintf(ro_model_name, "%s%s", class_dir_name, class_model[class_model.size() - 1].name);
		mtcnn_net.push_back(Classifier(proto_path + "det3.prototxt", ro_model_name, scale, mean, 1));
	}
	//Classifier pose("../../test_z.prototxt", "../../pose_z_iter_50000.caffemodel", scale, mean, 1);
	int camera_params[4] = { 850, 850, 640, 360 };
	while (cap.isOpened()){
		double time = getTickCount();  //���ڼ�ʱ
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
				circle(img1, Point(can.pts[i], can.pts[i + 1]), 2, Scalar(0, 255, 255), 2);  //ÿ���������ϻ�һ��Բ��
			for (int i = 0; i < 8; i += 2)
				line(img1, Point(can.pts[i % 8], can.pts[(i + 1) % 8]),
				Point(can.pts[(i + 2) % 8], can.pts[(i + 3) % 8]), Scalar(0, 255, 255), 2);
			/*
			//��ʾpose
			int size = max(can.width, can.height), cx = (can.ltx + can.rbx) / 2, cy = (can.lty + can.rby) / 2;
			int nx1 = cx - size / 2, nx2 = cx + size / 2, ny1 = cy - size / 2, ny2 = cy + size / 2;
			nx1 = max(nx1, 0);
			nx2 = min(nx2, img1.cols - 1);
			ny1 = max(ny1, 0);
			ny2 = min(ny2, img1.rows - 1);
			Mat cropImg = img(Range(ny1, ny2), Range(nx1, nx2));
			resize(cropImg, cropImg, Size(48, 48));
			//����box_info
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