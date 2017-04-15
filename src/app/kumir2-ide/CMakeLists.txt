project(kumir2-ide)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-ide
    SPLASHSCREEN    "coregui/splashscreens/professional.png"
    CONFIGURATION   "CourseManager,Editor,Actor*,KumirAnalizer,*CodeGenerator,KumirCodeRun,!CoreGUI"
)

