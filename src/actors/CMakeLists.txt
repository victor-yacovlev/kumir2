include(${CMAKE_SOURCE_DIR}/kumir2_common.cmake)
file(GLOB entries *)
foreach(entry ${entries})
  if(IS_DIRECTORY ${entry} AND EXISTS ${entry}/CMakeLists.txt)
      get_filename_component(subdir ${entry} NAME)
      add_opt_subdirectory(${subdir})
  endif()
endforeach(entry)

