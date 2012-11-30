#if defined(WIN32) || defined(_WIN32)
#   ifdef VM_LIBRARY
#       define VM_EXPORT __declspec(dllexport)
#   else
#       define VM_EXPORT __declspec(dllimport)
#   endif
#else
#   ifdef VM_LIBRARY
#       define VM_EXPORT /*__attribute__((visiblity("default")))*/
#   else
#       define VM_EXPORT
#   endif
#endif
