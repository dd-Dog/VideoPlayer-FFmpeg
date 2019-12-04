project(NativeVideoPlayer)

cmake_minimum_required(VERSION 3.4.1)

include_directories(include/)
include_directories(3rdparty/ffmpeg/include)

add_library(NativeVideoPlayer SHARED
        NativeVideoPlayer.cpp
        VideoDecoder.cpp
        AudioOutput.cpp
        )

####################添加ffmpeg库####################
add_library(avformat-57 SHARED IMPORTED)
add_library(avcodec-57 SHARED IMPORTED)
add_library(avdevice-57 SHARED IMPORTED)
add_library(avfilter-6 SHARED IMPORTED)
add_library(avutil-55 SHARED IMPORTED)
add_library(swresample-2 SHARED IMPORTED)
add_library(swscale-4 SHARED IMPORTED)

set(STATIC_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/armeabi-v7a/)

set_target_properties(avcodec-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavcodec-57.so)
set_target_properties(avdevice-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavdevice-57.so)
set_target_properties(avfilter-6
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavfilter-6.so)
set_target_properties(avformat-57
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavformat-57.so)
set_target_properties(avutil-55
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libavutil-55.so)
set_target_properties(swresample-2
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libswresample-2.so)
set_target_properties(swscale-4
        PROPERTIES IMPORTED_LOCATION
        ${STATIC_LIBS_DIR}/libswscale-4.so)
target_link_libraries(NativeVideoPlayer z) #链接系统libz库
target_link_libraries(NativeVideoPlayer swresample-2)
target_link_libraries(NativeVideoPlayer avdevice-57)
target_link_libraries(NativeVideoPlayer avformat-57)
target_link_libraries(NativeVideoPlayer avfilter-6)
target_link_libraries(NativeVideoPlayer avcodec-57)
target_link_libraries(NativeVideoPlayer avutil-55)
target_link_libraries(NativeVideoPlayer swscale-4)
####################添加ffmpeg库####################

#链接OpenSLES
target_link_libraries(NativeVideoPlayer OpenSLES)

find_library(log-lib log)

target_link_libraries(NativeVideoPlayer ${log-lib})