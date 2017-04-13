project(kumir2-highgrade)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-highgrade
    SPLASHSCREEN    "coregui/splashscreens/highgrade.png"
    CONFIGURATION   "CourseManager,Editor,Actor*,KumirAnalizer\(preload=Files,preload=Strings,preload=Keyboard\),*CodeGenerator,KumirCodeRun,!CoreGUI\(notabs,icon=highgrade,nostartpage,nosessions\)"
)
