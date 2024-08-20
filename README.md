## rtsp_pusher

[中文介绍](https://github.com/PHZ76/RtspServer/blob/master/README_CN.md)


* rtsp_pusher implemented in C++14. Further development based on [RtspServer](https://github.com/PHZ76/RtspServer/tree/master), mainly adding the direct introduction of images for streaming, converting images into H264 single-frame video streams, and using the original H264 streaming solution to stream images.

* The x264 library (libx264.so) is needed in the process of converting images to H264. In this code, the thirdparty folder and release folder contain the dynamic and static libraries of x264 for x86 and aarch64. The x264Encoder.cpp and x264Encoder.h in the code path src/convertor/ use the x264 library to convert the images read by opencv to H264 format. The code refers to the [csdn blog post](https://blog.csdn.net/leonardohaig/article/details/103624426) or this [cnblogs blog post](https://www.cnblogs.com/ziyu-trip/p/7075003.html) on the Internet. Of course, I have also modified the memory leak error here. It works!

* If you want to encapsulate it into a dynamic library that is only used to send pictures as rtsp streams, it is also feasible. For a simple usage example after encapsulation, please refer to the code [rtsp_push_simple].

* For descriptions of other codes, please refer to the most basic source of the code [RtspServer](https://github.com/PHZ76/RtspServer/tree/master).