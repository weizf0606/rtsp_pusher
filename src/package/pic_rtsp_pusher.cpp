#include "xop/RtspServer.h"
#include "x264Encoder.h"
#include "net/Timer.h"
#include "pic_rtsp_pusher.h"


namespace rtsp
{

struct ServerInfo
{
xop::MediaSession * session; 
std::shared_ptr<xop::EventLoop> event_loop;
std::shared_ptr<xop::RtspServer> server;
x264Encoder * x264_encoder;
};

PicRtspPusher::PicRtspPusher()
{   
    url_suffix_ = "live";
	srv_info_ = new ServerInfo();
    srv_info_->session = xop::MediaSession::CreateNew(url_suffix_); 
    srv_info_->event_loop = std::make_shared<xop::EventLoop>();
    srv_info_->server = xop::RtspServer::Create(srv_info_->event_loop.get());
    srv_info_->x264_encoder = new x264Encoder();
    sleep_time_msec_ = 40;
}

PicRtspPusher::~PicRtspPusher()
{
    delete srv_info_->session;
    delete srv_info_->x264_encoder;
    delete srv_info_;
}

PicRtspPusher::PicRtspPusher(std::string url_suffix)
{   
    url_suffix_ = url_suffix;
	srv_info_ = new ServerInfo();
    srv_info_->session = xop::MediaSession::CreateNew(url_suffix_); 
    srv_info_->event_loop = std::make_shared<xop::EventLoop>();
    srv_info_->server = xop::RtspServer::Create(srv_info_->event_loop.get());
    srv_info_->x264_encoder = new x264Encoder();
    sleep_time_msec_ = 40;
}


int PicRtspPusher::init(std::string ip, uint16_t port, uint32_t sleep_time_msec,FetchImageFunction fun)
{
    if (!srv_info_->server->Start(ip, port)) {
		// printf("RTSP Server listen on %d failed.\n", port);
        std::cerr << "RTSP Server listen on " << port << "failed.\n" << std::endl;
		return -1;
	}
    sleep_time_msec_ = sleep_time_msec > 0 ? sleep_time_msec: 40;
    srv_info_->session->AddSource(xop::channel_0, xop::H264Source::CreateNew()); 
	srv_info_->session->StartMulticast(); 
	srv_info_->session->AddNotifyConnectedCallback([] (xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port){
		printf("RTSP client connect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
	});
   
	srv_info_->session->AddNotifyDisconnectedCallback([](xop::MediaSessionId sessionId, std::string peer_ip, uint16_t peer_port) {
		printf("RTSP client disconnect, ip=%s, port=%hu \n", peer_ip.c_str(), peer_port);
	});

	
	thread_ = std::thread(&PicRtspPusher::sendFrameThread, this, fun );
	thread_.detach(); 

	std::string rtsp_url = "rtsp://" + ip + ":" + std::to_string(port) + "/" + url_suffix_;
	std::cout << "Play URL: " << rtsp_url << std::endl;
    return 0;
}

void PicRtspPusher::start()
{
    std::unique_lock<std::mutex> lock(enable_image_mutex_);
    enable_image_ = true;
}

void PicRtspPusher::stop()
{
    std::unique_lock<std::mutex> lock(enable_image_mutex_);
    enable_image_ = false;
}

int PicRtspPusher::readFrame(char* in_buf, int in_buf_size, bool* end, cv::Mat& frame)
{
    std::unique_lock<std::mutex> lock(mutex_);
	srv_info_->x264_encoder->Create(frame.cols, frame.rows, frame.channels());
	int size = srv_info_->x264_encoder->EncodeOneFrame(frame);
	uchar* encoder = srv_info_->x264_encoder->GetEncodedFrame();

	int res_size = (size<=in_buf_size ? size : in_buf_size);

	memcpy(in_buf, encoder, res_size);

	*end = false;
	srv_info_->x264_encoder->Destory();
	return res_size;
}


void PicRtspPusher::sendFrameThread(FetchImageFunction fun)
{      
	xop::MediaSessionId session_id = srv_info_->server->AddSession(srv_info_->session); 
	int buf_size = 4000000;
	std::unique_ptr<uint8_t> frame_buf(new uint8_t[buf_size]);
    
	int i = 0;
	while(1) {
		bool end_of_frame = false;
        enable_image_mutex_.lock();
        if (!enable_image_){
            enable_image_mutex_.unlock();
            xop::Timer::Sleep(sleep_time_msec_);
            continue;
        }
        enable_image_mutex_.unlock();
        cv::Mat frame; 
        int ret = fun(frame);
        if (ret != 0) {
            xop::Timer::Sleep(sleep_time_msec_);
            continue;
        }
		int frame_size = readFrame((char*)frame_buf.get(), buf_size, &end_of_frame, frame);
		if(frame_size > 0) {
			xop::AVFrame videoFrame = {0};
			videoFrame.type = 0; 
			videoFrame.size = frame_size;
			videoFrame.timestamp = xop::H264Source::GetTimestamp();
			videoFrame.buffer.reset(new uint8_t[videoFrame.size]);    
			memcpy(videoFrame.buffer.get(), frame_buf.get(), videoFrame.size);
			srv_info_->server->PushFrame(session_id, xop::channel_0, videoFrame);
		}
		else {
		}
      
		xop::Timer::Sleep(sleep_time_msec_); 
	};
}





}// end of namespace rtsp
