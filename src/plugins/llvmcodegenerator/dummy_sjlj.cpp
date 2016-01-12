#include <unwind.h>

extern "C" {
extern void _Unwind_SjLj_Register (struct SjLj_Function_Context *) {}
extern void _Unwind_SjLj_Unregister (struct SjLj_Function_Context *) {}
extern _Unwind_Reason_Code _Unwind_SjLj_RaiseException (struct _Unwind_Exception *) { return _Unwind_Reason_Code(0); }
extern _Unwind_Reason_Code _Unwind_SjLj_ForcedUnwind (struct _Unwind_Exception *, _Unwind_Stop_Fn, void *) { return _Unwind_Reason_Code(0); }
extern void _Unwind_SjLj_Resume (struct _Unwind_Exception *) {}
extern _Unwind_Reason_Code _Unwind_SjLj_Resume_or_Rethrow (struct _Unwind_Exception *) { return _Unwind_Reason_Code(0); }
extern _Unwind_Ptr _Unwind_GetRegionStart (struct _Unwind_Context *) { return _Unwind_Ptr(0); }
extern _Unwind_Ptr _Unwind_GetDataRelBase (struct _Unwind_Context *) { return _Unwind_Ptr(0); }
extern _Unwind_Ptr _Unwind_GetTextRelBase (struct _Unwind_Context *) { return _Unwind_Ptr(0); }
extern void *_Unwind_GetLanguageSpecificData (struct _Unwind_Context *) { return 0; }
extern void _Unwind_DeleteException (struct _Unwind_Exception *) {}
extern _Unwind_Ptr _Unwind_GetIPInfo (struct _Unwind_Context *, int *) { return _Unwind_Ptr(0); }
extern void _Unwind_SetIP (struct _Unwind_Context *, _Unwind_Ptr) {}
extern void _Unwind_SetGR (struct _Unwind_Context *, int, _Unwind_Word) {}
extern void *__emutls_get_address (struct __emutls_object *) { return 0; }
}

