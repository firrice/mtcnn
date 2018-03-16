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
void draw_Proj_Obj3d1(cv::Mat& dst, double* Q, double* pts)
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
	cv::waitKey(0);
}
void BoxTo3d(cv::Mat K, double *box, cv::Mat R, cv::Rect rect, double *XYZ, int *box_id)
{
	double fx = K.ptr<double>()[0];
	double fy = K.ptr<double>()[4];
	double cx = K.ptr<double>()[2];
	double cy = K.ptr<double>()[5];

	double uv1_d[3] = { rect.x + rect.width / 2, rect.y + rect.height / 2, 1 };
	cv::Mat uv1(3, 1, CV_64FC1, uv1_d);
	cv::Mat R32; R.convertTo(R32, CV_64FC1);
	cv::Mat KR = K*R32;
	cv::Mat KR_1 = KR.inv();
	//
	cv::Mat T1 = -KR_1*uv1;
	cv::Mat T164; T1.convertTo(T164, CV_64FC1);
	double z = 100;
	T164 *= z;
	//
	//定义外包盒8个物方点坐标X
	int n = 8;
	double coord3ds[8 * 3];
	coord3ds[0] = box[0]; coord3ds[1] = box[2]; coord3ds[2] = box[4];
	coord3ds[3] = box[0]; coord3ds[4] = box[3]; coord3ds[5] = box[4];
	coord3ds[6] = box[1]; coord3ds[7] = box[3]; coord3ds[8] = box[4];
	coord3ds[9] = box[1]; coord3ds[10] = box[2]; coord3ds[11] = box[4];
	coord3ds[12] = box[0]; coord3ds[13] = box[2]; coord3ds[14] = box[5];
	coord3ds[15] = box[0]; coord3ds[16] = box[3]; coord3ds[17] = box[5];
	coord3ds[18] = box[1]; coord3ds[19] = box[3]; coord3ds[20] = box[5];
	coord3ds[21] = box[1]; coord3ds[22] = box[2]; coord3ds[23] = box[5];

	cv::Mat object_points(n, 3, CV_64FC1, coord3ds);
	cv::Mat pts_proj(n, 2, CV_64FC1);
	//projectPoints2(object_points2, rotation_matrix, translate_T, intrinsic_matrix, pts_proj);
	cv::projectPoints(object_points, R, -R*T164, K, cv::Mat(), pts_proj);
	double *proj_coords = pts_proj.ptr<double>();

	int iL = -1, iR = -1, iT = -1, iB = -1;
	double max_x = -1e+5, min_x = -max_x;
	double max_y = -1e+5, min_y = -max_y;
	int offset = 0;
	for (int i = 0; i < n; i++)
	{
		double ui = proj_coords[offset];
		if (ui>max_x)
		{
			iR = i;
			max_x = ui;
		}
		if (ui<min_x)
		{
			iL = i;
			min_x = ui;
		}
		double vi = proj_coords[offset + 1];
		if (vi>max_y)
		{
			iB = i;
			max_y = vi;
		}
		if (vi<min_y)
		{
			iT = i;
			min_y = vi;
		}
		//printf("%d %f,%f\n",i,ui,vi);
		offset += 2;
	}
	assert(iL >= 0 && iL < n);
	assert(iR >= 0 && iR < n);
	assert(iT >= 0 && iT < n);
	assert(iB >= 0 && iB < n);
	if (box_id)
	{
		box_id[0] = iL;
		box_id[1] = iR;
		box_id[2] = iT;
		box_id[3] = iB;
		//printf("[%d %d %d %d]\n",iL,iR,iT,iB);
	}
	//assert(max_x>0 && min_x>0 && iT >= 0 && iB >= 0);
	//
	double uKL = (rect.x - cx) / fx;
	double uKR = (rect.x + rect.width - cx) / fx;
	double vKT = (rect.y - cy) / fy;
	double vKB = (rect.y + rect.width - cy) / fy;
	double *r = R.ptr<double>();
	double A_d[12] = {
		uKL*r[6] - r[0], uKL*r[7] - r[1], uKL*r[8] - r[2],
		uKR*r[6] - r[0], uKR*r[7] - r[1], uKR*r[8] - r[2],
		vKT*r[6] - r[3], vKT*r[7] - r[4], vKT*r[8] - r[5],
		vKB*r[6] - r[3], vKB*r[7] - r[4], vKB*r[8] - r[5],
	};
	cv::Mat A(4, 3, CV_64FC1, A_d);
	double XBox_d[4] = {
		(uKL*r[6] - r[0]) * coord3ds[iL * 3] + (uKL*r[7] - r[1])*coord3ds[iL * 3 + 1] + (uKL*r[8] - r[2])*coord3ds[iL * 3 + 2],
		(uKR*r[6] - r[0]) * coord3ds[iR * 3] + (uKR*r[7] - r[1])*coord3ds[iR * 3 + 1] + (uKR*r[8] - r[2])*coord3ds[iR * 3 + 2],
		(vKT*r[6] - r[3]) * coord3ds[iT * 3] + (vKT*r[7] - r[4])*coord3ds[iT * 3 + 1] + (vKT*r[8] - r[5])*coord3ds[iT * 3 + 2],
		(vKB*r[6] - r[3]) * coord3ds[iB * 3] + (vKB*r[7] - r[4])*coord3ds[iB * 3 + 1] + (vKB*r[8] - r[5])*coord3ds[iB * 3 + 2]
	};
	cv::Mat XBox(4, 1, CV_64FC1, XBox_d);
	//std::cout << A<<"\n";
	//std::cout << XBox << "\n";
	cv::Mat Tr(3, 1, CV_64FC1, XYZ);
#if 1
	cv::solve(A, XBox, Tr, CV_SVD);
#else
	cv::Mat Zr(1, 1, CV_64FC1);
	cv::solve(A*T1, XBox, Zr, CV_SVD);
	Tr = T1*Zr.ptr<double>()[0];
#endif
}

//画三轴。参数dst：图像；q:四元数；pts：第一个特征点；z:Z坐标
void draw_Proj_Obj3d2(cv::Mat& dst, double* Q, double* pts, double z)
{
	cv::Mat object_points2(8, 3, CV_64FC1);
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
	double coord3ds[8 * 3];   //物方坐标
	coord3ds[0] = 0;  coord3ds[1] = 0; coord3ds[2] = 0;
	coord3ds[3] = 14; coord3ds[4] = 0; coord3ds[5] = 0;
	coord3ds[6] = 14;  coord3ds[7] = 20; coord3ds[8] = 0;
	coord3ds[9] = 0;  coord3ds[10] = 20; coord3ds[11] = 0;
	coord3ds[12] = 0; coord3ds[13] = 0; coord3ds[14] = -1;
	coord3ds[15] = 14; coord3ds[16] = 0; coord3ds[17] = -1;
	coord3ds[18] = 14; coord3ds[19] = 20; coord3ds[20] = -1;
	coord3ds[21] = 0;  coord3ds[22] = 20; coord3ds[23] = -1;
	double *p1 = object_points2.ptr<double>();
	for (int i = 0; i < 8 * 3; i++)
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

	double L = z;
	p3[0] = X * (-1) * L;
	p3[1] = Y * (-1) * L;
	p3[2] = Z * (-1) * L;
	double d1 = p3[0], d2 = p3[1], d3 = p3[2];

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
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[6], proj_coords[7]), CV_RGB(255, 0, 0), 2);
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[2], proj_coords[3]), CV_RGB(0, 255, 0), 2);
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(0, 0, 255), 2);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[2], proj_coords[3]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[2], proj_coords[3]), cv::Point(proj_coords[4], proj_coords[5]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[4], proj_coords[5]), cv::Point(proj_coords[6], proj_coords[7]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[6], proj_coords[7]), cv::Point(proj_coords[0], proj_coords[1]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[8], proj_coords[9]), cv::Point(proj_coords[10], proj_coords[11]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[10], proj_coords[11]), cv::Point(proj_coords[12], proj_coords[13]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[12], proj_coords[13]), cv::Point(proj_coords[14], proj_coords[15]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[14], proj_coords[15]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[2], proj_coords[3]), cv::Point(proj_coords[10], proj_coords[11]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[4], proj_coords[5]), cv::Point(proj_coords[12], proj_coords[13]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[6], proj_coords[7]), cv::Point(proj_coords[14], proj_coords[15]), CV_RGB(255, 0, 0), 2);

	cv::imshow("fps", dst);
	//cv::waitKey(0);
}

//画三轴。参数dst：图像；q:四元数；pts：第一个特征点；T：X,Y,Z
void draw_Proj_Obj3d3(cv::Mat& dst, double* Q, double* T)
{
	cv::Mat object_points2(8, 3, CV_64FC1);
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
	double coord3ds[8 * 3];   //物方坐标
	coord3ds[0] = 0;  coord3ds[1] = 0; coord3ds[2] = 0;
	coord3ds[3] = 14; coord3ds[4] = 0; coord3ds[5] = 0;
	coord3ds[6] = 14;  coord3ds[7] = 20; coord3ds[8] = 0;
	coord3ds[9] = 0;  coord3ds[10] = 20; coord3ds[11] = 0;
	coord3ds[12] = 0; coord3ds[13] = 0; coord3ds[14] = -1;
	coord3ds[15] = 14; coord3ds[16] = 0; coord3ds[17] = -1;
	coord3ds[18] = 14; coord3ds[19] = 20; coord3ds[20] = -1;
	coord3ds[21] = 0;  coord3ds[22] = 20; coord3ds[23] = -1;
	double *p1 = object_points2.ptr<double>();
	for (int i = 0; i < 8 * 3; i++)
		p1[i] = coord3ds[i];

	//旋转矩阵
	double *p2 = rotation_matrix.ptr<double>();
	for (int i = 0; i < 3 * 3; i++)
		p2[i] = R[i];

	//X,Y,Z
	double X = T[0];
	double Y = T[1];
	double Z = T[2];
	double *p3 = translate_T.ptr<double>();

	//double L = 20;
	p3[0] = X;
	p3[1] = Y;
	p3[2] = Z;

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
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[6], proj_coords[7]), CV_RGB(255, 0, 0), 2);
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[2], proj_coords[3]), CV_RGB(0, 255, 0), 2);
	cv::arrowedLine(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(0, 0, 255), 2);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[2], proj_coords[3]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[2], proj_coords[3]), cv::Point(proj_coords[4], proj_coords[5]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[4], proj_coords[5]), cv::Point(proj_coords[6], proj_coords[7]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[6], proj_coords[7]), cv::Point(proj_coords[0], proj_coords[1]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[0], proj_coords[1]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[8], proj_coords[9]), cv::Point(proj_coords[10], proj_coords[11]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[10], proj_coords[11]), cv::Point(proj_coords[12], proj_coords[13]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[12], proj_coords[13]), cv::Point(proj_coords[14], proj_coords[15]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[14], proj_coords[15]), cv::Point(proj_coords[8], proj_coords[9]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[2], proj_coords[3]), cv::Point(proj_coords[10], proj_coords[11]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[4], proj_coords[5]), cv::Point(proj_coords[12], proj_coords[13]), CV_RGB(255, 0, 0), 2);
	cv::line(dst, cv::Point(proj_coords[6], proj_coords[7]), cv::Point(proj_coords[14], proj_coords[15]), CV_RGB(255, 0, 0), 2);

	cv::imshow("fps", dst);
	cv::waitKey(1);
}