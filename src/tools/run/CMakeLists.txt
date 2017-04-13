project(kumir2-run)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_tool(
    NAME        kumir2-run
    SOURCES     main.cpp
)

