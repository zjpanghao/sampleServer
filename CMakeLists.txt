
add_definitions(-std=c++11)
set(BASE ~/github/kunlib)
include_directories(./src include ${BASE}/user ${BASE}/include  /usr/local/include/zdb ${BASE}/include/hiredis ${BASE}/user)
link_directories(/usr/local/lib64 ${BASE}/libs ${BASE}/libs/opencv)
aux_source_directory(src SRC_LIST)
add_executable(sample ${SRC_LIST}) 
target_link_libraries(sample dbpool zdb kunconfig evdrv resource predis dbpool hiredis jsoncpp  event  glog pthread)

