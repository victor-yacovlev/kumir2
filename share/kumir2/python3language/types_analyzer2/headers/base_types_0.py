from .special_types import object, _self_type, _value_type, _any_of, _nth_arg_type, _nth_arg_value_type, \
    _nth_arg_key_type, _nth_arg_match, _nth_arg_callable_rtype, NoneType, _key_type, _any, _callable, _match_after, \
    _scriptable_match, _special_pseudo_type, _typed_callable

## Base pseudo-class for collections
## See https://docs.python.org/3/library/stdtypes.html#common-sequence-operations
class _iterable(object): pass
class _sequence(_iterable): parametrizable = True
class _mapping(_iterable): parametrizable = True

class _sequence(_iterable):
    parametrizable = True
    def __mul__(self, count: int) -> _self_type: pass
    def __rmul__(self, count: int) -> _self_type: pass
    def __add__(self, other: _self_type) -> _self_type: pass
    def __radd__(self, other: _self_type) -> _self_type: pass
    def __len__(self) -> int: pass
    def __bool__(self) -> bool: pass
    def __getitem__(self, index: int) -> _value_type: pass
    def __getslice__(self, i: int, j: int) -> _self_type: pass


class tuple(_sequence):
    key_type = int

## Base pseudo-class for text-oriented strings
## See https://docs.python.org/3/library/stdtypes.html#str
class _string(_sequence): pass
class _string(_sequence):
    def capitalize(self) -> _self_type: pass
    def casefold(self) -> _self_type: pass
    def center(self, width: int, fillchar: _self_type = " ") -> _self_type: pass
    def count(self, sub: int, start: int = 0, end: int = -1) -> int: pass
    def endswith(self, suffix: _self_type, start: int = 0, end: int = -1) -> bool: pass
    def expandtabs(self, tabsize: int = 8) -> _self_type: pass
    def find(self, sub: _any_of([_self_type, _value_type]), start: int = 0, end: int = -1) -> int: pass
    def index(self, sub: _any_of([_self_type, _value_type]), start: int = 0, end: int = -1) -> int: pass
    def isalnum(self) -> bool: pass
    def isalpha(self) -> bool: pass
    def isdigit(self) -> bool: pass
    def islower(self) -> bool: pass
    def isspace(self) -> bool: pass
    def istitle(self) -> bool: pass
    def isupper(self) -> bool: pass
    def isalnum(self) -> bool: pass
    def join(self, iterable: [_self_type]) -> _self_type: pass
    def ljust(self, width: int, fillchar: _self_type = " ") -> _self_type: pass
    def lower(self) -> _self_type: pass
    def lstrip(self, chars: _self_type = "") -> _self_type: pass
    def partition(self, sep: _self_type) -> (_self_type, _self_type, _self_type): pass
    def replace(self, old: _self_type, new: _self_type, count: int = -1) -> _self_type: pass
    def rfind(self, sub: _any_of([_self_type, _value_type]), start: int = 0, end: int = -1) -> int: pass
    def rindex(self, sub: _any_of([_self_type, _value_type]), start: int = 0, end: int = -1) -> int: pass
    def rjust(self, width: int, fillchar: _self_type = " ") -> _self_type: pass
    def rpartition(self, sep: _self_type) -> (_self_type, _self_type, _self_type): pass
    def rsplit(self, sep: _self_type = None, maxsplit: int = -1) -> [_self_type]: pass
    def rstrip(self, chars: _self_type = "") -> _self_type: pass
    def split(self, sep: _self_type = None, maxsplit: int = -1) -> [_self_type]: pass
    def splitlines(self, keepends: bool = False) -> [_self_type]: pass
    def startswith(self, prefix: _self_type, start: int = 0, end: int = -1) -> bool: pass
    def strip(self, chars: _self_type = "") -> _self_type: pass
    def swapcase(self) -> _self_type: pass
    def title(self) -> _self_type: pass
    def translate(self, table: _mapping) -> _nth_arg_value_type(2): pass
    def upper(self) -> _self_type: pass
    def zfill(self, width: int) -> _self_type: pass


class str(_string):
    key_type = int
    value_type = str

class bytes(_string):
    key_type = int
    value_type = int
    def hex(self) -> str: pass


## Base pseudo-class for mutable collections
## See https://docs.python.org/3/library/stdtypes.html#mutable-sequence-types
class _mutable_sequence(_sequence):
    def __setitem__(self, key: int, value: _value_type) -> NoneType: pass
    def __setslice__(self, i: int, j: int, sequence: _self_type) -> NoneType: pass
    def __delitem__(self, key: int) -> NoneType: pass
    def __delslice__(self, i: int, j: int) -> NoneType: pass
    def append(self, item: _value_type) -> NoneType: pass
    def clear(self) -> NoneType: pass
    def copy(self) -> _self_type: pass
    def extend(self, other: _sequence) -> NoneType: pass
    def insert(self, key: int, value: _value_type) -> NoneType: pass
    def pop(self, key: int) -> _value_type: pass
    def remove(self, value: _value_type) -> NoneType: pass
    def reverse(self) -> NoneType: pass

class list(_mutable_sequence):
    key_type = int
    def sort(self, key: object = None, reverse: bool = False) -> NoneType: pass

class bytearray(_string, _mutable_sequence):
    key_type = int
    value_type = int
    def hex(self) -> str: pass

## Base pseudo-class for numeric types
## See https://docs.python.org/3/library/stdtypes.html#numeric-types-int-float-complex
class _numeric(object): pass
class int(_numeric): pass
class float(_numeric): pass
class complex(_numeric): pass
class _numeric(object):
    def __lt__(self, other: _numeric) -> bool: pass
    def __le__(self, other: _numeric) -> bool: pass
    def __gt__(self, other: _numeric) -> bool: pass
    def __ge__(self, other: _numeric) -> bool: pass
    def __bool__(self) -> bool: pass
    def __neg__(self) -> _self_type: pass
    def __pos__(self) -> _self_type: pass

class int(_numeric):
    def __init__(self, x: _numeric = 0) -> _self_type: pass
    def __add__(self, other: _numeric) -> _nth_arg_type(2): pass
    def __radd__(self, other: _numeric) -> _nth_arg_type(2): pass
    def __sub__(self, other: _numeric) -> _nth_arg_type(2): pass
    def __mul__(self, other: _numeric) -> _nth_arg_type(2): pass
    def __rmul__(self, other: _numeric) -> _nth_arg_type(2): pass
    def __rdiv__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass

class float(_numeric):
    def __add__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass
    def __radd__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass
    def __sub__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass
    def __mul__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass
    def __rmul__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass
    def __rdiv__(self, other: _numeric) -> _nth_arg_match(2, {int:float, float:float, complex:complex}): pass

class complex(_numeric):
    def __add__(self, other: _numeric) -> complex: pass
    def __radd__(self, other: _numeric) -> complex: pass
    def __sub__(self, other: _numeric) -> complex: pass
    def __mul__(self, other: _numeric) -> complex: pass
    def __rmul__(self, other: _numeric) -> complex: pass
    def __rdiv__(self, other: _numeric) -> complex: pass

class bool(int): pass  # just inherits

class dict(_mapping):
    def __contains__(self, item: _value_type) -> bool: pass
    def __delitem__(self, key: _key_type) -> NoneType: pass
    def __getitem__(self, key: _key_type) -> _value_type: pass
    def __setitem__(self, key: _key_type, value: _value_type) -> NoneType: pass
    def clear(self) -> NoneType: pass
    def copy(self) -> _self_type: pass
    def items(self) -> [(_key_type, _value_type)]: pass
    def keys(self) -> [_key_type]: pass
    def values(self) -> [_value_type]: pass


## TODO set, dict, etc.
