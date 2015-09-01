g++ -I/usr/local/include -std=c++0x cam_worker.cpp ipcam_video_acquisition.cpp `pkg-config opencv --cflags --libs`  -lavformat -lavcodec -lavutil -lswscale -lm -lz -lpthread -lswresample -lrt
