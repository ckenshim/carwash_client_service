#include "camera.h"


Camera::Camera():
	camera_id_(""),
	websafe_url_(""),
	video_source_(""),
	is_active_(true),
	motion_threshold_(50)
{}



Camera::Camera(	const std::string & camera_id,
		const std::string & websafe_url,
		const std::string & video_source,
		const bool & is_active,
		const int & motion_threshold):
	camera_id_(camera_id),
	websafe_url_(websafe_url),
	video_source_(video_source),
	is_active_(is_active),
	motion_threshold_(motion_threshold)
{}


std::string Camera::camera_id() const
{
	return camera_id_;
}

void Camera::set_camera_id(const std::string & camera_id)
{
	camera_id_=camera_id;
}


std::string Camera::websafe_url() const
{
	return websafe_url_;
}

void Camera::set_websafe_url(const std::string & websafe_url)
{
	websafe_url_=websafe_url;
}
    

std::string Camera::video_source() const
{
	return video_source_;
}

void Camera::set_video_source(const std::string & video_source)
{
	video_source_ = video_source;
}


bool Camera::is_active() const
{
	return is_active_;
}

void Camera::set_active(const bool & is_active)
{
	is_active_=is_active;
}

int Camera::motion_threshold() const
{
	return motion_threshold_;
}
    
void Camera::set_motion_threshold(const int & motion_threshold)
{
	motion_threshold_=motion_threshold;
}
