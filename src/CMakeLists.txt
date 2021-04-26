
add_definitions(-std=c++11)
include_directories(include ${BASE}/user ${BASE}/include  /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib /usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
add_executable(ems main.cpp  emsRepo.cpp emsControl.cpp  emsService.cpp)
target_link_libraries(ems ktimer email dbpool zdb kunconfig evdrv resource predis dbpool hiredis jsoncpp  event  glog pthread)

