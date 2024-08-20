## rtsp_pusher

项目介绍
-
* C++14实现的RTSP服务器和推流器。基于[RtspServer](https://github.com/PHZ76/RtspServer/tree/master)而做进一步开发，主要是加入了直接引入图片进行推流，讲图片转化成H264单帧格式的视频流，利用原有的H264的推流方案，对图片进行推流。
* 图片转H264的过程中需要用到x264库（libx264.so），本代码中在thirdparty文件夹和release文件夹下都包含了x86以及aarch64版本的x264的动态库以及静态库，代码路径src/convertor/下的x264Encoder.cpp,x264Encoder.h便是利用x264的库将opencv读取的图片转化为H264格式，该代码参考了网上的[csdn博文](https://blog.csdn.net/leonardohaig/article/details/103624426)或者是这篇[cnblogs博文](https://www.cnblogs.com/ziyu-trip/p/7075003.html)，当然我这里也对其中的内存泄漏的错误进行了修改。亲测可用！
* 如果想对其进行封装可以成只用来将图片以rtsp流发送出去的动态库，也是可行的，封装后的简单使用示例可以参考代码[rtsp_push_simple]。
* 其他代码的说明可以参考该代码的最基础来源[RtspServer](https://github.com/PHZ76/RtspServer/tree/master)。

