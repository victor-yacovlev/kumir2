project(kumir2-python)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-python
    SPLASHSCREEN    "coregui/splashscreens/python.png"
    CONFIGURATION   "CourseManager,Actor*,Editor,Python3Language,!CoreGUI\(icon=python,nosessions\)"
)
