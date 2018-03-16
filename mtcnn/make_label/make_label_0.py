#coding=utf-8
#对样本数据画框以及标注点，生成PATH LX LY RX RY PTS 的label.txt文件

import cv2 
import os

filesDir = "../mydata/Mathbook/"
savsDir = "mydata/Mathbook/"
allFiles = os.listdir(filesDir)
labeltxt = open("label_0.txt", "a")

ltPoint = [] #左上角坐标
rdPoint = [] #右下角坐标
pts = []
obj = [] #第一项放左上角坐标，第二项放右下角坐标，第三项放pts
img1 = cv2.imread(filesDir + allFiles[0])

img2 = img1.copy()

def onMouse(event, x, y, flags, params):
	global ltPoint, rdPoint, pts, img2, obj, winName
	
	if event == cv2.EVENT_LBUTTONDOWN:  #如果鼠标左键按下
		if len(obj) >= 2 and len(obj) <= 5 and x > ltPoint[0] and x < rdPoint[0] and y > ltPoint[1] and y < rdPoint[1]:  #插入特征点
			pts = (x, y)
			obj.append(pts)
			cv2.circle(img2, pts, 2, (0, 255, 0), 2)  #以特征点为圆心画一个半径为2的圆
			cv2.imshow(winName, img2)
		elif len(obj) == 0:  #插入左上角坐标
			ltPoint = (x, y)
			obj.append(ltPoint)
		elif len(obj) == 1:  #插入右下角坐标          
			rdPoint = (x, y)
			obj.append(rdPoint)

	if event == cv2.EVENT_MOUSEMOVE:
		if len(obj) == 0:
			ltPoint = (x, y)
			img2 = img1.copy()
			cv2.line(img2, (x, 0), (x, img2.shape[0]), (255, 0, 0), 1)  #这两句是为了画两条十字交叉线
			cv2.line(img2, (0, y), (img2.shape[1], y), (255, 0, 0), 1)
		elif len(obj) == 1:
			rdPoint = (x, y)
			img2 = img1.copy()
			cv2.rectangle(img2, ltPoint, rdPoint, (0, 0, 255), 1)
		cv2.imshow(winName, img2)

def saveInfo(obj):  #label中包括左上角和右下角坐标以及四个特征点的坐标
	if len(obj) == 6:
		labeltxt.write(savsDir + winName + " %d %d %d %d %d %d %d %d %d %d %d %d\n" % (obj[0][0], obj[0][1], 
			obj[1][0], obj[1][1], obj[2][0], obj[2][1], obj[3][0], obj[3][1], 
			obj[4][0], obj[4][1], obj[5][0], obj[5][1]))

fileIndex = 0
unMarkFiles = allFiles[:]
while True:
	fileIndex = fileIndex if fileIndex >= 0 else len(allFiles) - 1  #可以在图片文件夹中的所有的图片索引进行循环索引
	fileIndex = fileIndex if fileIndex < len(allFiles) else 0
	imgName = allFiles[fileIndex]
	winName = str(imgName)
	img1 = cv2.imread(filesDir + imgName)
	img2 = img1.copy()
	cv2.imshow(winName, img2)
	cv2.setMouseCallback(winName, onMouse)
	key = cv2.waitKey()
	if key == ord('q'):  #q键退出
		break
	if key == ord('s'): #s键保存
		if len(obj) == 6:
			fileIndex += 1
			saveInfo(obj)
			unMarkFiles.remove(winName)
			print "未标注的图片为：", unMarkFiles  #输出剩下的图片
	if unMarkFiles==[]: #如果全部图片标注完成就结束标注
		break;
	if key == ord('n'):  #n键直接跳到后面一张图片
		fileIndex += 1
	if key == ord('p'):  #p键直接调到前面一张图片
		fileIndex -= 1
	obj = []
	cv2.destroyAllWindows()
labeltxt.close()