#!/usr/bin/env python3
import inspect
import symtable
import importlib
import token
import symbol
from io import BytesIO
import keyword
from tokenize import *
import parser

try:
    import _kumir
    debug = _kumir.debug
except ImportError:
    import logging
    logging.root.name = "analizer"
    logging.root.setLevel(logging.DEBUG)
    debug = logging.debug

LxTypeEmpty = 0x00000000  # empty mask
LxTypeComment = 0x00000001  # bit 0
LxTypeDoc = 0x00000002  # bit 1
LxTypeName = 0x0000001C  # bits [2..4]
LxTypeSecondaryKwd = 0x000001E0  # bits [5..8]
LxTypePrimaryKwd = 0x0000F800  # bits [11..15]
LxTypeConstant = 0x001E0000  # bits [17..20]
LxTypeOperator = 0x01F00000  # bits [21..25]
LxTypeError = 0x80000000   # bit 31

# LxTypeName values

LxNameClass = 0x4
LxNameVar = 0x8
LxNameAlg = 0xc
LxNameModule = 0x10

# LxTypeSecondaryKwd values

LxSecFor = 0x20
LxSecTimes = 0x40
LxSecFrom = 0x60
LxSecTo = 0x80
LxSecStep = 0xa0
LxSecWhile = 0xc0
LxSecNewline = 0xe0
LxSecIf = 0x100
LxSecAnd = 0x120
LxSecOr = 0x140
LxSecNot = 0x160
LxSecOut = 0x180
LxSecIn = 0x1a0
LxSecInout = 0x1c0


# LxTypePrimaryKwd values

LxPriModule = 0x800
LxPriEndModule = 0x1000
LxPriAlgHeader = 0x1800
LxPriAlgBegin = 0x2000
LxPriAlgEnd = 0x2800
LxPriPre = 0x3000
LxPriPost = 0x3800
LxPriIf = 0x4000
LxPriThen = 0x4800
LxPriElse = 0x5000
LxPriFi = 0x5800
LxPriSwitch = 0x6000
LxPriCase = 0x6800
LxPriLoop = 0x7000
LxPriEndLoop = 0x7800
LxPriInput = 0x8000
LxPriOutput = 0x8800
LxPriFinput = 0x9000
LxPriFoutput = 0x9800
LxPriAssign = 0xa000
LxPriAssert = 0xa800
LxPriImport = 0xb000
LxPriAssignFile = 0xb800
LxPriExit = 0xc000
LxPriPause = 0xc800
LxPriHalt = 0xd000


# LxTypeConstant values

LxConstInteger = 0x20000
LxConstReal = 0x40000
LxConstBoolTrue = 0x60000
LxConstBoolFalse = 0x80000
LxConstLiteral = 0xa0000

# LxTypeOperator values

LxOperPlus = 0x200000
LxOperMinus = 0x400000
LxOperEqual = 0x600000
LxOperAsterisk = 0x800000
LxOperSlash = 0xa00000
LxOperPower = 0xc00000
LxOperLeftBr = 0xe00000
LxOperRightBr = 0x1000000
LxOperLeftSqBr = 0x1200000
LxOperRightSqBr = 0x1400000
LxOperColon = 0x1600000
LxOperComa = 0x1800000
LxOperLess = 0x1a00000
LxOperGreater = 0x1c00000
LxOperLessOrEqual = 0x1e00000
LxOperGreaterOrEqual = 0x2000000
LxOperNotEqual = 0x2200000
LxOperSemicolon = 0x2400000
LxOperLeftBrace = 0x2600000
LxOperRightBrace = 0x2800000

MATCH = {
    NAME: LxTypeName,
    NUMBER: LxTypeConstant,
    STRING: LxConstLiteral,
    LPAR: LxTypeOperator,  # generic operator
    RPAR: LxTypeOperator,  # generic operator
    LSQB: LxOperLeftSqBr,
    RSQB: LxOperRightSqBr,
    COLON: LxOperColon,
    COMMA: LxOperComa,
    SEMI: LxOperSemicolon,
    PLUS: LxOperPlus,
    MINUS: LxOperMinus,
    STAR: LxOperAsterisk,
    SLASH: LxOperSlash,
    VBAR: LxTypeOperator,  # generic operator
    AMPER: LxTypeOperator,  # generic operator
    LESS: LxOperLess,
    GREATER: LxOperGreater,
    EQUAL: LxOperEqual,
    DOT: LxTypeOperator,  # generic operator
    PERCENT: LxTypeOperator,  # generic operator
    LBRACE: LxOperLeftBr,
    RBRACE: LxOperRightBr,
    EQEQUAL: LxTypeOperator,  # generic operator
    NOTEQUAL: LxOperNotEqual,
    LESSEQUAL: LxOperLessOrEqual,
    GREATEREQUAL: LxOperGreaterOrEqual,
    TILDE: LxTypeOperator,  # generic operator
    CIRCUMFLEX: LxTypeOperator,  # generic operator
    LEFTSHIFT: LxTypeOperator,  # generic operator
    RIGHTSHIFT: LxTypeOperator,  # generic operator
    DOUBLESTAR: LxOperPower,
    PLUSEQUAL: LxTypeOperator,  # generic operator
    MINEQUAL: LxTypeOperator,  # generic operator
    STAREQUAL: LxTypeOperator,  # generic operator
    SLASHEQUAL: LxTypeOperator,  # generic operator
    PERCENTEQUAL: LxTypeOperator,  # generic operator
    AMPEREQUAL: LxTypeOperator,  # generic operator
    VBAREQUAL: LxTypeOperator,  # generic operator
    CIRCUMFLEXEQUAL: LxTypeOperator,  # generic operator
    LEFTSHIFTEQUAL: LxTypeOperator,  # generic operator
    RIGHTSHIFTEQUAL: LxTypeOperator,  # generic operator
    DOUBLESTAREQUAL: LxTypeOperator,  # generic operator
    DOUBLESLASH: LxTypeOperator,  # generic operator
    DOUBLESLASHEQUAL: LxTypeOperator,  # generic operator
    AT: LxTypeOperator,  # generic operator
    # RARROW: LxTypeOperator,  # generic operator
    # ELLIPSIS: LxTypeOperator,  # generic operator
    OP: LxTypeOperator,  # generic operator
    ERRORTOKEN: LxTypeError
}


__documents = [None] * 128


class Atom(object):
    pass


class Namespace(object):
    def __init__(self, parent=None, name=None):
        self.parent = parent
        self.functions = {}
        self.types = {}
        self.modules = {}
        self.variables = {}
        self.parameters = {}
        self.name = name
        self.reset()

    def reset(self):
        self.functions = {}
        self.types = {}
        self.modules = {}
        self.variables = {}
        self.parameters = {}
        if self.parent is None:
            builtin_functions = [(name, obj)
                                 for name, obj in self.__builtin_items().items()
                                 if inspect.isbuiltin(obj) or inspect.isroutine(obj)]
            builtin_types = [(name, obj)
                             for name, obj in self.__builtin_items().items()
                             if inspect.isclass(obj)]
            self.variables['__name__'] = "__main__"
            for name, obj in builtin_functions:
                self.functions[name] = obj
            for name, obj in builtin_types:
                self.types[name] = obj

    def __builtin_items(self):
        return {
            'abs': abs, 'all': all, 'any': any, 'ascii': ascii, 'bin': bin, 'bool': bool, 'bytearray': bytearray,
            'bytes': bytes, 'callable': callable, 'chr': chr, 'classmethod': classmethod, 'compile': compile,
            'complex': complex, 'delattr': delattr, 'dict': dict, 'dir': dir, 'divmod': divmod, 'enumerate': enumerate,
            'eval': eval, 'exec': exec, 'filter': filter, 'float': float, 'format': format, 'frozenset': frozenset,
            'getattr': getattr, 'globals': globals, 'hasattr': hasattr, 'hash': hash, 'help': help, 'hex': hex,
            'id': id, 'input': input, 'int': int, 'isinstance': isinstance, 'issubclass': issubclass, 'iter': iter,
            'len': len, 'list': list, 'locals': locals, 'map': map, 'max': max, 'memoryview': memoryview,
            'min': min, 'next': next, 'object': object, 'oct': oct, 'open': open, 'ord': ord, 'pow': pow,
            'print': print, 'property': property, 'range': range, 'repr': repr, 'reversed': reversed, 'round': round,
            'set': set, 'setattr': setattr, 'slice': slice, 'sorted': sorted, 'staticmethod': staticmethod,
            'str': str, 'sum': sum, 'super': super, 'tuple': tuple, 'type': type, 'vars': vars, 'zip': zip,
            '__import__': __import__
        }

    def search_function(self, name):
        if name in self.functions:
            return self.functions[name]
        else:
            return None

    def search_type(self, name):
        if name in self.types:
            return self.types[name]
        else:
            return None

    def search_module(self, name):
        if name in self.modules:
            return self.modules[name]
        else:
            return None

    def find(self, name):
        result = None
        if name in self.variables:
            result = self.variables[name]
        elif name in self.parameters:
            result = self.parameters[name]
        elif name in self.functions:
            result = self.functions[name]
        elif name in self.types:
            result = self.types[name]
        elif name in self.modules:
            result = self.modules[name]
        if result is None and not self.parent is None:
            result = self.parent.find(name)
        return result


class Analizer(object):

    def __init__(self):
        self.__sourceText = u""
        self.line_properties = []
        self.__code = None
        self.__root_namespace = Namespace()
        self.errors = []
        self.__namespace_stack = []

    def __lexem_type_by_name(self, lexem):
        if lexem == "True":
            return LxConstBoolTrue
        elif lexem == "False":
            return LxConstBoolFalse
        elif not self.__root_namespace.search_module(lexem) is None:
            return LxNameModule
        elif not self.__root_namespace.search_function(lexem) is None:
            return LxNameAlg
        elif keyword.iskeyword(lexem):
            return LxTypePrimaryKwd
        else:
            return LxTypeName

    def line_prop(self, _, text):
        res = [LxTypeEmpty] * len(text)
        g = tokenize(BytesIO(text.encode('utf-8')).readline)
        lexems = []
        try:
            for t, _, (_, start), (_, end), _ in g:
                lexems += [(t, start, end)]
        except BaseException:
            pass

        for t, start, end in lexems:
            for i in range(start, end):
                kt = LxTypeEmpty
                if t in MATCH:
                    kt = MATCH[t]
                if t == NAME:
                    kt = self.__lexem_type_by_name(text[start:end])
                if t == ERRORTOKEN:
                    kt = LxTypeEmpty
                res[i] = kt

        return res

    def set_source_text(self, text):
        self.__sourceText = text
        self.__code = None
        self.errors = []
        lines = text.split("\n")
        self.line_properties = []
        for line in lines:
            prop = []
            for ch in line:
                prop += [LxTypeEmpty]
            self.line_properties += [prop]
        try:
            self.__code = compile(text, "<input>", "exec")
        except SyntaxError as e:
            self.errors += [(e.lineno - 1, e.offset - len(e.text), len(e.text.rstrip()), e.msg)]
        if self.__code:
            self.__symtable = symtable.symtable(text, "<input>", "exec")

        self.__analize(text)
        for error_line, err_start, err_len, err_text in self.errors:
            prop = self.line_properties[error_line]
            for i in range(err_start, err_start + err_len):
                prop[i] = LxTypeError

    def __analize(self, source):
        self.__root_namespace.reset()
        data = source.encode('utf-8')
        lexems = tokenize(BytesIO(data).readline)
        try:
            for type_, lexem, (srow, scol), (erow, ecol), _ in lexems:
                if erow == 0 or srow == 0:
                    continue
                kt = LxTypeEmpty
                if type_ in MATCH:
                    kt = MATCH[type_]
                if type_ == NAME:
                    kt = self.__lexem_type_by_name(lexem)
                if type_ == ERRORTOKEN:
                    kt = LxTypeEmpty
                row = srow
                while row <= erow:
                    if row == srow:
                        start = scol
                    else:
                        start = 0
                    if row == erow:
                        end = min([ecol, len(self.line_properties[row-1])])
                    else:
                        end = len(self.line_properties[row-1])
                    for col in range(start, end):
                        self.line_properties[row-1][col] = kt
                    row += 1
        except BaseException as err:
            pass
        st = parser.suite(source)
        self.__namespace_stack = [self.__root_namespace]
        items = parser.st2tuple(st, line_info=True, col_info=True)
        self.__analize_body(items)

    def __analize_body(self, items):
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.stmt:
                self.__analize_statement(item[1])

    def __analize_statement(self, item):
        if item[0] == symbol.simple_stmt:
            self.__analize_simple_statement(item[1])
        elif item[0] == symbol.compound_stmt:
            self.__analize_compound_statement(item[1])

    def __analize_simple_statement(self, item):
        if item[0] == symbol.small_stmt:
            self.__analize_small_statement(item[1])

    def __analize_small_statement(self, item):
        if item[0] == symbol.import_stmt:
            self.__analize_import_statement(item[1])
        elif item[0] == symbol.expr_stmt:
            self.__analize_expression_statement(item[1:])

    def __analize_import_statement(self, item):
        if item[0] == symbol.import_name:
            self.__analize_import_name(item[2])
        elif item[0] == symbol.import_from:
            self.__analize_import_from(item[1:])

    def __set_line_prop(self, line_no, start, len_, type_):
        if 0 <= line_no < len(self.line_properties):
            props = self.line_properties[line_no]
            start = max([0, start])
            end = min([len(props), start + len_])
            for j in range(start, end):
                props[j] = type_

    def __analize_import_from(self, item):
        _, (_, name, row, col) = item[1]
        items_to_import = item[3]
        module = None
        try:
            module = importlib.import_module(name)
        except ImportError as err:
            self.errors += [(row - 1, col, len(name), err.msg)]
        if module:
            self.__set_line_prop(row-1, col, len(name), LxNameModule)
            module_items = inspect.getmembers(module)
            for item in items_to_import:
                if isinstance(item, int) and item == token.STAR:
                    for module_item_name, module_item_obj in module_items:
                        if not module_item_name.startswith("_"):
                            item_obj = module_item_obj
                            dict_to_add = None
                            if inspect.isroutine(item_obj) or inspect.isbuiltin(item_obj):
                                dict_to_add = self.__namespace_stack[-1].functions
                            elif inspect.ismodule(item_obj):
                                dict_to_add = self.__namespace_stack[-1].modules
                            elif inspect.isclass(item_obj):
                                dict_to_add = self.__namespace_stack[-1].types
                            else:
                                dict_to_add = self.__namespace_stack[-1].variables
                            dict_to_add[module_item_name] = item_obj
                elif isinstance(item, tuple) and item[0] == symbol.import_as_name:
                    _, item_name, item_row, item_col = item[1]
                    item_obj = None
                    for module_item_name, module_item_obj in module_items:
                        if module_item_name == item_name:
                            item_obj = module_item_obj
                            break
                    if item_obj is None:
                        self.errors += [(item_row - 1, item_col, len(item_name),
                                         "No '%s' in module '%s'" % (item_name, name))]
                    else:
                        alias = item_name
                        alias_row = alias_col = None
                        if len(item) >= 4:
                            _, alias, alias_row, alias_col = item[3]
                        dict_to_add = None
                        if inspect.isroutine(item_obj) or inspect.isbuiltin(item_obj):
                            dict_to_add = self.__namespace_stack[-1].functions
                            self.__set_line_prop(item_row - 1, item_col, len(item_name), LxNameAlg)
                            if alias_row:
                                self.__set_line_prop(alias_row - 1, alias_col, len(alias), LxNameAlg)
                        elif inspect.ismodule(item_obj):
                            dict_to_add = self.__namespace_stack[-1].modules
                            self.__set_line_prop(item_row - 1, item_col, len(item_name), LxNameModule)
                            if alias_row:
                                self.__set_line_prop(alias_row - 1, alias_col, len(alias), LxNameModule)
                        elif inspect.isclass(item_obj):
                            dict_to_add = self.__namespace_stack[-1].types
                            self.__set_line_prop(item_row - 1, item_col, len(item_name), LxNameClass)
                            if alias_row:
                                self.__set_line_prop(alias_row - 1, alias_col, len(alias), LxNameClass)
                        else:
                            dict_to_add = self.__namespace_stack[-1].variables
                        dict_to_add[alias] = item_obj

    def __analize_import_name(self, item):
        if item[0] == symbol.dotted_as_names:
            self.__analize_import_as_names(item[1:])

    def __analize_import_as_names(self, item):
        for part in item:
            if part[0] == symbol.dotted_as_name:
                if len(part) >= 4:
                    alias = part[3]
                else:
                    alias = None
                self.__analize_import_as_name(part[1], alias)

    def __analize_import_as_name(self, item, alias=None):
        if item[0] == symbol.dotted_name and isinstance(item[1], tuple) and item[1][0] == token.NAME:
            _, name, row, col = item[1]
            if alias is None:
                alias = name
                alias_row = alias_col = None
            else:
                _, alias, alias_row, alias_col = alias
            module = None
            try:
                module = importlib.import_module(name)
            except ImportError as err:
                self.errors += [(row - 1, col, len(name), err.msg)]
            if module:
                self.__namespace_stack[-1].modules[alias] = module
                self.__set_line_prop(row - 1, col, len(name), LxNameModule)
                if alias_row:
                    self.__set_line_prop(alias_row - 1, alias_col, len(alias), LxNameModule)

    def __analize_compound_statement(self, item):
        if item[0] == symbol.funcdef:
            self.__analize_function_def(item[1:])
        elif item[0] == symbol.for_stmt:
            self.__analize_for_statement(item[1:])
        elif item[0] == symbol.if_stmt:
            self.__analize_if_statement(item[1:])

    def __analize_function_def(self, items):
        _, name, row, col = items[1]
        self.__set_line_prop(row - 1, col, len(name), LxNameAlg)
        ns = Namespace(name=name, parent=self.__namespace_stack[-1])
        self.__namespace_stack[-1].functions[name] = ns
        self.__namespace_stack.append(ns)
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.parameters:
                self.__analize_parameters(item[1:])
            elif isinstance(item, tuple) and item[0] == symbol.suite:
                self.__analize_body(item[1:])
        self.__namespace_stack.pop()

    def __analize_parameters(self, items):
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.typedargslist:
                self.__analize_typedarglist(item[1:])

    def __analize_typedarglist(self, items):
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.tfpdef:
                _, name, _, _ = item[1]
                self.__namespace_stack[-1].parameters[name] = Atom()

    def __analize_for_statement(self, items):
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.exprlist:
                self.__analize_exprlist(item)
            elif isinstance(item, tuple) and item[0] == symbol.testlist:
                self.__analize_testlist(item)
            elif isinstance(item, tuple) and item[0] == symbol.suite:
                self.__analize_body(item)

    def __analize_exprlist(self, items):
        atoms = self.__extract_atoms_from_expression(items)
        for _, name, _, _ in atoms:
            self.__namespace_stack[-1].variables[name] = Atom()

    def __analize_testlist(self, items):
        atoms = self.__extract_atoms_from_expression(items)
        ns = self.__namespace_stack[-1]
        for type_, name, row, col in atoms:
            obj = ns.find(name)
            if type_ == token.NAME and obj is None:
                self.errors += [(row - 1, col, len(name),
                                 "Name not declared")]
                self.__set_line_prop(row - 1, col, len(name), LxTypeError)
            elif inspect.isbuiltin(obj) or inspect.isroutine(obj):
                self.__set_line_prop(row - 1, col, len(name), LxNameAlg)
            elif inspect.ismodule(obj):
                self.__set_line_prop(row - 1, col, len(name), LxNameModule)
            elif inspect.isclass(obj):
                self.__set_line_prop(row - 1, col, len(name), LxNameClass)

    def __extract_atoms_from_expression(self, items):
        result = []
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.atom:
                result += [item[1]]
            elif isinstance(item, tuple):
                result += self.__extract_atoms_from_expression(item)
        return result

    def __analize_if_statement(self, items):
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.test:
                self.__analize_testlist(item)
            elif isinstance(item, tuple) and item[0] == symbol.suite:
                self.__analize_body(item)

    def __analize_expression_statement(self, items):
        left_parts = []
        right_parts = []
        parts = right_parts
        for item in items:
            if isinstance(item, tuple) and item[0] == symbol.testlist_star_expr:
                parts += [item[1:]]
            elif isinstance(item, tuple) and item[0] == token.EQUAL:
                left_parts = parts
                right_parts = []
                parts = right_parts
        self.__analize_exprlist(left_parts)
        self.__analize_testlist(right_parts)


def new_document():
    unused_index = __documents.index(None)
    __documents[unused_index] = Analizer()
    return unused_index


def line_prop(document_id, line_no, text):
    return __documents[document_id].line_prop(line_no, text)


def set_source_text(document_id, text):
    __documents[document_id].set_source_text(text)


def errors(document_id):
    return __documents[document_id].errors


def line_properties(document_id):
    return __documents[document_id].line_properties

if __name__ == "__main__":
    new_document()
    set_source_text(0, """
from sys import exit as ex
import sys
def main(args, hello="Hello"):
    for index, arg in enumerate(args):
        print(index)
        print(arg)
    return 0

if __name__ == "__main__":
    ret = main(sys.argv)
    ex(ret)
elif __name__ == "kumir":
    print("Hello")
else:
    alist = ['a', 'b', 'c', 'd']
    for index, item in enumerate(alist):
        a, b = index, item
        debug('index: %d, item: %s' % (index, item))
    """)
    print(errors(0))
    print(line_properties(0))


debug("Module 'analizer' initialized")
