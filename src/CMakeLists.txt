
add_definitions(-std=c++11)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../..)
set(BASE /home/panghao/kunlib)
include_directories(../include ${BASE}/include  ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
add_executable(sample main.cpp  faceControl.cpp  httpUtil.cpp  http-server.cpp  faceService.cpp)
target_link_libraries(sample kunconfig hiredis  jsoncpp  event  glog pthread)

