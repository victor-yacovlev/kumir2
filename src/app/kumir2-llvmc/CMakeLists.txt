project(kumir2-llvmc)
cmake_minimum_required(VERSION 3.0)

find_package(Kumir2 REQUIRED)

kumir2_add_launcher(
    NAME            kumir2-llvmc
    CONFIGURATION   "!KumirCompilerTool,LLVMCodeGenerator,KumirAnalizer\(teacher,preload=Files,preload=Strings\)"
)
