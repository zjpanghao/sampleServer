
add_definitions(-std=c++11)
set(RUNTIME_OUTPUT_DIRECTORY /home/panghao/github/serverSample/bin)
set(BASE /home/panghao/kunlib)
include_directories(../include ${BASE}/include  ${BASE}/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
add_executable(sample main.cpp  faceRepo.cpp faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(sample dbpool zdb kunconfig hiredis  jsoncpp  event  glog pthread)

