#include "ipcam_video_acquisition.h"
#include <iostream>

namespace vae{

IpCamVideoAcquisition::IpCamVideoAcquisition()
{
}

IpCamVideoAcquisition::~IpCamVideoAcquisition()
{
        av_free(pFrame_);
        av_free(pFrameRGB_);
        // Close the codec
        avcodec_close(pCodecCtx_);
        // Close the video file
        avformat_close_input(&pFormatCtx_);
}



void IpCamVideoAcquisition::set_url(const std::string &url)
{
    url_=url;
}

int IpCamVideoAcquisition::Init()
{
    try{

        if(url_.empty()){
            std::cout << "Error: URL of the video source is empty\n";
            return -2;
        }
	std::cout << "Registering all codecs: av_register_all()\n";
        av_register_all();
        avformat_network_init();

        av_dict_set(&optionsDict_, "rtsp_transport", "tcp", 0);
        //av_dict_set(&optionsDict_, "r", "25", 0);


        // Open video source
        if (avformat_open_input(&pFormatCtx_, url_.c_str(), NULL, &optionsDict_) != 0){
	    std::cout << "Error: Couldn't open video source : " << url_ << "\n";
            return -1;
        }

        // Retrieve stream information
        if (avformat_find_stream_info(pFormatCtx_, NULL)<0){
	    std::cout << "Error: Couldn't find stream information\n";
            return -1;
        }

        // Dump information about file onto standard error
        av_dump_format(pFormatCtx_, 0, url_.c_str(), 0);

        // Find the first video stream
        videoStream_ = -1;
        for (unsigned int i = 0; i < pFormatCtx_->nb_streams; i++)
            if (pFormatCtx_->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStream_ = i;
                break;
            }
        if (videoStream_ == -1){
            std::cout << "Error: Didn't find a video stream\n";
            return -1;
        }

        // Get a pointer to the codec context for the video stream
        pCodecCtx_ = pFormatCtx_->streams[videoStream_]->codec;

        // Find the decoder for the video stream
        pCodec_ = avcodec_find_decoder(pCodecCtx_->codec_id);
        if (pCodec_ == NULL) {
	    std::cout << "Error: Unsupported codec.\n";
            return -1;
        }

        // Open codec
        if (avcodec_open2(pCodecCtx_, pCodec_, &optionsDict_)<0){
	    std::cout << "Error: Could not open codec\n";
            return -1;
        }


        std::cout << "Allocating video frame...\n";
        pFrame_ = av_frame_alloc();
        pFrameRGB_ = av_frame_alloc();

	std::cout << "Preparing context for conversion of decoded frame into RGB format\n";
        int dst_fmt = PIX_FMT_BGR24;
        int dst_w = pCodecCtx_->width;
        int dst_h = pCodecCtx_->height;

        int numBytes= avpicture_get_size((PixelFormat)dst_fmt,dst_w, dst_h);
        uint8_t *buffer = (uint8_t*)av_malloc(numBytes*sizeof(uint8_t));
        avpicture_fill((AVPicture*)pFrameRGB_, buffer, (PixelFormat)dst_fmt,
                       dst_w, dst_h);

        img_convert_ctx_= sws_getContext(
                    dst_w ,dst_h,
                    pCodecCtx_->pix_fmt,
                    dst_w, dst_h, (PixelFormat)dst_fmt,
                    SWS_BILINEAR, NULL, NULL, NULL);
        // end of preparing context

        return 0;
    }
    catch(std::exception &ex){
	std::cout << "Error: Unable to initialize video source\n";
    }

    return -1;
}


// Retrieves next frame in the stream in RGB format
AVFrame *IpCamVideoAcquisition::NextFrame()
{
    frameFinished_=0;
    while(!frameFinished_){
        if (av_read_frame(pFormatCtx_, &packet_) >= 0) {
            // Is this a packet from the video stream?
            if (packet_.stream_index == videoStream_) {
                // Decode video frame
                avcodec_decode_video2(pCodecCtx_, pFrame_, &frameFinished_,&packet_);

                // Did we get a video frame?
                if (frameFinished_) {

                    //av_frame_unref(pFrameRGB_);
                    // converting decoded frame into RGB frame
                    sws_scale(img_convert_ctx_, pFrame_->data, pFrame_->linesize,
                              0, pCodecCtx_->height,
                              pFrameRGB_->data,
                              pFrameRGB_->linesize);

                    pFrameRGB_->height = pFrame_->height;
                    pFrameRGB_->width = pFrame_->width;
                    pFrameRGB_->format = PIX_FMT_BGR24;
                    av_frame_unref(pFrame_);
                }
            }
            // Free the packet that was allocated by av_read_frame
            av_free_packet(&packet_);
        }
    }

    return pFrameRGB_;
}


}
