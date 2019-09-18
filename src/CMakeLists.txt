
#add_definitions(-std=c++11)
add_definitions(-std=c++11 -DDEBUG=1)
set(BASE /home/panghao/github/kunlib)
include_directories(/home/panghao/pg/boost_1_69_0 /home/panghao/github/face_recognition/src ../include ${BASE}/include /usr/local/include/librdkafka /usr/local/include /usr/local/include/zdb ${BASE}/include/librdkafka ${BASE}/include/hiredis ${BASE}/user)
link_directories(/home/panghao/github/face_recognition/build /usr/local/lib ${BASE}/libs ${BASE}/libs/opencv)
link_directories(/usr/local/lib ${BASE}/libs ${BASE}/libs/opencv)
link_directories(/usr/local/lib64)
add_executable(face  helmetClient.cpp helmetTask.cpp trackControl.cpp detectServiceCvImpl.cpp  Helmet.cpp helmet_types.cpp track.cpp main.cpp  util.cpp  faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(face  thriftd thriftzd thriftnbd kunFace  opencv_dnn opencv_imgproc  opencv_imgcodecs opencv_core  pbase64 pkafka rdkafka dbpool zdb kunconfig hiredis  jsoncpp  event  glog pthread )
 

