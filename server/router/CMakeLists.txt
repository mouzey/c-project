PROJECT(router)

function(alldirsub base)
file(glob subdirs relative ${cmake_current_source_dir} ${base}/*)
foreach(dir $subdirs)
if(is_directory ${cmake_current_source_dir}/${dir})
include_directories(${dir})
alldirsub(${dir})
endif()
endforeach()
endfunction()

include_directories(. ../common ../common/libsource)

FILE(GLOB_RECURSE CPPSRCS "*.cpp")
add_EXECUTABLE(router ${CPPSRCS})
target_link_libraries(router common mysqlclient pthread ssl crypto rt dl)
