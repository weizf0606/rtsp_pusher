#ifndef PIC_RTSP_PUSHER_H
#define PIC_RTSP_PUSHER_H

#include <thread>
#include <memory>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp> 
#include <functional>

using namespace cv;


using FetchImageFunction = std::function<int(cv::Mat&)>;


namespace rtsp
{


struct ServerInfo;


class PicRtspPusher{
public:
PicRtspPusher();
~PicRtspPusher();
PicRtspPusher(std::string url_suffix);
int init(std::string ip, uint16_t port, uint32_t sleep_time_msec, FetchImageFunction fun);
void start();
void stop();

private:
int readFrame(char* in_buf, int in_buf_size, bool* end, cv::Mat& frame);
void sendFrameThread(FetchImageFunction fun);
std::string url_suffix_;
std::thread thread_;
bool enable_image_;
std::mutex mutex_;
std::mutex enable_image_mutex_;
uint32_t sleep_time_msec_;

ServerInfo *srv_info_;

};




}

#endif
