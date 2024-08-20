cmake_minimum_required(VERSION 3.10) # 设置CMake的最小版本
project(rtsp_pusher) # 定义项目名称

set(CMAKE_AARCH64_TARGET 1)

set(CMAKE_BUILD_TYPE  Debug CACHE STRING "Choose the type of build." FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -O0 -Wall -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -g -O3")

# set(SHARED_LIB_NAME "pic_rtsp_pusher")

if(${CMAKE_AARCH64_TARGET})

# 查找OpenCV库，REQUIRED参数表示如果找不到则会失
# find_package(OpenCV REQUIRED)
# set(x264_libs_DIR /usr/local/lib)
set(x264_libs_DIR /home/kotei/FileSaved/TASK/cds_task/rtsp_pusher/thirdparty/x264_arm/lib)
# set(x264_headers_DIR /usr/local/include)
set(x264_headers_DIR /home/kotei/FileSaved/TASK/cds_task/rtsp_pusher/thirdparty/x264_arm/include)
set(LINK_FLAGS "-L/usr/aarch64-linux-gnu/lib -lrt -pthread -lpthread -ldl -lm -D_DEBUG")
set(CMAKE_EXE_LINKER_FLAGS   "${LINK_FLAGS}")

set(OPENCV_INCLUDE_DIRS /home/kotei/arm_lib/OSS/include)
set(OPENCV_LIB_DIR /home/kotei/arm_lib/OSS/lib/j5/opencv)
set(OPENCV4_LIB_DIR /home/kotei/arm_lib/OSS/lib/j5/opencv-4.4)

file(GLOB_RECURSE OPENCV_LIBS_TMP  FOLLOW_SYMLINKS
    ${OPENCV_LIB_DIR}/*.so*
    ${OPENCV_LIB_DIR}/*.a
)

file(GLOB_RECURSE OPENCV4_LIBS  FOLLOW_SYMLINKS
    ${OPENCV4_LIB_DIR}/*.so*
    ${OPENCV4_LIB_DIR}/*.a
)

set(OPENCV_LIBS ${OPENCV_LIBS_TMP} ${OPENCV4_LIBS})

message(OPENCV_LIBS ${OPENCV_LIBS})

set(CMAKE_C_COMPILER /usr/bin/aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/aarch64-linux-gnu-g++)

message(END_OPENCV_LIBS)

link_directories(${OPENCV_LIB_DIR} ${OPENCV4_LIB_DIR})


else()

# 查找OpenCV库，REQUIRED参数表示如果找不到则会失败
find_package(OpenCV REQUIRED)
# set(x264_libs_DIR /usr/local/lib)
set(x264_libs_DIR /home/kotei/FileSaved/TASK/cds_task/rtsp_pusher/thirdparty/x264/lib)
# set(x264_headers_DIR /usr/local/include)
set(x264_headers_DIR /home/kotei/FileSaved/TASK/cds_task/rtsp_pusher/thirdparty/x264/include)
set(LINK_FLAGS "-lrt -pthread -lpthread -ldl -lm -D_DEBUG")
set(CMAKE_EXE_LINKER_FLAGS    "${LINK_FLAGS}")

set(OPENCV_INCLUDE_DIRS ${OpenCV_INCLUDE_DIRS})
set(OPENCV_LIBS ${OpenCV_LIBS})

message(${OPENCV_INCLUDE_DIRS})

endif()


file(GLOB_RECURSE RTSP_PUSHER_APP_SRC_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS

    src/*.hpp
    src/*.h
    src/*.c
    src/*.cc
    src/*.cpp

    src/3rdpart/md5/*.hpp
    src/3rdpart/md5/*.h
    src/3rdpart/md5/*.c
    src/3rdpart/md5/*.cc
    src/3rdpart/md5/*.cpp

    src/package/*.hpp
    src/package/*.h
    src/package/*.c
    src/package/*.cc
    src/package/*.cpp

    src/convertor/*.hpp
    src/convertor/*.h
    src/convertor/*.c
    src/convertor/*.cc
    src/convertor/*.cpp

    src/net/*.hpp
    src/net/*.h
    src/net/*.c
    src/net/*.cc
    src/net/*.cpp

    src/xop/*.hpp
    src/xop/*.h
    src/xop/*.c
    src/xop/*.cc
    src/xop/*.cpp
)

# message(${RTSP_PUSHER_APP_SRC_LIST})

link_libraries( 
    ${x264_libs_DIR}/libx264.so 
)


# 添加头文件路径到编译器的头文件搜索路径下，多个路径以空格分隔
include_directories(${x264_headers_DIR} ${OPENCV_INCLUDE_DIRS} src src/net src/3rdpart src/convertor src/package)

# include_directories( ${ffmpeg_headers_DIR}/libavutil)
link_directories(${x264_libs_DIR} )

message(${x264_libs_DIR})


# add_library(${SHARED_LIB_NAME} SHARED src/package/pic_rtsp_pusher.cpp)

# add_library(${SHARED_LIB_NAME} STATIC src/package/pic_rtsp_pusher.cpp)

# 添加可执行文件
add_executable(rtsp_h264_file  ${RTSP_PUSHER_APP_SRC_LIST}  example/rtsp_h264_file.cpp)

add_executable(rtsp_pusher  ${RTSP_PUSHER_APP_SRC_LIST}  example/rtsp_pusher.cpp)

add_executable(rtsp_server  ${RTSP_PUSHER_APP_SRC_LIST}  example/rtsp_server.cpp)

add_executable(rtsp_pic_h264  ${RTSP_PUSHER_APP_SRC_LIST}  example/rtsp_pic_h264.cpp)

add_executable(test_pic_pusher  ${RTSP_PUSHER_APP_SRC_LIST}  example/test_pic_pusher.cpp)



# 链接到OpenCV库
target_link_libraries(rtsp_h264_file ${OPENCV_LIBS})

target_link_libraries(rtsp_pusher ${OPENCV_LIBS})

target_link_libraries(rtsp_server ${OPENCV_LIBS})

target_link_libraries(rtsp_pic_h264 ${OPENCV_LIBS}  ${x264_libs_DIR}/libx264.so )

target_link_libraries(test_pic_pusher ${OPENCV_LIBS}  ${x264_libs_DIR}/libx264.so )


# file(COPY ${CMAKE_SOURCE_DIR}/src/package/pic_rtsp_pusher.h DESTINATION ${CMAKE_SOURCE_DIR}/release/include/)
# file(COPY ${CMAKE_SOURCE_DIR}/build/pic_rtsp_pusher.so DESTINATION ${CMAKE_SOURCE_DIR}/release/lib/)



