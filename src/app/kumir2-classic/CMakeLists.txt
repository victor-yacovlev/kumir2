project(kumir2-classic)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-classic
    SPLASHSCREEN    "coregui/splashscreens/classic.png"
    CONFIGURATION   "CourseManager,Editor,Actor*,KumirAnalizer\(preload=Files\),*CodeGenerator,KumirCodeRun(nobreakpoints),!CoreGUI\(notabs,icon=classic,nostartpage,nosessions\)"
)

