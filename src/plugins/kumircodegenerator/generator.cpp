#include "generator.h"
#include <kumir2-libs/vm/vm_tableelem.hpp>
#include <kumir2-libs/vm/vm.hpp>
#include <kumir2-libs/extensionsystem/pluginmanager.h>
#include <kumir2/actorinterface.h>
#include <kumir2-libs/dataformats/lexem.h>

namespace KumirCodeGenerator {
    
using namespace Shared;

template <typename T> std::vector<T>& operator<<(std::vector<T> & vec, const T & value)
{
    vec.push_back(value);
    return vec;
}


template <typename T> std::deque<T>& operator<<(std::deque<T> & vec, const T & value)
{
    vec.push_back(value);
    return vec;
}



Generator::Generator(QObject *parent)
    : QObject(parent)
    , byteCode_(nullptr)
    , debugLevel_(Shared::GeneratorInterface::LinesAndVariables)
{   
}

void Generator::setDebugLevel(DebugLevel debugLevel)
{
    debugLevel_ = debugLevel;
}

void Generator::reset(const AST::DataPtr ast, Bytecode::Data *bc)
{
    ast_ = ast;
    byteCode_ = bc;
    constants_.clear();
    externs_.clear();
}

static void getVarListSizes(const QVariant & var, int sizes[3], int fromDim)
{
    if (fromDim>2)
        return;
    sizes[0] = sizes[1] = sizes[2] = 1;
    QVariantList elems = var.toList();
    for (int i=0; i<elems.size(); i++) {
        if (elems.at(i).type()==QVariant::List) {
            getVarListSizes(elems[i], sizes, fromDim+1);
        }
    }
    sizes[fromDim] = qMax(sizes[fromDim], elems.size());
}

static VM::AnyValue makeAnyValue(const QVariant & val
                                 , const QList<Bytecode::ValueType> & vt
                                 , const QString & moduleName
                                 , const QString & className
                                 )
{
    VM::AnyValue result;
    if (val==QVariant::Invalid)
        return result;
    switch (vt.front())
    {
    case Bytecode::VT_int:
        result = VM::AnyValue(val.toInt()); break;
    case Bytecode::VT_real:
        result = VM::AnyValue(val.toDouble()); break;
    case Bytecode::VT_bool:
        result = VM::AnyValue(bool(val.toBool())); break;
    case Bytecode::VT_char:
        result = VM::AnyValue(Kumir::Char(val.toChar().unicode())); break;
    case Bytecode::VT_string:
        result = VM::AnyValue(val.toString().toStdWString()); break;
    case Bytecode::VT_record:
    {
        QVariantList valueFields = val.toList();
        VM::Record record;
        for (int i=1; i<vt.size(); i++) {
            Bytecode::ValueType fieldType = vt.at(i);
            switch (fieldType) {
            case Bytecode::VT_int:
                record.fields.push_back(VM::AnyValue(valueFields[i-1].toInt()));
                break;
            case Bytecode::VT_real:
                record.fields.push_back(VM::AnyValue(valueFields[i-1].toDouble()));
                break;
            case Bytecode::VT_bool:
                record.fields.push_back(VM::AnyValue(valueFields[i-1].toBool()));
                break;
            case Bytecode::VT_char:
                record.fields.push_back(VM::AnyValue(valueFields[i-1].toChar().unicode()));
                break;
            case Bytecode::VT_string:
                record.fields.push_back(VM::AnyValue(valueFields[i-1].toString().toStdWString()));
                break;
            default:
                break;
            }
        }
        result = VM::AnyValue(record);
        break;
    }
    default:
        break;
    }
    return result;
}

static Bytecode::TableElem makeConstant(const ConstValue & val)
{
    Bytecode::TableElem e;
    e.type = Bytecode::EL_CONST;
    e.vtype = val.baseType.toStdList();
    e.dimension = val.dimension;
    e.recordModuleLocalizedName = val.recordModuleName.toStdWString();
    e.recordClassLocalizedName  = val.recordClassLocalizedName.toStdWString();
    if (val.dimension==0) {
        VM::Variable var;
        VM::AnyValue vv = makeAnyValue(val.value,
                                       val.baseType,
                                       val.recordModuleName,
                                       val.recordClassLocalizedName
                                       );
        var.setValue(vv);
        var.setBaseType(val.baseType.front());
        var.setDimension(val.dimension);
        var.setConstantFlag(true);
        e.initialValue = var;
    }
    else {
        int sizes[3];
        getVarListSizes(val.value, sizes, 0);
        VM::Variable var;
        var.setConstantFlag(true);
        var.setBaseType(val.baseType.front());
        var.setDimension(val.dimension);
        int bounds[7] = { 1,  sizes[0], 1, sizes[1], 1, sizes[2], var.dimension()*2 };
        var.setBounds(bounds);
        var.init();
        if (var.dimension()==1) {
            QVariantList listX = val.value.toList();
            for (int x=1; x<=sizes[0]; x++) {
                if (x-1 < listX.size()) {
                    var.setValue(x, makeAnyValue(
                                 listX[x-1],
                                 val.baseType,
                                 val.recordModuleName,
                                 val.recordClassLocalizedName
                                 ));
                }
                else {
                    var.setValue(x, VM::AnyValue());
                }
            }
        } // end if (var.dimension()==1)
        else if (var.dimension()==2) {
            QVariantList listY = val.value.toList();
            for (int y=1; y<=sizes[0]; y++) {
                if (y-1 < listY.size()) {
                    QVariantList listX = listY[y-1].toList();
                    for (int x=1; x<=sizes[1]; x++) {
                        if (x-1 < listX.size()) {
                            var.setValue(y, x, makeAnyValue(
                                         listX[x-1],
                                         val.baseType,
                                         val.recordModuleName,
                                         val.recordClassLocalizedName
                                         ));
                        }
                        else {
                            var.setValue(y, x, VM::AnyValue());
                        }
                    }
                }
                else {
                    for (int x=1; x<=sizes[1]; x++) {
                        var.setValue(y, x, VM::AnyValue());
                    }
                }
            }
        } // end else if (var.dimension()==2)
        else if (var.dimension()==3) {
            QVariantList listZ = val.value.toList();
            for (int z=1; z<=sizes[0]; z++) {
                if (z-1 < listZ.size()) {
                    QVariantList listY = listZ[z-1].toList();
                    for (int y=1; y<=sizes[1]; y++) {
                        if (y-1 < listY.size()) {
                            QVariantList listX = listY[y-1].toList();
                            for (int x=1; x<=sizes[2]; x++) {
                                if (x-1 < listX.size()) {
                                    var.setValue(z, y, x, makeAnyValue(
                                                 listX[x-1],
                                                 val.baseType,
                                                 val.recordModuleName,
                                                 val.recordClassLocalizedName
                                                 ));
                                }
                                else {
                                    var.setValue(z, y, x, VM::AnyValue());
                                }
                            }
                        }
                        else {
                            for (int x=1; x<=sizes[1]; x++) {
                                var.setValue(z, y, x, VM::AnyValue());
                            }
                        }
                    }
                }
                else {
                    for (int y=1; y<=sizes[1]; y++) {
                        for (int x=1; x<=sizes[2]; x++) {
                            var.setValue(z, y, x, VM::AnyValue());
                        }
                    }
                }
            }
        } // end else if (var.dimension()==2)
        e.initialValue = var;
    }
    return e;
}

void Generator::generateConstantTable()
{
    for (int i=constants_.size()-1; i>=0; i--) {
        const ConstValue cons = constants_[i];
        Bytecode::TableElem e = makeConstant(cons);
        e.id = i;
        byteCode_->d.push_front(e);
    }
}

static const Shared::ActorInterface::Function & functionByInternalId(const Shared::ActorInterface * actor, uint32_t id)
{
    static Shared::ActorInterface::Function dummy;
    const Shared::ActorInterface::FunctionList & alist = actor->functionList();
    foreach (const Shared::ActorInterface::Function & func, alist) {
        if (func.id == id) {
            return func;
        }
    }
    return dummy;
}

static QString typeToSignature(const AST::Type & t) {
    QString result;
    if (t.kind == AST::TypeNone)
        result = "void";
    else if (t.kind == AST::TypeBoolean)
        result = "bool";
    else if (t.kind == AST::TypeInteger)
        result = "int";
    else if (t.kind == AST::TypeReal)
        result = "real";
    else if (t.kind == AST::TypeCharect)
        result = "char";
    else if (t.kind == AST::TypeString)
        result = "string";
    else if (t.kind == AST::TypeUser) {
        result = "record{";
        for (int i=0; i<t.userTypeFields.size(); i++) {
            if (i > 0)
                result += ";";
            result += typeToSignature(t.userTypeFields.at(i).second);
        }
        result += "}";
    }
    Q_ASSERT(result.length() > 0);
    return result;
}

static QString createMethodSignature(const AST::AlgorithmPtr alg)
{
    const QString rt = typeToSignature(alg->header.returnType);
    QStringList args;
    for (int i=0; i<alg->header.arguments.size(); i++) {
        QString sarg;
        const AST::VariablePtr karg = alg->header.arguments[i];
        if (karg->accessType == AST::AccessArgumentInOut)
            sarg = "inout ";
        else if (karg->accessType == AST::AccessArgumentOut)
            sarg = "out ";
        else
            sarg = "in ";
        sarg += typeToSignature(karg->baseType);
        for (uint8_t j=0; j<karg->dimension; j++) {
            sarg += "[]";
        }
        args << sarg;
    }
    const QString argline = args.size() > 0
            ? args.join(",") : QString();
    const QString result = argline.length() > 0
            ? rt + ":" + argline : rt;
    return result;
}

void Generator::generateExternTable()
{
    QSet<AST::ModulePtr> modulesImplicitlyImported;
    for (int i=externs_.size()-1; i>=0; i--) {
        QPair<quint8, quint16> ext = externs_[i];
        Bytecode::TableElem e;
        e.type = Bytecode::EL_EXTERN;
        e.module = ext.first;
        const AST::ModulePtr  mod = ast_->modules[ext.first];
        const QList<AST::AlgorithmPtr> table = mod->header.algorhitms + mod->header.operators;
        const AST::AlgorithmPtr  alg = table[ext.second];
        if (alg->header.external.moduleName.endsWith(".kum") ||
                alg->header.external.moduleName.endsWith(".kod"))
        {
            e.algId = e.id = ext.second;
        }
        else {
            e.algId = e.id = alg->header.external.id;
        }
        QList<ExtensionSystem::KPlugin*> plugins =
                ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
        QString moduleFileName;
        QString signature;
        for (int m=0; m<plugins.size(); m++) {
            Shared::ActorInterface * actor = qobject_cast<Shared::ActorInterface*>(plugins[m]);
            if (actor && actor->asciiModuleName()==mod->header.asciiName) {
                signature = createMethodSignature(alg);
                moduleFileName = plugins[m]->pluginSpec().libraryFileName;
                // Make filename relative
                int slashP = moduleFileName.lastIndexOf("/");
                if (slashP!=-1) {
                    moduleFileName = moduleFileName.mid(slashP+1);
                }
            }
        }
        if (mod->header.type == AST::ModTypeExternal)
            modulesImplicitlyImported.insert(mod);
        if (mod->header.type==AST::ModTypeCached)
            moduleFileName = mod->header.name;
        e.moduleAsciiName = std::string(mod->header.asciiName.constData());
        e.moduleLocalizedName = mod->header.name.toStdWString();
        e.name = alg->header.name.toStdWString();
        e.signature = signature.toStdWString();
        e.fileName = moduleFileName.toStdWString();
        byteCode_->d.push_front(e);
    }
    QSet<AST::ModulePtr> modulesExplicitlyImported;
    for (int i=0; i<ast_->modules.size(); i++) {
        AST::ModulePtr module = ast_->modules[i];
        if (module->header.type == AST::ModTypeExternal &&
                module->builtInID == 0x00
                ) {
            const QList<AST::ModuleWPtr> & used = module->header.usedBy;
            for (int j=0; j<used.size(); j++) {
                AST::ModuleWPtr reference = used[j];
                const AST::ModuleHeader & header = reference.data()->header;
                if (header.type == AST::ModTypeUser ||
                        header.type == AST::ModTypeUserMain ||
                        header.type == AST::ModTypeTeacher ||
                        header.type == AST::ModTypeTeacherMain
                        )
                {
                    modulesExplicitlyImported.insert(module);
                    break;
                }
            }
        }
    }
    foreach (AST::ModulePtr module, modulesExplicitlyImported - modulesImplicitlyImported) {
        Bytecode::TableElem e;
        e.type = Bytecode::EL_EXTERN_INIT;
        e.module = 0xFF;
        e.algId = e.id = 0xFFFF;
        e.moduleLocalizedName = module->header.name.toStdWString();
        QString moduleFileName;
        QList<ExtensionSystem::KPlugin*> plugins =
                ExtensionSystem::PluginManager::instance()->loadedPlugins("Actor*");
        for (int m=0; m<plugins.size(); m++) {
            Shared::ActorInterface * actor = qobject_cast<Shared::ActorInterface*>(plugins[m]);
            if (actor && actor->localizedModuleName(QLocale::Russian)==module->header.name) {
                moduleFileName = plugins[m]->pluginSpec().libraryFileName;
                // Make filename relative
                int slashP = moduleFileName.lastIndexOf("/");
                if (slashP!=-1) {
                    moduleFileName = moduleFileName.mid(slashP+1);
                }
            }
        }
        e.moduleAsciiName = std::string(module->header.asciiName.constData());
        e.moduleLocalizedName = module->header.name.toStdWString();
        e.fileName = moduleFileName.toStdWString();
        byteCode_->d.push_front(e);
    }
}


QList<Bytecode::ValueType> Generator::valueType(const AST::Type & t)
{
    QList<Bytecode::ValueType> result;
    if (t.kind==AST::TypeInteger)
        result << Bytecode::VT_int;
    else if (t.kind==AST::TypeReal)
        result << Bytecode::VT_real;
    else if (t.kind==AST::TypeBoolean)
        result << Bytecode::VT_bool;
    else if (t.kind==AST::TypeString)
        result << Bytecode::VT_string;
    else if (t.kind==AST::TypeCharect)
        result << Bytecode::VT_char;
    else if (t.kind==AST::TypeUser) {
        result << Bytecode::VT_record;
        for (int i=0; i<t.userTypeFields.size(); i++) {
            AST::Field field = t.userTypeFields[i];
            result << valueType(field.second);
        }
    }
    else
        result << Bytecode::VT_void;
    return result;
}

Bytecode::ValueKind Generator::valueKind(AST::VariableAccessType t)
{
    if (t==AST::AccessArgumentIn)
        return Bytecode::VK_In;
    else if (t==AST::AccessArgumentOut)
        return Bytecode::VK_Out;
    else if (t==AST::AccessArgumentInOut)
        return Bytecode::VK_InOut;
    else
        return Bytecode::VK_Plain;
}

Bytecode::InstructionType Generator::operation(AST::ExpressionOperator op)
{
    if (op==AST::OpSumm)
        return Bytecode::SUM;
    else if (op==AST::OpSubstract)
        return Bytecode::SUB;
    else if (op==AST::OpMultiply)
        return Bytecode::MUL;
    else if (op==AST::OpDivision)
        return Bytecode::DIV;
    else if (op==AST::OpPower)
        return Bytecode::POW;
    else if (op==AST::OpNot)
        return Bytecode::NEG;
    else if (op==AST::OpAnd)
        return Bytecode::AND;
    else if (op==AST::OpOr)
        return Bytecode::OR;
    else if (op==AST::OpEqual)
        return Bytecode::EQ;
    else if (op==AST::OpNotEqual)
        return Bytecode::NEQ;
    else if (op==AST::OpLess)
        return Bytecode::LS;
    else if (op==AST::OpGreater)
        return Bytecode::GT;
    else if (op==AST::OpLessOrEqual)
        return Bytecode::LEQ;
    else if (op==AST::OpGreaterOrEqual)
        return Bytecode::GEQ;
    else
        return Bytecode::NOP;
}

void Generator::addModule(const AST::ModulePtr mod)
{
    int id = ast_->modules.indexOf(mod);
    if (mod->header.type==AST::ModTypeExternal) {

    }
    else {
        addKumirModule(id, mod);
    }
}



void Generator::addKumirModule(int id, const AST::ModulePtr mod)
{
    for (int i=0; i<mod->impl.globals.size(); i++) {
        const AST::VariablePtr var = mod->impl.globals[i];
        Bytecode::TableElem glob;
        glob.type = Bytecode::EL_GLOBAL;
        glob.module = quint8(id);
        glob.id = quint16(i);
        glob.name = var->name.toStdWString();
        glob.dimension = quint8(var->dimension);
        glob.vtype = valueType(var->baseType).toStdList();
        glob.refvalue = valueKind(var->accessType);
        glob.recordModuleAsciiName = var->baseType.actor ?
                    std::string(var->baseType.actor->asciiModuleName().constData()) : std::string();
        glob.recordModuleLocalizedName = var->baseType.actor ?
                    var->baseType.actor->localizedModuleName(QLocale::Russian).toStdWString() : std::wstring();
        glob.recordClassLocalizedName = var->baseType.name.toStdWString();
        byteCode_->d.push_back(glob);
    }
    Bytecode::TableElem initElem;
    Bytecode::Instruction returnFromInit;
    returnFromInit.type = Bytecode::RET;
    initElem.type = Bytecode::EL_INIT;
    initElem.module = quint8(id);
    initElem.moduleLocalizedName = mod->header.name.toStdWString();
    initElem.instructions = instructions(id, -1, 0, mod->impl.initializerBody).toVector().toStdVector();
    if (!initElem.instructions.empty())
        initElem.instructions << returnFromInit;
    if (!initElem.instructions.empty())
        byteCode_->d.push_back(initElem);
    AST::ModulePtr  mainMod;
    AST::AlgorithmPtr  mainAlg;
    int mainModId = -1;
    int mainAlgorhitmId = -1;
    for (int i=0; i<mod->impl.algorhitms.size() ; i++) {
        const AST::AlgorithmPtr  alg = mod->impl.algorhitms[i];
        Bytecode::ElemType ft = Bytecode::EL_FUNCTION;
        if (mod->header.name.isEmpty() && i==0) {
            ft = Bytecode::EL_MAIN;
            if (!alg->header.arguments.isEmpty() || alg->header.returnType.kind!=AST::TypeNone)
            {
                mainMod = mod;
                mainAlg = alg;
                mainModId = id;
                mainAlgorhitmId = i;
            }
        }
        if (alg->header.specialType==AST::AlgorithmTypeTesting) {
            ft = Bytecode::EL_TESTING;
        }
        addFunction(i, id, ft, mod, alg);
    }
    if (mainMod && mainAlg) {
        addInputArgumentsMainAlgorhitm(mainModId, mainAlgorhitmId, mainMod, mainAlg);
    }
}

void Generator::shiftInstructions(QList<Bytecode::Instruction> &instrs, int offset)
{
    for (int i=0; i<instrs.size(); i++) {
        Bytecode::InstructionType t = instrs.at(i).type;
        if (t==Bytecode::JNZ || t==Bytecode::JZ || t==Bytecode::JUMP) {
            instrs[i].arg += offset;
        }
    }
}

void Generator::addInputArgumentsMainAlgorhitm(int moduleId, int algorhitmId, const AST::ModulePtr mod, const AST::AlgorithmPtr alg)
{
    // Generate hidden algorhitm, which will called before main to input arguments
    int algId = mod->impl.algorhitms.size();
    QList<Bytecode::Instruction> instrs = makeLineInstructions(alg->impl.headerLexems);
    QList<quint16> varsToOut;
    int locOffset = 0;

    // Add function return as local
    if (alg->header.returnType.kind!=AST::TypeNone) {
        const AST::VariablePtr retval = returnValue(alg);
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = algId;
        loc.id = 0;
        loc.name = tr("Function return value").toStdWString();
        loc.dimension = 0;
        loc.vtype = valueType(retval->baseType).toStdList();
        loc.refvalue = Bytecode::VK_Plain;
        byteCode_->d.push_back(loc);
        varsToOut << constantValue(Bytecode::VT_int, 0, 0, QString(), QString());
        locOffset = 1;
    }

    // Add arguments as locals
    for (int i=0; i<alg->header.arguments.size(); i++) {
        const AST::VariablePtr var = alg->header.arguments[i];
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = algId;
        loc.id = locOffset+i;
        loc.name = var->name.toStdWString();
        loc.dimension = var->dimension;
        loc.vtype = valueType(var->baseType).toStdList();
        loc.recordModuleAsciiName = var->baseType.actor ?
                    std::string(var->baseType.actor->asciiModuleName().constData()) : std::string();
        loc.recordModuleLocalizedName = var->baseType.actor ?
                    var->baseType.actor->localizedModuleName(QLocale::Russian).toStdWString() : std::wstring();
        loc.recordClassLocalizedName = var->baseType.name.toStdWString();
        loc.recordClassAsciiName = std::string(var->baseType.asciiName.constData());
        loc.refvalue = Bytecode::VK_Plain;
        byteCode_->d.push_back(loc);

    }

    for (int i=0; i<alg->header.arguments.size(); i++) {
        AST::VariablePtr  var = alg->header.arguments[i];
        // Initialize argument
        if (var->dimension > 0) {
            for (int j=var->dimension-1; j>=0 ; j--) {
                QList<Bytecode::Instruction> initBounds;
                initBounds << calculate(moduleId, algorhitmId, 0, var->bounds[j].second);
                initBounds << calculate(moduleId, algorhitmId, 0, var->bounds[j].first);
                for (int i_bounds=0; i_bounds<initBounds.size(); i_bounds++) {
                    Bytecode::Instruction & instr = initBounds[i_bounds];
                    if (instr.type==Bytecode::LOAD || instr.type==Bytecode::LOADARR) {
//                        if (instr.scope==Bytecode::LOCAL)
//                            instr.arg = uint16_t(instr.arg-locOffset);
                    }
                }
                instrs << initBounds;
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::SETARR;
            bounds.scope = Bytecode::LOCAL;
            bounds.arg = quint16(i+locOffset);
            instrs << bounds;
        }
        Bytecode::Instruction init;
        init.type = Bytecode::INIT;
        init.scope = Bytecode::LOCAL;
        init.arg = quint16(i+locOffset);
        instrs << init;
        if (var->initialValue.isValid() && var->dimension==0) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::CONSTT;
            load.arg = constantValue(valueType(var->baseType),
                                     0,
                                     var->initialValue,
                                     var->baseType.actor ?
                                         var->baseType.actor->localizedModuleName(QLocale::Russian) : "",
                                     var->baseType.name
                                     );
            instrs << load;
            Bytecode::Instruction store = init;
            store.type = Bytecode::STORE;
            instrs << store;
            Bytecode::Instruction pop;
            pop.type = Bytecode::POP;
            pop.registerr = 0;
            instrs << pop;
        }

        // If IN of INOUT argument -- require input
        // This made by special function call
        if (var->accessType==AST::AccessArgumentIn || var->accessType==AST::AccessArgumentInOut)  {
            Bytecode::Instruction varId;
            varId.type = Bytecode::LOAD;
            varId.scope = Bytecode::CONSTT;
            varId.arg = constantValue(Bytecode::VT_int, 0, i+locOffset, QString(), QString());

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xFF;
            call.arg = 0xBB01;

            instrs << varId << call;
        }
        if (var->accessType==AST::AccessArgumentOut || var->accessType==AST::AccessArgumentInOut) {
            varsToOut << constantValue(Bytecode::VT_int, 0, i+locOffset, QString(), QString());
        }
    }

    // Call main (first) algorhitm:
    //  -- 1) Push arguments
    for (int i=0; i<alg->header.arguments.size(); i++) {
        AST::VariableAccessType t = alg->header.arguments[i]->accessType;
        if (t==AST::AccessArgumentIn) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::LOCAL;
            load.arg = quint16(i+locOffset);
            instrs << load;
        }
        else if (t==AST::AccessArgumentOut||t==AST::AccessArgumentInOut) {
            Bytecode::Instruction ref;
            ref.type = Bytecode::REF;
            ref.scope = Bytecode::LOCAL;
            ref.arg = quint16(i+locOffset);
            instrs << ref;
        }
    }

    Bytecode::Instruction argsCount;
    argsCount.type = Bytecode::LOAD;
    argsCount.scope = Bytecode::CONSTT;
    argsCount.arg = constantValue(QList<Bytecode::ValueType>()<<Bytecode::VT_int,0,alg->header.arguments.size(), QString(), QString());
    instrs << argsCount;

    //  -- 2) Call main (first) algorhitm
    Bytecode::Instruction callInstr;
    callInstr.type = Bytecode::CALL;
    findFunction(alg, callInstr.module, callInstr.arg);
    instrs << callInstr;
    //  -- 3) Store return value
    if (alg->header.returnType.kind!=AST::TypeNone) {
        Bytecode::Instruction storeRetVal;
        storeRetVal.type = Bytecode::STORE;
        storeRetVal.scope = Bytecode::LOCAL;
        storeRetVal.arg = 0;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        instrs << storeRetVal << pop;
    }

    // Show what in result...

    for (int i=0; i<varsToOut.size(); i++) {
        Bytecode::Instruction arg;
        arg.type = Bytecode::LOAD;
        arg.scope = Bytecode::CONSTT;
        arg.arg = varsToOut[i];
        instrs << arg;
        Bytecode::Instruction callShow;
        callShow.type = Bytecode::CALL;
        callShow.module = 0xFF;
        callShow.arg = 0xBB02;
        instrs << callShow;
    }

    Bytecode::TableElem func;
    func.type = Bytecode::EL_BELOWMAIN;
    func.algId = func.id = algId;
    func.module = moduleId;
    func.moduleLocalizedName = mod->header.name.toStdWString();
    func.name = QString::fromLatin1("@below_main").toStdWString();
    func.instructions = instrs.toVector().toStdVector();
    byteCode_->d.push_back(func);

}

QString typeSignature(const AST::Type & tp) {
    QString signature;
    if (tp.kind==AST::TypeNone) {
        signature += "void";
    }
    else if (tp.kind==AST::TypeInteger) {
        signature += "int";
    }
    else if (tp.kind==AST::TypeReal) {
        signature += "real";
    }
    else if (tp.kind==AST::TypeBoolean) {
        signature += "bool";
    }
    else if (tp.kind==AST::TypeCharect) {
        signature += "char";
    }
    else if (tp.kind==AST::TypeString) {
        signature += "string";
    }
    else if (tp.kind==AST::TypeUser) {
        signature += "record "+tp.name+" {";
        for (int i=0; i<tp.userTypeFields.size(); i++) {
            signature += typeSignature(tp.userTypeFields.at(i).second);
            if (i<tp.userTypeFields.size()-1)
                signature += ";";
        }
        signature += "}";
    }
    return signature;
}

void Generator::addFunction(int id, int moduleId, Bytecode::ElemType type, const AST::ModulePtr  mod, const AST::AlgorithmPtr alg)
{
    QString headerError =  "";
    QString beginError = "";

    if (alg->impl.headerLexems.size()>0) {
        for (int i=0; i<alg->impl.headerLexems.size();i++) {
            if (alg->impl.headerLexems[i]->error.size()>0) {
                headerError = ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.headerLexems[i]->error);
                break;
            }
        }
    }

    if (alg->impl.beginLexems.size()>0) {
        for (int i=0; i<alg->impl.beginLexems.size();i++) {
            if (alg->impl.beginLexems[i]->error.size()>0) {
                beginError = ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.beginLexems[i]->error);
                break;
            }
        }
    }

    QString signature;

    signature = typeSignature(alg->header.returnType)+":";

    for (int i=0; i<alg->header.arguments.size(); i++) {
        const AST::VariablePtr  var = alg->header.arguments[i];
        if (var->accessType==AST::AccessArgumentIn)
            signature += "in ";
        else if (var->accessType==AST::AccessArgumentOut)
            signature += "out ";
        else if (var->accessType==AST::AccessArgumentInOut)
            signature += "inout ";
        signature += typeSignature(var->baseType);
        for (int j=0; j<var->dimension; j++) {
            signature += "[]";
        }
        if (i<alg->header.arguments.size()-1)
            signature += ",";
    }

    for (int i=0; i<alg->impl.locals.size(); i++) {
        const AST::VariablePtr  var = alg->impl.locals[i];
        Bytecode::TableElem loc;
        loc.type = Bytecode::EL_LOCAL;
        loc.module = moduleId;
        loc.algId = id;
        loc.id = i;
        loc.name = var->name.toStdWString();
        loc.dimension = var->dimension;
        loc.vtype = valueType(var->baseType).toStdList();
        loc.refvalue = valueKind(var->accessType);
        loc.recordModuleAsciiName = var->baseType.actor ?
                    std::string(var->baseType.actor->asciiModuleName().constData()) : std::string();
        loc.recordModuleLocalizedName = var->baseType.actor ?
                    var->baseType.actor->localizedModuleName(QLocale::Russian).toStdWString() : std::wstring();
        loc.recordClassAsciiName = std::string(var->baseType.asciiName);
        loc.recordClassLocalizedName = var->baseType.name.toStdWString();
        byteCode_->d.push_back(loc);
    }
    Bytecode::TableElem func;
    func.type = type;
    func.module = moduleId;
    func.algId = func.id = id;
    func.name = alg->header.name.toStdWString();
    func.signature = signature.toStdWString();
    func.moduleLocalizedName = mod->header.name.toStdWString();
    QList<Bytecode::Instruction> argHandle;

    argHandle += makeLineInstructions(alg->impl.headerLexems);

    if (headerError.length()>0) {
        Bytecode::Instruction err;
        err.type = Bytecode::ERRORR;
        err.scope = Bytecode::CONSTT;
        err.arg = constantValue(Bytecode::VT_string, 0, headerError, QString(), QString());
        argHandle << err;
    }

    if (alg->impl.headerRuntimeError.size()>0) {
        Bytecode::Instruction l;
        l.type = Bytecode::LINE;
        l.arg = alg->impl.headerRuntimeErrorLine;
        argHandle << l;
        Bytecode::setColumnPositionsToLineInstruction(l, 0u, 0u);
        argHandle << l;
        l.type = Bytecode::ERRORR;
        l.scope = Bytecode::CONSTT;
        l.arg = constantValue(Bytecode::VT_string, 0,
                              ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.headerRuntimeError)
                              , QString(), QString()
                              );
        argHandle << l;
    }

    if (alg->impl.endLexems.size()>0 && debugLevel_==GeneratorInterface::LinesAndVariables) {

        Bytecode::Instruction clearmarg;
        clearmarg.type = Bytecode::CLEARMARG;
        clearmarg.arg = alg->impl.endLexems[0]->lineNo;
        argHandle << clearmarg;
    }

    Bytecode::Instruction ctlOn, ctlOff;
    ctlOn.type = ctlOff.type = Bytecode::CTL;
    ctlOn.module = ctlOff.module = 0x01; // Set error stack offset lineno
    ctlOn.arg = 0x0001;
    ctlOff.arg = 0x0000;

    if (debugLevel_==GeneratorInterface::LinesAndVariables)
        argHandle << ctlOn;

    for (int i=alg->header.arguments.size()-1; i>=0; i--) {
        Bytecode::Instruction store;
        const AST::VariablePtr  var = alg->header.arguments[i];
        if (var->accessType==AST::AccessArgumentIn)
            store.type = Bytecode::STORE;
        else
            store.type = Bytecode::SETREF;
        findVariable(moduleId, id, var, store.scope, store.arg);
        argHandle << store;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        argHandle << pop;
    }

    for (int i=0; i<alg->header.arguments.size(); i++) {
        const AST::VariablePtr  var = alg->header.arguments[i];
        if (var->dimension>0) {
            for (int i=var->dimension-1; i>=0; i--) {
                argHandle << calculate(moduleId, id, 0, var->bounds[i].second);
                argHandle << calculate(moduleId, id, 0, var->bounds[i].first);
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::UPDARR;
            findVariable(moduleId, id, var, bounds.scope, bounds.arg);
            argHandle << bounds;
        }
        if (var->accessType==AST::AccessArgumentOut) {
            Bytecode::Instruction init;
            init.type = Bytecode::INIT;
            findVariable(moduleId, id, var, init.scope, init.arg);
            argHandle << init;
        }
    }

    if (alg->impl.beginLexems.size()) {
        argHandle += makeLineInstructions(alg->impl.beginLexems);
    }

    if (beginError.length()>0) {
        Bytecode::Instruction err;
        err.type = Bytecode::ERRORR;
        err.scope = Bytecode::CONSTT;
        err.arg = constantValue(Bytecode::VT_string, 0, beginError, QString(), QString());
        argHandle << err;
    }

    if (debugLevel_==GeneratorInterface::LinesAndVariables)
        argHandle << ctlOff;

    QList<Bytecode::Instruction> pre = instructions(moduleId, id, 0, alg->impl.pre);
    QList<Bytecode::Instruction> body = instructions(moduleId, id, 0, alg->impl.body);
    QList<Bytecode::Instruction> post = instructions(moduleId, id, 0, alg->impl.post);

    shiftInstructions(pre, argHandle.size());

    int offset = argHandle.size() + pre.size();
    shiftInstructions(body, offset);
    offset += body.size();
    shiftInstructions(post, offset);

    QList<Bytecode::Instruction> ret;

    int retIp = argHandle.size() + pre.size() + body.size() + post.size();

    setBreakAddress(body, 0, retIp);


    ret += makeLineInstructions(alg->impl.endLexems);

    if (alg->impl.endLexems.size()>0) {
        QString endError;
        for (int i=0; i<alg->impl.endLexems.size();i++) {
            if (alg->impl.endLexems[i]->error.size()>0) {
                endError = ErrorMessages::message("KumirAnalizer", QLocale::Russian, alg->impl.endLexems[i]->error);
                break;
            }
        }
        if (endError.length()>0) {
            Bytecode::Instruction err;
            err.type = Bytecode::ERRORR;
            err.scope = Bytecode::CONSTT;
            err.arg = constantValue(Bytecode::VT_string, 0, endError, QString(), QString());
            ret << err;
        }
    }


    const AST::VariablePtr  retval = returnValue(alg);
    if (retval) {
        Bytecode::Instruction loadRetval;
        loadRetval.type = Bytecode::LOAD;
        findVariable(moduleId, id, retval, loadRetval.scope, loadRetval.arg);
        ret << loadRetval;

    }

    Bytecode::Instruction retturn;
    retturn.type = Bytecode::RET;
    ret << retturn;

    QList<Bytecode::Instruction> instrs = argHandle + pre + body + post + ret;
    func.instructions = instrs.toVector().toStdVector();
    byteCode_->d.push_back(func);
}

QList<Bytecode::Instruction> Generator::instructions(
    int modId, int algId, int level,
    const QList<AST::StatementPtr > &statements)
{
    QList<Bytecode::Instruction> result;
    for (int i=0; i<statements.size(); i++) {
        const AST::StatementPtr  st = statements[i];
        switch (st->type) {
        case AST::StError:
            if (!st->skipErrorEvaluation)
                ERRORR(modId, algId, level, st, result);
            break;
        case AST::StAssign:
            ASSIGN(modId, algId, level, st, result);
            break;
        case AST::StAssert:
            ASSERT(modId, algId, level, st, result);
            break;
        case AST::StVarInitialize:
            INIT(modId, algId, level, st, result);
            break;
        case AST::StInput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StOutput:
            CALL_SPECIAL(modId, algId, level, st, result);
            break;
        case AST::StLoop:
            LOOP(modId, algId, level+1, st, result);
            break;
        case AST::StIfThenElse:
            IFTHENELSE(modId, algId, level, st, result);
            break;
        case AST::StSwitchCaseElse:
            SWITCHCASEELSE(modId, algId, level, st, result);
            break;
        case AST::StBreak:
            BREAK(modId, algId, level, st, result);
            break;
        case AST::StPause:
            PAUSE_STOP(modId, algId, level, st, result);
            break;
        case AST::StHalt:
            PAUSE_STOP(modId, algId, level, st, result);
            break;
        default:
            break;
        }
    }
    return result;
}

quint16 Generator::constantValue(Bytecode::ValueType type, quint8 dimension, const QVariant &value,
                                 const QString & moduleName, const QString & className)
{
    QList<Bytecode::ValueType> vt;
    vt.push_back(type);
    return constantValue(vt, dimension, value, QString(), QString());
}

quint16 Generator::constantValue(const QList<Bytecode::ValueType> & type, quint8 dimension, const QVariant &value,
                                 const QString & moduleName, const QString & className
                                 )
{
    ConstValue c;
    c.baseType = type;
    c.dimension = dimension;
    c.value = value;
    c.recordModuleName = moduleName;
    c.recordClassLocalizedName = className;
    if (!constants_.contains(c)) {
        constants_ << c;
        return constants_.size()-1;
    }
    else {
        return constants_.indexOf(c);
    }
}

void Generator::ERRORR(int , int , int , const AST::StatementPtr  st, QList<Bytecode::Instruction>  & result)
{
    result += makeLineInstructions(st->lexems);
    const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->error);
    Bytecode::Instruction e;
    e.type = Bytecode::ERRORR;
    e.scope = Bytecode::CONSTT;
    e.arg = constantValue(Bytecode::VT_string, 0, error, QString(), QString());
    result << e;
}

void Generator::findVariable(int modId, int algId, const AST::VariablePtr  var, Bytecode::VariableScope & scope, quint16 & id) const
{
    const AST::ModulePtr  mod = ast_->modules.at(modId);
    for (quint16 i=0; i<mod->impl.globals.size(); i++) {
        if (mod->impl.globals.at(i)==var) {
            scope = Bytecode::GLOBAL;
            id = i;
            return;
        }
    }
    const AST::AlgorithmPtr  alg = mod->impl.algorhitms[algId];
    for (quint16 i=0; i<alg->impl.locals.size(); i++) {
        if (alg->impl.locals.at(i)==var) {
            scope = Bytecode::LOCAL;
            id = i;
            return;
        }
    }

}

void Generator::findFunction(const AST::AlgorithmPtr alg, quint8 &module, quint16 &id)
{
    for (quint8 i=0; i<ast_->modules.size(); i++) {
        const AST::ModulePtr  mod = ast_->modules[i];
        QList<AST::AlgorithmPtr> table;
        if (mod->header.type==AST::ModTypeExternal || mod->header.type==AST::ModTypeCached)
            table = mod->header.algorhitms + mod->header.operators;
        else
            table = mod->impl.algorhitms + mod->header.operators;
        for (quint16 j=0; j<table.size(); j++) {
            if (alg==table[j]) {
                module = i;
                id = j;
                if (mod->header.type == AST::ModTypeExternal) {
                    id = alg->header.external.id;
                }
                if (mod->header.type==AST::ModTypeCached ||
                        (mod->header.type==AST::ModTypeExternal && (mod->builtInID & 0xF0) == 0) )
                {
                    QPair<quint8,quint16> ext(module, id);
                    if (!externs_.contains(ext))
                        externs_ << ext;
                }
                if (mod->builtInID)
                    module = mod->builtInID;
                return;
            }
        }
    }
}

QList<Bytecode::Instruction> Generator::makeLineInstructions(const QList<AST::LexemPtr> &lexems) const
{
    QList<Bytecode::Instruction> result;
    if (debugLevel_ != GeneratorInterface::NoDebug) {
        Bytecode::Instruction lineNoInstruction, lineColInstruction;
        lineNoInstruction.type = lineColInstruction.type = Bytecode::LINE;
        lineNoInstruction.lineSpec = Bytecode::LINE_NUMBER;
        if (lexems.size() > 0 && lexems.first()->lineNo != -1) {
            AST::LexemPtr first = lexems.first();
            AST::LexemPtr last = first;
            foreach (AST::LexemPtr lx, lexems) {
                if (lx->type != Shared::LxTypeComment)
                    last = lx;
            }
            quint16 lineNo = first->lineNo;
            lineNoInstruction.arg = lineNo;
            quint32 colStart = first->linePos;
            quint32 colEnd = last->linePos + last->data.length();
            if (last->type == Shared::LxConstLiteral)
                colEnd += 2;  // two quote symbols are not in lexem data
            Bytecode::setColumnPositionsToLineInstruction(lineColInstruction, colStart, colEnd);
            result << lineNoInstruction << lineColInstruction;
        }
    }
    return result;
}

void Generator::ASSIGN(int modId, int algId, int level, const AST::StatementPtr st, QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    const AST::ExpressionPtr rvalue = st->expressions[0];
    QList<Bytecode::Instruction> rvalueInstructions = calculate(modId, algId, level, rvalue);
    shiftInstructions(rvalueInstructions, result.size());
    result << rvalueInstructions;


    if (st->expressions.size()>1) {
        const AST::ExpressionPtr  lvalue = st->expressions[1];

        int diff = lvalue->operands.size()-lvalue->variable->dimension;

        if (diff>0) {
            // Load source string
            Bytecode::Instruction load;
            findVariable(modId, algId, lvalue->variable, load.scope, load.arg);
            load.type = lvalue->variable->dimension>0? Bytecode::LOADARR : Bytecode::LOAD;
            for (int i=lvalue->variable->dimension-1; i>=0 ;i--) {
                result << calculate(modId, algId, level, lvalue->operands[i]);
            }
            result << load;
        }

        if (diff==1) {
            // Set character

            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-1]);
            Bytecode::Instruction argsCount;
            argsCount.type = Bytecode::LOAD;
            argsCount.scope = Bytecode::CONSTT;
            argsCount.arg = constantValue(Bytecode::VT_int, 0, 3, QString(), QString());
            result << argsCount;

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xff;
            call.arg = 0x05;
            result << call;
        }

        if (diff==2) {
            // Set slice

            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-2]);
            result << calculate(modId, algId, level,
                                lvalue->operands[lvalue->operands.count()-1]);
            Bytecode::Instruction argsCount;
            argsCount.type = Bytecode::LOAD;
            argsCount.scope = Bytecode::CONSTT;
            argsCount.arg = constantValue(Bytecode::VT_int, 0, 4, QString(), QString());
            result << argsCount;

            Bytecode::Instruction call;
            call.type = Bytecode::CALL;
            call.module = 0xff;
            call.arg = 0x07;
            result << call;
        }

        Bytecode::Instruction store;
        findVariable(modId, algId, lvalue->variable, store.scope, store.arg);
        store.type = lvalue->variable->dimension>0? Bytecode::STOREARR : Bytecode::STORE;
        if (lvalue->kind==AST::ExprArrayElement) {
            for (int i=lvalue->variable->dimension-1; i>=0 ;i--) {
                result << calculate(modId, algId, level, lvalue->operands[i]);
            }
        }

        result << store;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        result << pop;
    }
}

QList<Bytecode::Instruction> Generator::calculate(int modId, int algId, int level, const AST::ExpressionPtr st)
{
    QList<Bytecode::Instruction> result;
    if (st->useFromCache) {
        Bytecode::Instruction instr;
        memset(&instr, 0, sizeof(Bytecode::Instruction));
        instr.type = Bytecode::CLOAD;
        result << instr;
    }
    else if (st->kind==AST::ExprConst) {
        int constId = constantValue(valueType(st->baseType), st->dimension, st->constant,
                                    st->baseType.actor ? st->baseType.actor->localizedModuleName(QLocale::Russian) : "",
                                    st->baseType.name
                                    );
        Bytecode::Instruction instr;
        instr.type = Bytecode::LOAD;
        instr.scope = Bytecode::CONSTT;
        instr.arg = constId;
        result << instr;
    }
    else if (st->kind==AST::ExprVariable) {
        Bytecode::Instruction instr;
        instr.type = Bytecode::LOAD;
        findVariable(modId, algId, st->variable, instr.scope, instr.arg);
        result << instr;
    }
    else if (st->kind==AST::ExprArrayElement) {

        Bytecode::Instruction instr;
        findVariable(modId, algId, st->variable, instr.scope, instr.arg);
        instr.type = Bytecode::LOAD;
        if (st->variable->dimension>0) {
            for (int i=st->variable->dimension-1; i>=0; i--) {
                result << calculate(modId, algId, level, st->operands[i]);
            }
            instr.type = Bytecode::LOADARR;
        }
        result << instr;
        int diff = st->operands.size() - st->variable->dimension;
        Bytecode::Instruction argsCount;
        argsCount.type = Bytecode::LOAD;
        argsCount.scope = Bytecode::CONSTT;
        Bytecode::Instruction specialFunction;
        specialFunction.type = Bytecode::CALL;
        specialFunction.module = 0xff;
        if (diff==1) {
            // Get char
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-1]);
            argsCount.arg = constantValue(Bytecode::VT_int, 0, 2, QString(), QString());
            result << argsCount;
            specialFunction.arg = 0x04;
            result << specialFunction;
        }
        else if (diff==2) {
            // Get slice
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-2]);
            result << calculate(modId, algId, level,
                                st->operands[st->operands.count()-1]);
            argsCount.arg = constantValue(Bytecode::VT_int, 0, 3, QString(), QString());
            result << argsCount;
            specialFunction.arg = 0x06;
            result << specialFunction;
        }
    }
    else if (st->kind==AST::ExprFunctionCall) {
        const AST::AlgorithmPtr  alg = st->function;

        // Push calculable arguments to stack
        int argsCount = 0;
        for (int i=0; i<st->operands.size(); i++) {
            AST::VariableAccessType t = alg->header.arguments[i]->accessType;
            bool arr = alg->header.arguments[i]->dimension>0;
            if (t==AST::AccessArgumentOut||t==AST::AccessArgumentInOut) {
                Bytecode::Instruction ref;
                ref.type = Bytecode::REF;
                findVariable(modId, algId, st->operands[i]->variable, ref.scope, ref.arg);
                result << ref;
            }
            else if (t==AST::AccessArgumentIn && !arr)
                result << calculate(modId, algId, level, st->operands[i]);
            else if (t==AST::AccessArgumentIn && arr) {
                // load the whole array into stack
                Bytecode::Instruction load;
                load.type = Bytecode::LOAD;
                findVariable(modId, algId, st->operands[i]->variable, load.scope, load.arg);
                result << load;
            }
            argsCount ++;
        }
        Bytecode::Instruction b;
        b.type = Bytecode::LOAD;
        b.scope = Bytecode::CONSTT;
        b.arg = constantValue(Bytecode::VT_int, 0, argsCount, QString(), QString());
        result << b;


        Bytecode::Instruction instr;
        instr.type = Bytecode::CALL;
        findFunction(st->function, instr.module, instr.arg);
        result << instr;
    }
    else if (st->kind==AST::ExprSubexpression) {
        std::list<int> jmps;
        for (int i=0; i<st->operands.size(); i++) {
            QList<Bytecode::Instruction> operandInstrs = calculate(modId, algId, level, st->operands[i]);
            shiftInstructions(operandInstrs, result.size());
            result << operandInstrs;
            // Drop cached value in case of compare-chains calculations and Z-flag
            if (st->operands[i]->clearCacheOnFailure) {
                Bytecode::Instruction clearCacheOnZ;
                memset(&clearCacheOnZ, 0, sizeof(Bytecode::Instruction));
                clearCacheOnZ.type = Bytecode::CDROPZ;
                result << clearCacheOnZ;
            }
            // Do short circuit calculation for AND and OR operations
            if (i==0 && (st->operatorr==AST::OpAnd || st->operatorr==AST::OpOr)) {
                // Simple case: just JZ/JNZ to end
                Bytecode::Instruction gotoEnd;
                gotoEnd.registerr = 0;
                gotoEnd.type = st->operatorr==AST::OpAnd? Bytecode::JZ : Bytecode::JNZ;
                jmps.push_back(result.size());
                result << gotoEnd;
            }
            else if (st->operatorr==AST::OpAnd || st->operatorr==AST::OpOr) {
                // We must remove pre-previous value from stack
                Bytecode::Instruction gotoCheckNext;
                gotoCheckNext.registerr = 0;
                gotoCheckNext.arg = result.size()+5;
                gotoCheckNext.type = st->operatorr==AST::OpAnd? Bytecode::JNZ : Bytecode::JZ;
                result << gotoCheckNext;
                Bytecode::Instruction pop;
                pop.registerr = 0;
                pop.type = Bytecode::POP;
                result << pop << pop;
                Bytecode::Instruction load;
                load.type = Bytecode::LOAD;
                load.scope = Bytecode::CONSTT;
                load.arg = constantValue(Bytecode::VT_bool, 0,
                                         st->operatorr==AST::OpAnd
                                         ? false
                                         : true
                                           , QString(), QString()
                                         );
                result << load;
                jmps.push_back(result.size());
                Bytecode::Instruction gotoEnd;
                gotoEnd.type = Bytecode::JUMP;
                result << gotoEnd;
            }
        }
        Bytecode::Instruction instr;
        instr.type = operation(st->operatorr);
        if (st->operatorr==AST::OpSubstract && st->operands.size()==1) {
            instr.type = Bytecode::NEG;
        }
        result << instr;
        for (std::list<int>::iterator it=jmps.begin(); it!=jmps.end(); it++) {
            int index = *it;
            result[index].arg = result.size();
        }
    }


    if (st->keepInCache) {
        Bytecode::Instruction instr;
        memset(&instr, 0, sizeof(Bytecode::Instruction));
        instr.type = Bytecode::CSTORE;
        result << instr;
    }


    return result;
}

void Generator::PAUSE_STOP(int , int , int , const AST::StatementPtr  st, QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    Bytecode::Instruction a;
    a.type = st->type==AST::StPause? Bytecode::PAUSE : Bytecode::HALT;
    a.arg = 0u;
    result << a;
}

void Generator::ASSERT(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    for (int i=0; i<st->expressions.size(); i++) {
        QList<Bytecode::Instruction> exprInstrs;
        exprInstrs = calculate(modId, algId, level, st->expressions[i]);
        shiftInstructions(exprInstrs, result.size());
        result << exprInstrs;
        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        result << pop;
        Bytecode::Instruction showreg;
        showreg.type = Bytecode::SHOWREG;
        showreg.registerr = pop.registerr;
        result << showreg;
        int ip = result.size(); // pointing to next of calculation (i.e. JNZ instruction)
        int targetIp = ip + 2;
        Bytecode::Instruction jnz;
        jnz.type = Bytecode::JNZ;
        jnz.registerr = 0;
        jnz.arg = targetIp;
        result << jnz;
        Bytecode::Instruction err;
        err.type = Bytecode::ERRORR;
        err.scope = Bytecode::CONSTT;
        err.arg = constantValue(Bytecode::VT_string, 0, tr("Assertion false"), QString(), QString());
        result << err;
    }
}

void Generator::INIT(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    for (int i=0; i<st->variables.size(); i++) {
        const AST::VariablePtr  var = st->variables[i];
        if (var->dimension > 0 && var->bounds.size()>0) {
            for (int i=var->dimension-1; i>=0 ; i--) {
                result << calculate(modId, algId, level, var->bounds[i].second);
                result << calculate(modId, algId, level, var->bounds[i].first);
            }
            Bytecode::Instruction bounds;
            bounds.type = Bytecode::SETARR;
            findVariable(modId, algId, var, bounds.scope, bounds.arg);
            result << bounds;
        }
        else if (var->dimension > 0 && var->bounds.size()==0) {
            // TODO : Implement me after compiler support
        }
        Bytecode::Instruction init;
        init.type = Bytecode::INIT;
        findVariable(modId, algId, var, init.scope, init.arg);
        result << init;
        if (var->initialValue.isValid()) {
            Bytecode::Instruction load;
            load.type = Bytecode::LOAD;
            load.scope = Bytecode::CONSTT;
            load.arg = constantValue(valueType(var->baseType), var->dimension, var->initialValue,
                                     var->baseType.actor ? var->baseType.actor->localizedModuleName(QLocale::Russian) : "",
                                     var->baseType.name
                                     );
            result << load;
            Bytecode::Instruction store = init;
            store.type = Bytecode::STORE;
            result << store;
            Bytecode::Instruction pop;
            pop.type = Bytecode::POP;
            pop.registerr = 0;
            result << pop;
        }
        else {
            // TODO constant values for dimension > 0
        }
    }
}

void Generator::CALL_SPECIAL(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    quint16 argsCount;


    if (st->type==AST::StOutput) {
        int varsCount = st->expressions.size() / 3;

        for (int i = 0; i<varsCount; ++i) {
            const AST::ExpressionPtr  expr = st->expressions[3*i];
            const AST::ExpressionPtr  format1 = st->expressions[3*i+1];
            const AST::ExpressionPtr  format2 = st->expressions[3*i+2];
            QList<Bytecode::Instruction> instrs;
            instrs = calculate(modId, algId, level, expr);
            shiftInstructions(instrs, result.size());
            result << instrs;

            instrs = calculate(modId, algId, level, format1);
            shiftInstructions(instrs, result.size());
            result << instrs;

            instrs = calculate(modId, algId, level, format2);
            shiftInstructions(instrs, result.size());
            result << instrs;
        }

        if (st->expressions.size() % 3) {
            // File handle
            QList<Bytecode::Instruction> instrs = calculate(modId, algId, level, st->expressions.last());
            shiftInstructions(instrs, result.size());
            result << instrs;
        }

        argsCount = st->expressions.size();
    }
    else if (st->type==AST::StInput) {
        int varsCount = st->expressions.size();
        for (int i = varsCount-1; i>=0; i--) {
            const AST::ExpressionPtr  varExpr = st->expressions[i];
            Bytecode::Instruction ref;
            if (varExpr->kind==AST::ExprConst) {
                ref.scope = Bytecode::CONSTT;
                ref.arg = constantValue(valueType(varExpr->baseType), 0, varExpr->constant,
                                        varExpr->baseType.actor ? varExpr->baseType.actor->localizedModuleName(QLocale::Russian) : "",
                                        varExpr->baseType.name
                                        );
            }
            else {
                findVariable(modId, algId, varExpr->variable, ref.scope, ref.arg);
            }
            if (varExpr->kind==AST::ExprVariable || varExpr->kind==AST::ExprConst) {
                ref.type = Bytecode::REF;
                result << ref;
            }
            else if (varExpr->kind == AST::ExprArrayElement) {
                ref.type = Bytecode::REFARR;
                for (int j=varExpr->operands.size()-1; j>=0; j--) {
                    QList<Bytecode::Instruction> operandInstrs = calculate(modId, algId, level, varExpr->operands[j]);
                    shiftInstructions(operandInstrs, result.size());
                    result << operandInstrs;
                }
                result << ref;
            }
            else {
                QList<Bytecode::Instruction> operandInstrs = calculate(modId, algId, level, varExpr);
                shiftInstructions(operandInstrs, result.size());
                result << operandInstrs;
            }

        }
        argsCount = st->expressions.size();
    }

    Bytecode::Instruction pushCount;
    pushCount.type = Bytecode::LOAD;
    pushCount.scope = Bytecode::CONSTT;
    pushCount.arg = constantValue(Bytecode::VT_int, 0, argsCount, QString(), QString());
    result << pushCount;

    Bytecode::Instruction call;
    call.type = Bytecode::CALL;
    call.module = 0xFF;
    if (st->type==AST::StInput)
        call.arg = 0x0000;
    else if (st->type==AST::StOutput)
        call.arg = 0x0001;

    result << call;
}


void Generator::IFTHENELSE(int modId, int algId, int level, const AST::StatementPtr  st, QList<Bytecode::Instruction> &result)
{
    int jzIP = -1;
    result += makeLineInstructions(st->lexems);

    if (st->conditionals[0].condition) {
        QList<Bytecode::Instruction> conditionInstructions = calculate(modId, algId, level, st->conditionals[0].condition);
        shiftInstructions(conditionInstructions, result.size());
        result << conditionInstructions;

        Bytecode::Instruction pop;
        pop.type = Bytecode::POP;
        pop.registerr = 0;
        result << pop;

        Bytecode::Instruction showreg;
        showreg.type = Bytecode::SHOWREG;
        showreg.registerr = 0;
        result << showreg;


        if (st->headerError.size()>0) {
            Bytecode::Instruction garbage;
            garbage.type = Bytecode::LINE;
            garbage.arg = st->headerErrorLine;
            result << garbage;
            Bytecode::setColumnPositionsToLineInstruction(garbage, 0u, 0u);
            result << garbage;
            garbage.type = Bytecode::ERRORR;
            garbage.scope = Bytecode::CONSTT;
            garbage.arg = constantValue(Bytecode::VT_string, 0,
                                        ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->headerError)
                                        , QString(), QString()
                                        );
            result << garbage;
        }

        jzIP = result.size();
        Bytecode::Instruction jz;
        jz.type = Bytecode::JZ;
        jz.registerr = 0;
        result << jz;
    }


    Bytecode::Instruction error;
    if (st->conditionals[0].conditionError.size()>0) {        
        if (st->conditionals[0].lexems.isEmpty()) {
            result += makeLineInstructions(st->lexems);
        }
        else {
            result += makeLineInstructions(st->conditionals[0].lexems);
        }
        const QString msg = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->conditionals[0].conditionError);
        error.type = Bytecode::ERRORR;
        error.scope = Bytecode::CONSTT;
        error.arg = constantValue(Bytecode::VT_string, 0, msg, QString(), QString());
        result << error;
    }
    else {
        QList<Bytecode::Instruction> thenInstrs = instructions(modId, algId, level, st->conditionals[0].body);
        shiftInstructions(thenInstrs, result.size());
        result += thenInstrs;
    }

    if (jzIP!=-1)
        result[jzIP].arg = result.size();

    if (st->conditionals.size()>1) {
        int jumpIp = result.size();
        Bytecode::Instruction jump;
        jump.type = Bytecode::JUMP;
        result << jump;
        result[jzIP].arg = result.size();
        if (st->conditionals[1].conditionError.size()>0) {
            const QString msg = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->conditionals[1].conditionError);
            if (st->conditionals[1].lexems.isEmpty()) {
                result += makeLineInstructions(st->lexems);
            }
            else {
                result += makeLineInstructions(st->conditionals[1].lexems);
            }
            error.type = Bytecode::ERRORR;
            error.scope = Bytecode::CONSTT;
            error.arg = constantValue(Bytecode::VT_string, 0, msg, QString(), QString());
            result << error;
        }
        else {
            QList<Bytecode::Instruction> elseInstrs = instructions(modId, algId, level, st->conditionals[1].body);
            shiftInstructions(elseInstrs, result.size());
            result += elseInstrs;
        }
        result[jumpIp].arg = result.size();
    }

    if (st->endBlockError.size()>0) {
        const QString msg = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->endBlockError);
        if (st->conditionals[0].lexems.isEmpty()) {
            result += makeLineInstructions(st->lexems);
        }
        else {
            result += makeLineInstructions(st->endBlockLexems);
        }
        error.type = Bytecode::ERRORR;
        error.scope = Bytecode::CONSTT;
        error.arg = constantValue(Bytecode::VT_string, 0, msg, QString(), QString());
        result << error;
    }

}

void Generator::SWITCHCASEELSE(int modId, int algId, int level, const AST::StatementPtr st, QList<Bytecode::Instruction> & result)
{
    if (st->headerError.size()>0) {
        Bytecode::Instruction garbage;
        garbage.type = Bytecode::LINE;
        garbage.arg = st->headerErrorLine;
        result << garbage;
        Bytecode::setColumnPositionsToLineInstruction(garbage, 0u, 0u);
        result << garbage;
        garbage.type = Bytecode::ERRORR;
        garbage.scope = Bytecode::CONSTT;
        garbage.arg = constantValue(Bytecode::VT_string, 0,
                                    ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->headerError)
                                    , QString(), QString()
                                    );
        result << garbage;
        return;
    }

    if (st->beginBlockError.size()>0) {
        const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->beginBlockError);
        Bytecode::Instruction err;
        err.type = Bytecode::ERRORR;
        err.scope = Bytecode::CONSTT;
        err.arg = constantValue(Bytecode::VT_string, 0, error, QString(), QString());
        result << err;
        return;
    }

    int lastJzIp = -1;
    QList<int> jumpIps;



    for (int i=0; i<st->conditionals.size(); i++) {
        if (lastJzIp!=-1) {
            result[lastJzIp].arg = result.size();
            lastJzIp = -1;
        }
        result += makeLineInstructions(st->conditionals[i].lexems);
        if (!st->conditionals[i].conditionError.isEmpty()) {
            const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->conditionals[i].conditionError);
            Bytecode::Instruction err;
            err.type = Bytecode::ERRORR;
            err.scope = Bytecode::CONSTT;
            err.arg = constantValue(Bytecode::VT_string, 0, error, QString(), QString());
            result << err;
        }
        else {
            if (st->conditionals[i].condition) {
                QList<Bytecode::Instruction> condInstrs = calculate(modId, algId, level, st->conditionals[i].condition);
                shiftInstructions(condInstrs, result.size());
                result << condInstrs;
                Bytecode::Instruction pop;
                pop.type = Bytecode::POP;
                pop.registerr = 0;
                result << pop;
                Bytecode::Instruction showreg;
                showreg.type = Bytecode::SHOWREG;
                showreg.registerr = 0;
                result << showreg;
                Bytecode::Instruction jz;
                jz.type = Bytecode::JZ;
                jz.registerr = 0;
                lastJzIp = result.size();
                result << jz;
            }
            QList<Bytecode::Instruction> instrs = instructions(modId, algId, level, st->conditionals[i].body);
            shiftInstructions(instrs, result.size());
            result += instrs;
            if (i<st->conditionals.size()-1) {
                Bytecode::Instruction jump;
                jump.type = Bytecode::JUMP;
                jumpIps << result.size();
                result << jump;
            }
        }
    }
    if (lastJzIp!=-1)
        result[lastJzIp].arg = result.size();
    for (int i=0; i<jumpIps.size(); i++) {
        result[jumpIps[i]].arg = result.size();
    }
}

const AST::VariablePtr  Generator::returnValue(const AST::AlgorithmPtr  alg)
{
    const QString name = alg->header.name;
    for (int i=0; i<alg->impl.locals.size(); i++) {
        if (alg->impl.locals[i]->name == name)
            return alg->impl.locals[i];
    }
    return AST::VariablePtr();
}

void Generator::BREAK(int , int , int level,
                      const AST::StatementPtr  st,
                      QList<Bytecode::Instruction> & result)
{
    result += makeLineInstructions(st->lexems);

    Bytecode::Instruction jump;
//    jump.type = Bytecode::JUMP;
    jump.type = Bytecode::InstructionType(127);
    jump.registerr = level;

    result << jump;
}

void Generator::LOOP(int modId, int algId,
                     int level,
                     const AST::StatementPtr st,
                     QList<Bytecode::Instruction> &result)
{
    Bytecode::Instruction ctlOn;
    ctlOn.module = 0x00;
    ctlOn.arg = 0x0001;

    Bytecode::Instruction ctlOff;
    ctlOff.module = 0x00;
    ctlOff.arg = 0x0000;

    ctlOn.type = ctlOff.type = Bytecode::CTL;

    if (st->beginBlockError.size()>0) {
        const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->beginBlockError);
        result += makeLineInstructions(st->lexems);
        Bytecode::Instruction err;
        err.type = Bytecode::ERRORR;
        err.scope = Bytecode::CONSTT;
        err.arg = constantValue(Bytecode::VT_string, 0, error, QString(), QString());
        result << err;
        return;
    }

    Bytecode::Instruction swreg;
    swreg.type = Bytecode::SHOWREG;
    swreg.registerr = level * 5;

    Bytecode::Instruction clmarg;
    if (st->loop.endLexems.size()>0 && debugLevel_==GeneratorInterface::LinesAndVariables) {
        clmarg.type = Bytecode::CLEARMARG;
        clmarg.arg = st->loop.endLexems[0]->lineNo;
    }

    int beginIp = result.size();
    int jzIp = -1;


    if (st->loop.type==AST::LoopWhile || st->loop.type==AST::LoopForever) {
        // Highlight line and clear margin
        result += makeLineInstructions(st->lexems);

        if (st->loop.whileCondition) {
            // Calculate condition
            QList<Bytecode::Instruction> whileCondInstructions = calculate(modId, algId, level, st->loop.whileCondition);
            shiftInstructions(whileCondInstructions, result.size());
            result << whileCondInstructions;

            // Check condition result
            Bytecode::Instruction a;
            a.type = Bytecode::POP;
            a.registerr = level * 5;
            result << a;

            if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                    debugLevel_==GeneratorInterface::LinesAndVariables)
            {
                if (st->loop.type==AST::LoopWhile) {
                    // Show condition at margin only in case if
                    // condition if typed by user
                    result << swreg;
                }
            }

            jzIp = result.size();
            a.type = Bytecode::JZ;
            a.registerr = level * 5;
            result << a;

            if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                    debugLevel_==GeneratorInterface::LinesAndVariables)
            {
                result << clmarg;
                result << swreg;
            }
        }
        else {

            if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                    debugLevel_==GeneratorInterface::LinesAndVariables) {
                result << clmarg;
            }
        }

    }
    else if (st->loop.type==AST::LoopTimes) {
        // Highlight line
        result += makeLineInstructions(st->lexems);

        // Calculate times value
        QList<Bytecode::Instruction> timesValueInstructions = calculate(modId, algId, level, st->loop.timesValue);
        shiftInstructions(timesValueInstructions, result.size());
        result << timesValueInstructions;
        Bytecode::Instruction a;

        // Store value in register
        a.type = Bytecode::POP;
        a.registerr = level * 5 - 1;
        result << a;

        // Store initial value "0" in nearest register
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONSTT;
        a.arg = constantValue(Bytecode::VT_int, 0, 0, QString(), QString());
        result << a;

        a.type = Bytecode::POP;
        a.registerr = level * 5;
        result << a;


        // Make begin
        beginIp = result.size();

        // Highlight line and clear margin

        result += makeLineInstructions(st->lexems);
        if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                 debugLevel_==GeneratorInterface::LinesAndVariables) {
            result << clmarg;
        }

        // Increase value in register
        a.type = Bytecode::PUSH;
        a.registerr = level * 5;
        result << a;
        a.type = Bytecode::LOAD;
        a.scope = Bytecode::CONSTT;
        a.arg = constantValue(Bytecode::VT_int, 0, 1, QString(), QString());
        result << a;
        a.type = Bytecode::SUM;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = level * 5;
        result << a;
        a.type = Bytecode::PUSH;
        a.registerr = level * 5;
        result << a;

        // Compare value to "times" value
        a.type = Bytecode::PUSH;
        a.registerr = level * 5 - 1;
        result << a;
        a.type = Bytecode::GT;
        result << a;
        a.type = Bytecode::POP;
        a.registerr = 0;
        result << a;

        // Check if register value > 0 (i.e. "counter" > "times")
        jzIp = result.size();
        a.type = Bytecode::JNZ;
        a.registerr = 0;
        result << a;

        // Show counter value at margin
        if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                 debugLevel_==GeneratorInterface::LinesAndVariables) {
           result << swreg;
        }
    }
    else if (st->loop.type==AST::LoopFor) {

        // Highlight line
        result += makeLineInstructions(st->lexems);

        // Calculate 'from'-value
        result << calculate(modId, algId, level, st->loop.fromValue);

        Bytecode::Instruction popFrom;
        popFrom.type = Bytecode::POP;
        popFrom.registerr = level * 5 - 2;
        result << popFrom;

        Bytecode::Instruction pushFrom;
        pushFrom.type = Bytecode::PUSH;
        pushFrom.registerr = popFrom.registerr;


        // First time: Load 'to'-value and store it in register
        result << calculate(modId, algId, level, st->loop.toValue);

        Bytecode::Instruction popTo;
        popTo.type = Bytecode::POP;
        popTo.registerr = level * 5 - 3;
        result << popTo;

        Bytecode::Instruction pushTo;
        pushTo.type = Bytecode::PUSH;
        pushTo.registerr = popTo.registerr;

        // First time: Load 'step'-value and store it in register
        if (st->loop.stepValue) {
            result << calculate(modId, algId, level, st->loop.stepValue);
        }
        else {
            Bytecode::Instruction loadOneStep;
            loadOneStep.type = Bytecode::LOAD;
            loadOneStep.scope = Bytecode::CONSTT;
            loadOneStep.arg = constantValue(Bytecode::VT_int, 0, 1, QString(), QString());
            result << loadOneStep;
        }

        Bytecode::Instruction popStep;
        popStep.type = Bytecode::POP;
        popStep.registerr = level * 5 - 4;
        result << popStep;

        Bytecode::Instruction pushStep;
        pushStep.type = Bytecode::PUSH;
        pushStep.registerr = level * 5 - 4;


        // First time: decrease value by 'step', so in will
        // be increased in nearest future

        Bytecode::Instruction subInitial;
        subInitial.type = Bytecode::SUB;
        result << pushFrom << pushStep << subInitial;

        Bytecode::Instruction popCurrent;
        popCurrent.type = Bytecode::POP;
        popCurrent.registerr = level * 5;

        Bytecode::Instruction pushCurrent;
        pushCurrent.type = Bytecode::PUSH;
        pushCurrent.registerr = popCurrent.registerr;

        result << popCurrent;

        Bytecode::Instruction popVoid;
        popVoid.type = Bytecode::POP;
        popVoid.registerr = 0;

        // Begin
        beginIp = result.size();

        // Begin of loop -- check if loop variable in corresponding range
        //    a) push variables for INRANGE
        result << pushStep << pushFrom << pushTo;
        //    b) calculate current value and store into variable
        result << pushCurrent << pushStep;
        Bytecode::Instruction sum;
        sum.type = Bytecode::SUM;
        result << sum;
        result << popCurrent << pushCurrent;
        //    c) check if variable in range

        Bytecode::Instruction inRange;
        inRange.type = Bytecode::INRANGE;
        result << inRange;

        Bytecode::Instruction gotoEnd;
        gotoEnd.type = Bytecode::JZ;
        gotoEnd.registerr = 0;
        jzIp = result.size();
        result << gotoEnd;

        // Clear margin
        result += makeLineInstructions(st->lexems);

        if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                debugLevel_==GeneratorInterface::LinesAndVariables) {
            result << clmarg;
        }

        // Set variable to current loop value
        Bytecode::Instruction setVariableValue;
        setVariableValue.type = Bytecode::STORE;
        findVariable(modId, algId, st->loop.forVariable, setVariableValue.scope, setVariableValue.arg);
        result << pushCurrent << setVariableValue << popVoid;
    }

    QList<Bytecode::Instruction> instrs = instructions(modId, algId, level, st->loop.body);
    shiftInstructions(instrs, result.size());
    result += instrs;

    bool endsWithError = st->endBlockError.length()>0;
    if (endsWithError) {
        const QString error = ErrorMessages::message("KumirAnalizer", QLocale::Russian, st->endBlockError);
        result += makeLineInstructions(st->loop.endLexems);
        Bytecode::Instruction ee;
        ee.type = Bytecode::ERRORR;
        ee.scope = Bytecode::CONSTT;
        ee.arg = constantValue(Bytecode::VT_string, 0, error, QString(), QString());
        result << ee;
        return;
    }

    int endJzIp = -1;

    if (st->loop.endCondition) {
        result += makeLineInstructions(st->loop.endLexems);
        QList<Bytecode::Instruction> endCondInstructions = calculate(modId, algId, level, st->loop.endCondition);
        shiftInstructions(endCondInstructions, result.size());
        result << endCondInstructions;
        Bytecode::Instruction e;
        e.type = Bytecode::POP;
        e.registerr = 0;
        result << e;
        // Show counter value at margin
        swreg.registerr = 0;
        if (st->lexems.size() > 0 && st->lexems.first()->lineNo!=-1 &&
                debugLevel_==GeneratorInterface::LinesAndVariables) {
            result << swreg;
        }
        endJzIp = result.size();
        e.type = Bytecode::JNZ;
        e.registerr = 0;
        result << e;
    }    
    else if (debugLevel_!=GeneratorInterface::NoDebug) {
        result += makeLineInstructions(st->loop.endLexems);
    }

    int jzIp2 = -1;

    // Jump to loop begin
//    lineNo = st->loop.endLexems[0]->lineNo;
//    l.arg = lineNo;
//    result << l;
    Bytecode::Instruction e;
    e.type = Bytecode::JUMP;
    e.registerr = 0;
    e.arg = beginIp;
    result << e;

    // Found end of loop
    if (jzIp!=-1) {
        result[jzIp].arg = result.size();
    }
    if (endJzIp!=-1) {
        result[endJzIp].arg = result.size();
    }
    if (jzIp2!=-1) {
        result[jzIp2].arg = result.size();
    }

    int endPos = result.size();

    setBreakAddress(result, level, endPos);

}

void Generator::setBreakAddress(QList<Bytecode::Instruction> &instrs,
                                int level,
                                int address)
{
    for (int i=0; i<instrs.size(); i++) {
        if (int(instrs[i].type)==127 && instrs[i].registerr==level) {
            instrs[i].type = Bytecode::JUMP;
            instrs[i].arg = address;
        }
    }
}

} // namespace KumirCodeGenerator
