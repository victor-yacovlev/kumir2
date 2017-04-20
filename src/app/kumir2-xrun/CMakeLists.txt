project(kumir2-xrun)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-xrun
    CONFIGURATION   "!KumirCodeRun\(console\)"
)
