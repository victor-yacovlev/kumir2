find_package(Llvm)
find_package(PythonLibs 3.2)

# Mainstream plugins to be always build

add_subdirectory(kumiranalizer)
add_subdirectory(kumircodegenerator)
add_subdirectory(kumircoderun)
add_subdirectory(kumircompilertool)
add_subdirectory(browser)
add_subdirectory(editor)
add_subdirectory(coregui)
add_subdirectory(coursemanager)
if(Llvm_FOUND)
    add_subdirectory(llvmcodegenerator)
else()
    message(WARNING "LLVM developer libraries not found, building of LLVM support disabled")
endif(Llvm_FOUND)


# Experimental plugins from various git branches

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/python3language")
    if(PYTHONLIBS_FOUND)
        add_subdirectory(python3language)
    else()
        message(WARNING "No Python 3.x libs found, building of Python language support disabled")
    endif()
endif() # python3language exists

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fpcanalizer")
    add_subdirectory(fpcanalizer)
endif() # fpcanalizer exists

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lldbrun")
    add_subdirectory(lldbrun)
endif() # gdbrun exists

