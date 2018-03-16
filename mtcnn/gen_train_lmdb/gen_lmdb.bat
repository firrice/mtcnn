@echo off
"D:/project/caffe-master/Build/x64/Release/convert_imageset.exe" "" "train_12_label.txt" "../lmdb/train_lmdb12" --backend=lmdb --shuffle=true
"D:/project/caffe-master/Build/x64/Release/convert_imageset.exe" "" "train_24_label_0.txt" "../lmdb/train_lmdb24_0" --backend=mtcnn --shuffle=true
"D:/project/caffe-master/Build/x64/Release/convert_imageset.exe" "" "train_48_label_0.txt" "../lmdb/train_lmdb48_0" --backend=mtcnn --shuffle=true
pause