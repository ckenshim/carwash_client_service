g++ -I/usr/local/include -L/usr/lib -std=c++0x cam_worker.cpp ipcam_video_acquisition.cpp camera.cpp `pkg-config opencv --cflags --libs`  -lavformat -lavcodec -lavutil -lswscale -lm -lz -lpthread -lswresample -lrt -lPocoFoundation -lPocoNet -lPocoNetSSL -lPocoJSON -lstdc++
