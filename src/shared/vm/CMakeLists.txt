include(../../kumir2_library.cmake)


include_directories(${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")

set(HEADERS
    vm.hpp
    bc_tableelem.hpp
    bc_instruction.hpp
    bytecode.hpp
    variant.hpp
)

set(SOURCES vm.cpp)

add_library(VM STATIC  ${SOURCES})
#target_link_libraries(VM)
set_property(TARGET VM APPEND PROPERTY COMPILE_DEFINITIONS VM_LIBRARY)

#handleTranslation(DataFormats)

install(TARGETS VM DESTINATION ${LIBS_DIR})
