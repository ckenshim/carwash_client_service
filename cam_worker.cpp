#include <iostream>
#include "ipcam_video_acquisition.h"


int main()
{
    bool work_ = true;
    vae::VideoAcquisition* ipcam_ = new vae::IpCamVideoAcquisition();

    ipcam_->set_url("carwash5.mp4");

    if(ipcam_->Init() != 0){
        std::cout << "Error: Initialization failed.";
        return -1;
    }
    AVFrame* pframe = NULL;
    while(work_){

        pframe = ipcam_->NextFrame();

        if(pframe!=NULL){
	   std::cout << "frame ready\n";
	    // process frame 
        }
    }
}
