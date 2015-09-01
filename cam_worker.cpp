#include <string>
#include <iostream>
#include "ipcam_video_acquisition.h"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <ctime>
#include <chrono>
using namespace std::chrono;


int main(int argc, char* argv[])
{
    if (argc < 2){
	std::cout << "usage: camworker [source_url]\n";
        return -1;
    }

    bool work_ = true;
    vae::VideoAcquisition* ipcam_ = new vae::IpCamVideoAcquisition();
    std::string url = argv[1];
    ipcam_->set_url(url);

    if(ipcam_->Init() != 0){
        std::cout << "Error: Initialization failed.";
        return -1;
    }
	
    cv::Ptr<cv::BackgroundSubtractor> mog = cv::createBackgroundSubtractorMOG2();
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::Mat kernel_close = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(6, 6));

    cv::Mat im_gray;
    cv::Mat edges;
    cv::Mat frame_small;
    int counter = 0;

    AVFrame* pframe = NULL;
    while(work_){

        pframe = ipcam_->NextFrame();

        if(pframe!=NULL){
	   cv::Mat image(pframe->height, pframe->width, CV_8UC3, pframe->data[0]);
	   int new_w = image.size().width / 6;
	   int new_h = image.size().height / 6;
	   cv::resize(image, frame_small, cv::Size(new_w, new_h));
	   
	   cvtColor(frame_small, im_gray, CV_BGR2GRAY);

	   cv::Mat mask;
           mog->apply(im_gray, mask);
	   
	   //cv::Mat morph;

	   cv::morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
	   //cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel_close);
	   //cv::morphologyEx(morph, morph, cv::MORPH_OPEN, kernel_close);
           
	   //GaussianBlur(edges, edges, cv::Size(7, 7), 1.5, 1.5);
			
	   int blob = 0;

	   for (int i = 0; i < new_h; i++){
	       for (int j = 0; j < new_w; j++){
		    uchar value = mask.at<uchar>(i, j);
		    if (value != 0){
		        blob++;
		    }
		}
            }
  	    if (blob > 1500){
	        std::cout << blob << "\n";
		milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		cv::imwrite("5E5311FA-B1A1-472D_" + std::to_string(ms.count()) + ".jpg", image);
	    }
	    //imshow("morph", mask);
	    //imshow("real", frame);
	    //if (cv::waitKey(1) >= 0) break;
        }
    }
    return 0;
}
