
#add_definitions(-std=c++11)
add_definitions(-std=c++11)
set(BASE /home/test/kunlib)
include_directories(/home/test/pg/boost_1_69_0 /home/test/github/face_recognition2/src ../include ${BASE}/include /usr/local/include/librdkafka /usr/local/include/opencv4 /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/home/test/github/face_recognition2/build /usr/local/lib ${BASE}/libs ${BASE}/libs/opencv)
link_directories(/usr/local/lib /home/test/github/darknet ${BASE}/libs ${BASE}/libs/opencv)
add_executable(face  helmetClient.cpp helmetTask.cpp trackControl.cpp detectServiceCvImpl.cpp  Helmet.cpp helmet_types.cpp track.cpp main.cpp  util.cpp  faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(face  thriftd thriftzd thriftnbd kunFace opencv_tracking opencv_dnn opencv_imgproc  opencv_imgcodecs opencv_core  pbase64 pkafka rdkafka dbpool zdb kunconfig hiredis  jsoncpp  event  glog pthread )

