#ifndef IPCAMVIDEOACQUISITION_H
#define IPCAMVIDEOACQUISITION_H


#include "video_acquisition.h"



namespace vae{

class IpCamVideoAcquisition : public VideoAcquisition
{
private:
    std::string url_;

    AVFormatContext *pFormatCtx_;

    AVCodecContext  *pCodecCtx_;
    AVCodec         *pCodec_;
    AVFrame         *pFrame_;
    AVFrame         *pFrameRGB_;
    AVFrame         *pFrameResized_;
    AVDictionary    *optionsDict_;
    AVPacket        packet_;

    struct SwsContext *img_convert_ctx_;
    struct SwsContext *sws_ctx_resize_;
    int             videoStream_;
    int             frameFinished_;
    double          resize_ratio_;

public:
    IpCamVideoAcquisition();
    ~IpCamVideoAcquisition();

    // VideoAcquisition interface
    void set_url(const std::string &url);
    int Init();
    AVFrame *NextFrame();

};

}
#endif // IPCAMVIDEOACQUISITION_H
