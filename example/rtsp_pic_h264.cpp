// RTSP Server

#include "xop/RtspServer.h"
#include "net/Timer.h"
#include <thread>
#include <memory>
#include <iostream>
#include <string>
#include "x264Encoder.h"


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

	int ReadFrame(char* in_buf, int in_buf_size, bool* end);
    
private:
	char *m_buf = NULL;
	int  m_buf_size = 0;
	int  m_bytes_used = 0;
	int  m_count = 0;
	x264Encoder x264_encoder_;
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

int PicFile::ReadFrame(char* in_buf, int in_buf_size, bool* end)
{
	frame_ = cv::imread(path_);
	// return 71000;
	x264_encoder_.Create(frame_.cols, frame_.rows, frame_.channels());
	int size = x264_encoder_.EncodeOneFrame(frame_);
	uchar* encoder = x264_encoder_.GetEncodedFrame();

	int res_size = (size<=in_buf_size ? size : in_buf_size);

	memcpy(in_buf, encoder, res_size);

	*end = false;
	x264_encoder_.Destory();
	return res_size;
}


void SendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id, PicFile* pic_file);

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

	std::string suffix = "live";
	std::string ip = "127.0.0.1";
	std::string port = "8554";
	std::string rtsp_url = "rtsp://" + ip + ":" + port + "/" + suffix;
	
	std::shared_ptr<xop::EventLoop> event_loop(new xop::EventLoop());
	std::shared_ptr<xop::RtspServer> server = xop::RtspServer::Create(event_loop.get());

	if (!server->Start("0.0.0.0", atoi(port.c_str()))) {
		printf("RTSP Server listen on %s failed.\n", port.c_str());
		return 0;
	}

#ifdef AUTH_CONFIG
	server->SetAuthConfig("-_-", "admin", "12345");
#endif

	xop::MediaSession *session = xop::MediaSession::CreateNew("live"); 
	session->AddSource(xop::channel_0, xop::H264Source::CreateNew()); 
	//session->StartMulticast(); 
	session->AddNotifyConnectedCallback([] (xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port){
		printf("RTSP client connect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
	});
   
	session->AddNotifyDisconnectedCallback([](xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port) {
		printf("RTSP client disconnect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
	});

	xop::MediaSessionId session_id = server->AddSession(session);
         
	std::thread t1(SendFrameThread, server.get(), session_id, &pic_file);
	t1.detach(); 

	std::cout << "Play URL: " << rtsp_url << std::endl;

	while (1) {
		xop::Timer::Sleep(100);
	}

	getchar();
	return 0;
}

void SendFrameThread(xop::RtspServer* rtsp_server, xop::MediaSessionId session_id, PicFile* pic_file)
{       
	int buf_size = 2000000;
	std::unique_ptr<uint8_t> frame_buf(new uint8_t[buf_size]);

	while(1) {
		bool end_of_frame = false;
		int frame_size = pic_file->ReadFrame((char*)frame_buf.get(), buf_size, &end_of_frame);
		if(frame_size > 0) {
			xop::AVFrame videoFrame = {0};
			videoFrame.type = 0; 
			videoFrame.size = frame_size;
			videoFrame.timestamp = xop::H264Source::GetTimestamp();
			videoFrame.buffer.reset(new uint8_t[videoFrame.size]);    
			memcpy(videoFrame.buffer.get(), frame_buf.get(), videoFrame.size);
			rtsp_server->PushFrame(session_id, xop::channel_0, videoFrame);
		}
		else {
			break;
		}
      
		xop::Timer::Sleep(40); 
	};
}
