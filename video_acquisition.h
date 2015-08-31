#ifndef VIDEOACQUISITION_H
#define VIDEOACQUISITION_H


#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
extern "C" {
#include <stdint.h>
}
#endif


extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
}

#include <stddef.h>
#include <string>

namespace vae{

class  VideoAcquisition
{
public:

    virtual ~VideoAcquisition(){}

    //Sets URL of the video source
    virtual void set_url(const std::string & url)=0;

    // Initializes video source
    virtual int Init()=0;

    // Retrieves next frame in the stream in RGB format
    virtual AVFrame* NextFrame()=0;
};

}
#endif // VIDEOACQUISITION_H
