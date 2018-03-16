#coding=utf-8
#根据label.txt中的标注，产生12*12,24*24,48*48的一系列训练样本

import numpy as np 
import numpy.random as npr
import cv2
import os

def calculateIOU(rec1, rec2):  #计算IOU，即交集除以并集
	rec1_width = rec1[2] - rec1[0] + 1
	rec1_height = rec1[3] - rec1[1] + 1 
	rec2_width = rec2[2] - rec2[0] + 1 
	rec2_height = rec2[3] - rec2[1] + 1 
	rec1_area = rec1_height * rec1_width
	rec2_area = rec2_height * rec2_width

	max_rbx = max(rec1[2], rec2[2])
	min_ltx = min(rec1[0], rec2[0])
	overlay_width = max(rec1_width + rec2_width - (max_rbx - min_ltx) - 1 , 0)
	max_rdy = max(rec1[3], rec2[3])
	min_lty = min(rec1[1], rec2[1])
	overlay_height = max(rec1_height + rec2_height - (max_rdy - min_lty) - 1 , 0)
	overlay_area = overlay_height * overlay_width

	iou = overlay_area / float(rec2_area + rec1_area - overlay_area)
	return iou 

def genbox(scale1, scale2, rect): #scale1控制中心点的偏移，正相关；scale2控制宽高的放缩倍数，负相关，二者均为 < 1的正数
	gtcx = (rect[2] + rect[0]) / 2 #gt为groundtruth的缩写
	gtcy = (rect[3] + rect[1]) / 2
	gtWidth = rect[2] - rect[0]
	gtHeigth = rect[3] - rect[1]

	boxcx = gtcx + npr.randint(-gtWidth * scale1, gtWidth * scale1) if scale1 < 1 else gtcx
	boxcx = max(boxcx, 0)
	boxcy = gtcy + npr.randint(-gtHeigth * scale1, gtHeigth * scale1) if scale1 < 1 else gtcy
	boxWidth = npr.randint(gtWidth * scale2, gtWidth / 0.8) if scale2 < 1 else gtWidth
	boxHeight = boxWidth
	#boxHeight = npr.randint(gtHeigth * scale2, gtHeigth / 0.8) if scale2 < 1 else gtHeigth 

	box = [0, 0, 0, 0]
	box[0] = max(boxcx - boxWidth / 2, 0)
	box[1] = max(boxcy - boxHeight / 2, 0)
	box[2] = boxcx + boxWidth / 2
	box[3] = boxcy + boxHeight / 2
	return box 



pixelNums = [12, 24, 48]
imgTypes = ["negative", "part", "positive"]  #总共有三类样本：正、负、部分样本
savePath = [] #对应文件夹目录循序为12_negtive, 12_part, 12_positive, 24_.. 48_..

#创建文件夹
for pN in pixelNums:
	path = str(pN)
	if not os.path.exists(path):
			os.mkdir(path)
	for iT in imgTypes:
		path = str(pN) + "/" + str(pN) + "_" + iT + "/"
		savePath.append(path)  #savepath中的格式类似为/12/12_positive/
		if not os.path.exists(path):
			os.mkdir(path)

train_12_label = open("train_12_label.txt", "a")  #创建train_12_label.txt文件



class_label = 1
#该循环为图片循环
label_list = os.listdir("../make_label/")
for class_num in xrange(0, len(label_list) - 1):  #减4是因为有四个.py文件，减去4后的值即类别数
	label_name = "label_" + str(class_num) + ".txt"
	labeltxt = open("../make_label/" + label_name, 'r')
	labeltxt = labeltxt.readlines()
	train_24_label = open("train_24_" + label_name, "a")
	train_48_label = open("train_48_" + label_name, "a")
	txt = [train_12_label, train_24_label, train_48_label]
	for i in xrange(0,len(labeltxt)):
		label = labeltxt[i]
		label = label.split(' ')
		fileName = label[0]
		rect = map(int, label[1:5])
		pts = map(int, label[5:])
		img = cv2.imread('../' + fileName)
		img1 = img.copy()
		loop = 0

		#该循环为尺度循环，次数为3，分别是12， 24， 48
		for pixelNum in pixelNums:
			neg_num = 0
			par_num = 0
			pos_num = 0

			#negative样本
			#该循环为，特定图片的特定尺度下negative样本数量的循环，100次，产生100张样本
			while neg_num < 200:
				box = genbox(0.5, 0.3, rect)
				iou = calculateIOU(box, rect)
				if(iou < 0.3):
					crop_img = img[box[1] : box[3], box[0] : box[2], :]
					crop_img = cv2.resize(crop_img, (pixelNum, pixelNum), interpolation=cv2.INTER_LINEAR)
					thePath = savePath[loop] + fileName[16:-4]  + "_" + str(pixelNum) + "_" + str(neg_num) + "_" + str(class_num) +".jpg"   #形式如：“0_12_0_0.jpg”
					cv2.imwrite(thePath, crop_img)
					if pixelNum == 12 :
						txt[loop / 3].write(thePath + " %d \n" % (0))  #Pnet是负责分类
					elif pixelNum == 24:
						txt[loop / 3].write(thePath + " %d %d %d %d %d \n" % (0, -1, -1, -1, -1))  #Rnet负责分类和矩形框回归
					else :
						txt[loop / 3].write(thePath + 
							" %d %d %d %d %d %d %d %d %d %d %d %d %d\n" 
							% (0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1))  #Onet负责分类，矩形框回归以及特征点坐标回归
					neg_num +=1

			#产生part样本
			while par_num < 100:
				box = genbox(0.3, 0.8, rect)
				boxWidth = box[2] - box[0]
				boxHeight = box[3] - box[1]
				iou = calculateIOU(box, rect)

				if iou >= 0.4 and iou < 0.65:
					offsetltx = (rect[0] - box[0]) / float(boxWidth)
					offsetlty = (rect[1] - box[1]) / float(boxHeight)
					offsetrbx = (rect[2] - box[0]) / float(boxWidth)
					offsetrby = (rect[3] - box[1]) / float(boxHeight)
					offsetpts1x = (pts[0] - box[0]) / float(boxWidth)
					offsetpts1y = (pts[1] - box[1]) / float(boxHeight)
					offsetpts2x = (pts[2] - box[0]) / float(boxWidth)
					offsetpts2y = (pts[3] - box[1]) / float(boxHeight)
					offsetpts3x = (pts[4] - box[0]) / float(boxWidth)
					offsetpts3y = (pts[5] - box[1]) / float(boxHeight)
					offsetpts4x = (pts[6] - box[0]) / float(boxWidth)
					offsetpts4y = (pts[7] - box[1]) / float(boxHeight)
					crop_img = img[box[1] : box[3], box[0] : box[2], :]
					crop_img = cv2.resize(crop_img, (pixelNum, pixelNum), interpolation=cv2.INTER_LINEAR)
					thePath = savePath[loop + 1] + fileName[16:-4]  + "_" + str(pixelNum) + "_" + str(par_num) + "_" + str(class_num) +".jpg"
					cv2.imwrite(thePath, crop_img)
					if pixelNum == 12:
						txt[loop / 3].write(thePath + " %d \n" % (-1))
					elif pixelNum == 24:
						txt[loop / 3].write(thePath + " %d %f %f %f %f \n" % (-1, offsetltx, offsetlty, offsetrbx, offsetrby))
					else:
						txt[loop / 3].write(thePath + 
							" %d %f %f %f %f %f %f %f %f %f %f %f %f \n" 
							% (1, offsetltx, offsetlty, offsetrbx, offsetrby, 
								offsetpts1x, offsetpts1y, offsetpts2x, offsetpts2y, 
								offsetpts3x, offsetpts3y, offsetpts4x, offsetpts4y))
					par_num += 1

			#positive
			while pos_num < 100:
				box = genbox(0.2, 0.8, rect)
				boxWidth = box[2] - box[0]
				boxHeight = box[3] - box[1]
				iou = calculateIOU(box, rect)

				if iou >= 0.65:
					offsetltx = (rect[0] - box[0]) / float(boxWidth)
					offsetlty = (rect[1] - box[1]) / float(boxHeight)
					offsetrbx = (rect[2] - box[0]) / float(boxWidth)
					offsetrby = (rect[3] - box[1]) / float(boxHeight)
					offsetpts1x = (pts[0] - box[0]) / float(boxWidth)
					offsetpts1y = (pts[1] - box[1]) / float(boxHeight)
					offsetpts2x = (pts[2] - box[0]) / float(boxWidth)
					offsetpts2y = (pts[3] - box[1]) / float(boxHeight)
					offsetpts3x = (pts[4] - box[0]) / float(boxWidth)
					offsetpts3y = (pts[5] - box[1]) / float(boxHeight)
					offsetpts4x = (pts[6] - box[0]) / float(boxWidth)
					offsetpts4y = (pts[7] - box[1]) / float(boxHeight)
					crop_img = img[box[1] : box[3], box[0] : box[2], :]
					crop_img = cv2.resize(crop_img, (pixelNum, pixelNum), interpolation=cv2.INTER_LINEAR)
					thePath = savePath[loop + 2] + fileName[16:-4]  + "_" + str(pixelNum) + "_" + str(pos_num) + "_" + str(class_num) +".jpg"
					cv2.imwrite(thePath, crop_img)
					if pixelNum == 12:
						txt[loop / 3].write(thePath + " %d \n" % (1))
					elif pixelNum == 24:
						txt[loop / 3].write(thePath + " %d %f %f %f %f \n" % (1, offsetltx, offsetlty, offsetrbx, offsetrby))
					else:
						txt[loop / 3].write(thePath + 
							" %d %f %f %f %f %f %f %f %f %f %f %f %f \n" 
							% (1, offsetltx, offsetlty, offsetrbx, offsetrby, 
								offsetpts1x, offsetpts1y, offsetpts2x, offsetpts2y, 
								offsetpts3x, offsetpts3y, offsetpts4x, offsetpts4y))
					pos_num += 1
			loop = loop + 3
		print fileName, "ok!!"
	


	

