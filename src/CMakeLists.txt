
#add_definitions(-std=c++11 -g -DCUDNN=1 -DGPU=1)
add_definitions(-std=c++11)
set(BASE /home/test/kunlib)
include_directories(/usr/local/cuda/include /home/test/github/darknet/src /home/test/github/darknet/include /home/test/pg/boost_1_69_0 /home/test/github/faceApi ../include ${BASE}/include /usr/local/include/librdkafka /usr/local/include/opencv4 /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/home/test/github/faceApi/build /usr/local/lib ${BASE}/libs ${BASE}/libs/opencv)
link_directories(/usr/local/cuda/lib64 /usr/local/lib /home/test/github/darknet ${BASE}/libs ${BASE}/libs/opencv)
add_executable(face  detectServiceCvImpl.cpp detectService.cpp PersonSearch.cpp person_types.cpp Helmet.cpp helmet_types.cpp track.cpp main.cpp  util.cpp faceRepo.cpp faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(face cuda cudnn cublas cudart darknet thriftd thriftzd thriftnbd kunFace opencv_tracking opencv_dnn opencv_imgproc  opencv_imgcodecs opencv_core  pbase64 pkafka rdkafka dbpool zdb kunconfig hiredis  jsoncpp  event  glog pthread)

add_executable(test  detectServiceCvImpl.cpp detectService.cpp PersonSearch.cpp person_types.cpp Helmet.cpp helmet_types.cpp track.cpp test.cpp  util.cpp faceRepo.cpp faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(test cuda cudnn cublas cudart darknet thriftd thriftzd thriftnbd kunFace opencv_tracking opencv_dnn opencv_imgproc  opencv_imgcodecs opencv_core  pbase64 pkafka rdkafka dbpool zdb kunconfig hiredis  jsoncpp  event  glog pthread)
