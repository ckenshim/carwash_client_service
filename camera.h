#ifndef CAMERA_H
#define CAMERA_H

#include <string>

class Camera
{
private:
    std::string		camera_id_;
    std::string		websafe_url_;
    std::string 	video_source_;
    bool 		is_active_;
    int 		motion_threshold_;
    

public:
    Camera();
    Camera(	const std::string & camera_id,
		const std::string & websafe_url,
		const std::string & video_source,
		const bool & is_active,
		const int & motion_threshold);
    ~Camera() {};

    std::string camera_id() const;
    void set_camera_id(const std::string & camera_id);
	
    std::string websafe_url() const;
    void set_websafe_url(const std::string & websafe_url);
    

    std::string video_source() const;
    void set_video_source(const std::string & video_source);


    bool is_active() const;
    void set_active(const bool & is_active);

    int motion_threshold() const;
    void set_motion_threshold(const int & motion_threshold);

};

#endif // CAMERA_H