cmake_path(SET SHARED_INCLUDE ${CMAKE_CURRENT_LIST_DIR})

target_sources(deluge PUBLIC main.c resetprg.c)

add_subdirectory(deluge)
add_subdirectory(RZA1)
add_subdirectory(RTT)
add_subdirectory(fatfs)
add_subdirectory(NE10)
