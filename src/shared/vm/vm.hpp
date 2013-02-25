#ifndef VM_HPP
#define VM_HPP


#include <map>
#include <deque>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>

#include "stdlib/kumirstdlib.hpp"
#include "vm_bytecode.hpp"
#include "stack.hpp"
#include "variant.hpp"
#include "context.hpp"
#include "stack.hpp"

#ifndef MAX_RECURSION_SIZE
#define MAX_RECURSION_SIZE 4000
#endif

namespace VM {



/*abstract*/ class CriticalSectionLocker {
public:
    virtual void lock() = 0;
    virtual void unlock() = 0;
};


/*abstract*/ class AbstractInteractionHandler {
public:
#ifndef NO_EXTERNS
    inline virtual bool resetExternalModule(const String & /*moduleName*/) { return false; }
    inline virtual bool loadExternalModule(const String & /*moduleName*/,
                                           const String & /*fileName*/,
                                           /*out*/ std::list<String> & availableMethods) { return false; }
    inline virtual bool evaluateExternalFunction(
            const String & /*moduleName*/, // IN
            uint16_t /*functionId*/, // IN
            const std::deque<Variable> & /*inArguments*/, // IN
            std::deque<Variable> & /*outArguments*/, // OUT
            Variable & /*result*/,  // OUT
            String & /*moduleRuntimeError*/  // OUT
            ) { return false; }
#endif
    inline virtual bool makeInput(
            std::deque<Variable> & /*references*/
            ) { return false; }
    inline virtual bool makeOutput(
            const std::deque< std::pair<int,int> > & /*formats*/,
            const std::deque<Variable> & /*values*/
            ) { return false; }
    inline virtual bool makeInputArgument(
            Variable & /*reference*/
            ) { return false; }
    inline virtual bool makeOutputArgument(
            const Variable & /*reference*/
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
    inline virtual bool debuggerReset() { return false; }
    inline virtual bool debuggerPushContext(
            const Kumir::String & /*contextName*/,
            const std::deque<Kumir::String> & /*variableNames*/,
            const std::deque<Kumir::String> & /*typeNames*/,
            const std::deque<uint8_t> & /*dimensions*/
            ) { return false; }
    inline virtual bool debuggerSetGlobals(
            const Kumir::String & /*moduleName*/,
            const std::deque<Kumir::String> & /*variableNames*/,
            const std::deque<Kumir::String> & /*typeNames*/,
            const std::deque<uint8_t> & /*dimensions*/
            ) { return false; }
    inline virtual bool debuggerPopContext() { return false; }
    inline virtual bool debuggerUpdateLocalVariable(
            const Kumir::String & /*name*/,
            const Kumir::String & /*value*/
            ) { return false; }
    inline virtual bool debuggerUpdateGlobalVariable(
            const Kumir::String & /*moduleName*/,
            const Kumir::String & /*name*/,
            const Kumir::String & /*value*/
            ) { return false; }
    inline virtual bool debuggerUpdateLocalTableBounds(
            const Kumir::String & /*name*/,
            const int[7] /*bounds*/
            ) { return false; }
    inline virtual bool debuggerUpdateGlobalTableBounds(
            const Kumir::String & /*moduleName*/,
            const Kumir::String & /*name*/,
            const int[7] /*bounds*/
            ) { return false; }
    inline virtual bool debuggerSetLocalReference(
            const Kumir::String & /*name*/,
            const Kumir::String & /*targetName*/,
            const int[4] /*tableIndeces*/,
            const int /*stackStepsBackward*/,
            const Kumir::String & /*global value module name*/
            ) { return false; }
    inline virtual bool debuggerForceUpdateValues() { return false; }
    inline virtual bool debuggerUpdateLocalTableValue(
            const Kumir::String & /*name*/,
            const int[4] /*indeces*/
            ) { return false; }
    inline virtual bool debuggerUpdateGlobalTableValue(
            const Kumir::String & /*moduleName*/,
            const Kumir::String & /*name*/,
            const int[4] /*indeces*/
            ) { return false; }
};

struct ReferenceInfo {
    bool valid;
    uint8_t module;
    uint16_t algorithm;
    uint16_t variable;
    inline ReferenceInfo() {
        valid = true;
        module = 0;
        algorithm = variable = 0;
    }
};



class KumirVM {
public /*typedefs*/:
    enum EntryPoint { EP_Main, EP_Testing };

public /*methods*/:
    /** Set parsed Kumir bytecode */
    inline void setProgram(const Bytecode::Data & data, bool isMain, const String & filename);
    inline void setProgramDirectory(const Kumir::String & path) { s_programDirectory = path; };

    inline bool loadProgramFromBinaryBuffer(std::list<char> & stream, bool isMain, const String & filename, String & error);
    inline bool loadProgramFromTextBuffer(const std::string & stream, bool isMain, const String & filename, String & error);

    /** Set entry point to Main or Testing algorithm */
    inline void setEntryPoint(EntryPoint ep) { e_entryPoint = ep; }

    /** Set 'blind' mode flag: do not emit debugging information */
    inline void setDebugOff(bool b) { b_blindMode = b; }

    /** Set 'step into' flag for next function call */
    inline void setStepInto(bool b) { b_nextCallInto = b; }

    /** Reset program to initial state */
    inline void reset();

    /** Sets the External Interaction handler for this VM */
    inline void setExternalHandler(AbstractInteractionHandler *h) { m_externalHandler = h; }

    /** In GUI mode it's need a Mutex to protect internal data
     *  from been discovered.
     *     You must set actual Mutex implementation object
     *     (Qt or C++11) with methods: lock() and unlock()
     */
    inline void setMutex(CriticalSectionLocker * m) { m_dontTouchMe = m;}

    /** The following two functions are basic to use for actual run:
     *  while ( vm.hasMoreInstructions() )
     *      vm.evaluateNextInstruction();
     */
    inline bool hasMoreInstructions() const;
    inline void evaluateNextInstruction();

    /** Return current 'line number' or -1 if not applicable */
    inline int effectiveLineNo() const;

    /** Returns 'true' if evaluating non-toplevel user function */
    inline bool canStepOut() const;

    /** Used by debugger. Returns local variables for a given stack frame */
    inline const std::vector<Variable> & getLocals(int frameNo) const;

    /** Used by debugger. Returns global variables for a given module name */
    inline const std::vector<Variable> & getGlobals(const Kumir::String &moduleName) const;

    /** Return a value */
    inline AnyValue value(int moduleId, int algId, int varId) const;

    /** Return bounds */
    inline std::list<int> bounds(int moduleId, int algId, int varId) const;

    /** Return reference information */
    inline ReferenceInfo reference(int moduleId, int algId, int varId) const;

    /** Returns last error */
    inline const String & error() const {
        if (s_error.length()==0 && Kumir::Core::getError().length()>0)
            return Kumir::Core::getError();
        else
            return s_error;
    }

    /** Returns values kept after program finished */
    inline std::list<AnyValue> remainingValues() const;

    /** Debug control methods */
    inline void setNextCallInto();
    inline void setNextCallOut();
    inline void setNextCallToEnd();
    inline void setNextCallStepOver();



private /*fields*/:
//#ifndef NO_EXTERNS
//    struct ExternalMethod {
//        String moduleName;
//        uint16_t methodId;
//    };

//    std::map<uint32_t, ExternalMethod> externalMethods;
//    std::map<TwoStrings, ExternalMethod> availableExternalMethods;
//#endif

    Bytecode::TableElem mainProgram;
    Bytecode::TableElem testingProgram;
    std::vector<ModuleContext> moduleContexts;
    EntryPoint e_entryPoint;
    bool b_blindMode;
    bool b_nextCallInto;
    CriticalSectionLocker * m_dontTouchMe;
    AbstractInteractionHandler * m_externalHandler;
    Context last_context;
    int i_backtraceSkip;
    String s_error;
    AnyValue register0;
    Stack<Variable> stack_values;
    Stack<Context> stack_contexts;
    Kumir::String s_programDirectory;
    std::vector<Variable> * currentConstants;
    std::vector<Variable> * currentGlobals;
    std::vector<Variable> * currentLocals;
public /*constructors*/:
    inline KumirVM();
private /*methods*/:
    inline static Variable fromTableElem(const Bytecode::TableElem & e);
    inline int contextByIds(int moduleId, int algorhitmId) const;
    inline Context & currentContext();
    inline void nextIP();

//    template <class T>
//    inline static Record toRecordValue(const T & t);

//    template <class T>
//    inline static T fromRecordValue(const Record & record);

    inline static Record toRecordValue(const Kumir::FileType & ft);
    inline static Kumir::FileType fromRecordValue(const Record & record);

    inline Variable & findVariable(uint8_t scope, uint16_t id);
    inline void debuggerPushContext(
            const bool isGlobals,
            const Kumir::String & name,
            const std::vector<Variable> * variables,
            bool force
            );
    inline void updateDebugger();
    inline void debuggerPopContext();

private /*instruction methods*/:
    inline void do_call(uint8_t, uint16_t);
    inline void do_stdcall(uint16_t);
    inline void do_filescall(uint16_t);
    inline void do_stringscall(uint16_t);
    inline void do_specialcall(uint16_t);
    inline void do_externalcall(const String & name, uint16_t id);
    inline void do_init(uint8_t, uint16_t);
    inline void do_setarr(uint8_t, uint16_t);
    inline void do_updarr(uint8_t, uint16_t);
    inline void do_store(uint8_t, uint16_t);
    inline void do_storearr(uint8_t, uint16_t);
    inline void do_load(uint8_t, uint16_t);
    inline void do_loadarr(uint8_t, uint16_t);
    inline void do_jump(uint16_t);
    inline void do_jz(uint8_t, uint16_t);
    inline void do_jnz(uint8_t, uint16_t);
    inline void do_pop(uint8_t);
    inline void do_push(uint8_t);
    inline void do_ret();
    inline void do_error(uint8_t, uint16_t);
    inline void do_line(uint16_t);
    inline void do_ref(uint8_t, uint16_t);
    inline void do_setref(uint8_t, uint16_t);
    inline void do_refarr(uint8_t, uint16_t);
    inline void do_showreg(uint8_t);
    inline void do_clearmarg(uint16_t);
    inline void do_pause(uint16_t);
    inline void do_halt(uint16_t);
    inline void do_ctl(uint8_t parameter, uint16_t value);
    inline void do_inrange();

    inline void do_sum();
    inline void do_sub();
    inline void do_mul();
    inline void do_div();
    inline void do_pow();
    inline void do_neg();
    inline void do_and();
    inline void do_or();
    inline void do_eq();
    inline void do_neq();
    inline void do_ls();
    inline void do_gt();
    inline void do_leq();
    inline void do_geq();
};


/**** IMPLEMENTATION ******/

Context & KumirVM::currentContext() {
    return stack_contexts.top();
}

Variable & KumirVM::findVariable(uint8_t scope, uint16_t id)
{
    if (VariableScope(scope)==Bytecode::CONSTT) {
        return currentConstants->at(id);
    }
    else if (VariableScope(scope)==Bytecode::LOCAL) {
        return currentLocals->at(id);
    }
    else {
        return currentGlobals->at(id);
    }
}

void KumirVM::setProgram(const Bytecode::Data &program, bool isMain, const String & filename)
{
    if (isMain) {
        moduleContexts.clear();
        mainProgram.type = EL_NONE;
        mainProgram.instructions.clear();
//#ifndef NO_EXTERNS
//        externalMethods.clear();
//#endif
    }
    moduleContexts.push_back(ModuleContext());
    moduleContexts.back().filename = filename;
    int currentModuleContext = moduleContexts.size()-1;
    moduleContexts.back().globals.clear();
    moduleContexts.back().constants.clear();
    moduleContexts.back().globals.reserve(16);
    moduleContexts.back().moduleNames.clear();
    moduleContexts.back().moduleNames.reserve(16);
    for (int i=0; i<moduleContexts.back().globals.size(); i++) {
        moduleContexts.back().globals.at(i).clear();
        moduleContexts.back().globals.at(i).reserve(256);
    }
    moduleContexts.back().constants.reserve(256);
    LocalsMap locals;
    for (int i=0; i<program.d.size(); i++) {
        const TableElem e = program.d[i];
        if (e.type==EL_GLOBAL) {
            if (moduleContexts[currentModuleContext].globals.size()<=e.module) {
                moduleContexts[currentModuleContext].globals.resize(e.module+1);
                moduleContexts[currentModuleContext].globals[e.module].reserve(256);
                moduleContexts[currentModuleContext].moduleNames.resize(e.module+1);
                moduleContexts[currentModuleContext].moduleNames[e.module] = e.moduleName;
            }
            if (moduleContexts[currentModuleContext].globals[e.module].size() <= e.id) {
                moduleContexts[currentModuleContext].globals[e.module].resize(e.id+1);
            }
            moduleContexts[currentModuleContext].globals[e.module][e.id] = fromTableElem(e);
        }
        else if (e.type==EL_CONST) {
            if (moduleContexts[currentModuleContext].constants.size()<=e.id)
                moduleContexts[currentModuleContext].constants.resize(e.id+1);
            moduleContexts[currentModuleContext].constants[e.id] = fromTableElem(e);

        }
        else if (e.type==EL_LOCAL) {
            uint32_t key = 0x00000000;
            uint32_t alg = e.algId;
            uint32_t mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            VariantArray lcs;
            if (locals.count(key))
                lcs = locals[key];
            lcs.push_back(fromTableElem(e));
            locals[key] = lcs;
        }
        else if (e.type==EL_EXTERN &&
                 e.fileName.length()>4 &&
                 e.fileName.substr(e.fileName.length()-4)==Kumir::Core::fromAscii(".kod"))
        {
#if !defined(WIN32) && !defined(_WIN32)
#define VM_LOCALE Kumir::UTF8
#else
#define VM_LOCALE Kumir::CP866
#endif
            int externModuleContext = -1;
            for (int m=0; m<moduleContexts.size(); m++) {
                if (moduleContexts[m].filename==e.fileName) {
                    externModuleContext = m;
                    break;
                }
            }
            if (externModuleContext==-1) {
                externModuleContext = currentModuleContext + 1;
                Kumir::String modulePath;
                if (e.fileName[0]==Kumir::Char('/') || s_programDirectory.length()==0)
                    modulePath = e.fileName;
                else {
                    modulePath = s_programDirectory;
                    modulePath.push_back(Kumir::Char('/'));
                    modulePath += e.fileName;
                }
                const std::string filename = Kumir::Coder::encode(VM_LOCALE, modulePath);
                std::ifstream externalfile(filename.c_str());
                if (
                        !Kumir::Files::exist(modulePath)
                        || !externalfile.is_open()
                   )
                {
                    int error = errno;
                    Kumir::String errorMessage = Kumir::Core::fromUtf8("Не могу загрузить внешний исполнитель: ")
                            +modulePath
                            +Kumir::Core::fromUtf8(" (ошибка ")
                            +Kumir::Converter::sprintfInt(error,10,0,0)
                            +Kumir::Core::fromAscii(") ")
                            ;
                    throw errorMessage;
                }
                Bytecode::Data programData;
                Bytecode::bytecodeFromDataStream(externalfile, programData);
                externalfile.close();
                setProgram(programData, false, e.fileName);
            }
            uint32_t key = 0x00000000;
            uint32_t alg = e.algId;
            uint32_t mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            uint32_t extKey = 0x00000000;
            bool found = false;
            const FunctionMap & externMap = moduleContexts[externModuleContext].functions;
            for (FunctionMap::const_iterator it = externMap.begin();
                 it != externMap.end();
                 ++it)
            {
                extKey = (*it).first;
                const String fname = (*it).second.name;
                if (fname==e.name) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                Kumir::String errorMessage = Kumir::Core::fromUtf8("Во внешнем исполнителе \"")+
                        e.fileName+
                        Kumir::Core::fromUtf8("\" нет алгоритма \"")+
                        e.name+
                        Kumir::Core::fromAscii("\"");
                throw errorMessage;
            }
            ExternReference reference;
            reference.funcKey = extKey;
            reference.moduleContext = externModuleContext;
            reference.moduleName = e.fileName;
            moduleContexts[currentModuleContext].externs[key] = reference;
        }
//        else if (e.type==EL_EXTERN) {
//#ifndef NO_EXTERNS
//            uint32_t key = 0x00000000;
//            uint32_t alg = e.algId;
//            uint32_t mod = e.module;
//            mod = mod << 16;
//            key = mod | alg;
//            moduleContexts[currentModuleContext].externs[key] = e;
//            TwoStrings qualifedKey;
//            qualifedKey.first = e.moduleName;
//            qualifedKey.second = e.signature.length()? e.signature : e.name;
//            if (availableExternalMethods.count(qualifedKey)==0 && m_externalHandler) {
//                std::list<String> moduleMethods;
//                if (m_externalHandler->loadExternalModule(e.moduleName, e.fileName, moduleMethods)) {
//                    typedef std::list<String>::const_iterator It;
//                    uint16_t id = 0;
//                    for (It i=moduleMethods.begin(); i!=moduleMethods.end(); ++i) {
//                        TwoStrings aKey(e.moduleName, *i);
//                        ExternalMethod method;
//                        method.moduleName = e.moduleName;
//                        method.methodId = id;
//                        availableExternalMethods[aKey] = method;
//                        id++;
//                    }
//                }
//            }
//            if (availableExternalMethods.count(qualifedKey)) {
//                ExternalMethod em = availableExternalMethods[qualifedKey];
//                externalMethods[key] = em;
//            }
//#endif
//        }
        else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_BELOWMAIN || e.type==EL_TESTING) {
            uint32_t key = 0x00000000;
            uint32_t alg = e.algId;
            uint32_t mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            moduleContexts[currentModuleContext].functions[key] = e;
            moduleContexts[currentModuleContext].exportModuleId = mod;
        }
        else if (e.type==EL_INIT ) {
            uint8_t key = e.module;
            moduleContexts[currentModuleContext].inits.push_back(e);
        }
    }
    for (FunctionMap::iterator it = moduleContexts[currentModuleContext].functions.begin();
         it!=moduleContexts[currentModuleContext].functions.end();
         ++it)
    {
        uint32_t key = (*it).first;
        const String & algName = (*it).second.name;
        if (locals.count(key)) {
            for (size_t j=0; j<locals[key].size(); j++) {
                locals[key][j].setAlgorhitmName(algName);
            }
        }
    }
    for (LocalsMap::iterator it = locals.begin(); it!=locals.end(); ++it) {
        uint32_t key = (*it).first;
        const VariantArray & arr = (*it).second;
        moduleContexts[currentModuleContext].cleanLocalTables[key] = arr;
    }
    currentLocals = 0;
    currentGlobals = 0;
    currentConstants = 0;
}

KumirVM::KumirVM() {
    e_entryPoint = EP_Main;
    b_blindMode = true;
    b_nextCallInto = false;
    m_dontTouchMe = 0;
    m_externalHandler = 0;
}

Variable KumirVM::fromTableElem(const Bytecode::TableElem &e) {
    Variable r = e.initialValue;
    r.setDimension(e.dimension);
    r.setBaseType(e.vtype.front());
    r.setName(e.name);
    r.setConstantFlag(e.type==EL_CONST);
    r.setModuleName(e.moduleName);
    return r;
}

void KumirVM::reset()
{
    // Clear everything
    last_context = Context();
    b_blindMode = false;
    b_nextCallInto = false;
    i_backtraceSkip = 0;
    s_error.clear();
    register0 = AnyValue();
    Variable::ignoreUndefinedError = false;
    stack_values.reset();
    stack_contexts.reset();
    mainProgram.instructions.clear();
    testingProgram.instructions.clear();
    currentLocals = 0;
    currentGlobals = 0;
    currentConstants = 0;

    // Find testing and main algorithms
    for (FunctionMap::iterator it=moduleContexts.front().functions.begin();
         it!=moduleContexts.front().functions.end();
         ++it)
    {
        const TableElem & e = (*it).second;
        if (e.type==EL_MAIN || e.type==EL_BELOWMAIN)
            mainProgram = e;
        if (e.type==EL_TESTING)
            testingProgram = e;
    }

    // Prepare startup context
    Context c;
    if (e_entryPoint==EP_Main && (mainProgram.type==EL_MAIN || mainProgram.type==EL_BELOWMAIN) ) {
        uint32_t mod = mainProgram.module;
        uint32_t alg = mainProgram.algId;
        uint32_t key = (mod << 16) | alg;
        c.locals = moduleContexts.front().cleanLocalTables[key];
        c.program = &(mainProgram.instructions);
        c.type = mainProgram.type;
        c.runMode = CRM_ToEnd;
        c.algId = mainProgram.algId;
        c.moduleId = mainProgram.module;
        c.name = mainProgram.name;
    }

    if (e_entryPoint==EP_Testing && testingProgram.type==EL_TESTING) {
        uint32_t mod = testingProgram.module;
        uint32_t alg = testingProgram.algId;
        uint32_t key = (mod << 16) | alg;
        c.locals = moduleContexts.front().cleanLocalTables[key];
        c.program = &(testingProgram.instructions);
        c.type = EL_TESTING;
        c.runMode = CRM_ToEnd;
        c.algId = testingProgram.algId;
        c.moduleId = testingProgram.module;
        c.name = testingProgram.name;
    }
    c.IP = 0;

    currentGlobals = &(moduleContexts[0].globals.back());
    currentConstants = &(moduleContexts[0].constants);

    if (m_externalHandler)
        m_externalHandler->debuggerReset();

    // Push startup context to stack (if non empty)
    if (c.program) {
        stack_contexts.push(c);
        currentLocals = &(stack_contexts.top().locals);
        currentGlobals = &(moduleContexts[0].globals[c.moduleId]);
    }


    // Each kumir module have 'initialization' section,
    // so push all these sections (if any) into stack
    // to call them BEFORE startup context
    for (int moduleContextNo=moduleContexts.size()-1;
         moduleContextNo>=0;
         moduleContextNo--)
    {
        const ModuleContext & currentModule = moduleContexts.at(moduleContextNo);
        const std::deque<Bytecode::TableElem> & inits = currentModule.inits;
        for (int initNo=inits.size()-1; initNo>=0; initNo--)
        {
            const Bytecode::TableElem & e = inits.at(initNo);
            if (e.instructions.size()>0) {
                Context initContext;
                initContext.program = &(e.instructions);
                initContext.type = EL_INIT;
                initContext.runMode = moduleContextNo==0? CRM_OneStep : CRM_ToEnd;
                initContext.moduleId = e.module;
                initContext.algId = -1;
                initContext.IP = -1;
                initContext.moduleContextNo = moduleContextNo;
                stack_contexts.push(initContext);
                currentConstants = &(moduleContexts[moduleContextNo].constants);
                currentGlobals = &(moduleContexts[moduleContextNo].globals[e.module]);
            }
        }
    }
    // Prepare standard library
    Kumir::initStandardLibrary();

    // Push globals to debugger
    for (size_t i_context=0; i_context<moduleContexts.size(); i_context++) {
        const ModuleContext & mc = moduleContexts[i_context];
        const GlobalsMap & contextGlobals = mc.globals;
        for (size_t i_module=0; i_module<contextGlobals.size(); i_module++) {
            const Kumir::String & contextName = i_context==0
                    ? Kumir::String()
                    : mc.filename;
            const Kumir::String & moduleName = Kumir::String();
            const Kumir::String fullName = moduleName.length()>0
                    ? moduleName
                    : contextName;
            debuggerPushContext(
                        true,
                        fullName,
                        &(contextGlobals[i_module]),
                        false
                        );
        }
    }
}

void KumirVM::debuggerPopContext()
{
    if (m_externalHandler==nullptr
            || b_blindMode)
    {
        return; // show nothing
    }
    if (currentContext().runMode==CRM_OneStep)
        m_externalHandler->debuggerPopContext();
    else
        m_externalHandler->debuggerForceUpdateValues();
}

void KumirVM::debuggerPushContext(
        const bool isGlobals,
        const Kumir::String &name,
        const std::vector<Variable> *variables,
        bool force)
{
    if (m_externalHandler==nullptr)
        return;
    if (!force)
        if (
            b_blindMode
            || currentContext().runMode!=CRM_OneStep
        )
    {
        return; // show nothing
    }
    if (isGlobals && variables->size()==0) {
        return; // nothing to push
    }
    std::deque<Kumir::String> names;
    std::deque<Kumir::String> types;
    std::deque<uint8_t> dims;
    for (size_t i=0; i<variables->size(); i++) {
        const Variable & var = variables->at(i);
        names.push_back(var.name());
        switch (var.baseType()) {
            case VT_int: types.push_back(Kumir::Core::fromUtf8("цел")); break;
            case VT_real: types.push_back(Kumir::Core::fromUtf8("вещ")); break;
            case VT_bool: types.push_back(Kumir::Core::fromUtf8("лог")); break;
            case VT_string: types.push_back(Kumir::Core::fromUtf8("лит")); break;
            case VT_char: types.push_back(Kumir::Core::fromUtf8("сим")); break;
            default: types.push_back(Kumir::Core::fromUtf8("")); break;
        }
        dims.push_back(var.dimension());
    }
    if (isGlobals)
        m_externalHandler->debuggerSetGlobals(name, names, types, dims);
    else
        m_externalHandler->debuggerPushContext(name, names, types, dims);
}

void KumirVM::nextIP()
{
    if (stack_contexts.size()>0) {
        stack_contexts.top().IP ++;
    }
}


bool KumirVM::hasMoreInstructions() const
{
    if (stack_contexts.size()>0) {
        const std::vector<Bytecode::Instruction> * program = stack_contexts.at(0).program;
        int IP = stack_contexts.at(0).IP;
        return program && IP < int(program->size());
    }
    else {
        return false;
    }
}

void KumirVM::evaluateNextInstruction()
{
    int ip = stack_contexts.top().IP;
    if (ip==-1) {
        ip = 0;
    }
    const std::vector<Instruction> * program = stack_contexts.top().program;
    const Instruction & instr = program->at(ip);
    switch (instr.type) {
    case CALL:
        do_call(instr.module, instr.arg);
        break;
    case INIT:
        do_init(instr.scope, instr.arg);
        break;
    case SETARR:
        do_setarr(instr.scope, instr.arg);
        break;
    case UPDARR:
        do_updarr(instr.scope, instr.arg);
        break;
    case STORE:
        do_store(instr.scope, instr.arg);
        break;
    case STOREARR:
        do_storearr(instr.scope, instr.arg);
        break;
    case LOAD:
        do_load(instr.scope, instr.arg);
        break;
    case LOADARR:
        do_loadarr(instr.scope, instr.arg);
        break;
    case JUMP:
        do_jump(instr.arg);
        break;
    case JNZ:
        do_jnz(instr.registerr, instr.arg);
        break;
    case JZ:
        do_jz(instr.registerr, instr.arg);
        break;
    case POP:
        do_pop(instr.registerr);
        break;
    case PUSH:
        do_push(instr.registerr);
        break;
    case RET:
        do_ret();
        break;
    case ERRORR:
        do_error(instr.scope, instr.arg);
        break;
    case LINE:
        do_line(instr.arg);
        break;
    case REF:
        do_ref(instr.scope, instr.arg);
        break;
    case REFARR:
        do_refarr(instr.scope, instr.arg);
        break;
    case SETREF:
        do_setref(instr.scope, instr.arg);
        break;
    case CTL:
        do_ctl(instr.module, instr.arg);
        break;
    case INRANGE:
        do_inrange();
        break;
    case SUM:
        do_sum();
        break;
    case SUB:
        do_sub();
        break;
    case MUL:
        do_mul();
        break;
    case DIV:
        do_div();
        break;
    case POW:
        do_pow();
        break;
    case NEG:
        do_neg();
        break;
    case AND:
        do_and();
        break;
    case OR:
        do_or();
        break;
    case EQ:
        do_eq();
        break;
    case NEQ:
        do_neq();
        break;
    case LS:
        do_ls();
        break;
    case GT:
        do_gt();
        break;
    case LEQ:
        do_leq();
        break;
    case GEQ:
        do_geq();
        break;
    case SHOWREG:
        do_showreg(instr.registerr);
        break;
    case CLEARMARG:
        do_clearmarg(instr.arg);
        break;
    case PAUSE:
        do_pause(instr.arg);
        break;
    case HALT:
        do_halt(instr.arg);
        break;
    default:
        nextIP();
        break;
    }
    if (s_error.length()==0 && Kumir::Core::getError().length()>0)
        s_error = Kumir::Core::getError();
    if (m_externalHandler &&
            s_error.length()>0 &&
            currentContext().runMode!=CRM_OneStep)
    {
        updateDebugger();
    }
}

/***** BEGIN INSTRUCTIONS IMPLEMENTATION *****/

void KumirVM::do_call(uint8_t mod, uint16_t alg)
{
    uint32_t module = mod << 16;
    uint32_t algorithm = alg;
    uint32_t p = module | algorithm;

    if (mod==0xF0) // stdlib
        do_stdcall(alg);
    else if (mod==0xF1)
        do_filescall(alg);
    else if (mod==0xF2)
        do_stringscall(alg);
    else if (mod==0xFF)
        do_specialcall(alg);
//#ifndef NO_EXTERNS
//    else if (externalMethods.count(p)) {
//        const ExternalMethod & em = externalMethods[p];
//        if (m_externalHandler) {
//            do_externalcall(em.moduleName, em.methodId);
//        }
//        else {
//            s_error = Kumir::Core::fromUtf8("Вызов алгоритмов исполнителей запрещен");
//        }
//    }
//#endif
    else if (moduleContexts[stack_contexts.top().moduleContextNo].functions.count(p)) {

        if (stack_contexts.size()>=MAX_RECURSION_SIZE) {
            s_error = Kumir::Core::fromUtf8("Слишком много вложенных вызовов алгоритмов");
        }
        else {
            if (m_dontTouchMe)
                m_dontTouchMe->lock();
            Context c;
            c.program = & (moduleContexts[stack_contexts.top().moduleContextNo].functions[p].instructions );
            c.locals = moduleContexts[stack_contexts.top().moduleContextNo].cleanLocalTables[p];
            c.type = moduleContexts[stack_contexts.top().moduleContextNo].functions[p].type;
            if (b_nextCallInto)
                c.runMode = CRM_OneStep;
            else if (stack_contexts.top().type==EL_BELOWMAIN && c.type==EL_MAIN)
                c.runMode = stack_contexts.top().runMode;
            else
                c.runMode = CRM_ToEnd;
            c.moduleId = moduleContexts[stack_contexts.top().moduleContextNo].functions[p].module;
            c.algId = moduleContexts[stack_contexts.top().moduleContextNo].functions[p].algId;
            if (!b_blindMode)
                c.name = moduleContexts[stack_contexts.top().moduleContextNo].functions[p].name;
            c.moduleContextNo = stack_contexts.top().moduleContextNo;
            stack_contexts.push(c);
            if (c.runMode==CRM_OneStep)
                debuggerPushContext(false, c.name, &(stack_contexts.top().locals),false);
            b_nextCallInto = false;
            stack_values.pop(); // current implementation doesn't requere args count
            currentLocals = &(stack_contexts.top().locals);
            currentGlobals = &(moduleContexts[c.moduleContextNo].globals[c.moduleId]);
            currentConstants = &(moduleContexts[c.moduleContextNo].constants);
            if (m_dontTouchMe)
                m_dontTouchMe->unlock();
        }

    }
    else if (moduleContexts[stack_contexts.top().moduleContextNo].externs.count(p)) {
        if (stack_contexts.size()>=MAX_RECURSION_SIZE) {
            s_error = Kumir::Core::fromUtf8("Слишком много вложенных вызовов алгоритмов");
        }
        else {
            if (m_dontTouchMe)
                m_dontTouchMe->lock();
            ExternReference reference = moduleContexts[stack_contexts.top().moduleContextNo].externs[p];
            uint32_t key = reference.funcKey;
            Context c;
            c.program = & (moduleContexts[reference.moduleContext].functions[key].instructions );
            c.locals = moduleContexts[reference.moduleContext].cleanLocalTables[key];
            c.type = moduleContexts[reference.moduleContext].functions[key].type;
            c.runMode = CRM_ToEnd;
            c.moduleId = moduleContexts[reference.moduleContext].functions[key].module;
            c.algId = moduleContexts[reference.moduleContext].functions[key].algId;
            c.moduleContextNo = reference.moduleContext;
            stack_contexts.push(c);
            currentLocals = &(stack_contexts.top().locals);
            currentGlobals = &(moduleContexts[c.moduleContextNo].globals[c.moduleId]);
            currentConstants = &(moduleContexts[reference.moduleContext].constants);
            b_nextCallInto = false;
            stack_values.pop(); // current implementation doesn't requere args count
            if (m_dontTouchMe)
                m_dontTouchMe->unlock();
        }
    }
    else {
        s_error = Kumir::Core::fromUtf8("Вызов алгоритма из недоступного исполнителя");
    }
    if (Kumir::Core::getError().length()>0 && s_error.length()==0) {
        s_error = Kumir::Core::getError();
    }
    nextIP();
}

void KumirVM::do_stdcall(uint16_t alg)
{
    if (m_dontTouchMe)
        m_dontTouchMe->lock();
    stack_values.pop(); // remove arguments count -- all is known
    switch(alg) {
    /* алг вещ abs(вещ x) */
    case 0x0000: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::abs(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ arccos(вещ x) */
    case 0x0001: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::arccos(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ arcctg(вещ x) */
    case 0x0002: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::arcctg(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ arcsin(вещ x) */
    case 0x0003: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::arcsin(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ arctg(вещ x) */
    case 0x0004: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::arctg(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ cos(вещ x) */
    case 0x0005: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::cos(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ ctg(вещ x) */
    case 0x0006: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::ctg(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг delay(цел x) */
    case 0x0007: {
        // TODO not implemented
        break;
    }
    /* алг цел div(цел x, цел y) */
    case 0x0008: {
        int y = stack_values.pop().toInt();
        int x = stack_values.pop().toInt();
        int r = Kumir::Math::div(x, y);
        stack_values.push(Variable(r));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ exp(вещ x) */
    case 0x0009: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::exp(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг цел iabs(цел x) */
    case 0x000a: {
        int x = stack_values.pop().toInt();
        int y = Kumir::Math::iabs(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел imax(цел x, цел y) */
    case 0x000b: {
        int y = stack_values.pop().toInt();
        int x = stack_values.pop().toInt();
        int r = Kumir::Math::imax(x, y);
        stack_values.push(Variable(r));
        break;
    }
    /* алг цел imin(цел x, цел y) */
    case 0x000c: {
        int y = stack_values.pop().toInt();
        int x = stack_values.pop().toInt();
        int r = Kumir::Math::imin(x, y);
        stack_values.push(Variable(r));
        break;
    }
    /* алг цел int(вещ x) */
    case 0x000d: {
        real x = stack_values.pop().toReal();
        int y = Kumir::Math::intt(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел irand(цел x, цел y) */
    case 0x000e: {
        int y = stack_values.pop().toInt();
        int x = stack_values.pop().toInt();
        int r = Kumir::Random::irand(x, y);
        stack_values.push(Variable(r));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг цел irnd(цел x) */
    case 0x000f: {
        int x = stack_values.pop().toInt();
        int y = Kumir::Random::irnd(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ lg(вещ x) */
    case 0x0010: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::lg(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ ln(вещ x) */
    case 0x0011: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::ln(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ max(вещ x, вещ y) */
    case 0x0012: {
        real  y = stack_values.pop().toReal();
        real  x = stack_values.pop().toReal();
        real  r = Kumir::Math::rmax(x, y);
        stack_values.push(Variable(r));
        break;
    }
    /* алг вещ min(вещ x, вещ y) */
    case 0x0013: {
        real  y = stack_values.pop().toReal();
        real  x = stack_values.pop().toReal();
        real  r = Kumir::Math::rmin(x, y);
        stack_values.push(Variable(r));
        break;
    }
    /* алг цел mod(цел x, цел y) */
    case 0x0014: {
        int y = stack_values.pop().toInt();
        int x = stack_values.pop().toInt();
        int r = Kumir::Math::mod(x, y);
        stack_values.push(Variable(r));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ rand(вещ x, вещ y) */
    case 0x0015: {
        real  y = stack_values.pop().toReal();
        real  x = stack_values.pop().toReal();
        real  r = Kumir::Random::rrand(x, y);
        stack_values.push(Variable(r));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ rnd(вещ x) */
    case 0x0016: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Random::rrnd(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел sign(вещ x) */
    case 0x0017: {
        real x = stack_values.pop().toReal();
        int y = Kumir::Math::sign(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ sin(вещ x) */
    case 0x0018: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::sin(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ sqrt(вещ x) */
    case 0x0019: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::sqrt(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ tg(вещ x) */
    case 0x001a: {
        real x = stack_values.pop().toReal();
        real y = Kumir::Math::tg(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг вещ МАКСВЕЩ */
    case 0x001b: {
        real r = Kumir::Math::maxreal();
        stack_values.push(Variable(r));
        break;
    }
    /* алг цел МАКСЦЕЛ */
    case 0x001c: {
        int r = Kumir::Math::maxint();
        stack_values.push(Variable(r));
        break;
    }
    /* алг лит вещ_в_лит(вещ x) */
    case 0x001d: {
        real x = stack_values.pop().toReal();
        const Kumir::String y = Kumir::Converter::realToString(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел время */
    case 0x001e: {
        int r = Kumir::System::time();
        stack_values.push(Variable(r));
        break;
    }
    /* алг цел длин(лит s) */
    case 0x001f: {
        const String x = stack_values.pop().toString();
        int y = Kumir::StringUtils::length(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел код(сим ch) */
    case 0x0020: {
        Char x = stack_values.pop().toChar();
        int y = Kumir::StringUtils::code(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вещ лит_в_вещ(лит s, рез лог success) */
    case 0x0021: {
        Variable sf = stack_values.pop().toReference();
        const String x = stack_values.pop().toString();
        bool ok;
        real y = Kumir::Converter::stringToReal(x, ok);
        stack_values.push(Variable(y));
        sf.setValue(AnyValue(ok));
        break;
    }
    /* алг цел лит_в_цел(лит s, рез лог success) */
    case 0x0022: {
        Variable sf = stack_values.pop().toReference();
        const String x = stack_values.pop().toString();
        bool ok;
        int y = Kumir::Converter::stringToInt(x, ok);
        stack_values.push(Variable(y));
        sf.setValue(AnyValue(ok));
        break;
    }
    /* алг сим символ(цел n) */
    case 0x0023: {
        int x = stack_values.pop().toInt();
        Char y = Kumir::StringUtils::symbol(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг сим символ2(цел n) */
    case 0x0024: {
        int x = stack_values.pop().toInt();
        Char y = Kumir::StringUtils::unisymbol(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лит цел_в_лит(цел n) */
    case 0x0025: {
        int x = stack_values.pop().toInt();
        String y = Kumir::Converter::intToString(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг цел юникод(сим ch) */
    case 0x0026: {
        Char x = stack_values.pop().toChar();
        int y = Kumir::StringUtils::unicode(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    default: {
        s_error = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (m_dontTouchMe)
        m_dontTouchMe->unlock();
}

void KumirVM::do_filescall(uint16_t alg)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    stack_values.pop(); // Args count
    switch (alg) {
    /* алг файл открыть на чтение(лит имя файла) */
    case 0x0000: {
        const String x = stack_values.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Read);
        Record yy = toRecordValue(y);
        Variable res(yy);
        stack_values.push(res);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг файл открыть на запись(лит имя файла) */
    case 0x0001: {
        const String x = stack_values.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Write);
        Record yy = toRecordValue(y);
        stack_values.push(Variable(yy));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг файл открыть на добавление(лит имя файла) */
    case 0x0002: {
        const String x = stack_values.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Append);
        Record yy = toRecordValue(y);
        stack_values.push(Variable(yy));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг закрыть(файл ключ) */
    case 0x0003: {
        const Variable xvar = stack_values.pop();
        const Record xx = xvar.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        Kumir::Files::close(x);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг начать чтение(файл ключ) */
    case 0x0004: {
        const Variable & xval = stack_values.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        Kumir::Files::reset(x);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лог конец файла(файл ключ) */
    case 0x0005: {
        const Variable & xval = stack_values.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        bool y = Kumir::Files::eof(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг установить кодировку(лит имя кодировки) */
    case 0x0006: {
        const String x = stack_values.pop().toString();
        Kumir::Files::setFileEncoding(x);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лог можно открыть на чтение(лит имя файла) */
    case 0x0007: {
        const String x = stack_values.pop().toString();
        bool y = Kumir::Files::canOpenForRead(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лог можно открыть на запись(лит имя файла) */
    case 0x0008: {
        const String x = stack_values.pop().toString();
        bool y = Kumir::Files::canOpenForWrite(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лог есть данные(файл ключ) */
    case 0x0009: {
        const Variable xval = stack_values.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        bool y = Kumir::Files::hasData(x);
        stack_values.push(Variable(y));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лог существует(лит имя файла Или каталога) */
    case 0x000a: {
        const String x = stack_values.pop().toString();
        bool y = Kumir::Files::exist(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг лог является каталогом(лит имя файла Или каталога) */
    case 0x000b: {
        const String x = stack_values.pop().toString();
        bool y = Kumir::Files::isDirectory(x);
        stack_values.push(Variable(y));
        break;
    }
    case 0x000c: {
        const String x = stack_values.pop().toString();
        bool y = Kumir::Files::mkdir(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг лит полный путь(лит имя файла Или каталога) */
    case 0x000d: {
        const String x = stack_values.pop().toString();
        const String y = Kumir::Files::getAbsolutePath(x);
        stack_values.push(Variable(y));
        break;
    }
    /* алг удалить_файл(лит имя файла) */
    case 0x000e: {
        const String x = stack_values.pop().toString();
        Kumir::Files::unlink(x);
        break;
    }
    /* алг НАЗНАЧИТЬ ВВОД(лит имя файла) */
    case 0x000f: {
        const String x = stack_values.pop().toString();
        Kumir::Files::assignInStream(x);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг НАЗНАЧИТЬ ВЫВОД(лит имя файла) */
    case 0x0010: {
        const String x = stack_values.pop().toString();
        Kumir::Files::assignOutStream(x);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лит РАБОЧИЙ КАТАЛОГ */
    case 0x0011: {
        const String res = Kumir::Files::CurrentWorkingDirectory();
        stack_values.push(Variable(res));
        break;
    }
    /* алг лит КАТАЛОГ ПРОГРАММЫ */
    case 0x0012: {
        String res;
        if (s_programDirectory.length()==0)
            res = Kumir::Files::CurrentWorkingDirectory();
        else
            res = s_programDirectory;
        stack_values.push(Variable(res));
        break;
    }
    /* алг лог =(фaйл ф1, файл ф2) */
    case 0x0014: {
        const Variable f2var = stack_values.pop();
        const Variable f1var = stack_values.pop();
        const Record f2rec = f2var.toRecord();
        const Record f1rec = f1var.toRecord();
        const Kumir::FileType f1 = fromRecordValue(f1rec);
        const Kumir::FileType f2 = fromRecordValue(f2rec);
        stack_values.push(Variable(f1==f2));
        break;
    }
    /* алг лог =(фaйл ф1, файл ф2) */
    case 0x0015: {
        const Variable f2var = stack_values.pop();
        const Variable f1var = stack_values.pop();
        const Record f2rec = f2var.toRecord();
        const Record f1rec = f1var.toRecord();
        const Kumir::FileType f1 = fromRecordValue(f1rec);
        const Kumir::FileType f2 = fromRecordValue(f2rec);
        stack_values.push(Variable(f1!=f2));
        break;
    }
    /* алг удалить_каталог(лит имя файла) */
    case 0x0013: {
        const String x = stack_values.pop().toString();
        Kumir::Files::rmdir(x);
        break;
    }
    default: {
        s_error = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_stringscall(uint16_t alg)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    stack_values.pop(); // Args count
    switch (alg) {
    /* алг лит верхний регистр(лит строка) */
    case 0x0000: {
        const String x = stack_values.pop().toString();
        const String y = Kumir::StringUtils::toUpperCase(x);
        Variable res(y);
        stack_values.push(res);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг лит нижний регистр(лит строка) */
    case 0x0001: {
        const String x = stack_values.pop().toString();
        const String y = Kumir::StringUtils::toLowerCase(x);
        Variable res(y);
        stack_values.push(res);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг цел позиция после(цел от, лит фрагмент, лит строка) */
    case 0x0002: case 0x0007: {
        const String s = stack_values.pop().toString();
        const String sub = stack_values.pop().toString();
        const int from = stack_values.pop().toInt();
        const int y = Kumir::StringUtils::find(from+1, sub, s);
        Variable res(y);
        stack_values.push(res);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг цел позиция(лит фрагмент, лит строка) */
    case 0x0003: case 0x0008: {
        const String s = stack_values.pop().toString();
        const String sub = stack_values.pop().toString();
        const int y = Kumir::StringUtils::find(sub, s);
        Variable res(y);
        stack_values.push(res);
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг вставить(лит фрагмент, аргрез лит строка, цел позиция) */
    case 0x0004: {
        const int from = stack_values.pop().toInt();
        Variable sr = stack_values.pop().toReference();
        String s = sr.value().toString();
        const String sub = stack_values.pop().toString();
        Kumir::StringUtils::insert(sub, s, from);
        sr.setValue(AnyValue(s));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг заменить(аргрез лит строка, лит старый фрагмент, лит новый фрагмент, лог каждый) */
    case 0x0005: {
        const bool all = stack_values.pop().toBool();
        const String newSub = stack_values.pop().toString();
        const String oldSub = stack_values.pop().toString();
        Variable sr = stack_values.pop().toReference();
        String s = sr.value().toString();
        Kumir::StringUtils::replace(s, oldSub, newSub, all);
        sr.setValue(AnyValue(s));
        s_error = Kumir::Core::getError();
        break;
    }
    /* алг удалить(аргрез лит строка, цел начало, цел количество) */
    case 0x0006: {
        const int count = stack_values.pop().toInt();
        const int from = stack_values.pop().toInt();
        Variable sr = stack_values.pop().toReference();
        String s = sr.value().toString();
        Kumir::StringUtils::remove(s, from, count);
        sr.setValue(AnyValue(s));
        s_error = Kumir::Core::getError();
        break;
    }

    default: {
        s_error = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}


void KumirVM::do_externalcall(const String &name, uint16_t id)
{
    if (m_dontTouchMe)
        m_dontTouchMe->lock();
    // Prepare arguments
    int argsCount = stack_values.pop().toInt();
    std::deque<Variable> args;
    std::deque<Variable> refs;
    for (int i=0; i<argsCount; i++) {
        const Variable val = stack_values.pop();
        args.push_back(val);
    }
    // Prepare references
    int refsCount = stack_values.pop().toInt();
    for (int i=0; i<refsCount; i++) {
        Variable ref = stack_values.pop();
        refs.push_back(ref);
    }
    String error;
    if (m_dontTouchMe)
        m_dontTouchMe->unlock();
    // Make actual call
    Variable result;
    bool processed = m_externalHandler->evaluateExternalFunction(
                name, id, args, refs, result, error
                );
    if (!processed) {
        s_error = Kumir::Core::fromUtf8("Вызов алгоритмов исполнителей запрещен");
        return;
    }
    if (error.length()>0 && s_error.length()==0 && Kumir::Core::getError().length()==0)
        s_error = error;
    // Push external function result to stack
    if (result.isValid()) {
        if (m_dontTouchMe) m_dontTouchMe->lock();
        stack_values.push(Variable(result));
        if (m_dontTouchMe) m_dontTouchMe->unlock();
    }
}

//template <class T>
//T KumirVM::fromRecordValue(const Record & record)
//{
//    T result;
//    T * ptr = & result;
//    size_t offset = 0;
//    const size_t N = Kumir::Math::imin(strlen(T::_()), record.size());
//    for (size_t i=0; i<N; i++) {
//        switch (T::_()[i]) {
//        case 'i': {
//            int * pval = reinterpret_cast<int*>(ptr+offset);
//            *pval = record.at(i).toInt();
//            break;
//        }
//        case 'd': {
//            Kumir::real * pval = reinterpret_cast<Kumir::real*>(ptr+offset);
//            *pval = record.at(i).toReal();
//            break;
//        }
//        case 'b': {
//            bool * pval = reinterpret_cast<bool*>(ptr+offset);
//            *pval = record.at(i).toBool();
//            break;
//        }
//        case 'c': {
//            Kumir::Char * pval = reinterpret_cast<Kumir::Char*>(ptr+offset);
//            *pval = record.at(i).toChar();
//            break;
//        }
//        case 's': {
//            Kumir::String * pval = reinterpret_cast<Kumir::String*>(ptr+offset);
//            *pval = record.at(i).toString();
//            break;
//        }
//        default:
//            break;
//        }
//    }
//    return result;
//}

//template <class T>
//Record KumirVM::toRecordValue(const T & t)
//{
//    Record result;
//    const T * ptr = & t;
//    size_t offset = 0;
//    const size_t N = strlen(T::_());
//    for (size_t i=0; i<N; i++) {
//        switch (T::_()[i]) {
//        case 'i': {
//            const int * pval = reinterpret_cast<const int*>(ptr+offset);
//            result.push_back(AnyValue(*pval));
//            break;
//        }
//        case 'd': {
//            const Kumir::real * pval = reinterpret_cast<const Kumir::real*>(ptr+offset);
//            result.push_back(AnyValue(*pval));
//            break;
//        }
//        case 'b': {
//            const bool * pval = reinterpret_cast<const bool*>(ptr+offset);
//            result.push_back(AnyValue(*pval));
//            break;
//        }
//        case 'c': {
//            const Kumir::Char * pval = reinterpret_cast<const Kumir::Char*>(ptr+offset);
//            result.push_back(AnyValue(*pval));
//            break;
//        }
//        case 's': {
//            const Kumir::String * pval = reinterpret_cast<const Kumir::String*>(ptr+offset);
//            result.push_back(AnyValue(*pval));
//            break;
//        }
//        default:
//            break;
//        }
//    }
//    return result;
//}

inline Kumir::FileType KumirVM::fromRecordValue(const Record & record) {
    Kumir::FileType result;
    result.fullPath = record.at(0).toString();
    result.mode = record.at(1).toInt();
    result.valid = record.at(2).toBool();
    return result;
}

inline Record KumirVM::toRecordValue(const Kumir::FileType & ft) {
    Record record;
    record.resize(3);
    record.at(0) = AnyValue(ft.fullPath);
    record.at(1) = AnyValue(ft.mode);
    record.at(2) = AnyValue(ft.valid);
    return record;
}

void KumirVM::do_specialcall(uint16_t alg)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    int argsCount = stack_values.pop().toInt();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    // Special calls
    if (alg==0x00) {
        // Input
        if (m_dontTouchMe) m_dontTouchMe->lock();
        bool fileIO = false;
        int varsCount = argsCount;
        Kumir::FileType fileReference;
        std::deque<Variable> references;
        for (int i=0; i<varsCount; i++) {
            const Variable & ref = stack_values.pop();
            if (ref.baseType()==VT_record) {
                fileIO = true;
                if (!ref.isValid()) {
                    s_error = Kumir::Core::fromUtf8("Нет значения у ключа файла");
                    return;
                }
                const Record fileReferenceRecord = ref.toRecord();
                fileReference = fromRecordValue(fileReferenceRecord);
            }
            else {
                references.push_back(ref);
            }
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
        if (m_externalHandler && !fileIO && !Kumir::Files::overloadedStdIn() && m_externalHandler->makeInput(references)) {
            // pass
        }
        else {
            if (m_dontTouchMe) m_dontTouchMe->lock();

            for (int i=0; i<references.size(); i++) {
                if (references.at(i).baseType()==VT_int) {
                    int value = Kumir::IO::readInteger(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                else if (references.at(i).baseType()==VT_real) {
                    real value = Kumir::IO::readReal(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                else if (references.at(i).baseType()==VT_bool) {
                    bool value = Kumir::IO::readBool(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                else if (references.at(i).baseType()==VT_char &&
                         references.at(i).isConstant() &&
                         references.at(i).value().toChar()==Char('\n')
                         ) {
                    // Skip everything to next line
                    Kumir::IO::readLine(fileReference, !fileIO);
                }
                else if (references.at(i).baseType()==VT_char) {
                    Char value = Kumir::IO::readChar(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                else if (references.at(i).baseType()==VT_string) {
                    const String & value = Kumir::IO::readString(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                if (Kumir::Core::getError().length()>0 && s_error.length()==0) {
                    s_error = Kumir::Core::getError();
                }
                if (s_error.length()>0)
                    break;
            }
            if (m_dontTouchMe) m_dontTouchMe->unlock();
        }
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 && m_externalHandler &&
                !b_blindMode &&
                stack_contexts.top().type != EL_BELOWMAIN &&
                s_error.empty()
                )
        {
            String margin;
            margin.reserve(100);
            for (int i=0; i<references.size(); i++) {
                if (references.at(i).isConstant())
                    continue;
                if (i>0) {
                    margin.push_back(',');
                    margin.push_back(' ');
                }
                margin += references.at(i).fullReferenceName();
                margin.push_back('=');
                if (references.at(i).baseType()==VT_char)
                    margin.push_back('\'');
                if (references.at(i).baseType()==VT_string)
                    margin.push_back('"');
                margin += references.at(i).value().toString();
                if (references.at(i).baseType()==VT_char)
                    margin.push_back('\'');
                if (references.at(i).baseType()==VT_string)
                    margin.push_back('"');
            }
            m_externalHandler->noticeOnValueChange(lineNo, margin);
        }
    }
    if (alg==0x01) {
        // Output
        if (m_dontTouchMe) m_dontTouchMe->lock();
        bool fileIO = false;
        Kumir::FileType fileReference;
        if (argsCount % 3) {
            fileIO = true;
            const Variable fileVar = stack_values.pop();
            if (!fileVar.isValid()) {
                s_error = Kumir::Core::fromUtf8("Нет значения у ключа файла");
                return;
            }
            const Record fileReferenceRecord = fileVar.toRecord();
            fileReference = fromRecordValue(fileReferenceRecord);

        }
        int varsCount = argsCount / 3;
        std::deque< std::pair<int,int> > formats;
        std::deque<Variable> values;
        for (int i=0; i<varsCount; i++) {
            std::pair<int, int> format;
            format.second = stack_values.pop().toInt();
            format.first = stack_values.pop().toInt();
            formats.push_back(format);
            const Variable & ref = stack_values.pop();
            values.push_back(ref);
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
        if (m_externalHandler && !fileIO && !Kumir::Files::overloadedStdOut() && m_externalHandler->makeOutput(formats, values)) {
            // pass
        }
        else {
            for (int i=0; i<varsCount; i++) {
                if (values.at(i).baseType()==VT_int) {
                    Kumir::IO::writeInteger(formats[i].first, values.at(i).toInt(), fileReference, !fileIO);
                }
                else if (values.at(i).baseType()==VT_real) {
                    Kumir::IO::writeReal(formats[i].first, formats[i].second, values.at(i).toReal(), fileReference, !fileIO);
                }
                else if (values.at(i).baseType()==VT_bool) {
                    Kumir::IO::writeBool(formats[i].first, values.at(i).toBool(), fileReference, !fileIO);
                }
                else if (values.at(i).baseType()==VT_char) {
                    Kumir::IO::writeChar(formats[i].first, values.at(i).toChar(), fileReference, !fileIO);
                }
                else if (values.at(i).baseType()==VT_string) {
                    Kumir::IO::writeString(formats[i].first, values.at(i).toString(), fileReference, !fileIO);
                }
                if (Kumir::Core::getError().length()>0 && s_error.length()==0) {
                    s_error = Kumir::Core::getError();
                }
                if (s_error.length()>0)
                    break;
            }
        }
    }
    else if (alg==0x04) {
        // Get char from string
        if (m_dontTouchMe) m_dontTouchMe->lock();
        Variable second = stack_values.pop();
        Variable first = stack_values.pop();
        int index = second.value().toInt();
        const String & s = first.value().toString();
        s_error = Kumir::Core::getError();
        if (s_error.length()==0) {
            if (index<1 || index>s.length()) {
                s_error = Kumir::Core::fromUtf8("Индекс символа больше длины строки");
            }
            else {
                Char result = s[index-1];
                Variable r(result);
                stack_values.push(r);
            }
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
    }
    else if (alg==0x05) {
        // Set char in string
        if (m_dontTouchMe) m_dontTouchMe->lock();
        Variable third = stack_values.pop();
        Variable second = stack_values.pop();
        Variable first = stack_values.pop();
        int index = third.value().toInt();
        String source = second.value().toString();
        Char ch = first.value().toChar();
        s_error = Kumir::Core::getError();
        if (s_error.length()==0) {
            if (index<1) {
                s_error = Kumir::Core::fromUtf8("Индекс символа меньше 1");
            }
            else if (index>source.length()) {
                s_error = Kumir::Core::fromUtf8("Индекс символа больше длины строки");
            }
            else {
                source[index-1] = ch;
                Variable r(source);
                stack_values.push(r);
            }
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
    }
    else if (alg==0x06) {
        // Get slice from string
        if (m_dontTouchMe) m_dontTouchMe->lock();
        Variable third = stack_values.pop();
        Variable second = stack_values.pop();
        Variable first = stack_values.pop();
        int start = second.value().toInt();
        int end   = third.value().toInt();
        const String & s = first.value().toString();
        s_error = Kumir::Core::getError();
        if (s_error.length()==0) {
            if (start<1 || start>s.length()) {
                s_error = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start) {
                String empty;
                stack_values.push(Variable(empty));
            }
            else if (end<1 || end>s.length()) {
                s_error = Kumir::Core::fromUtf8("Правая граница вырезки за пределами строки");
            }
            else {
                String result = s.substr(start-1, end-start+1);
                Variable r(result);
                stack_values.push(r);
            }
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
    }
    else if (alg==0x07) {
        // Set slice in string
        if (m_dontTouchMe) m_dontTouchMe->lock();
        Variable fourth = stack_values.pop();
        Variable third = stack_values.pop();
        Variable second = stack_values.pop();
        Variable first = stack_values.pop();
        int end = fourth.value().toInt();
        int start = third.value().toInt();
        String source = second.value().toString();
        String ch = first.value().toString();
        s_error = Kumir::Core::getError();
        if (s_error.length()==0) {
            if (end<start && start==0) {
                source = ch + source;
                Variable r(source);
                stack_values.push(r);
            }
            else if (start>source.length()) {
                s_error = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start && start>0) {
                source.insert(start, ch);
                Variable r(source);
                stack_values.push(r);
            }
            else if (start==source.length()+1 && end<=start) {
                source.append(ch);
                Variable r(source);
                stack_values.push(r);
            }
            else if (end<1 || end>source.length()) {
                s_error = Kumir::Core::fromUtf8("Правая граница вырезки за пределами строки");
            }
            else if (start<1 || start>source.length()) {
                s_error = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start) {
                s_error = Kumir::Core::fromUtf8("Ошибка в границах вырезки");
            }
            else {
                source = source.substr(0,start-1)+ch+source.substr(end);
                Variable r(source);
                stack_values.push(r);
            }
        }
        if (m_dontTouchMe) m_dontTouchMe->unlock();
    }
    else if (alg==0xBB01) {
        if (m_dontTouchMe) m_dontTouchMe->lock();
        // Input argument
        int localId = argsCount; // Already removed from stack
        Variable ref = stack_contexts.top().locals[localId].toReference();
        if (m_dontTouchMe) m_dontTouchMe->unlock();
        if (m_externalHandler && m_externalHandler->makeInputArgument(ref)) {
            // pass
        }
        else {
            if (m_dontTouchMe) m_dontTouchMe->lock();
//            Kumir::IO::writeString(String(), Kumir::Core::fromUtf8("Введите ")+ref.name()+Kumir::Core::fromAscii(": "));
//            if (ref.baseType()==VT_int) {
//                int val = Kumir::IO::readInteger(String());
//                ref.setValue(AnyValue(val));
//            }
//            else if (ref.baseType()==VT_int) {
//                real val = Kumir::IO::readReal(String());
//                ref.setValue(AnyValue(val));
//            }
//            else if (ref.baseType()==VT_bool) {
//                bool val = Kumir::IO::readBool(String());
//                ref.setValue(AnyValue(val));
//            }
//            else if (ref.baseType()==VT_char) {
//                Char val = Kumir::IO::readChar(String());
//                ref.setValue(AnyValue(val));
//            }
//            else if (ref.baseType()==VT_int) {
//                const String & val = Kumir::IO::readString(String());
//                ref.setValue(AnyValue(val));
//            }
            if (m_dontTouchMe) m_dontTouchMe->unlock();
        }
    }
    else if (alg==0xBB02) {
        if (m_dontTouchMe) m_dontTouchMe->lock();
        // Output argument or return value
        int localId = argsCount; // Already removed from stack
        Variable ref = stack_contexts.top().locals[localId].toReference();
        if (m_dontTouchMe) m_dontTouchMe->unlock();
        if (m_externalHandler && ref.isValid())
            m_externalHandler->makeOutputArgument(ref);
    }
}

void KumirVM::do_init(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    findVariable(s,id).init();
    nextIP();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_setarr(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable & var = findVariable(s, id);
    const int dim = var.dimension();
    int bounds[7];    
    if (dim>0) {
        String name;
        bounds[6] = dim*2;
        for (int i=0; i<dim*2; i++) {
            bounds[i] = stack_values.pop().toInt();
        }
        var.setBounds(bounds);
        if (!b_blindMode)
            name = var.name();
        s_error = Kumir::Core::getError();
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
                !b_blindMode &&
                stack_contexts.top().type != EL_BELOWMAIN
                )
        {
            String boundsText;
            for (int i=0; i<dim; i++) {
                boundsText += Kumir::Converter::sprintfInt(bounds[i*2],10,0,0);
                boundsText.push_back(Char(':'));
                boundsText += Kumir::Converter::sprintfInt(bounds[i*2+1],10,0,0);
                if (i<dim-1) {
                    boundsText.push_back(Char(','));
                }
            }
            const String notice = name+Kumir::Core::fromAscii("[")+boundsText+Kumir::Core::fromAscii("]");
            if (m_externalHandler && currentContext().runMode==CRM_OneStep) {
                m_externalHandler->noticeOnValueChange(lineNo, notice);
                if (VariableScope(s)==Bytecode::LOCAL) {
                    m_externalHandler->debuggerUpdateLocalTableBounds(name, bounds);
                }
                else if (VariableScope(s)==Bytecode::GLOBAL) {
                    m_externalHandler->debuggerUpdateGlobalTableBounds(var.moduleName(), name, bounds);
                }
            }
        }
    }
    nextIP();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_updarr(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable & var = findVariable(s, id);
    const int dim = var.dimension();
    int bounds[7];
    if (dim>0) {
        String name;
        int effectiveBounds[7];
        bounds[6] = effectiveBounds[6] = dim*2;
        for (int i=0; i<dim*2; i++) {
            bounds[i] = stack_values.pop().toInt();
        }
        var.updateBounds(bounds);
        var.getEffectiveBounds(effectiveBounds);
        if (!b_blindMode)
            name = var.myName();
        s_error = Kumir::Core::getError();
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
                !b_blindMode &&
                stack_contexts.top().type != EL_BELOWMAIN
                )
        {
            String boundsText;
            for (int i=0; i<dim; i++) {
                boundsText += Kumir::Converter::sprintfInt(bounds[i*2],10,0,0);
                boundsText.push_back(Char(':'));
                boundsText += Kumir::Converter::sprintfInt(bounds[i*2+1],10,0,0);
                if (i<dim-1) {
                    boundsText.push_back(Char(','));
                }
            }
            const String notice = name+Kumir::Core::fromAscii("[")+boundsText+Kumir::Core::fromAscii("]");
            if (m_externalHandler)
                m_externalHandler->noticeOnValueChange(lineNo, notice);
        }
    }
    nextIP();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_store(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    const Variable & value = stack_values.top();
    String name;
    String svalue;
    const int lineNo = stack_contexts.top().lineNo;
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    ValueType t = variable.baseType();
    Variable * reference = variable.reference();
    int bounds[7];
    if (dim>0)
        value.getBounds(bounds);
    if (value.isConstant())
        variable.setConstValue(value);
    else {
        if (dim>0)
            variable.setBounds(bounds);
        variable.setValue(value.value());
        variable.setDimension(value.dimension());
    }
    if (lineNo!=-1 && !b_blindMode) {
        name = variable.myName();
        svalue = variable.toString();
    }
    if (lineNo!=-1 &&
            !b_blindMode &&
            stack_contexts.top().type != EL_BELOWMAIN &&
            value.dimension()==0
            )
    {
        if (t==VT_string) {
            svalue.insert(0, 1, Char('"'));
            svalue.push_back(Char('"'));
        }
        if (t==VT_char) {
            svalue.insert(0, 1, Char('\''));
            svalue.push_back(Char('\''));
        }
        String qn;
        bool startsWithAt = reference && reference->algorhitmName().length()>0 &&
                reference->algorhitmName().at(0)==Char('@');
        if (reference && !startsWithAt) {
            qn = reference->algorhitmName().empty()
                            ? reference->name()
                            : reference->algorhitmName()+Kumir::Core::fromAscii("::")+reference->name();
        }
        qn += name;
        qn.push_back(Char('='));
        qn += svalue;
        if (m_externalHandler) {
            m_externalHandler->noticeOnValueChange(lineNo, qn);
        }
        if (m_externalHandler && currentContext().runMode==CRM_OneStep) {
            if (VariableScope(s)==Bytecode::LOCAL)
                m_externalHandler->debuggerUpdateLocalVariable(name, svalue);
            else if (VariableScope(s)==Bytecode::GLOBAL)
                m_externalHandler->debuggerUpdateGlobalVariable(
                            variable.moduleName(),
                            variable.name(),
                            svalue
                            );

        }
    }
    if (stack_contexts.top().type==Bytecode::EL_BELOWMAIN)
        Variable::unsetError();
    s_error = Kumir::Core::getError();
    nextIP();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_load(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable val;
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    int bounds[7];
    val.setBaseType(variable.baseType());
    val.setDimension(variable.dimension());
    if (dim>0) {
        variable.getBounds(bounds);
        val.setBounds(bounds);
    }
    if (VariableScope(s)==CONSTT) {
        bool wasError = Kumir::Core::getError().length()>0;
        AnyValue v = variable.value();
        if (!wasError)
            Variable::unsetError();
        val.setValue(v);
        val.setConstantFlag(true);
    }
    else {
        val.setValue(variable.value());
    }

    bool isRetVal = VariableScope(s)==LOCAL
            && stack_contexts.top().locals[id].algorhitmName()==stack_contexts.top().locals[id].name();
    if (isRetVal && stack_contexts.top().type==Bytecode::EL_MAIN)
        Variable::unsetError();
    if (Kumir::Core::getError().length()==0) {
        stack_values.push(val);
        if (val.dimension()==0)
            register0 = val.value();
        if (isRetVal && stack_contexts.top().type==Bytecode::EL_MAIN)
            Variable::unsetError();
    }
    s_error = Kumir::Core::getError();
    nextIP();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}

void KumirVM::do_storearr(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    String name;
    String svalue;
    const int lineNo = stack_contexts.top().lineNo;
    String sindeces;
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    if (!b_blindMode) {
        name = variable.name();
    }
    int indeces[4];
    indeces[3] = dim;
    if (dim>0) {
        for (int i=0; i<dim; i++) {
            indeces[i] = stack_values.pop().toInt();
            if (!sindeces.empty())
                sindeces.push_back(',');
            sindeces += Kumir::Converter::sprintfInt(indeces[i], 10, 0, 0);
        }
        const Variable & value = stack_values.top();
        ValueType t = VT_void;
        if (!b_blindMode)
            svalue = value.toString();
        variable.setValue(indeces, value.value());
        t = variable.baseType();
        if (t==VT_string)
            if (lineNo!=-1 && !b_blindMode) {
                svalue.insert(0, 1, Char('"'));
                svalue.push_back(Char('"'));
            }
        if (t==VT_char)
            if (lineNo!=-1 && !b_blindMode) {
                svalue.insert(0, 1, Char('\''));
                svalue.push_back(Char('\''));
            }
    }
    if (lineNo!=-1 &&
            !b_blindMode
            )
    {
        String notice = name;
        notice.push_back(Char('['));
        notice += sindeces;
        notice.push_back(Char(']'));
        notice.push_back(Char('='));
        notice += svalue;
        if (m_externalHandler) {
            m_externalHandler->noticeOnValueChange(lineNo, notice);
        }
        if (m_externalHandler && currentContext().runMode==CRM_OneStep) {
            if (VariableScope(s)==Bytecode::LOCAL)
                m_externalHandler->debuggerUpdateLocalTableValue(name, indeces);
            else if (VariableScope(s)==Bytecode::GLOBAL) {
                m_externalHandler->debuggerUpdateGlobalTableValue(
                            variable.moduleName(),
                            name,
                            indeces
                            );
            }
        }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_loadarr(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    const ValueType vt = variable.baseType();

    if (dim>0 || vt==VT_string) {
        int indeces[4];
        indeces[3] = dim;
        for (int i=0; i<dim; i++) {
            indeces[i] = stack_values.pop().toInt();
        }
        Variable val;
        AnyValue vv;
        val.setBaseType(vt);
        vv = variable.value(indeces);
        if (vv.isValid()) {
            val.setValue(vv);
            stack_values.push(val);
            if (val.baseType()==VT_int)
                register0 = val.toInt();
            else if (val.baseType()==VT_real)
                register0 = val.toReal();
            else if (val.baseType()==VT_char)
                register0 = val.toChar();
            else if (val.baseType()==VT_string)
                register0 = val.toString();
            else if (val.baseType()==VT_bool)
                register0 = val.toBool();
        }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_ref(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable & variable = findVariable(s, id);
    Variable ref = variable.toReference();
    if (!b_blindMode) {
        ref.setReferenceStackContextNo(VariableScope(s)==Bytecode::LOCAL
                                       ? stack_contexts.size()-1
                                       : -1
                    );
    }
    if (ref.isReference()) {
        stack_values.push(ref);
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_setref(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable ref = stack_values.top();
    int bounds[7];
    ref.getEffectiveBounds(bounds);
    String name;
    if (!ref.isReference()) {
        s_error = Kumir::Core::fromAscii("Internal error");
    }
    else {
        Variable & variable = findVariable(s, id);
        variable.setReference(ref.reference(), bounds);
        name = variable.myName();
    }
    const int lineNo = stack_contexts.top().lineNo;
    if (lineNo!=-1 &&
            !b_blindMode &&
            stack_contexts.top().type != EL_BELOWMAIN
            )
    {
        const String qn = ref.algorhitmName().empty()
                ? ref.name()
                : ref.algorhitmName()+Kumir::Core::fromAscii("::")+ref.name();
        bool startsWithAt = qn.length()>0 && qn.at(0)==Char('@');
        if (!startsWithAt) {
//            name.push_back('=');
//            name.push_back('&');
//            name += qn;
            if (m_externalHandler) {
                m_externalHandler->noticeOnValueChange(lineNo, name);
            }
            if (m_externalHandler && currentContext().runMode==CRM_OneStep) {
                if (VariableScope(s)==Bytecode::LOCAL) {
                    int dummyIndeces[4];
                    dummyIndeces[3] = 0;
                    int targetStackPos = ref.referenceStackContextNo();
                    m_externalHandler->debuggerSetLocalReference(
                                name,
                                ref.name(),
                                dummyIndeces,
                                targetStackPos==-1
                                    ? -1
                                    : stack_contexts.size()-1-targetStackPos,
                                targetStackPos==-1
                                    ? ref.moduleName()
                                    : Kumir::String()
                                );
                }
            }
        }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_refarr(uint8_t s, uint16_t id)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    if (dim>0) {
        int indeces[4];
        indeces[3] = dim;
        for (int i=0; i<dim; i++) {
            indeces[i] = stack_values.pop().toInt();
        }
        Variable ref = variable.toReference(indeces);
        stack_values.push(ref);
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_jump(uint16_t ip)
{
    stack_contexts.top().IP = ip;
}

void KumirVM::do_jnz(uint8_t r, uint16_t ip)
{
    bool v = false;
    if (r==0)
        v = register0.toBool();
    else
        v = stack_contexts.top().registers[r] > 0;
    if (v) {
        stack_contexts.top().IP = ip;
    }
    else {
        nextIP();
    }
}


void KumirVM::do_jz(uint8_t r, uint16_t ip)
{
    bool v = false;
    if (r==0)
        v = register0.toBool();
    else
        v = stack_contexts.top().registers[r] > 0;
    if (v) {
        nextIP();
    }
    else {
        stack_contexts.top().IP = ip;
    }
}

void KumirVM::do_push(uint8_t r)
{
    AnyValue v;
    if (r==0)
        v = register0;
    else
        v = stack_contexts.top().registers[r];
    stack_values.push(Variable(v));
    nextIP();
}

void KumirVM::do_pop(uint8_t r)
{
    Variable v = stack_values.pop();
    if (r==0 && v.hasValue()) {
        if (v.baseType()==VT_int) {
            register0 = v.toInt();
        }
        else if (v.baseType()==VT_real) {
            register0 = v.toReal();
        }
        else if (v.baseType()==VT_bool) {
            register0 = v.toBool();
        }
        else if (v.baseType()==VT_char) {
            register0 = v.toChar();
        }
        else if (v.baseType()==VT_string) {
            register0 = v.toString();
        }
    }
    else if (r!=0) {
        stack_contexts.top().registers[r] = v.toInt();
    }
    nextIP();
}

void KumirVM::do_showreg(uint8_t regNo) {
    if (!b_blindMode) {
        const int lineNo = stack_contexts.top().lineNo;
        if (lineNo!=-1 &&
                !b_blindMode
                )
        {
            AnyValue val;
            if (regNo==0)
                val = register0;
            else
                val = stack_contexts.top().registers[regNo];
            if (m_externalHandler)
                m_externalHandler->noticeOnValueChange(lineNo, val.toString());
        }
    }
    nextIP();
}

void KumirVM::do_clearmarg(uint16_t toLine)
{
    const int lineNo = stack_contexts.top().lineNo;
    if (!b_blindMode && lineNo!=-1
            )
    {
        if (m_externalHandler)
            m_externalHandler->clearMargin(lineNo, toLine);
    }
    nextIP();
}

void KumirVM::do_ret()
{
    if (stack_contexts.top().runMode==CRM_UntilReturn) {
        if (m_externalHandler)
            m_externalHandler->noticeOnFunctionReturn(stack_contexts.top().lineNo);
        stack_contexts.top().runMode=CRM_ToEnd;
    }
    else {
        last_context = stack_contexts.top();
        if (m_externalHandler && !b_blindMode) {
            if (last_context.type==Bytecode::EL_MAIN
                    || last_context.type==Bytecode::EL_FUNCTION
                    ) {
                debuggerPopContext();
            }
        }
        stack_contexts.pop();
        if (last_context.type==Bytecode::EL_INIT
                && last_context.runMode == CRM_OneStep
                )
        {
            if (stack_contexts.size()>0 && stack_contexts.top().moduleContextNo==0) {
                stack_contexts.top().runMode = CRM_OneStep;
                if (m_externalHandler && !b_blindMode
                        && stack_contexts.top().type==EL_MAIN)
                {
                    debuggerPushContext(false,
                                        stack_contexts.top().name,
                                        &(stack_contexts.top().locals),
                                        false
                                        );
                }
            }
        }
        if (stack_contexts.size()>0) {
            nextIP();
        }
    }
    if (stack_contexts.size()>0) {
        currentLocals = &(stack_contexts.top().locals);
        currentGlobals = &(moduleContexts[stack_contexts.top().moduleContextNo].globals[stack_contexts.top().moduleId]);
        currentConstants = &(moduleContexts[stack_contexts.top().moduleContextNo].constants);
    }
}

void KumirVM::do_error(uint8_t s, uint16_t id)
{
    s_error = findVariable(s,id).toString();
}

void KumirVM::do_line(uint16_t no)
{
    if (!b_blindMode && stack_contexts.top().runMode==CRM_OneStep) {
        if (stack_contexts.top().lineNo!=no) {
            if (m_externalHandler)
                m_externalHandler->noticeOnLineNoChanged(no);
        }
    }
    stack_contexts.top().lineNo = no;
    nextIP();
}

void KumirVM::do_sum()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()+b.toInt());
        stack_values.push(r);
        if (!Kumir::Math::checkSumm(a.toInt(), b.toInt())) {
            s_error = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()+b.toReal());
        stack_values.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            s_error = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    else if (a.baseType()==VT_string || a.baseType()==VT_char) {
        Variable r(a.toString()+b.toString());
        stack_values.push(r);
    }
    nextIP();
}

void KumirVM::do_sub()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()-b.toInt());
        stack_values.push(r);
        if (!Kumir::Math::checkDiff(a.toInt(), b.toInt())) {
            s_error = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()-b.toReal());
        stack_values.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            s_error = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    nextIP();
}

void KumirVM::do_mul()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()*b.toInt());
        stack_values.push(r);
        if (!Kumir::Math::checkProd(a.toInt(), b.toInt())) {
            s_error = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()*b.toReal());
        stack_values.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            s_error = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    nextIP();
}

void KumirVM::do_div()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && b.toInt()==0) {
        s_error = Kumir::Core::fromUtf8("Деление на ноль");
    }
    else if (b.baseType()==VT_real && b.toReal()==0.0) {
        s_error = Kumir::Core::fromUtf8("Деление на ноль");
    }
    else {
        Variable r(a.toReal()/b.toReal());
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            s_error = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
        stack_values.push(r);
    }
    nextIP();
}

void KumirVM::do_pow()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    Variable r;
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        r = Variable(Kumir::Math::ipow(a.toInt(), b.toInt()));
    }
    else {
        r = Variable(Kumir::Math::pow(a.toReal(), b.toReal()));
    }
    stack_values.push(r);
    nextIP();
}

void KumirVM::do_neg()
{
    Variable a = stack_values.pop();
    if (a.baseType()==VT_bool) {
        Variable r(!a.toBool());
        stack_values.push(r);
        register0 = AnyValue(!a.toBool());
    }
    else if (a.baseType()==VT_int) {
        Variable r(-a.toInt());
        stack_values.push(r);
    }
    else if (a.baseType()==VT_real) {
        Variable r(0.0-a.toReal());
        stack_values.push(r);
    }
    nextIP();
}

void KumirVM::do_and()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variable r(a.toBool() && b.toBool());
        stack_values.push(r);
    }
    nextIP();
}

void KumirVM::do_or()
{
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variable r(a.toBool() || b.toBool());
        stack_values.push(r);
    }
    nextIP();
}

void KumirVM::do_eq()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()==b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()==b.toReal();;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()==b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() == b.toChar();
    }

    Variable r(result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_neq()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()==b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()==b.toReal();;
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toBool()==b.toBool();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()==b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() == b.toChar();
    }

    Variable r(!result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_ls()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()<b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()<b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()<b.toInt();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()<b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() < b.toChar();
    }
    Variable r(result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_gt()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()>b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()>b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()>b.toInt();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()>b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() > b.toChar();
    }
    Variable r(result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_leq()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()<=b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()<=b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()<=b.toInt();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()<=b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() <= b.toChar();
    }
    Variable r(result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_geq()
{
    bool result = false;
    Variable b = stack_values.pop();
    Variable a = stack_values.pop();
    if (b.baseType()==VT_int && a.baseType()==VT_int) {
        result = a.toInt()>=b.toInt();
    }
    if (b.baseType()==VT_real || a.baseType()==VT_real) {
        result = a.toReal()>=b.toReal();
    }
    if (b.baseType()==VT_bool && a.baseType()==VT_bool) {
        result = a.toInt()>=b.toInt();
    }
    if (a.baseType()==VT_string || b.baseType()==VT_string)
    {
        result = a.toString()>=b.toString();
    }
    if (a.baseType()==VT_char && b.baseType()==VT_char) {
        result = a.toChar() >= b.toChar();
    }
    Variable r(result);
    stack_values.push(r);
    register0 = AnyValue(result);
    nextIP();
}

void KumirVM::do_ctl(uint8_t parameter, uint16_t value)
{
    if (parameter==0x00) {
        Variable::ignoreUndefinedError = value>0;
    }
    else if (parameter==0x01) {
        i_backtraceSkip = value;
    }
    nextIP();
}

void KumirVM::do_inrange()
{
    Variable value = stack_values.pop();
    Variable to = stack_values.pop();
    Variable from = stack_values.pop();
    Variable step = stack_values.pop();

    int iValue = value.toInt();
    int iStep = step.toInt();
    int iFrom = from.toInt();
    int iTo = to.toInt();
    bool res = iStep>=0
            ? (iFrom <= iValue) && (iValue <= iTo)
            : (iFrom >= iValue) && (iValue >= iTo);
    register0 = res;
    nextIP();
}

void KumirVM::updateDebugger()
{
    m_externalHandler->debuggerReset();
    for (int i=0; i<stack_contexts.size(); i++) {
        const Context & c = stack_contexts.at(i);
        if (c.type==EL_MAIN || c.type==EL_FUNCTION) {
            debuggerPushContext(false, c.name, &(c.locals), true);
            for (size_t i_var=0; i_var<c.locals.size(); i_var++) {
                const Variable & var = c.locals.at(i_var);
                if (var.isReference()) {
                    int referenceIndeces[4];
                    var.getReferenceIndeces(referenceIndeces);
                    m_externalHandler->debuggerSetLocalReference(
                                var.myName(),
                                var.name(),
                                referenceIndeces,
                                var.referenceStackContextNo()==-1
                                ? -1
                                : i-var.referenceStackContextNo(),
                                var.referenceStackContextNo()==-1
                                ? var.moduleName()
                                : Kumir::String()
                                );
                }
                if (var.dimension()>0) {
                    int bounds[7];
                    var.getEffectiveBounds(bounds);
                    m_externalHandler->debuggerUpdateLocalTableBounds(
                                var.myName(),
                                bounds
                                );
                }
                if (var.isValid() && var.dimension()==0) {
                    m_externalHandler->debuggerUpdateLocalVariable(
                                var.myName(),
                                var.value().toString()
                                );
                }
            }
        }
    }
    for (size_t i_context=0; i_context<moduleContexts.size(); i_context++) {
        const ModuleContext & mc = moduleContexts[i_context];
        const GlobalsMap & contextGlobals = mc.globals;
        for (size_t i_module=0; i_module<contextGlobals.size(); i_module++) {
            const Kumir::String & contextName = i_context==0
                    ? Kumir::String()
                    : mc.filename;
            const Kumir::String & moduleName = Kumir::String();
            const Kumir::String fullName = moduleName.length()>0
                    ? moduleName
                    : contextName;
            debuggerPushContext(
                        true,
                        fullName,
                        &(contextGlobals[i_module]),
                        true
                        );
            for (size_t i_var=0; i_var<contextGlobals[i_module].size(); i_var++) {
                const Variable & var = contextGlobals[i_module].at(i_var);
                if (var.dimension()>0) {
                    int bounds[7];
                    var.getEffectiveBounds(bounds);
                    m_externalHandler->debuggerUpdateGlobalTableBounds(
                                var.moduleName(),
                                var.myName(),
                                bounds
                                );
                }
                else if (var.isValid()) {
                    m_externalHandler->debuggerUpdateGlobalVariable(
                                var.moduleName(),
                                var.myName(),
                                var.value().toString()
                                );
                }
            }
        }
    }
}

void KumirVM::do_pause(uint16_t lineNo)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    ContextRunMode prevRunMode = CRM_OneStep;
    if (stack_contexts.size()>0) {
        prevRunMode = currentContext().runMode;
        currentContext().runMode = CRM_OneStep;
    }
    b_blindMode = false;
    if (m_externalHandler) {
        if (prevRunMode!=CRM_OneStep) {
            m_externalHandler->noticeOnLineNoChanged(lineNo);
            m_externalHandler->makePause();
            m_externalHandler->noticeOnLineNoChanged(lineNo);
            updateDebugger();
        }
    }
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    nextIP();
}

void KumirVM::do_halt(uint16_t)
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    static const String STOP = Kumir::Core::fromUtf8("\nСТОП.");
    static std::deque< std::pair<int,int> > formats;
    formats.push_back(std::pair<int,int>(0,0));
    static std::deque<Variable> values;
    values.push_back(Variable(STOP));
    if (m_externalHandler && m_externalHandler->makeOutput(formats, values)) {
        // pass
    }
    else {
        Kumir::IO::writeString(0, STOP);
    }
    stack_contexts.reset();
    if (m_dontTouchMe) m_dontTouchMe->unlock();
}


void KumirVM::setNextCallInto()
{
    b_nextCallInto = true;
}

void KumirVM::setNextCallOut()
{
    if (stack_contexts.size()==0)
        return;
    stack_contexts.top().runMode = CRM_UntilReturn;
}

void KumirVM::setNextCallToEnd()
{
    for (int i=0; i<stack_contexts.size(); i++) {
        stack_contexts.at(i).runMode = CRM_ToEnd;
    }
}

void KumirVM::setNextCallStepOver()
{
    if (stack_contexts.size()==0)
        return;
    ContextRunMode prevMode = currentContext().runMode;
    currentContext().runMode = CRM_OneStep;
    if (prevMode!=CRM_OneStep) {
        updateDebugger();
    }
}


int KumirVM::effectiveLineNo() const
{
    if (stack_contexts.size()==0)
        return -1;
    int index = Kumir::Math::imax(0, int(stack_contexts.size()-1 - i_backtraceSkip));
    return stack_contexts.at(index).lineNo;
}

bool KumirVM::canStepOut() const
{
    if (stack_contexts.size()==0)
        return false;
    else
        return stack_contexts.top().type==EL_FUNCTION;
}

bool KumirVM::loadProgramFromBinaryBuffer(std::list<char> &stream, bool isMain, const String & filename, String & error)
{
    error.clear();
    Bytecode::Data d;
    bool ok;
    try {
        Bytecode::bytecodeFromDataStream(stream, d);
        ok = true;
    }
    catch (String e) {
        error = e;
        ok = false;
    }
    catch (std::string e) {
        error = Kumir::Core::fromAscii(e);
        ok = false;
    }
    if (ok) {
        setProgram(d, isMain, filename);
    }
    return ok;
}

bool KumirVM::loadProgramFromTextBuffer(const std::string &buffer, bool isMain, const String & filename, String & error)
{
    error.clear();
    Bytecode::Data d;
    bool ok;
    std::istringstream stream(buffer);
    try {
        Bytecode::bytecodeFromTextStream(stream, d);
        ok = true;
    }
    catch (String e) {
        error = e;
        ok = false;
    }
    catch (std::string e) {
        error = Kumir::Core::fromAscii(e);
        ok = false;
    }
    if (ok) {
        setProgram(d, isMain, filename);
    }
    return ok;
}

int KumirVM::contextByIds(int moduleId, int algorhitmId) const
{
    for (int i=stack_contexts.size()-1; i>=0; i--) {
        if (stack_contexts.at(i).algId==algorhitmId && stack_contexts.at(i).moduleId==moduleId)
            return i;
    }
    if (last_context.algId==algorhitmId && last_context.moduleId==moduleId)
        return -2;
    return -1;
}

const std::vector<Variable> & KumirVM::getLocals(int frameNo) const
{
    int curFrameNo = -1;
    int frameIndex = -1;
    for (int i=0; i<stack_contexts.size(); i++) {
        if (stack_contexts.at(i).type==EL_MAIN
                || stack_contexts.at(i).type==EL_FUNCTION)
        {
            curFrameNo += 1;
            if (curFrameNo==frameNo) {
                frameIndex = i;
                break;
            }
        }
    }
    const Context & c = stack_contexts.at(frameIndex);
    return c.locals;
}

const std::vector<Variable> &
KumirVM::getGlobals(const Kumir::String &moduleName) const
{
    size_t contextNumber = 0;
    bool isFileName = false;
    if (moduleName.length()>0) {
        // If module name is empty, then it is the main module,
        // which always first in contest. Otherwise, find corresponding
        // context
        for (size_t i_fileNo = 0; i_fileNo<moduleContexts.size(); i_fileNo++) {
            const ModuleContext & context = moduleContexts[i_fileNo];
            if (context.filename==moduleName) {
                isFileName = true;
                contextNumber = i_fileNo;
                break;
            }
        }
    }
    const ModuleContext & context = moduleContexts[contextNumber];
    const GlobalsMap & allGlobals = context.globals;
    int moduleNumber = -1;
    for (int i_modNo=allGlobals.size()-1; i_modNo>=0; i_modNo--) {
        // Find a module in a context
        const Kumir::String & currentModuleName = context.moduleNames[i_modNo];
        if (isFileName && currentModuleName.length()==0) {
            moduleNumber = i_modNo;
            break;
        }
        else if (moduleName==currentModuleName) {
            moduleNumber = i_modNo;
            break;
        }
    }
    // ASSERT(moduleNumber!=-1);
    return allGlobals.at(moduleNumber);
}

AnyValue KumirVM::value(int moduleId, int algorhitmId, int variableId) const
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    AnyValue result;
    // TODO implement me
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    return result;
}

std::list<int> KumirVM::bounds(int moduleId, int algorhitmId, int variableId) const
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    std::list<int> result;
    // TODO implement me
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    return result;
}

ReferenceInfo KumirVM::reference(int moduleId, int algorhitmId, int variableId) const
{
    if (m_dontTouchMe) m_dontTouchMe->lock();
    ReferenceInfo result;
    // TODO implement me
    if (m_dontTouchMe) m_dontTouchMe->unlock();
    return result;
}

std::list<AnyValue> KumirVM::remainingValues() const
{
    std::list<AnyValue> result;
    // TODO implement me
    return result;
}

} // end namespace VM
#endif
