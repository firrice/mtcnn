# mtcnn
该mtcnn工程基于修改过的caffe（修改之处，就是在caffe中加入了新的数据结构：mtcnnDatum，原始的只有Datum），该项目实现了基于mtcnn的单类检测及边缘检测，并且很容易推广到多类目标。
下面对该项目的文件结构做一下梳理：
（后面将会加上修改过的caffe）
一、训练阶段
文件结构如下：
（1）video2pic.py实现从一段视频中每隔10帧提取一帧，从而获得原始样本存储到“mydata”文件夹中（视频文件放在同级目录下）；
（2）“make_label”文件夹中的make_label_0.py实现对单类物体样本图片的标注，如果有多类的话，就把这个文件复制几次就行了（之所以这么做，是由mtcnn进行检测时候的机制决定的），制作好的label.txt格式为：“mydata/Mathbook/Mathbook_0.jpg 330 141 889 542 528 169 363 312 733 496 865 302”，图片路径、左上角和右下角坐标，四个特征点pts的坐标；
（3）然后是“get_train_lmdb”文件夹，其中的“gen_train_label.py”对原始样本进行数据增强（使用平移，缩放等等），形成三类样本（positive、negative、part），并且有三类尺寸（12、24、48），然后使用“gen_lmdb.bat”生成对应的LMDB文件；
（4）之后就用"train"文件夹中的.bat进行训练，训练顺序为：“统一训练Pnet-》分别单独训练各类的Rnet-》分别单独训练各类的Onet”，其中训练用的到的.prototxt文件均在prototxt文件中；
（5）训练，生成的models在“train->models”中；


二、测试阶段：
使用“classtest”文件夹中的程序进行测试
