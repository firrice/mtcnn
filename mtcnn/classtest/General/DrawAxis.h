#include <opencv2\opencv.hpp>


//将四元数Q转为旋转矩阵R
void mtQ2R(double *q, double *r)
{
	r[0] = q[0] * q[0] + q[1] * q[1] - q[2] * q[2] - q[3] * q[3];
	r[1] = 2.0*(q[1] * q[2] + q[0] * q[3]);
	r[2] = 2.0*(q[1] * q[3] - q[0] * q[2]);
	r[3] = 2.0*(q[1] * q[2] - q[0] * q[3]);
	r[4] = q[0] * q[0] - q[1] * q[1] + q[2] * q[2] - q[3] * q[3];
	r[5] = 2.0*(q[2] * q[3] + q[0] * q[1]);
	r[6] = 2.0*(q[1] * q[3] + q[0] * q[2]);
	r[7] = 2.0*(q[2] * q[3] - q[0] * q[1]);
	r[8] = q[0] * q[0] - q[1] * q[1] - q[2] * q[2] + q[3] * q[3];
}

//画三轴。参数dst：图像；q:四元数；pts：第一个特征点
void draw_Proj_Obj3d(cv::Mat& dst, double* Q, double* pts)
{
	cv::Mat object_points2(4, 3, CV_64FC1);
	cv::Mat rotation_matrix(3, 3, CV_64FC1);
	cv::Mat translate_T(3, 1, CV_64FC1);
	cv::Mat translation_vectors(3, 1, CV_64FC1);
	cv::Mat intrinsic_matrix(3, 3, CV_64FC1);
	cv::Mat distortion_coeffe(1, 4, CV_64FC1);
	cv::Mat pts_proj(4, 2, CV_64FC1);
	//相机内参
	double fx = 850;
	double fy = 850;
	double cx = 640;
	double cy = 360;

	double R[9];
	mtQ2R(Q, R);

	//物方坐标
	double coord3ds[4 * 3];   //物方坐标
	coord3ds[0] = 0;  coord3ds[1] = 0; coord3ds[2] = 0;
	coord3ds[3] = 4; coord3ds[4] = 0; coord3ds[5] = 0;
	coord3ds[6] = 0;  coord3ds[7] = 4; coord3ds[8] = 0;
	coord3ds[9] = 0;  coord3ds[10] = 0; coord3ds[11] = 4;
	double *p1 = object_points2.ptr<double>();
	for (int i = 0; i < 3 * 4; i++)
		p1[i] = coord3ds[i];

	//旋转矩阵
	double *p2 = rotation_matrix.ptr<double>();
	for (int i = 0; i < 3 * 3; i++)
		p2[i] = R[i];

	//X,Y,Z
	double X = pts[0] * R[0] / fx + pts[1] * R[3] / fy
		- R[0] * cx / fx - R[3] * cy / fy + R[6];
	double Y = pts[0] * R[1] / fx + pts[1] * R[4] / fy
		- R[1] * cx / fx - R[4] * cy / fy + R[7];
	double Z = pts[0] * R[2] / fx + pts[1] * R[5] / fy
		- R[2] * cx / fx - R[5] * cy / fy + R[8];
	double *p3 = translate_T.ptr<double>();

	double L = 20;
	p3[0] = X * (-1) * L;
	p3[1] = Y * (-1) * L;
	p3[2] = Z * (-1) * L;

	translation_vectors = -rotation_matrix * translate_T;

	//相机内参矩阵
	double *p4 = intrinsic_matrix.ptr<double>();
	p4[0] = fx; p4[1] = 0;  p4[2] = cx;
	p4[3] = 0;  p4[4] = fy; p4[5] = cy;
	p4[6] = 0;  p4[7] = 0;  p4[8] = 1;

	//相机畸变参数
	double* p5 = distortion_coeffe.ptr<double>();
	for (int i = 0; i < 1 * 4; i++)
		p5[i] = 0;

	//反投影，在图像上画三个轴
	cv::projectPoints(object_points2, rotation_matrix, translation_vectors, intrinsic_matrix, distortion_coeffe, pts_proj);
	double* proj_coords = pts_proj.ptr<double>();
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[2], proj_coords[3]), CV_RGB(255, 0, 0), 3);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[4], proj_coords[5]), CV_RGB(0, 255, 0), 3);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[6], proj_coords[7]), CV_RGB(0, 0, 255), 3);

	cv::imshow("fps", dst);
	//cv::waitKey(0);
}