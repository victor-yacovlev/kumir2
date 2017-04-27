find_package(Kumir2 REQUIRED)
# find_package(Llvm)

# Various GUI configurations

add_opt_subdirectory(kumir2-classic)
add_opt_subdirectory(kumir2-highgrade)
add_opt_subdirectory(kumir2-ide)
add_opt_subdirectory(kumir2-teacher)
add_opt_subdirectory(kumir2-checkcourse)


add_opt_subdirectory(kumir2-bc)
add_opt_subdirectory(kumir2-xrun)

# kumir2-llvmc is optional in case if LLVM libraries present
#if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/kumir2-llvmc")
#    if(Llvm_FOUND)
#        add_opt_subdirectory(kumir2-llvmc)
#    else()
#        message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
#    endif(Llvm_FOUND)
#endif()
