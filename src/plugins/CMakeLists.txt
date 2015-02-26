include(${CMAKE_SOURCE_DIR}/kumir2_common.cmake)

find_package(Llvm)
find_package(PythonLibs 3.2)

# Mainstream plugins to be always build

add_subdirectory(browser)
add_subdirectory(editor)
add_subdirectory(coregui)
add_subdirectory(coursemanager)


add_opt_subdirectory(kumiranalizer)
add_opt_subdirectory(kumircodegenerator)
add_opt_subdirectory(kumircoderun)
add_opt_subdirectory(kumircompilertool)

if(Llvm_FOUND)
    add_opt_subdirectory(llvmcodegenerator)
else()
    message(STATUS "LLVM developer libraries not found, building of LLVM support disabled")
endif(Llvm_FOUND)

if(PYTHONLIBS_FOUND)
    add_opt_subdirectory(python3language)
else()
    message(STATUS "No Python 3.x libs found, building of Python language support disabled")
endif()

add_opt_subdirectory(fpcanalizer)
add_opt_subdirectory(lldbrun)
