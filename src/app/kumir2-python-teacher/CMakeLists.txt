project(kumir2-python-teacher)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-python-teacher
    SPLASHSCREEN    "coregui/splashscreens/python.png"
    CONFIGURATION   "CourseManager,Actor*,Editor\(teacher\),Python3Language,!CoreGUI\(notabs,icon=python,nostartpage,nosessions\)"
)

