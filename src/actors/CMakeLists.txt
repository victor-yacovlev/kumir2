include(${CMAKE_SOURCE_DIR}/kumir2_common.cmake)
file(GLOB entries *)
foreach(entry ${entries})
  if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
      add_opt_subdirectory(${entry})
  endif()
endforeach(entry)

