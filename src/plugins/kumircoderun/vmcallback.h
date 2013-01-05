#ifndef VMCALLBACK_H
#define VMCALLBACK_H
#include <QtCore>
#define DO_NOT_DECLARE_STATIC


namespace KumirCodeRun {

class VMCallBack
        : public QObject
        , public VM::AbstractInteractionHandler
{
    Q_OBJECT
public:
#ifndef NO_EXTERNS
    inline virtual bool resetExternalModule(const String & /*moduleName*/) { return false; }
    inline virtual bool evaluateExternalFunction(
            const String & /*moduleName*/, // IN
            uint16_t /*functionId*/, // IN
            const std::deque<Variant> & /*arguments*/, // IN
            Variant & /*result*/,  // OUT
            String & /*moduleRuntimeError*/  // OUT
            ) { return false; }
#endif
    inline virtual bool makeInput(
            const std::deque<String> & /*formats*/,
            const std::deque<Variant> & /*references*/
            ) { return false; }
    inline virtual bool makeOutput(
            const std::deque<String> & /*formats*/,
            const std::deque<Variant> & /*values*/
            ) { return false; }
    inline virtual bool makeInputArgument(
            Variant & /*reference*/
            ) { return false; }
    inline virtual bool makeOutputArgument(
            const Variant & /*reference*/
            ) { return false; }
    inline virtual bool makePause() { return false; }
    inline virtual bool noticeOnValueChange(
            int /*lineNo*/,
            const String & /*noticeText*/
            ) { return false; }
    inline virtual bool clearMargin(
            int /*fromLine*/, int /*toLine*/
            ) { return false; }
    inline virtual bool noticeOnFunctionReturn(
            int /*lineNo*/
            ) { return false; }
    inline virtual bool noticeOnLineNoChanged(
            int /*lineNo*/
            ) { return false; }
};


}

#endif

