#ifndef ERROR_H
#define ERROR_H

namespace Shared {

struct Error
{
    int line; // line number from 0
    int start; // position in line from 0
    int len; // error markup length
    int code; // error code
};

} // namespace KumirAnalizer

#endif // ERROR_H
