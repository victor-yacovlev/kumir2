__author__ = 'velkerr'

import csv
import re

try:
    from _kumir import debug
except ImportError:
    debug = print

modules = []
types_table = []
vars_table = []


class Module:
    """
    :type mod_id: int
    :type name: str
    :type global_vars: list[int] # (or dict[str, Type])
    :type global_funcs: list[Function]
    :type classes: list[Type]
    :type imports: list[Module]
    """
    def __init__(self, mod_id=None, name=None, global_vars=None, global_funcs=None, classes=None, imports=None):
        self.mod_id = mod_id if mod_id is not None else len(modules) + 1
        self.name = name
        self.global_vars = global_vars
        self.global_funcs = global_funcs
        self.classes = classes
        self.imports = imports

    @staticmethod
    def get_name(fname):
        return fname.split('/')[-1].split('.')[0]


class Type:
    default_name = 'default'

    """
    :type type_id: int
    :type full_name: str
    :type bases: list[Type]
    :type fields: list[Field]
    :type methods: list[Method]
    :param marker: current variable, related to type
    """
    def __init__(self,
                 type_id=None,
                 full_name=default_name,
                 bases=[], fields=[], methods=[],
                 lineno=None, scope=None, marker=None,
                 candidates=[]):
        self.type_id = type_id if type_id is not None else len(types_table)
        self.full_name = full_name
        self.bases = bases
        self.fields = fields
        self.methods = methods
        self.lineno = lineno
        self.scope = scope
        self.marker = marker
        self.candidates = candidates  # if we can't clearly define the type now

    def assign(self, other):
        class IncompatibleTypesException(Exception):
            def __init__(self, objects):
                self._type_names = [type(obj) for obj in objects]

            def __str__(self):
                mess = 'Incompatible types! ' + self._type_names
                if len(self._type_names) > 1:
                    return mess + ', but ' + self._type_names[1] + ' required.'
                return mess

        if not isinstance(other, type(self)):
            raise IncompatibleTypesException(other, self)
        for attrib in dir(other):
            try:
                setattr(self, attrib, getattr(other, attrib))
            except AttributeError:
                pass  # can't assign value to @property

    def __eq__(self, other):
        return self.full_name == other.full_name

    def __hash__(self):
        return hash(self.full_name)

    def __dir__(self):
        return list(filter(lambda x: not re.match('__\S+__', x), super().__dir__()))

    @property
    def simple_name(self):
        return self.full_name.split('.')[-1]

    @property
    def is_undefined(self):
        return self.full_name == Type.default_name and self.type_id != 0

    def update_name(self, fname):
        self.full_name = fname + '.' + self.full_name

    @staticmethod
    def build_full_name(classname, fname):
        return fname + '.' + classname


class Variable:
    """
    :type var_id: int
    :type name: str
    :type clazz: Type
    :type scope: tuple[int]
    :type add_to_table: bool
    :type visibility_area: list[int]
    """
    def __init__(self, name, clazz, var_id=None, scope=None, lineno=None, add_to_table=True):
        self.id = var_id if var_id is not None else len(vars_table)
        self.name = name
        self.clazz = clazz
        self.scope = scope
        self.lineno = lineno
        self.add_to_table = add_to_table  # both variables needed for receiving attributes from Field
        if add_to_table:
            vars_table.append(self)


class Field(Variable):
    def __init__(self, name, clazz=None, var_id=None, scope=None, lineno=None, add_to_table=True, is_static=False):  # type_id is not None
        if isinstance(name, Variable):
            super().__init__(name.name, name.clazz, name.id, name.scope, name.lineno, name.add_to_table)
        else:
            super().__init__(name, clazz, var_id, scope, lineno, add_to_table)
        self.is_static = is_static


class Method:
    def __init__(self, name, formals=None, return_type=None, annotation=None, inners=[], lineno=None, scope=None, func_id=None):
        self.id = func_id if func_id is not None else len(vars_table)
        self.name = name
        self.formals = formals
        self.return_type = return_type
        self.annotation = annotation
        self.inners = inners  # inner functions or classes
        self.lineno = lineno
        self.scope = scope


def parsing_file(fname):
    with open(fname) as f:
        source = f.read().strip().split('\n')
    f.close()

    def get_indent_level(string):
            return (len(string) - len(string.lstrip())) / 4  # todo: lambda!

    def parsing_code(src):
        class_indent = 0
        init_indent = 0
        current_class = None
        init_flag = False
        for num, code_str in enumerate(src):
            current_indent = get_indent_level(code_str)
            if 'class' in code_str:
                candidate = code_str.replace('(', ' ').split()[1].rstrip(':')
                for current in types_table:
                    if candidate in current.full_name:
                        current.update_name(fname)
                        current_class = current  # there is no classes with same names in 1 module
                        class_indent = current_indent
                        break
            if '__init__' in code_str and current_indent == class_indent + 1:
                init_flag = True
                init_indent = current_indent
            if init_flag and current_indent > init_indent and 'self' in code_str:
                current_class.fields.append(Field(code_str.split('.')[1].split('=')[0].strip()))
            elif current_indent < init_indent or ('class' or 'def') in code_str:
                init_flag = not init_flag

    def update_parent_attributes(clazz):
        if not clazz.bases:
            return clazz.fields, clazz.methods
        for base in clazz.bases:
            elements = update_parent_attributes(base)
            clazz.fields.append(elements[0])
            clazz.methods.append(elements[1])
        return clazz.fields, clazz.methods

    def define_linenos():
        undef_line_elements = filter(lambda var: var.lineno is None, types_table + vars_table)
        # todo! walk through all strings for each vars

    def update_visibility_areas():
        code_length = len(source)
        for current_element in vars_table + types_table:
            if current_element.lineno is None:
                current_element.scope = [0, code_length]
            elif get_indent_level(source[current_element.lineno]) == 0:
                current_element.scope = [current_element.lineno, code_length]

    parsing_code(source)
    [update_parent_attributes(clazz) for clazz in types_table]
    update_visibility_areas()
    define_linenos()

    # [debug(var.name) for var in vars_table if var.scope[1] != 0]


def fill_initial():
    """
    Initial filling table by base types and methods
    """
    # todo: After writing base types and methods work around imported modules (inspect)
    # todo: Area of visibility private fields of classes (_f, __f)

    types_array = [1, 1.0, "1", [1], (1,), {1}, {1: 1}, True, bytes(1)]
    global types_table
    types_table.append(Type())
    types_table.append(Type(full_name='object'))

    def build_path(mode):
        return ['unary/', 'binary/'][mode] + name + '.csv'

    for num, name in enumerate([str(type(types_array[j]))[8:-2] for j in range(len(types_array))]):
        methods = []
        f_reader = csv.DictReader(open(build_path(1)), delimiter=';')
        for info_num, type_info in enumerate(f_reader):
            if info_num == num:
                [methods.append(Method(key, formals=[Variable(None, value.split(':')[1], add_to_table=False)], return_type=[(value.split(':')[0])])) for key, value in zip(type_info.keys(), type_info.values())]

        f_reader = csv.DictReader(open(build_path(0)), delimiter=';')
        for info_num, type_info in enumerate(f_reader):
            methods.extend([Method(key, return_type=(value)) for key, value in zip(type_info.keys(), type_info.values())])
        types_table.append(Type(full_name=name, methods=methods))
