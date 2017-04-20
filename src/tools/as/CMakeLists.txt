project(kumir2-as)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_tool(
    NAME    kumir2-as
    SOURCES main.cpp
)
