/* $Id$ */

#ifndef VIDEO_VIDEO_DRIVER_HPP
#define VIDEO_VIDEO_DRIVER_HPP

#include "../driver.h"

class VideoDriver: public Driver {
public:
	virtual void MakeDirty(int left, int top, int width, int height) = 0;

	virtual void MainLoop() = 0;

	virtual bool ChangeResolution(int w, int h) = 0;

	virtual void ToggleFullscreen(bool fullscreen) = 0;
};

class VideoDriverFactoryBase: public DriverFactoryBase {
};

template <class T>
class VideoDriverFactory: public VideoDriverFactoryBase {
public:
	VideoDriverFactory() { this->RegisterDriver(((T *)this)->GetName(), Driver::DT_VIDEO, ((T *)this)->priority); }

	/**
	 * Get the long, human readable, name for the Driver-class.
	 */
	const char *GetName();
};

extern VideoDriver *_video_driver;

#endif /* VIDEO_VIDEO_DRIVER_HPP */
