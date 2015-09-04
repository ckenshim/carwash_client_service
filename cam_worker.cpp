
#include <iostream>
#include "ipcam_video_acquisition.h"
#include "camera.h"
#include <opencv2/opencv.hpp>
#include <math.h>
#include <ctime>
#include <chrono>
#include <sstream>
#include <string>
#include "Poco/Net/HTTPSClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/Context.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include <Poco/Net/HTTPCredentials.h>
#include "Poco/StreamCopier.h"
#include "Poco/NullStream.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"


using Poco::Net::HTTPSClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;
using Poco::StreamCopier;
using Poco::Path;
using Poco::URI;
using Poco::Exception;
using namespace std::chrono;


bool doRequest(Poco::Net::HTTPSClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response, Camera & cam);
void ObtainCameraSettings(const std::string & camera_id, Camera & cam);


int main(int argc, char* argv[])
{
    if (argc < 2){
	std::cout << "usage: camworker [camera_id]\n";
        return -1;
    }

    Camera camera;
    ObtainCameraSettings(argv[1], camera);

    bool work_ = true;
    vae::VideoAcquisition* ipcam_ = new vae::IpCamVideoAcquisition();
    std::string url = camera.video_source();
    ipcam_->set_url("rtsp://admin:ABYas2013@192.168.1.102:554/cam/realmonitor");

    if(ipcam_->Init() != 0){
        std::cout << "Error: Initialization failed.\n";
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



void ObtainCameraSettings(const std::string & camera_id, Camera & cam)
{
	try
	{
		std::string url = "https://image-receiver-dot-carwashauthority-1007.appspot.com/_ah/api/imagereceiverendpoints/v1/discoverCamera?val=" + camera_id;
		
		URI uri(url);
		std::string path(uri.getPathAndQuery());
		if (path.empty()) path = "/";

		const Poco::Net::Context::Ptr ptrContext( new Poco::Net::Context( Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH" ) );
		HTTPSClientSession session(uri.getHost(), uri.getPort(), ptrContext);
		HTTPRequest request(HTTPRequest::HTTP_GET, path, HTTPMessage::HTTP_1_1);
		HTTPResponse response;
		for (int i=0;i<5;i++){
			std::cout << "Sending request to server...\n";
			bool success = doRequest(session, request, response, cam);
			if(success){
				std::cout << "Success.\n";				
				break;
			}
			else{
				std::cout << "Failure. Repeating request...\n";
			}
		}	
		
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
		
	}

}

bool doRequest(Poco::Net::HTTPSClientSession& session, Poco::Net::HTTPRequest& request, Poco::Net::HTTPResponse& response, Camera & cam)
{
	session.sendRequest(request);
	std::istream& rs = session.receiveResponse(response);
	std::cout << response.getStatus() << " " << response.getReason() << std::endl;
	if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
	{
		std::ostringstream oss;
		StreamCopier::copyStream(rs, oss);
		
		std::string response = oss.str();

		Poco::JSON::Parser parser;
		Poco::Dynamic::Var result = parser.parse(response);
		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();
		Poco::Dynamic::Var camId = object->get("cameraId");
		Poco::Dynamic::Var sourceUrl = object->get("sourceUrl");
		Poco::Dynamic::Var urlSafeKey = object->get("urlSafeKey");
		Poco::Dynamic::Var isActive = object->get("isActive");
		Poco::Dynamic::Var threshold = object->get("threshold");

		if(!camId.isEmpty() && camId.isString()){
			cam.set_camera_id(camId.toString());
			std::cout << "camId: " << camId.toString() << "\n";
		}
		
		if(!sourceUrl.isEmpty() && sourceUrl.isString())
		{	
			cam.set_video_source(sourceUrl.toString());
			std::cout << "sourceUrl: " << sourceUrl.toString() << "\n";
		}	

		if(!urlSafeKey.isEmpty() && urlSafeKey.isString()){
			cam.set_websafe_url(urlSafeKey.toString());
			std::cout << "urlSafeKey: " << cam.websafe_url() << "\n";
		}
		
		if(!isActive.isEmpty() && isActive.isBoolean()){
			bool val = isActive.convert<bool>();	
			cam.set_active(val);
			std::cout << "isActive: " << val << "\n";
		}

		if(!threshold.isEmpty() && threshold.isString()){
			int val = threshold.convert<int>();	
			cam.set_motion_threshold(val);
			std::cout << "threshold: " << val << "\n";
		}

		return true;
	}
	else
	{
		Poco::NullOutputStream null;
		StreamCopier::copyStream(rs, null);
		return false;
	}
}



