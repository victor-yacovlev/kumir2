#include "kumir_core.h"
#include <wchar.h>
#include <stdio.h>

#include "errors.ru.h"

EXTERN void __error__(unsigned int code)
{
    wprintf(L"%s\n", RU[code]);
}
