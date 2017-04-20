project(kumir2-teacher)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-teacher
    SPLASHSCREEN    "coregui/splashscreens/teacher.png"
    CONFIGURATION   "CourseManager,Editor\(teacher\),Actor*,KumirAnalizer\(teacher\),KumirCodeGenerator,KumirCodeRun,!CoreGUI\(icon=teacher\)"
)
