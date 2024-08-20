// RTSP Server

#include "xop/RtspServer.h"
#include "net/Timer.h"
#include <thread>
#include <memory>
#include <iostream>
#include <string>
#include "pic_rtsp_pusher.h"
#include <functional>


using namespace cv;
class PicFile
{
public:
	PicFile();
	~PicFile();

	bool Open(const char *path);
	void Close();
	bool IsOpened() const
	{ return true; }

    int handle_picture(cv::Mat& out);

private:
	char *m_buf = NULL;
	int  m_buf_size = 0;
	int  m_bytes_used = 0;
	int  m_count = 0;
	Mat frame_ ;
	std::string path_;
	Mat frame_cpy_;

};

PicFile::PicFile()
{
	
}

PicFile::~PicFile()
{

}

bool PicFile::Open(const char *path)
{
	path_ = path;
	frame_ = cv::imread(path_);
	if (frame_.empty()) {
		return false;
	} 
	return true;
}

void PicFile::Close()
{

}


int PicFile::handle_picture(cv::Mat& out)
{
    out = frame_;
    return 0;
}


int main(int argc, char **argv)
{	
	if(argc != 2) {
		printf("Usage: %s test.h264 \n", argv[0]);
		return 0;
	}

	PicFile pic_file;
	if(!pic_file.Open(argv[1])) {
		printf("Open %s failed.\n", argv[1]);
		return 0;
	}

    FetchImageFunction fun = std::bind(&PicFile::handle_picture, pic_file, std::placeholders::_1);

    rtsp::PicRtspPusher pusher("live");
    std::string suffix = "live";
	std::string ip = "127.0.0.1";
	int port = 8554;
    pusher.init("0.0.0.0", port, 40, fun); // arm 平台上这里输入必须是 "0.0.0.0" 而不能是 "127.0.0.1"
    pusher.start();
    
	while (1) {
		xop::Timer::Sleep(100);
	}

	getchar();
	return 0;
}
