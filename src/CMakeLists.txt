
add_definitions(-std=c++11 -DUSE_MYSQL)
set(BASE /home/test/kunlib)
include_directories(../include ${BASE}/include  /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
add_executable(sample main.cpp  faceRepo.cpp faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(sample predis pbase64 dbpool zdb kunconfig hiredis  jsoncpp  evhtp event  glog pthread)

