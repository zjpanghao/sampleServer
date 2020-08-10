
add_definitions(-std=c++11)
set(BASE /home/test/github/kunlib)
include_directories(include ${BASE}/user ${BASE}/include  /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
add_executable(sample main.cpp  faceRepo.cpp faceControl.cpp  faceService.cpp)
target_link_libraries(sample dbpool zdb kunconfig evdrv resource predis dbpool hiredis jsoncpp  event  glog pthread)

