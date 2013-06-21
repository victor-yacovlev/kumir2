#ifndef VM_HPP
#define VM_HPP


#include <map>
#include <deque>
#include <set>
#include <sstream>
#include <iostream>
#include <fstream>
#include <memory>
#include <algorithm>

#include "stdlib/kumirstdlib.hpp"
#include "vm_bytecode.hpp"
#include "stack.hpp"
#include "variant.hpp"
#include "context.hpp"
#include "stack.hpp"
#include "vm_abstract_handlers.h"

#ifndef MAX_RECURSION_SIZE
#define MAX_RECURSION_SIZE 4000
#endif

namespace VM {


class KumirVM {
public /*typedefs*/:
    enum EntryPoint { EP_Main, EP_Testing };

public /*methods*/:
    /** Set parsed Kumir bytecode */
    inline void setProgram(const Bytecode::Data & data, bool isMain, const String & filename);
    inline void setProgramDirectory(const Kumir::String & path) { programDirectory_ = path; }

    inline bool loadProgramFromBinaryBuffer(std::list<char> & stream, bool isMain, const String & filename, String & error);
    inline bool loadProgramFromTextBuffer(const std::string & stream, bool isMain, const String & filename, String & error);

    /** Set entry point to Main or Testing algorithm */
    inline void setEntryPoint(EntryPoint ep) { entryPoint_ = ep; }
    inline EntryPoint entryPoint() const { return entryPoint_; }

    /** Set 'blind' mode flag: do not emit debugging information */
    inline void setDebugOff(bool b) { blindMode_ = b; }

    /** Set 'step into' flag for next function call */
    inline void setStepInto(bool b) { nextCallInto_ = b; }

    /** Reset program to initial state */
    inline void reset();

    inline bool hasTestingAlgorithm() const;
    inline unsigned long int stepsDone() const { return stepsCounter_; }

    /** Sets the Debugging Interaction handler for this VM */
    inline void setDebuggingHandler(
            DebuggingInteractionHandler * h
            )
    { debugHandler_ = h; }

    /** Sets a distinct functor */
    inline void setFunctor(Functor * functor);

    /** In GUI mode it's need a Mutex to protect internal data
     *  from been discovered.
     *     You must set actual Mutex implementation object
     *     (Qt or C++11) with methods: lock() and unlock()
     */
    inline void setMutex(CriticalSectionLocker * m)
    { stacksMutex_ = m;}

    /**
     * Returns top-level value stack scalar item or returns invalid if not
     * applicable
     */
    inline AnyValue topLevelStackValue() const /* nothrow */;

    /** The following two functions are basic to use for actual run:
     *  while ( vm.hasMoreInstructions() )
     *      vm.evaluateNextInstruction();
     */
    inline bool hasMoreInstructions() const;
    inline void evaluateNextInstruction();

    /** Return current 'line number' or -1 if not applicable */
    inline int effectiveLineNo() const;
    inline std::pair<uint32_t,uint32_t> effectiveColumn() const;

    /** Returns 'true' if evaluating non-toplevel user function */
    inline bool canStepOut() const;

    /** Used by debugger. Returns local variables for a given stack frame */
    inline const std::vector<Variable> & getLocals(int frameNo) const;

    /** Used by debugger. Returns global variables for a given module name */
    inline const std::vector<Variable> & getGlobals(const Kumir::String &moduleName) const;

    /** Returns last error */
    inline const String & error() const {
        if (error_.length()==0 && Kumir::Core::getError().length()>0)
            return Kumir::Core::getError();
        else
            return error_;
    }

    inline void setConsoleInputBuffer(Kumir::AbstractInputBuffer * b) { consoleInputBuffer_=b; }
    inline Kumir::AbstractInputBuffer * consoleInputBuffer() const { return consoleInputBuffer_; }

    inline void setConsoleOutputBuffer(Kumir::AbstractOutputBuffer * b) { consoleOutputBuffer_=b; }
    inline Kumir::AbstractOutputBuffer * consoleOutputBuffer() const { return consoleOutputBuffer_; }

    /** Debug control methods */
    inline void setNextCallInto();
    inline void setNextCallOut();
    inline void setNextCallToEnd();
    inline void setNextCallStepOver();



private /*fields*/:
    std::vector<ModuleContext> moduleContexts_;
    EntryPoint entryPoint_;
    bool blindMode_;
    bool nextCallInto_;
    CriticalSectionLocker * stacksMutex_;
    DebuggingInteractionHandler * debugHandler_;
    ExternalModuleLoadFunctor * externalModuleLoad_;
    ExternalModuleResetFunctor * externalModuleReset_;
    ExternalModuleCallFunctor * externalModuleCall_;
    InputFunctor * input_;
    OutputFunctor * output_;
    CustomTypeFromStringFunctor * customTypeFromString_;
    CustomTypeToStringFunctor * customTypeToString_;
    GetMainArgumentFunctor * getMainArgument_;
    ReturnMainValueFunctor * returnMainValue_;
    PauseFunctor * pause_;

    Context lastContext_;
    int backtraceSkip_;
    String error_;
    AnyValue register0_;
    Stack<Variable> valuesStack_;
    Stack<Context> contextsStack_;
    Kumir::String programDirectory_;
    typedef std::vector<Variable> VariablesTable;
    VariablesTable * currentConstants_;
    VariablesTable * currentGlobals_;
    VariablesTable * currentLocals_;
    unsigned long int stepsCounter_;
    Kumir::AbstractInputBuffer * consoleInputBuffer_;
    Kumir::AbstractOutputBuffer * consoleOutputBuffer_;

public /*constructors*/:
    inline KumirVM();
private /*methods*/:
    inline static Variable fromTableElem(const Bytecode::TableElem & e);
    inline int contextByIds(int moduleId, int algorhitmId) const;
    inline Context & currentContext();
    inline void nextIP();

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
    inline void checkFunctors();

private /*instruction methods*/:
    inline void do_call(uint8_t, uint16_t);
    inline void do_stdcall(uint16_t);
    inline void do_filescall(uint16_t);
    inline void do_stringscall(uint16_t);
    inline void do_specialcall(uint16_t);

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
    inline void do_line(const Bytecode::Instruction & instr);
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

AnyValue KumirVM::topLevelStackValue() const {
    AnyValue result;
    if (valuesStack_.size() > 0) {
        const Variable & var = valuesStack_.top();
        if (var.dimension() == 0) {
            result = var.value();
        }
    }
    return result;
}

Context & KumirVM::currentContext() {
    return contextsStack_.top();
}

inline String makeCanonicalName(const String & filename) {
    Kumir::String result;
    static const Kumir::Char slash = Char('/');
    size_t slashPos = filename.find_last_of(slash);
    if (slashPos==Kumir::String::npos) {
        result = filename;
    }
    else {
        result = filename.substr(slashPos+1);
    }
    if (result.length()>3 &&
            result[0]==Char('l') &&
            result[1]==Char('i') &&
            result[2]==Char('b'))
    {
        result = result.substr(3);
    }
    if (result.length()>3 &&
            result[result.length()-3]==Char('.') &&
            result[result.length()-2]==Char('s') &&
            result[result.length()-1]==Char('o')
            )
    {
        result.resize(result.length()-3);
    }
    if (result.length()>4 &&
            result[result.length()-4]==Char('.') &&
            result[result.length()-3]==Char('d') &&
            result[result.length()-2]==Char('l') &&
            result[result.length()-1]==Char('l')
            )
    {
        result.resize(result.length()-4);
    }
    if (result.length()>6 &&
            result[result.length()-6]==Char('.') &&
            result[result.length()-5]==Char('d') &&
            result[result.length()-4]==Char('y') &&
            result[result.length()-3]==Char('l') &&
            result[result.length()-2]==Char('i') &&
            result[result.length()-1]==Char('b')
            )
    {
        result.resize(result.length()-6);
    }
    return result;
}

Variable & KumirVM::findVariable(uint8_t scope, uint16_t id)
{
    if (VariableScope(scope)==Bytecode::CONSTT) {
        return currentConstants_->at(id);
    }
    else if (VariableScope(scope)==Bytecode::LOCAL) {
        return currentLocals_->at(id);
    }
    else {
        return currentGlobals_->at(id);
    }
}

void KumirVM::setFunctor(Functor * functor)
{
    if (!functor) return;
    switch (functor->type()) {
    case Functor::ExternalModuleReset:
        externalModuleReset_ =
                dynamic_cast<ExternalModuleResetFunctor*>(functor);
        break;
    case Functor::ExternalModuleLoad:
        externalModuleLoad_ =
                dynamic_cast<ExternalModuleLoadFunctor*>(functor);
        break;
    case Functor::ExternalModuleCall:
        externalModuleCall_ =
                dynamic_cast<ExternalModuleCallFunctor*>(functor);
        break;
    case Functor::Input:
        input_ =
                dynamic_cast<InputFunctor*>(functor);
        break;
    case Functor::Output:
        output_ =
                dynamic_cast<OutputFunctor*>(functor);
        break;
    case Functor::ConvertFromString:
        customTypeFromString_ =
                dynamic_cast<CustomTypeFromStringFunctor*>(functor);
        break;
    case Functor::ConvertToString:
        customTypeToString_ =
                dynamic_cast<CustomTypeToStringFunctor*>(functor);
        break;
    case Functor::GetMainArgument:
        getMainArgument_ =
                dynamic_cast<GetMainArgumentFunctor*>(functor);
        break;
    case Functor::ReturnMainValue:
        returnMainValue_ =
                dynamic_cast<ReturnMainValueFunctor*>(functor);
        break;
    case Functor::Pause:
        pause_ =
                dynamic_cast<PauseFunctor*>(functor);
        break;
    default:
        break;
    }
}

void KumirVM::setProgram(const Bytecode::Data &program, bool isMain, const String & filename)
{
    if (isMain) {
        moduleContexts_.clear();
    }
    moduleContexts_.push_back(ModuleContext());
    moduleContexts_.back().filename = filename;
    int currentModuleContext = moduleContexts_.size()-1;
    moduleContexts_.back().globals.clear();
    moduleContexts_.back().constants.clear();
    moduleContexts_.back().globals.reserve(16);
    moduleContexts_.back().moduleNames.clear();
    moduleContexts_.back().moduleNames.reserve(16);
    for (int i=0; i<moduleContexts_.back().globals.size(); i++) {
        moduleContexts_.back().globals.at(i).clear();
        moduleContexts_.back().globals.at(i).reserve(256);
    }
    moduleContexts_.back().constants.reserve(256);
    LocalsMap locals;
    for (int i=0; i<program.d.size(); i++) {
        const TableElem e = program.d[i];
        if (e.type==EL_GLOBAL) {
            if (moduleContexts_[currentModuleContext].globals.size()<=e.module) {
                moduleContexts_[currentModuleContext].globals.resize(e.module+1);
                moduleContexts_[currentModuleContext].globals[e.module].reserve(256);
                moduleContexts_[currentModuleContext].moduleNames.resize(e.module+1);
                moduleContexts_[currentModuleContext].moduleNames[e.module] = e.moduleName;
            }
            if (moduleContexts_[currentModuleContext].globals[e.module].size() <= e.id) {
                moduleContexts_[currentModuleContext].globals[e.module].resize(e.id+1);
            }
            moduleContexts_[currentModuleContext].globals[e.module][e.id] = fromTableElem(e);
        }
        else if (e.type==EL_CONST) {
            if (moduleContexts_[currentModuleContext].constants.size()<=e.id)
                moduleContexts_[currentModuleContext].constants.resize(e.id+1);
            moduleContexts_[currentModuleContext].constants[e.id] = fromTableElem(e);

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
            for (int m=0; m<moduleContexts_.size(); m++) {
                if (moduleContexts_[m].filename==e.fileName) {
                    externModuleContext = m;
                    break;
                }
            }
            if (externModuleContext==-1) {
                externModuleContext = currentModuleContext + 1;
                Kumir::String modulePath;
                if (e.fileName[0]==Kumir::Char('/') || programDirectory_.length()==0)
                    modulePath = e.fileName;
                else {
                    modulePath = programDirectory_;
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
            const FunctionMap & externMap = moduleContexts_[externModuleContext].functions;
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
            reference.platformDependent = false;
            reference.funcKey = extKey;
            reference.moduleContext = externModuleContext;
            reference.fileName = e.fileName;
            reference.moduleName = e.moduleName.length()>0
                    ? e.name
                    : e.fileName;
            moduleContexts_[currentModuleContext].externs[key] = reference;
        }
        else if (e.type==EL_EXTERN_INIT) {
            ExternReference reference;
            reference.platformDependent = true;
            reference.funcKey = 0xFFFFFFFF;
            reference.moduleName = e.moduleName;
            reference.fileName = e.fileName;
            reference.platformModuleName = Kumir::Coder::encode(
                        VM_LOCALE,
                        makeCanonicalName(e.fileName)
                        );
            moduleContexts_[currentModuleContext].externInits.push_back(reference);
            if (externalModuleLoad_)
                (*externalModuleLoad_)(reference.moduleName, reference.platformModuleName);
        }
        else if (e.type==EL_EXTERN) {
            ExternReference reference;
            uint32_t key = 0x00000000;
            uint32_t alg = e.algId;
            uint32_t mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            reference.platformDependent = true;
            reference.funcKey = alg;
            reference.moduleName = e.moduleName;
            reference.fileName = e.fileName;
            reference.platformModuleName = Kumir::Coder::encode(
                        VM_LOCALE,
                        makeCanonicalName(e.fileName)
                        );
            moduleContexts_[currentModuleContext].externs[key] = reference;
            if (externalModuleLoad_) {
                std::list< Kumir::String > algorithms =
                        (*externalModuleLoad_)(
                            reference.moduleName,
                            reference.platformModuleName
                            );
            }
        }
        else if (e.type==EL_FUNCTION || e.type==EL_MAIN || e.type==EL_BELOWMAIN || e.type==EL_TESTING) {
            uint32_t key = 0x00000000;
            uint32_t alg = e.algId;
            uint32_t mod = e.module;
            mod = mod << 16;
            key = mod | alg;
            moduleContexts_[currentModuleContext].functions[key] = e;
            moduleContexts_[currentModuleContext].exportModuleId = mod;
        }
        else if (e.type==EL_INIT ) {
            uint8_t key = e.module;
            moduleContexts_[currentModuleContext].inits.push_back(e);
        }
    }
    for (FunctionMap::iterator it = moduleContexts_[currentModuleContext].functions.begin();
         it!=moduleContexts_[currentModuleContext].functions.end();
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
        moduleContexts_[currentModuleContext].cleanLocalTables[key] = arr;
    }
    currentLocals_ = nullptr;
    currentGlobals_ = nullptr;
    currentConstants_ = nullptr;
}

KumirVM::KumirVM()
    : moduleContexts_(std::vector<ModuleContext>())
    , entryPoint_(EP_Main)
    , blindMode_(true)
    , nextCallInto_(false)
    , stacksMutex_(nullptr)
    , debugHandler_(nullptr)
    , externalModuleLoad_(nullptr)
    , externalModuleReset_(nullptr)
    , externalModuleCall_(nullptr)
    , input_(nullptr)
    , output_(nullptr)
    , customTypeFromString_(nullptr)
    , customTypeToString_(nullptr)
    , getMainArgument_(nullptr)
    , returnMainValue_(nullptr)
    , pause_(nullptr)
    , lastContext_(Context())
    , backtraceSkip_(0)
    , error_(Kumir::String())
    , register0_(AnyValue(0))
    , valuesStack_(Stack<Variable>())
    , contextsStack_(Stack<Context>())
    , programDirectory_(Kumir::String())
    , currentConstants_(nullptr)
    , currentGlobals_(nullptr)
    , currentLocals_(nullptr)
    , consoleInputBuffer_(nullptr)
{

}

Variable KumirVM::fromTableElem(const Bytecode::TableElem &e) {
    Variable r = e.initialValue;
    r.setDimension(e.dimension);
    r.setBaseType(e.vtype.front());
    r.setName(e.name);
    r.setConstantFlag(e.type==EL_CONST);
    r.setModuleName(e.moduleName);
    r.setRecordModuleName(e.recordModuleName);
    r.setRecordClassName(e.recordClassName);
    return r;
}

void KumirVM::checkFunctors()
{
    if (!externalModuleLoad_) {
        static ExternalModuleLoadFunctor dummy;
        externalModuleLoad_ = &dummy;
    }
    if (!externalModuleReset_) {
        static ExternalModuleResetFunctor dummy;
        externalModuleReset_ = &dummy;
    }
    if (!externalModuleCall_) {
        static ExternalModuleCallFunctor dummy;
        externalModuleCall_ = &dummy;
    }
    if (!input_) {
        static InputFunctor dummy;
        input_ = &dummy;
    }
    if (!output_) {
        static OutputFunctor dummy;
        output_ = &dummy;
    }
    if (!customTypeFromString_) {
        static CustomTypeFromStringFunctor dummy;
        customTypeFromString_ = &dummy;
    }
    if (!customTypeToString_) {
        static CustomTypeToStringFunctor dummy;
        customTypeToString_ = &dummy;
    }
    if (!getMainArgument_) {
        static GetMainArgumentFunctor dummy;
        getMainArgument_ = &dummy;
    }
    if (!returnMainValue_) {
        static ReturnMainValueFunctor dummy;
        returnMainValue_ = &dummy;
    }
    if (!pause_) {
        static PauseFunctor dummy;
        pause_ = &dummy;
    }
}

bool KumirVM::hasTestingAlgorithm() const
{
    const ModuleContext & mainContexnt = moduleContexts_.front();
    const FunctionMap & functions = mainContexnt.functions;
    typedef FunctionMap::const_iterator It;
    for (It i = functions.begin(); i!=functions.end(); i++) {
        const Bytecode::TableElem & element = i->second;
        if (element.type == Bytecode::EL_TESTING) {
            return true;
        }
    }
    return false;
}

void KumirVM::reset()
{
    // Clear everything
    lastContext_ = Context();
    blindMode_ = false;
    nextCallInto_ = false;
    backtraceSkip_ = 0;
    stepsCounter_ = 0u;
    error_.clear();
    register0_ = AnyValue();
    Variable::ignoreUndefinedError = false;
    valuesStack_.reset();
    contextsStack_.reset();

    checkFunctors();

    currentLocals_ = nullptr;
    currentGlobals_ = nullptr;
    currentConstants_ = nullptr;

    Bytecode::TableElem * pMainProgram = nullptr;
    Bytecode::TableElem * pTestingProgram = nullptr;

    ModuleContext & mainModuleContext = moduleContexts_.front();

    // Find testing and main algorithms
    for (FunctionMap::iterator it=mainModuleContext.functions.begin();
         it!=mainModuleContext.functions.end();
         ++it)
    {
        TableElem & e = (*it).second;
        if (e.type==EL_MAIN || e.type==EL_BELOWMAIN)
            pMainProgram = &e;
        if (e.type==EL_TESTING)
            pTestingProgram = &e;
    }

    // Prepare startup context
    Context c;
    if (entryPoint_==EP_Main && pMainProgram && (pMainProgram->type==EL_MAIN || pMainProgram->type==EL_BELOWMAIN) ) {
        uint32_t mod = pMainProgram->module;
        uint32_t alg = pMainProgram->algId;
        uint32_t key = (mod << 16) | alg;
        c.locals = mainModuleContext.cleanLocalTables[key];
        c.program = &(pMainProgram->instructions);
        c.type = pMainProgram->type;
        c.runMode = CRM_ToEnd;
        c.algId = pMainProgram->algId;
        c.moduleId = pMainProgram->module;
        c.name = pMainProgram->name;
    }

    if (entryPoint_==EP_Testing && pTestingProgram && pTestingProgram->type==EL_TESTING) {
        uint32_t mod = pTestingProgram->module;
        uint32_t alg = pTestingProgram->algId;
        uint32_t key = (mod << 16) | alg;
        c.locals = mainModuleContext.cleanLocalTables[key];
        c.program = &(pTestingProgram->instructions);
        c.type = EL_TESTING;
        c.runMode = CRM_ToEnd;
        c.algId = pTestingProgram->algId;
        c.moduleId = pTestingProgram->module;
        c.name = pTestingProgram->name;
    }
    c.IP = -1;

    if (c.program) {
        // Push startup context to stack (if non empty)
        contextsStack_.push(c);
        currentLocals_ = &(contextsStack_.top().locals);
    }
    currentConstants_ = &(mainModuleContext.constants);
    currentGlobals_ = mainModuleContext.globals.empty()
            ? nullptr
            : &(mainModuleContext.globals.back());


    // Each kumir module have 'initialization' section,
    // so push all these sections (if any) into stack
    // to call them BEFORE startup context
    for (int moduleContextNo=moduleContexts_.size()-1;
         moduleContextNo>=0;
         moduleContextNo--)
    {
        const ModuleContext & currentModule = moduleContexts_.at(moduleContextNo);
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
                contextsStack_.push(initContext);
                currentConstants_ =
                        &(moduleContexts_[moduleContextNo].constants);
                currentGlobals_ =
                        &(moduleContexts_[moduleContextNo].globals[e.module]);
            }
        }
    }

    if (debugHandler_)
        debugHandler_->debuggerReset();

    std::set<Kumir::String> usedExternalModules;

    // Push globals to debugger and make a list of used external modules
    for (size_t i_context=0; i_context<moduleContexts_.size(); i_context++) {
        const ModuleContext & mc = moduleContexts_[i_context];
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


        const ExternsMap & contextExterns = mc.externs;
        const std::list<ExternReference> & contextExternInits = mc.externInits;
        for (ExternsMap::const_iterator itExtern = contextExterns.begin();
             itExtern!=contextExterns.end(); ++itExtern)
        {
            const ExternReference & externReference = itExtern->second;
            if (externReference.platformDependent) {
                const Kumir::String & externModuleName =
                        externReference.moduleName;
                if (!usedExternalModules.count(externModuleName))
                    usedExternalModules.insert(externModuleName);
            }
        }

        for (std::list<ExternReference>::const_iterator itExtern = contextExternInits.begin();
             itExtern != contextExternInits.end(); ++itExtern)
        {
            const ExternReference & externReference = *itExtern;
            if (externReference.platformDependent) {
                const Kumir::String & externModuleName =
                        externReference.moduleName;
                if (!usedExternalModules.count(externModuleName))
                    usedExternalModules.insert(externModuleName);
            }
        }
    }

    // Prepare standard library
    Kumir::initStandardLibrary();

    // Reset used external modules
    if (externalModuleReset_) {
        for (std::set<Kumir::String>::const_iterator it=usedExternalModules.begin();
             it!=usedExternalModules.end();
             ++it
             )
        {
            const Kumir::String & moduleName = *it;
            try {
                (*externalModuleReset_)(moduleName);
            }
            catch (const std::string & msg) {
                error_ = Kumir::Core::fromUtf8(msg);
            }
            catch (const Kumir::String & msg) {
                error_ = msg;
            }
        }
    }

    if (consoleInputBuffer_) {
        consoleInputBuffer_->clear();
    }
    Kumir::Files::setConsoleInputBuffer(consoleInputBuffer_);
    Kumir::Files::setConsoleOutputBuffer(consoleOutputBuffer_);

}

void KumirVM::debuggerPopContext()
{
    if (debugHandler_==nullptr
            || blindMode_)
    {
        return; // show nothing
    }
    if (currentContext().runMode==CRM_OneStep)
        debugHandler_->debuggerPopContext();
    else
        debugHandler_->debuggerForceUpdateValues();
}

void KumirVM::debuggerPushContext(
        const bool isGlobals,
        const Kumir::String &name,
        const std::vector<Variable> *variables,
        bool force)
{
    if (debugHandler_==nullptr)
        return;
    if (!force)
        if (
            blindMode_
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
        names.push_back(var.myName());
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
        debugHandler_->debuggerSetGlobals(name, names, types, dims);
    else
        debugHandler_->debuggerPushContext(name, names, types, dims);
}

void KumirVM::nextIP()
{
    if (contextsStack_.size()>0) {
        contextsStack_.top().IP ++;
    }
}


bool KumirVM::hasMoreInstructions() const
{
    if (contextsStack_.size()>0) {
        const std::vector<Bytecode::Instruction> * program = contextsStack_.at(0).program;
        int IP = contextsStack_.at(0).IP;
        return program && IP < int(program->size());
    }
    else {
        return false;
    }
}

void KumirVM::evaluateNextInstruction()
{
    int ip = contextsStack_.top().IP;
    if (ip==-1) {
        ip = 0;
    }
    const std::vector<Instruction> * program = contextsStack_.top().program;
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
        do_line(instr);
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
    if (error_.length()==0 && Kumir::Core::getError().length()>0)
        error_ = Kumir::Core::getError();
    if (debugHandler_ &&
            error_.length()>0 &&
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
    else if (moduleContexts_[contextsStack_.top().moduleContextNo].functions.count(p)) {

        if (contextsStack_.size()>=MAX_RECURSION_SIZE) {
            error_ = Kumir::Core::fromUtf8("Слишком много вложенных вызовов алгоритмов");
        }
        else {
            if (stacksMutex_)
                stacksMutex_->lock();
            Context c;
            c.program = & (moduleContexts_[contextsStack_.top().moduleContextNo].functions[p].instructions );
            c.locals = moduleContexts_[contextsStack_.top().moduleContextNo].cleanLocalTables[p];
            c.type = moduleContexts_[contextsStack_.top().moduleContextNo].functions[p].type;
            if (nextCallInto_)
                c.runMode = CRM_OneStep;
            else if (contextsStack_.top().type==EL_BELOWMAIN && c.type==EL_MAIN)
                c.runMode = contextsStack_.top().runMode;
            else
                c.runMode = CRM_ToEnd;
            c.moduleId = moduleContexts_[contextsStack_.top().moduleContextNo].functions[p].module;
            c.algId = moduleContexts_[contextsStack_.top().moduleContextNo].functions[p].algId;
            if (!blindMode_)
                c.name = moduleContexts_[contextsStack_.top().moduleContextNo].functions[p].name;
            c.moduleContextNo = contextsStack_.top().moduleContextNo;
            contextsStack_.push(c);
            if (c.runMode==CRM_OneStep)
                debuggerPushContext(false, c.name, &(contextsStack_.top().locals),false);
            nextCallInto_ = false;
            valuesStack_.pop(); // current implementation doesn't requere args count
            currentLocals_ = &(contextsStack_.top().locals);
            currentGlobals_ =
                    &(moduleContexts_[c.moduleContextNo].globals[c.moduleId]);
            currentConstants_ =
                    &(moduleContexts_[c.moduleContextNo].constants);
            if (stacksMutex_)
                stacksMutex_->unlock();
        }

    }
    else if (moduleContexts_[contextsStack_.top().moduleContextNo].externs.count(p)) {
        if (contextsStack_.size()>=MAX_RECURSION_SIZE) {
            error_ = Kumir::Core::fromUtf8("Слишком много вложенных вызовов алгоритмов");
        }
        else {
            ExternReference reference = moduleContexts_[contextsStack_.top().moduleContextNo].externs[p];
            if (!reference.platformDependent) {
                // External call of algorithm found in another kumir file
                if (stacksMutex_) stacksMutex_->lock();
                uint32_t key = reference.funcKey;
                Context c;
                c.program = & (moduleContexts_[reference.moduleContext].functions[key].instructions );
                c.locals = moduleContexts_[reference.moduleContext].cleanLocalTables[key];
                c.type = moduleContexts_[reference.moduleContext].functions[key].type;
                c.runMode = CRM_ToEnd;
                c.moduleId = moduleContexts_[reference.moduleContext].functions[key].module;
                c.algId = moduleContexts_[reference.moduleContext].functions[key].algId;
                c.moduleContextNo = reference.moduleContext;
                contextsStack_.push(c);
                currentLocals_ = &(contextsStack_.top().locals);
                currentGlobals_ =
                        &(moduleContexts_[c.moduleContextNo].globals[c.moduleId]);
                currentConstants_ =
                        &(moduleContexts_[reference.moduleContext].constants);
                nextCallInto_ = false;
                valuesStack_.pop(); // current implementation doesn't requere args count
                if (stacksMutex_) stacksMutex_->unlock();
            }
            else if (externalModuleCall_) {
                uint16_t algKey = reference.funcKey & 0xffff;
                const Kumir::String moduleName = reference.moduleName;
                if (stacksMutex_) stacksMutex_->lock();
                int argsCount = valuesStack_.pop().toInt();
                std::deque<Variable> args;
                for (int i=0; i<argsCount; i++) {
                    Variable arg = valuesStack_.pop();
                    args.push_front(arg);
                }
                if (stacksMutex_) stacksMutex_->unlock();
                AnyValue algResult;
                Kumir::String localError;
                try {
                    algResult = (*externalModuleCall_)(
                                moduleName, algKey, args
                                );
                }
                catch (const std::string & error) {
                    localError = Kumir::Core::fromUtf8(error);
                }
                catch (const Kumir::String & error) {
                    localError = error;
                }
                if (stacksMutex_) stacksMutex_->lock();
                if (localError.length()>0) {
                    if (error_.length()==0)
                        error_ = localError;
                }
                else {
                    if (algResult.isValid())
                        valuesStack_.push(Variable(algResult));
                }
                if (stacksMutex_) stacksMutex_->unlock();
            }
            else {
                error_ = Kumir::Core::fromUtf8("Вызов алгоритма из недоступного исполнителя");
            }
        }
    }
    else {
        error_ = Kumir::Core::fromUtf8("Вызов алгоритма из недоступного исполнителя");
    }
    if (Kumir::Core::getError().length()>0 && error_.length()==0) {
        error_ = Kumir::Core::getError();
    }
    nextIP();
}

void KumirVM::do_stdcall(uint16_t alg)
{
    if (stacksMutex_)
        stacksMutex_->lock();
    valuesStack_.pop(); // remove arguments count -- all is known
    switch(alg) {
    /* алг вещ abs(вещ x) */
    case 0x0000: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::abs(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ arccos(вещ x) */
    case 0x0001: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::arccos(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ arcctg(вещ x) */
    case 0x0002: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::arcctg(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ arcsin(вещ x) */
    case 0x0003: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::arcsin(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ arctg(вещ x) */
    case 0x0004: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::arctg(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ cos(вещ x) */
    case 0x0005: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::cos(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ ctg(вещ x) */
    case 0x0006: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::ctg(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг delay(цел x) */
    case 0x0007: {
        // TODO not implemented
        break;
    }
    /* алг цел div(цел x, цел y) */
    case 0x0008: {
        int y = valuesStack_.pop().toInt();
        int x = valuesStack_.pop().toInt();
        int r = Kumir::Math::div(x, y);
        valuesStack_.push(Variable(r));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ exp(вещ x) */
    case 0x0009: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::exp(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг цел iabs(цел x) */
    case 0x000a: {
        int x = valuesStack_.pop().toInt();
        int y = Kumir::Math::iabs(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел imax(цел x, цел y) */
    case 0x000b: {
        int y = valuesStack_.pop().toInt();
        int x = valuesStack_.pop().toInt();
        int r = Kumir::Math::imax(x, y);
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг цел imin(цел x, цел y) */
    case 0x000c: {
        int y = valuesStack_.pop().toInt();
        int x = valuesStack_.pop().toInt();
        int r = Kumir::Math::imin(x, y);
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг цел int(вещ x) */
    case 0x000d: {
        real x = valuesStack_.pop().toReal();
        int y = Kumir::Math::intt(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел irand(цел x, цел y) */
    case 0x000e: {
        int y = valuesStack_.pop().toInt();
        int x = valuesStack_.pop().toInt();
        int r = Kumir::Random::irand(x, y);
        valuesStack_.push(Variable(r));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг цел irnd(цел x) */
    case 0x000f: {
        int x = valuesStack_.pop().toInt();
        int y = Kumir::Random::irnd(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ lg(вещ x) */
    case 0x0010: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::lg(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ ln(вещ x) */
    case 0x0011: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::ln(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ max(вещ x, вещ y) */
    case 0x0012: {
        real  y = valuesStack_.pop().toReal();
        real  x = valuesStack_.pop().toReal();
        real  r = Kumir::Math::rmax(x, y);
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг вещ min(вещ x, вещ y) */
    case 0x0013: {
        real  y = valuesStack_.pop().toReal();
        real  x = valuesStack_.pop().toReal();
        real  r = Kumir::Math::rmin(x, y);
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг цел mod(цел x, цел y) */
    case 0x0014: {
        int y = valuesStack_.pop().toInt();
        int x = valuesStack_.pop().toInt();
        int r = Kumir::Math::mod(x, y);
        valuesStack_.push(Variable(r));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ rand(вещ x, вещ y) */
    case 0x0015: {
        real  y = valuesStack_.pop().toReal();
        real  x = valuesStack_.pop().toReal();
        real  r = Kumir::Random::rrand(x, y);
        valuesStack_.push(Variable(r));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ rnd(вещ x) */
    case 0x0016: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Random::rrnd(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел sign(вещ x) */
    case 0x0017: {
        real x = valuesStack_.pop().toReal();
        int y = Kumir::Math::sign(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ sin(вещ x) */
    case 0x0018: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::sin(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ sqrt(вещ x) */
    case 0x0019: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::sqrt(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ tg(вещ x) */
    case 0x001a: {
        real x = valuesStack_.pop().toReal();
        real y = Kumir::Math::tg(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг вещ МАКСВЕЩ */
    case 0x001b: {
        real r = Kumir::Math::maxreal();
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг цел МАКСЦЕЛ */
    case 0x001c: {
        int r = Kumir::Math::maxint();
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг лит вещ_в_лит(вещ x) */
    case 0x001d: {
        real x = valuesStack_.pop().toReal();
        const Kumir::String y = Kumir::Converter::realToString(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел время */
    case 0x001e: {
        int r = Kumir::System::time();
        valuesStack_.push(Variable(r));
        break;
    }
    /* алг цел длин(лит s) */
    case 0x001f: {
        const String x = valuesStack_.pop().toString();
        int y = Kumir::StringUtils::length(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел код(сим ch) */
    case 0x0020: {
        Char x = valuesStack_.pop().toChar();
        int y = Kumir::StringUtils::code(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вещ лит_в_вещ(лит s, рез лог success) */
    case 0x0021: {
        Variable sf = valuesStack_.pop().toReference();
        const String x = valuesStack_.pop().toString();
        bool ok;
        real y = Kumir::Converter::stringToReal(x, ok);
        valuesStack_.push(Variable(y));
        sf.setValue(AnyValue(ok));
        break;
    }
    /* алг цел лит_в_цел(лит s, рез лог success) */
    case 0x0022: {
        Variable sf = valuesStack_.pop().toReference();
        const String x = valuesStack_.pop().toString();
        bool ok;
        int y = Kumir::Converter::stringToInt(x, ok);
        valuesStack_.push(Variable(y));
        sf.setValue(AnyValue(ok));
        break;
    }
    /* алг сим символ(цел n) */
    case 0x0023: {
        int x = valuesStack_.pop().toInt();
        Char y = Kumir::StringUtils::symbol(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг сим символ2(цел n) */
    case 0x0024: {
        int x = valuesStack_.pop().toInt();
        Char y = Kumir::StringUtils::unisymbol(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лит цел_в_лит(цел n) */
    case 0x0025: {
        int x = valuesStack_.pop().toInt();
        String y = Kumir::Converter::intToString(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг цел юникод(сим ch) */
    case 0x0026: {
        Char x = valuesStack_.pop().toChar();
        int y = Kumir::StringUtils::unicode(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    default: {
        error_ = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (stacksMutex_)
        stacksMutex_->unlock();
}

void KumirVM::do_filescall(uint16_t alg)
{
    if (stacksMutex_) stacksMutex_->lock();
    valuesStack_.pop(); // Args count
    switch (alg) {
    /* алг файл открыть на чтение(лит имя файла) */
    case 0x0000: {
        const String x = valuesStack_.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Read);
        Record yy = toRecordValue(y);
        Variable res(yy, Kumir::Core::fromUtf8("файл"));
        valuesStack_.push(res);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг файл открыть на запись(лит имя файла) */
    case 0x0001: {
        const String x = valuesStack_.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Write);
        Record yy = toRecordValue(y);
        valuesStack_.push(Variable(yy, Kumir::Core::fromUtf8("файл")));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг файл открыть на добавление(лит имя файла) */
    case 0x0002: {
        const String x = valuesStack_.pop().toString();
        Kumir::FileType y = Kumir::Files::open(x, Kumir::FileType::Append);
        Record yy = toRecordValue(y);
        valuesStack_.push(Variable(yy, Kumir::Core::fromUtf8("файл")));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг закрыть(файл ключ) */
    case 0x0003: {
        const Variable xvar = valuesStack_.pop();
        const Record xx = xvar.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        Kumir::Files::close(x);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг начать чтение(файл ключ) */
    case 0x0004: {
        const Variable & xval = valuesStack_.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        Kumir::Files::reset(x);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лог конец файла(файл ключ) */
    case 0x0005: {
        const Variable & xval = valuesStack_.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        bool y = Kumir::Files::eof(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг установить кодировку(лит имя кодировки) */
    case 0x0006: {
        const String x = valuesStack_.pop().toString();
        Kumir::Files::setFileEncoding(x);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лог можно открыть на чтение(лит имя файла) */
    case 0x0007: {
        const String x = valuesStack_.pop().toString();
        bool y = Kumir::Files::canOpenForRead(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лог можно открыть на запись(лит имя файла) */
    case 0x0008: {
        const String x = valuesStack_.pop().toString();
        bool y = Kumir::Files::canOpenForWrite(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лог есть данные(файл ключ) */
    case 0x0009: {
        const Variable xval = valuesStack_.pop();
        const Record xx = xval.toRecord();
        Kumir::FileType x = fromRecordValue(xx);
        bool y = Kumir::Files::hasData(x);
        valuesStack_.push(Variable(y));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лог существует(лит имя файла Или каталога) */
    case 0x000a: {
        const String x = valuesStack_.pop().toString();
        bool y = Kumir::Files::exist(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг лог является каталогом(лит имя файла Или каталога) */
    case 0x000b: {
        const String x = valuesStack_.pop().toString();
        bool y = Kumir::Files::isDirectory(x);
        valuesStack_.push(Variable(y));
        break;
    }
    case 0x000c: {
        const String x = valuesStack_.pop().toString();
        bool y = Kumir::Files::mkdir(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг лит полный путь(лит имя файла Или каталога) */
    case 0x000d: {
        const String x = valuesStack_.pop().toString();
        const String y = Kumir::Files::getAbsolutePath(x);
        valuesStack_.push(Variable(y));
        break;
    }
    /* алг удалить_файл(лит имя файла) */
    case 0x000e: {
        const String x = valuesStack_.pop().toString();
        Kumir::Files::unlink(x);
        break;
    }
    /* алг НАЗНАЧИТЬ ВВОД(лит имя файла) */
    case 0x000f: {
        const String x = valuesStack_.pop().toString();
        Kumir::Files::assignInStream(x);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг НАЗНАЧИТЬ ВЫВОД(лит имя файла) */
    case 0x0010: {
        const String x = valuesStack_.pop().toString();
        Kumir::Files::assignOutStream(x);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лит РАБОЧИЙ КАТАЛОГ */
    case 0x0011: {
        const String res = Kumir::Files::CurrentWorkingDirectory();
        valuesStack_.push(Variable(res));
        break;
    }
    /* алг лит КАТАЛОГ ПРОГРАММЫ */
    case 0x0012: {
        String res;
        if (programDirectory_.length()==0)
            res = Kumir::Files::CurrentWorkingDirectory();
        else
            res = programDirectory_;
        valuesStack_.push(Variable(res));
        break;
    }
    /* алг удалить_каталог(лит имя файла) */
    case 0x0013: {
        const String x = valuesStack_.pop().toString();
        Kumir::Files::rmdir(x);
        break;
    }
    /* алг файл консоль */
    case 0x0014: {
        const Kumir::FileType console = Kumir::Files::getConsoleBuffer();
        const Record record = toRecordValue(console);
        valuesStack_.push(Variable(record, Kumir::Core::fromUtf8("файл")));
        break;
    }
    /* алг лог =(фaйл ф1, файл ф2) */
    case 0x0015: {
        const Variable f2var = valuesStack_.pop();
        const Variable f1var = valuesStack_.pop();
        const Record f2rec = f2var.toRecord();
        const Record f1rec = f1var.toRecord();
        const Kumir::FileType f1 = fromRecordValue(f1rec);
        const Kumir::FileType f2 = fromRecordValue(f2rec);
        valuesStack_.push(Variable(f1==f2));
        break;
    }
    /* алг лог =(фaйл ф1, файл ф2) */
    case 0x0016: {
        const Variable f2var = valuesStack_.pop();
        const Variable f1var = valuesStack_.pop();
        const Record f2rec = f2var.toRecord();
        const Record f1rec = f1var.toRecord();
        const Kumir::FileType f1 = fromRecordValue(f1rec);
        const Kumir::FileType f2 = fromRecordValue(f2rec);
        valuesStack_.push(Variable(f1!=f2));
        break;
    }    
    default: {
        error_ = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_stringscall(uint16_t alg)
{
    if (stacksMutex_) stacksMutex_->lock();
    valuesStack_.pop(); // Args count
    switch (alg) {
    /* алг лит верхний регистр(лит строка) */
    case 0x0000: {
        const String x = valuesStack_.pop().toString();
        const String y = Kumir::StringUtils::toUpperCase(x);
        Variable res(y);
        valuesStack_.push(res);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг лит нижний регистр(лит строка) */
    case 0x0001: {
        const String x = valuesStack_.pop().toString();
        const String y = Kumir::StringUtils::toLowerCase(x);
        Variable res(y);
        valuesStack_.push(res);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг цел позиция после(цел от, лит фрагмент, лит строка) */
    case 0x0002: case 0x0007: {
        const String s = valuesStack_.pop().toString();
        const String sub = valuesStack_.pop().toString();
        const int from = valuesStack_.pop().toInt();
        const int y = Kumir::StringUtils::find(from+1, sub, s);
        Variable res(y);
        valuesStack_.push(res);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг цел позиция(лит фрагмент, лит строка) */
    case 0x0003: case 0x0008: {
        const String s = valuesStack_.pop().toString();
        const String sub = valuesStack_.pop().toString();
        const int y = Kumir::StringUtils::find(sub, s);
        Variable res(y);
        valuesStack_.push(res);
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг вставить(лит фрагмент, аргрез лит строка, цел позиция) */
    case 0x0004: {
        const int from = valuesStack_.pop().toInt();
        Variable sr = valuesStack_.pop().toReference();
        String s = sr.value().toString();
        const String sub = valuesStack_.pop().toString();
        Kumir::StringUtils::insert(sub, s, from);
        sr.setValue(AnyValue(s));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг заменить(аргрез лит строка, лит старый фрагмент, лит новый фрагмент, лог каждый) */
    case 0x0005: {
        const bool all = valuesStack_.pop().toBool();
        const String newSub = valuesStack_.pop().toString();
        const String oldSub = valuesStack_.pop().toString();
        Variable sr = valuesStack_.pop().toReference();
        String s = sr.value().toString();
        Kumir::StringUtils::replace(s, oldSub, newSub, all);
        sr.setValue(AnyValue(s));
        error_ = Kumir::Core::getError();
        break;
    }
    /* алг удалить(аргрез лит строка, цел начало, цел количество) */
    case 0x0006: {
        const int count = valuesStack_.pop().toInt();
        const int from = valuesStack_.pop().toInt();
        Variable sr = valuesStack_.pop().toReference();
        String s = sr.value().toString();
        Kumir::StringUtils::remove(s, from, count);
        sr.setValue(AnyValue(s));
        error_ = Kumir::Core::getError();
        break;
    }

    default: {
        error_ = Kumir::Core::fromUtf8("Вызов неизвестного алгоримта, возможно из более новой версии Кумир");
    }
    }
    if (stacksMutex_) stacksMutex_->unlock();
}


inline Kumir::FileType KumirVM::fromRecordValue(const Record & record) {
    Kumir::FileType result;
    result.fullPath = record.fields[0].toString();
    result.mode = record.fields[1].toInt();
    result.type = record.fields[2].toInt();
    result.valid = record.fields[3].toBool();
    return result;
}

inline Record KumirVM::toRecordValue(const Kumir::FileType & ft) {
    Record record;
    record.fields.resize(4);
    record.fields[0] = AnyValue(ft.fullPath);
    record.fields[1] = AnyValue(ft.mode);
    record.fields[2] = AnyValue(ft.type);
    record.fields[3] = AnyValue(ft.valid);
    return record;
}

void KumirVM::do_specialcall(uint16_t alg)
{
    if (stacksMutex_) stacksMutex_->lock();
    int argsCount = valuesStack_.pop().toInt();
    if (stacksMutex_) stacksMutex_->unlock();
    // Special calls
    if (alg==0x00) {
        // Input
        if (stacksMutex_) stacksMutex_->lock();
        bool fileIO = false;
        int varsCount = argsCount;
        Kumir::FileType fileReference;
        std::deque<Variable> references;
        for (int i=0; i<varsCount; i++) {
            const Variable & ref = valuesStack_.pop();
            if (ref.baseType()==VT_record
                    && ref.recordClassName()==Kumir::Core::fromUtf8("файл")) {
                fileIO = true;
                if (!ref.isValid()) {
                    error_ = Kumir::Core::fromUtf8("Нет значения у ключа файла");
                    return;
                }
                const Record fileReferenceRecord = ref.toRecord();
                fileReference = fromRecordValue(fileReferenceRecord);
            }
            else {
                references.push_back(ref);
            }
        }
        if (stacksMutex_) stacksMutex_->unlock();
        bool hasInput = false;
        if (input_&& !fileIO && !Kumir::Files::overloadedStdIn()) {
            // input functor works like input operator here
            try {
                hasInput = (*input_)(references);
            }
            catch (const String & message) {
                error_ = message;
            }
            catch (const std::string & message) {
                error_ = Kumir::Core::fromUtf8(message);
            }
        }
        else {
            hasInput = true;
            if (stacksMutex_) stacksMutex_->lock();

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
                    const String & value = Kumir::IO::readLine(fileReference, !fileIO);
                    references.at(i).setValue(AnyValue(value));
                }
                if (Kumir::Core::getError().length()>0 && error_.length()==0) {
                    error_ = Kumir::Core::getError();
                }
                if (error_.length()>0)
                    break;
            }
            if (stacksMutex_) stacksMutex_->unlock();
        }
        const int lineNo = contextsStack_.top().lineNo;
        if (lineNo!=-1 && debugHandler_ && hasInput &&
                !blindMode_ &&
                contextsStack_.top().type != EL_BELOWMAIN &&
                error_.empty()
                )
        {
            String margin;
            margin.reserve(100);
            for (int i=0; i<references.size(); i++) {
                const Variable & var = references.at(i);
                if (var.isConstant())
                    continue;
                String svalue;
                if (var.baseType()==VT_char) {
                    svalue.push_back('\'');
                    svalue.push_back(var.value().toChar());
                    svalue.push_back('\'');
                }
                else if (var.baseType()==VT_string) {
                    svalue.push_back('"');
                    svalue.append(var.value().toString());
                    svalue.push_back('"');
                }
                else if (var.baseType()==VT_record) {
                    try {
                        svalue = (*customTypeToString_)(var);
                    }
                    catch (...) {
                        // do nothing for margin value
                    }
                }
                else {
                    svalue = var.value().toString();
                }
                if (svalue.length()==0)
                    continue;
                if (i>0) {
                    margin.push_back(',');
                    margin.push_back(' ');
                }
                margin += var.fullReferenceName();
                margin.push_back('=');
                margin += svalue;
            }
            if (contextsStack_.top().moduleContextNo == 0)
                debugHandler_->appendTextToMargin(lineNo, margin);
        }
    }
    if (alg==0x01) {
        // Output
        if (stacksMutex_) stacksMutex_->lock();
        bool fileIO = false;
        Kumir::FileType fileReference;
        if (argsCount % 3) {
            fileIO = true;
            const Variable fileVar = valuesStack_.pop();
            if (!fileVar.isValid()) {
                error_ = Kumir::Core::fromUtf8("Нет значения у ключа файла");
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
            format.second = valuesStack_.pop().toInt();
            format.first = valuesStack_.pop().toInt();
            formats.push_back(format);
            const Variable & ref = valuesStack_.pop();
            values.push_back(ref);
        }
        if (stacksMutex_) stacksMutex_->unlock();
        if (output_ && !fileIO && !Kumir::Files::overloadedStdOut()) {
            // output functor works like output operator here
            try {
                (*output_)(values, formats);
            }
            catch (const String & message) {
                error_ = message;
            }
            catch (const std::string & message) {
                error_ = Kumir::Core::fromUtf8(message);
            }
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
                if (Kumir::Core::getError().length()>0 && error_.length()==0) {
                    error_ = Kumir::Core::getError();
                }
                if (error_.length()>0)
                    break;
            }
        }
    }
    else if (alg==0x04) {
        // Get char from string
        if (stacksMutex_) stacksMutex_->lock();
        Variable second = valuesStack_.pop();
        Variable first = valuesStack_.pop();
        int index = second.value().toInt();
        const String & s = first.value().toString();
        error_ = Kumir::Core::getError();
        if (error_.length()==0) {
            if (index<1 || index>s.length()) {
                error_ = Kumir::Core::fromUtf8("Индекс символа больше длины строки");
            }
            else {
                Char result = s[index-1];
                Variable r(result);
                valuesStack_.push(r);
            }
        }
        if (stacksMutex_) stacksMutex_->unlock();
    }
    else if (alg==0x05) {
        // Set char in string
        if (stacksMutex_) stacksMutex_->lock();
        Variable third = valuesStack_.pop();
        Variable second = valuesStack_.pop();
        Variable first = valuesStack_.pop();
        int index = third.value().toInt();
        String source = second.value().toString();
        Char ch = first.value().toChar();
        error_ = Kumir::Core::getError();
        if (error_.length()==0) {
            if (index<1) {
                error_ = Kumir::Core::fromUtf8("Индекс символа меньше 1");
            }
            else if (index>source.length()) {
                error_ = Kumir::Core::fromUtf8("Индекс символа больше длины строки");
            }
            else {
                source[index-1] = ch;
                Variable r(source);
                valuesStack_.push(r);
            }
        }
        if (stacksMutex_) stacksMutex_->unlock();
    }
    else if (alg==0x06) {
        // Get slice from string
        if (stacksMutex_) stacksMutex_->lock();
        Variable third = valuesStack_.pop();
        Variable second = valuesStack_.pop();
        Variable first = valuesStack_.pop();
        int start = second.value().toInt();
        int end   = third.value().toInt();
        const String & s = first.value().toString();
        error_ = Kumir::Core::getError();
        if (error_.length()==0) {
            if (start<1 || start>s.length()) {
                error_ = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start) {
                String empty;
                valuesStack_.push(Variable(empty));
            }
            else if (end<1 || end>s.length()) {
                error_ = Kumir::Core::fromUtf8("Правая граница вырезки за пределами строки");
            }
            else {
                String result = s.substr(start-1, end-start+1);
                Variable r(result);
                valuesStack_.push(r);
            }
        }
        if (stacksMutex_) stacksMutex_->unlock();
    }
    else if (alg==0x07) {
        // Set slice in string
        if (stacksMutex_) stacksMutex_->lock();
        Variable fourth = valuesStack_.pop();
        Variable third = valuesStack_.pop();
        Variable second = valuesStack_.pop();
        Variable first = valuesStack_.pop();
        int end = fourth.value().toInt();
        int start = third.value().toInt();
        String source = second.value().toString();
        String ch = first.value().toString();
        error_ = Kumir::Core::getError();
        if (error_.length()==0) {
            if (end<start && start==0) {
                source = ch + source;
                Variable r(source);
                valuesStack_.push(r);
            }
            else if (start>source.length()) {
                error_ = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start && start>0) {
                source.insert(start, ch);
                Variable r(source);
                valuesStack_.push(r);
            }
            else if (start==source.length()+1 && end<=start) {
                source.append(ch);
                Variable r(source);
                valuesStack_.push(r);
            }
            else if (end<1 || end>source.length()) {
                error_ = Kumir::Core::fromUtf8("Правая граница вырезки за пределами строки");
            }
            else if (start<1 || start>source.length()) {
                error_ = Kumir::Core::fromUtf8("Левая граница вырезки за пределами строки");
            }
            else if (end<start) {
                error_ = Kumir::Core::fromUtf8("Ошибка в границах вырезки");
            }
            else {
                source = source.substr(0,start-1)+ch+source.substr(end);
                Variable r(source);
                valuesStack_.push(r);
            }
        }
        if (stacksMutex_) stacksMutex_->unlock();
    }
    else if (alg==0xBB01) {
        if (stacksMutex_) stacksMutex_->lock();
        // Input argument
        int localId = argsCount; // Already removed from stack
        Variable ref = contextsStack_.top().locals[localId].toReference();
        if (stacksMutex_) stacksMutex_->unlock();
        try {
            (*getMainArgument_)(ref);
        }
        catch (const String & message) {
            error_ = message;
        }
        catch (const std::string & message) {
            error_ = Kumir::Core::fromUtf8(message);
        }
    }
    else if (alg==0xBB02) {
        if (stacksMutex_) stacksMutex_->lock();
        // Output argument or return value
        int localId = argsCount; // Already removed from stack
        Variable ref = contextsStack_.top().locals[localId].toReference();
        if (stacksMutex_) stacksMutex_->unlock();
        try {
            (*returnMainValue_)(ref);
        }
        catch (const String & message) {
            error_ = message;
        }
        catch (const std::string & message) {
            error_ = Kumir::Core::fromUtf8(message);
        }
    }
}

void KumirVM::do_init(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    findVariable(s,id).init();
    nextIP();
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_setarr(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable & var = findVariable(s, id);
    const int dim = var.dimension();
    int bounds[7];    
    if (dim>0) {
        String name;
        bounds[6] = dim*2;
        for (int i=0; i<dim*2; i++) {
            bounds[i] = valuesStack_.pop().toInt();
        }
        var.setBounds(bounds);
        if (!blindMode_)
            name = var.name();
        error_ = Kumir::Core::getError();
        const int lineNo = contextsStack_.top().lineNo;
        if (lineNo!=-1 &&
                !blindMode_ &&
                contextsStack_.top().type != EL_BELOWMAIN
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
            if (debugHandler_ && currentContext().runMode==CRM_OneStep) {                
                if (contextsStack_.top().moduleContextNo == 0)
                    debugHandler_->appendTextToMargin(lineNo, notice);
                if (VariableScope(s)==Bytecode::LOCAL) {
                    debugHandler_->debuggerUpdateLocalTableBounds(var.myName(), bounds);
                }
                else if (VariableScope(s)==Bytecode::GLOBAL) {
                    debugHandler_->debuggerUpdateGlobalTableBounds(var.moduleName(), name, bounds);
                }
            }
        }
    }
    nextIP();
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_updarr(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable & var = findVariable(s, id);
    const int dim = var.dimension();
    int bounds[7];
    if (dim>0) {
        String name;
        int effectiveBounds[7];
        bounds[6] = effectiveBounds[6] = dim*2;
        for (int i=0; i<dim*2; i++) {
            bounds[i] = valuesStack_.pop().toInt();
        }
        var.updateBounds(bounds);
        var.getEffectiveBounds(effectiveBounds);
        if (!blindMode_)
            name = var.myName();
        error_ = Kumir::Core::getError();
        const int lineNo = contextsStack_.top().lineNo;
        if (lineNo!=-1 &&
                !blindMode_ &&
                contextsStack_.top().moduleContextNo == 0 &&
                contextsStack_.top().type != EL_BELOWMAIN
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
            if (debugHandler_)
                if (contextsStack_.top().moduleContextNo == 0)
                    debugHandler_->appendTextToMargin(lineNo, notice);
        }
    }
    nextIP();
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_store(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    const Variable & value = valuesStack_.top();
    const int lineNo = contextsStack_.top().lineNo;
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
    if (lineNo!=-1 &&
            !blindMode_ &&
            contextsStack_.top().type != EL_BELOWMAIN &&
            contextsStack_.top().moduleContextNo == 0 &&
            value.dimension()==0
            )
    {
        const String & name = variable.myName();
        String svalue;
        if (t==VT_string) {
            const String valueString = value.toString();
            svalue.reserve(valueString.length()+2);
            svalue.push_back(Char('"'));
            svalue.append(valueString);
            svalue.push_back(Char('"'));
        }
        else if (t==VT_char) {
            const Char valueChar = value.toChar();
            svalue.reserve(3);
            svalue.push_back(Char('\''));
            svalue.push_back(valueChar);
            svalue.push_back(Char('\''));
        }
        else if (t==VT_int) {
            svalue = Kumir::Converter::sprintfInt(value.toInt(), 10, 0, 0);
        }
        else if (t==VT_real) {
            svalue = Kumir::Converter::sprintfReal(value.toReal(), '.', false, 0 ,0, 0);
        }
        else if (t==VT_bool) {
            static const String YES = Kumir::Core::fromUtf8("да");
            static const String NO = Kumir::Core::fromUtf8("нет");
            svalue = value.toBool()? YES : NO;
        }
        else if (t==VT_record) {
            try {
                svalue = (*customTypeToString_)(variable);
            }
            catch (...) {
                // do nothing in case of margin value print
            }
        }
        if (debugHandler_ && svalue.length()>0) {
            const String message = name+Char('=')+svalue;
            if (contextsStack_.top().moduleContextNo == 0)
                debugHandler_->appendTextToMargin(lineNo, message);
        }
        if (debugHandler_ && currentContext().runMode==CRM_OneStep) {
            if (VariableScope(s)==Bytecode::LOCAL)
                debugHandler_->debuggerUpdateLocalVariable(variable.myName(), svalue);
            else if (VariableScope(s)==Bytecode::GLOBAL)
                debugHandler_->debuggerUpdateGlobalVariable(
                            variable.moduleName(),
                            variable.name(),
                            svalue
                            );

        }
    }
    if (contextsStack_.top().type==Bytecode::EL_BELOWMAIN)
        Variable::unsetError();
    error_ = Kumir::Core::getError();
    nextIP();
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_load(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
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
            && contextsStack_.top().locals[id].algorhitmName()==contextsStack_.top().locals[id].name();
    if (isRetVal && contextsStack_.top().type==Bytecode::EL_MAIN)
        Variable::unsetError();
    if (Kumir::Core::getError().length()==0) {
        valuesStack_.push(val);
        if (val.dimension()==0)
            register0_ = val.value();
        if (isRetVal && contextsStack_.top().type==Bytecode::EL_MAIN)
            Variable::unsetError();
    }
    error_ = Kumir::Core::getError();
    nextIP();
    if (stacksMutex_) stacksMutex_->unlock();
}

void KumirVM::do_storearr(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    String name;
    String svalue;
    const int lineNo = contextsStack_.top().lineNo;
    String sindeces;
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    if (!blindMode_) {
        name = variable.name();
    }
    int indeces[4];
    indeces[3] = dim;
    if (dim>0) {
        for (int i=0; i<dim; i++) {
            indeces[i] = valuesStack_.pop().toInt();
            if (!sindeces.empty())
                sindeces.push_back(',');
            sindeces += Kumir::Converter::sprintfInt(indeces[i], 10, 0, 0);
        }
        const Variable & value = valuesStack_.top();
        ValueType t = VT_void;
        if (!blindMode_)
            svalue = value.toString();
        variable.setValue(indeces, value.value());
        t = variable.baseType();
        if (t==VT_string)
            if (lineNo!=-1 && !blindMode_) {
                svalue.insert(0, 1, Char('"'));
                svalue.push_back(Char('"'));
            }
        if (t==VT_char)
            if (lineNo!=-1 && !blindMode_) {
                svalue.insert(0, 1, Char('\''));
                svalue.push_back(Char('\''));
            }
    }
    if (lineNo!=-1 &&
            !blindMode_
            )
    {
        String notice = name;
        notice.push_back(Char('['));
        notice += sindeces;
        notice.push_back(Char(']'));
        notice.push_back(Char('='));
        notice += svalue;
        if (debugHandler_) {
            if (contextsStack_.top().moduleContextNo == 0)
                debugHandler_->appendTextToMargin(lineNo, notice);
        }
        if (debugHandler_ && currentContext().runMode==CRM_OneStep) {
            if (VariableScope(s)==Bytecode::LOCAL)
                debugHandler_->debuggerUpdateLocalTableValue(variable.myName(), indeces);
            else if (VariableScope(s)==Bytecode::GLOBAL) {
                debugHandler_->debuggerUpdateGlobalTableValue(
                            variable.moduleName(),
                            name,
                            indeces
                            );
            }
        }
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_loadarr(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    const ValueType vt = variable.baseType();

    if (dim>0 || vt==VT_string) {
        int indeces[4];
        indeces[3] = dim;
        for (int i=0; i<dim; i++) {
            indeces[i] = valuesStack_.pop().toInt();
        }
        Variable val;
        AnyValue vv;
        val.setBaseType(vt);
        vv = variable.value(indeces);
        if (vv.isValid()) {
            val.setValue(vv);
            valuesStack_.push(val);
            if (val.baseType()==VT_int)
                register0_ = val.toInt();
            else if (val.baseType()==VT_real)
                register0_ = val.toReal();
            else if (val.baseType()==VT_char)
                register0_ = val.toChar();
            else if (val.baseType()==VT_string)
                register0_ = val.toString();
            else if (val.baseType()==VT_bool)
                register0_ = val.toBool();
        }
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_ref(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable & variable = findVariable(s, id);
    Variable ref = variable.toReference();
    if (!blindMode_) {
        ref.setReferenceStackContextNo(VariableScope(s)==Bytecode::LOCAL
                                       ? contextsStack_.size()-1
                                       : -1
                    );
    }
    if (ref.isReference()) {
        valuesStack_.push(ref);
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_setref(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable ref = valuesStack_.top();
    int bounds[7];
    ref.getEffectiveBounds(bounds);
    String name;
    if (!ref.isReference()) {
        error_ = Kumir::Core::fromAscii("Internal error");
    }
    else {
        Variable & variable = findVariable(s, id);
        variable.setReference(ref.reference(), bounds);
        name = variable.myName();
    }
    const int lineNo = contextsStack_.top().lineNo;
    if (lineNo!=-1 &&
            !blindMode_ &&
            contextsStack_.top().type != EL_BELOWMAIN
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
            if (debugHandler_) {
                if (contextsStack_.top().moduleContextNo == 0)
                    debugHandler_->appendTextToMargin(lineNo, name);
            }
            if (debugHandler_ && currentContext().runMode==CRM_OneStep) {
                if (VariableScope(s)==Bytecode::LOCAL) {
                    int dummyIndeces[4];
                    dummyIndeces[3] = 0;
                    int targetStackPos = ref.referenceStackContextNo();
                    debugHandler_->debuggerSetLocalReference(
                                name,
                                ref.name(),
                                dummyIndeces,
                                targetStackPos==-1
                                    ? -1
                                    : contextsStack_.size()-1-targetStackPos,
                                targetStackPos==-1
                                    ? ref.moduleName()
                                    : Kumir::String()
                                );
                }
            }
        }
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_refarr(uint8_t s, uint16_t id)
{
    if (stacksMutex_) stacksMutex_->lock();
    Variable & variable = findVariable(s, id);
    const int dim = variable.dimension();
    if (dim>0) {
        int indeces[4];
        indeces[3] = dim;
        for (int i=0; i<dim; i++) {
            indeces[i] = valuesStack_.pop().toInt();
        }
        Variable ref = variable.toReference(indeces);
        valuesStack_.push(ref);
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_jump(uint16_t ip)
{
    contextsStack_.top().IP = ip;
}

void KumirVM::do_jnz(uint8_t r, uint16_t ip)
{
    const AnyValue & registerValue = r==0u
            ? register0_
            : currentContext().registers[r];

    const bool value = registerValue.toBool();
    if (value) {
        currentContext().IP = ip;
    }
    else {
        nextIP();
    }
}


void KumirVM::do_jz(uint8_t r, uint16_t ip)
{
    const AnyValue & registerValue = r==0u
            ? register0_
            : currentContext().registers[r];

    const bool value = registerValue.toBool();
    if (! value) {
        currentContext().IP = ip;
    }
    else {
        nextIP();
    }
}

void KumirVM::do_push(uint8_t r)
{
    AnyValue v;
    if (r==0)
        v = register0_;
    else
        v = contextsStack_.top().registers[r];
    valuesStack_.push(Variable(v));
    nextIP();
}

void KumirVM::do_pop(uint8_t r)
{
    Variable v = valuesStack_.pop();
    AnyValue & registerToStore = r==0u
            ? register0_
           : currentContext().registers[r];
    if (v.hasValue()) {
        registerToStore = v.value();
    }

    nextIP();
}

void KumirVM::do_showreg(uint8_t regNo) {
    if (!blindMode_) {
        const int lineNo = contextsStack_.top().lineNo;
        if (lineNo!=-1 &&
                !blindMode_
                )
        {
            const AnyValue & val = regNo==0u
                    ? register0_
                    : currentContext().registers[regNo];
            if (debugHandler_)
                if (contextsStack_.top().moduleContextNo == 0)
                    debugHandler_->appendTextToMargin(lineNo, val.toString());
        }
    }
    nextIP();
}

void KumirVM::do_clearmarg(uint16_t toLine)
{
    const int lineNo = contextsStack_.top().lineNo;
    if (!blindMode_ && lineNo!=-1
            )
    {
        if (debugHandler_)
            debugHandler_->clearMargin(lineNo, toLine);
    }
    nextIP();
}

void KumirVM::do_ret()
{
    if (contextsStack_.top().runMode==CRM_UntilReturn) {
        if (debugHandler_)
            debugHandler_->noticeOnFunctionReturn(contextsStack_.top().lineNo);
        contextsStack_.top().runMode=CRM_ToEnd;
    }
    else {
        lastContext_ = contextsStack_.top();
        if (debugHandler_ && !blindMode_) {
            if (lastContext_.type==Bytecode::EL_MAIN
                    || lastContext_.type==Bytecode::EL_FUNCTION
                    ) {
                debuggerPopContext();
            }
        }
        contextsStack_.pop();
        if (lastContext_.type==Bytecode::EL_INIT
                && lastContext_.runMode == CRM_OneStep
                )
        {
            if (contextsStack_.size()>0) {
                contextsStack_.top().runMode = CRM_OneStep;
                if (debugHandler_ && !blindMode_
                        && contextsStack_.top().type==EL_MAIN)
                {
                    debuggerPushContext(false,
                                        contextsStack_.top().name,
                                        &(contextsStack_.top().locals),
                                        false
                                        );
                }
            }
        }
        if (contextsStack_.size()>0) {
            nextIP();
        }
    }
    if (contextsStack_.size()>0) {
        currentLocals_ = &(contextsStack_.top().locals);
        currentGlobals_ =
                &(moduleContexts_[contextsStack_.top().moduleContextNo].globals[contextsStack_.top().moduleId]);
        currentConstants_ =
                &(moduleContexts_[contextsStack_.top().moduleContextNo].constants);
    }
}

void KumirVM::do_error(uint8_t s, uint16_t id)
{
    const int lineNo = currentContext().lineNo;
    error_ = findVariable(s,id).toString();
    if (debugHandler_ && lineNo!=-1) {
        debugHandler_->setTextToMargin(lineNo, error_, true);
    }
}

void KumirVM::do_line(const Bytecode::Instruction & instr)
{
    uint32_t from = 0u, to = 0u;
    if (extractColumnPositionsFromLineInstruction(instr, from, to)) {
        currentContext().columnStart = from;
        currentContext().columnEnd = to;
        if (!blindMode_ && contextsStack_.top().runMode==CRM_OneStep && contextsStack_.top().moduleContextNo==0) {
            if (debugHandler_)
                debugHandler_->noticeOnLineChanged(currentContext().lineNo, from, to);
        }
        if (currentContext().IP!=-1) {
            stepsCounter_ ++;
            if (!blindMode_ && debugHandler_) {
                debugHandler_->noticeOnStepsChanged(stepsCounter_);
            }
            else if (blindMode_ && debugHandler_ && (stepsCounter_ % 1000 == 1)) {
                debugHandler_->noticeOnStepsChanged(stepsCounter_);
            }
        }
    }
    else {
        int no = instr.arg;
//        if (!blindMode_ && contextsStack_.top().runMode==CRM_OneStep) {
//            if (contextsStack_.top().lineNo!=no) {
//                if (debugHandler_)
//                    debugHandler_->noticeOnLineChanged(no, 0u, 0u);
//            }
//        }
        currentContext().lineNo = no;
        currentContext().columnStart = currentContext().columnEnd = 0u;
    }
    nextIP();
}

void KumirVM::do_sum()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()+b.toInt());
        valuesStack_.push(r);
        if (!Kumir::Math::checkSumm(a.toInt(), b.toInt())) {
            error_ = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()+b.toReal());
        valuesStack_.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            error_ = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    else if (a.baseType()==VT_string || a.baseType()==VT_char) {
        Variable r(a.toString()+b.toString());
        valuesStack_.push(r);
    }
    nextIP();
}

void KumirVM::do_sub()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()-b.toInt());
        valuesStack_.push(r);
        if (!Kumir::Math::checkDiff(a.toInt(), b.toInt())) {
            error_ = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()-b.toReal());
        valuesStack_.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            error_ = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    nextIP();
}

void KumirVM::do_mul()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        Variable r(a.toInt()*b.toInt());
        valuesStack_.push(r);
        if (!Kumir::Math::checkProd(a.toInt(), b.toInt())) {
            error_ = Kumir::Core::fromUtf8("Целочисленное переполнение");
        }
    }
    else if (a.baseType()==VT_real || b.baseType()==VT_real) {
        Variable r(a.toReal()*b.toReal());
        valuesStack_.push(r);
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            error_ = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
    }
    nextIP();
}

void KumirVM::do_div()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (b.baseType()==VT_int && b.toInt()==0) {
        error_ = Kumir::Core::fromUtf8("Деление на ноль");
    }
    else if (b.baseType()==VT_real && b.toReal()==0.0) {
        error_ = Kumir::Core::fromUtf8("Деление на ноль");
    }
    else {
        Variable r(a.toReal()/b.toReal());
        if (!Kumir::Math::isCorrectReal(r.toReal())) {
            error_ = Kumir::Core::fromUtf8("Вещественное переполнение");
        }
        valuesStack_.push(r);
    }
    nextIP();
}

void KumirVM::do_pow()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    Variable r;
    if (a.baseType()==VT_int && b.baseType()==VT_int) {
        r = Variable(Kumir::Math::ipow(a.toInt(), b.toInt()));
    }
    else {
        r = Variable(Kumir::Math::pow(a.toReal(), b.toReal()));
    }
    valuesStack_.push(r);
    nextIP();
}

void KumirVM::do_neg()
{
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_bool) {
        Variable r(!a.toBool());
        valuesStack_.push(r);
        register0_ = AnyValue(!a.toBool());
    }
    else if (a.baseType()==VT_int) {
        Variable r(-a.toInt());
        valuesStack_.push(r);
    }
    else if (a.baseType()==VT_real) {
        Variable r(0.0-a.toReal());
        valuesStack_.push(r);
    }
    nextIP();
}

void KumirVM::do_and()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variable r(a.toBool() && b.toBool());
        valuesStack_.push(r);
    }
    nextIP();
}

void KumirVM::do_or()
{
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
    if (a.baseType()==VT_bool && b.baseType()==VT_bool) {
        Variable r(a.toBool() || b.toBool());
        valuesStack_.push(r);
    }
    nextIP();
}

void KumirVM::do_eq()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(result);
    nextIP();
}

void KumirVM::do_neq()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(!result);
    nextIP();
}

void KumirVM::do_ls()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(result);
    nextIP();
}

void KumirVM::do_gt()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(result);
    nextIP();
}

void KumirVM::do_leq()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(result);
    nextIP();
}

void KumirVM::do_geq()
{
    bool result = false;
    Variable b = valuesStack_.pop();
    Variable a = valuesStack_.pop();
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
    valuesStack_.push(r);
    register0_ = AnyValue(result);
    nextIP();
}

void KumirVM::do_ctl(uint8_t parameter, uint16_t value)
{
    if (parameter==0x00) {
        Variable::ignoreUndefinedError = value>0;
    }
    else if (parameter==0x01) {
        backtraceSkip_ = value;
    }
    nextIP();
}

void KumirVM::do_inrange()
{
    Variable value = valuesStack_.pop();
    Variable to = valuesStack_.pop();
    Variable from = valuesStack_.pop();
    Variable step = valuesStack_.pop();

    int iValue = value.toInt();
    int iStep = step.toInt();
    int iFrom = from.toInt();
    int iTo = to.toInt();
    bool res = iStep>=0
            ? (iFrom <= iValue) && (iValue <= iTo)
            : (iFrom >= iValue) && (iValue >= iTo);
    register0_ = res;
    nextIP();
}

void KumirVM::updateDebugger()
{
    debugHandler_->debuggerReset();
    for (int i=0; i<contextsStack_.size(); i++) {
        const Context & c = contextsStack_.at(i);
        if (c.type==EL_MAIN || c.type==EL_FUNCTION || c.type==EL_TESTING) {
            debuggerPushContext(false, c.name, &(c.locals), true);
            for (size_t i_var=0; i_var<c.locals.size(); i_var++) {
                const Variable & var = c.locals.at(i_var);
                if (var.isReference()) {
                    int referenceIndeces[4];
                    var.getReferenceIndeces(referenceIndeces);
                    debugHandler_->debuggerSetLocalReference(
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
                    debugHandler_->debuggerUpdateLocalTableBounds(
                                var.myName(),
                                bounds
                                );
                }
                if (var.isValid() && var.dimension()==0) {
                    debugHandler_->debuggerUpdateLocalVariable(
                                var.myName(),
                                var.value().toString()
                                );
                }
            }
        }
    }
    for (size_t i_context=0; i_context<moduleContexts_.size(); i_context++) {
        const ModuleContext & mc = moduleContexts_[i_context];
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
                    debugHandler_->debuggerUpdateGlobalTableBounds(
                                var.moduleName(),
                                var.myName(),
                                bounds
                                );
                }
                else if (var.isValid()) {
                    debugHandler_->debuggerUpdateGlobalVariable(
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
    if (stacksMutex_) stacksMutex_->lock();
    ContextRunMode prevRunMode = CRM_OneStep;
    if (contextsStack_.size()>0) {
        prevRunMode = currentContext().runMode;
        currentContext().runMode = CRM_OneStep;
    }
    blindMode_ = false;
    if (prevRunMode!=CRM_OneStep) {
        if (debugHandler_) {
            debugHandler_->noticeOnLineChanged(currentContext().lineNo,
                                               currentContext().columnStart,
                                               currentContext().columnEnd);
        }
        (*pause_)();
        if (debugHandler_) {
            debugHandler_->noticeOnLineChanged(currentContext().lineNo,
                                               currentContext().columnStart,
                                               currentContext().columnEnd);
        }
        if (debugHandler_) updateDebugger();
    }
    if (stacksMutex_) stacksMutex_->unlock();
    nextIP();
}

void KumirVM::do_halt(uint16_t)
{
    if (stacksMutex_) stacksMutex_->lock();
    static const String STOP = Kumir::Core::fromUtf8("\nСТОП.");
    static std::deque< std::pair<int,int> > formats;
    formats.push_back(std::pair<int,int>(0,0));
    static std::deque<Variable> values;
    values.push_back(Variable(STOP));
    try {
        (*output_)(values, formats);
    }
    catch(...) {
        // it is not an error while halting program
    }
    contextsStack_.reset();
    if (stacksMutex_) stacksMutex_->unlock();
}


void KumirVM::setNextCallInto()
{
    nextCallInto_ = true;
}

void KumirVM::setNextCallOut()
{
    if (contextsStack_.size()==0)
        return;
    contextsStack_.top().runMode = CRM_UntilReturn;
}

void KumirVM::setNextCallToEnd()
{
    for (int i=0; i<contextsStack_.size(); i++) {
        contextsStack_.at(i).runMode = CRM_ToEnd;
    }
}

void KumirVM::setNextCallStepOver()
{
    if (contextsStack_.size()==0)
        return;
    ContextRunMode prevMode = currentContext().runMode;
    currentContext().runMode = CRM_OneStep;
    if (prevMode!=CRM_OneStep) {
        updateDebugger();
    }
}

std::pair<uint32_t,uint32_t> KumirVM::effectiveColumn() const
{
    std::pair<uint32_t, uint32_t> result(0u, 0u);
    if (contextsStack_.size() > 0) {
        result.first = contextsStack_.top().columnStart;
        result.second = contextsStack_.top().columnEnd;
    }
    return result;
}

int KumirVM::effectiveLineNo() const
{
    if (contextsStack_.size()==0)
        return -1;
    int index = Kumir::Math::imax(0, int(contextsStack_.size()-1 - backtraceSkip_));
    return contextsStack_.at(index).lineNo;
}

bool KumirVM::canStepOut() const
{
    if (contextsStack_.size()==0)
        return false;
    else
        return contextsStack_.top().type==EL_FUNCTION;
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
    catch (const String & e) {
        error = e;
        ok = false;
    }
    catch (const std::string & e) {
        error = Kumir::Core::fromUtf8(e);
        ok = false;
    }
    if (ok) {
        try {
            setProgram(d, isMain, filename);
            ok = true;
        }
        catch (const String & e) {
            error = e;
            ok = false;
        }
        catch (const std::string & e) {
            error = Kumir::Core::fromUtf8(e);
            ok = false;
        }
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
    for (int i=contextsStack_.size()-1; i>=0; i--) {
        if (contextsStack_.at(i).algId==algorhitmId && contextsStack_.at(i).moduleId==moduleId)
            return i;
    }
    if (lastContext_.algId==algorhitmId && lastContext_.moduleId==moduleId)
        return -2;
    return -1;
}

const std::vector<Variable> & KumirVM::getLocals(int frameNo) const
{
    int curFrameNo = -1;
    int frameIndex = -1;
    for (int i=0; i<contextsStack_.size(); i++) {
        if (contextsStack_.at(i).type==EL_MAIN
                || contextsStack_.at(i).type==EL_FUNCTION)
        {
            curFrameNo += 1;
            if (curFrameNo==frameNo) {
                frameIndex = i;
                break;
            }
        }
    }
    const Context & c = contextsStack_.at(frameIndex);
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
        for (size_t i_fileNo = 0; i_fileNo<moduleContexts_.size(); i_fileNo++) {
            const ModuleContext & context = moduleContexts_[i_fileNo];
            if (context.filename==moduleName) {
                isFileName = true;
                contextNumber = i_fileNo;
                break;
            }
        }
    }
    const ModuleContext & context = moduleContexts_[contextNumber];
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


} // end namespace VM
#endif
