from .base_types_0 import object, NoneType, _self_type, _any
from .base_types_0 import _any_of, _nth_arg_type, _nth_arg_key_type, _nth_arg_value_type, _nth_arg_match
from .base_types_0 import _key_type, _value_type
from .base_types_0 import _iterable, _mapping, _sequence, _mutable_sequence
from .base_types_0 import list, str, tuple, bytearray, bytes, _string
from .base_types_0 import _numeric, float, int, complex, bool
from .base_types_0 import _callable, _typed_callable, _nth_arg_callable_rtype, _match_after

def abs(x: _numeric) -> _nth_arg_type(1): pass
def all(iterable: _sequence) -> bool: pass
def any(iterable: _sequence) -> bool: pass
def ascii(object: object) -> str: pass
def bin(x: int) -> str: pass
def callable(object: object) -> bool: pass
def chr(i: int) -> str: pass
## TODO def classmethod(function)
## TODO def compile(source, filename, mode, flags=0, dont_inherit=False, optimize=1)
def delattr(object: object, name: str) -> NoneType: pass
def dir(object: object = None) -> [str]: pass
def divmod(a: _numeric, b: _numeric) -> (int, int): pass
def enumerate(iterable: _sequence, start: int = 0) -> [(int, _nth_arg_value_type(1))]: pass
def eval(expression: str, globals: { str: object } = None, locals: { str: object } = None) -> object : pass
def exec(executable: _any_of([str, object]), globals: { str: object } = None, locals: { str: object } = None) -> object : pass
def filter(function: _match_after([2],_typed_callable(bool, [_nth_arg_value_type(2)])), iterable: _sequence) -> [_nth_arg_value_type(2)]: pass
## TODO format(value [, format_spec])
def getattr(object: object, name: str, default: object = None) -> _nth_arg_type(3): pass
def globals() -> { str : object }: pass
def hasattr(object: object, name: str) -> bool: pass
def hash(object: object) -> int: pass
def help(object: object = None) -> NoneType: pass
def hex(x: int) -> str: pass
def id(object: object) -> int: pass
def input(prompt: object = None) -> str: pass
## TODO isinstance(object, classinfo)
## TODO issubclass(class, classinfo)
## TODO iter(object, [, sentinel])
def len(s: _iterable) -> int: pass
def locals() -> { str : object }: pass
def map(function: _match_after([2],_typed_callable(_any, [_nth_arg_value_type(2)])), iterable: _sequence) -> [_nth_arg_callable_rtype(1)]: pass
def max(iterable: _iterable) -> _nth_arg_value_type(1): pass
## TODO Declare also in builtins_1.py: def max(arg1, arg2, *args, key)
## TODO memoryview(obj)
def min(iterable: _iterable) -> _nth_arg_value_type(1): pass  ## TODO add keyword-only parameters processing
## TODO Declare also in builtins_1.py: def min(arg1, arg2, *args, key)
## TODO next(iterator, [default])
def oct(x: int) -> str: pass
## TODO open(file, mode='r', buffering=1, encoding=None, errors=None, newline=None, closefd=True, opener=None)
def ord(c: _string) -> int: pass
## TODO pow(x, y, [,z])
def print(*objects, sep='', end='\n', file: object = object, flush=False) -> NoneType : pass
def range(start: int, stop: int = -1, step: int = 1) -> [int]: pass
def repr(object: object) -> str: pass
def reversed(seq: _sequence) -> [ _nth_arg_value_type(1) ]: pass
from .helper_annotation_classes import _round_return_type
def round(number: _numeric, ndigits: int = 0) -> _round_return_type: pass
def setattr(object: object, name: str, value: object) -> NoneType: pass
## TODO slice(stop) | slice(start, stop [, step])
## TODO sorted(iterable [, key][, reverse])
## TODO staticmethod(function)
## TODO def sum(iterable: _sequence, start: _nth_arg_value_type(1) = None) -> _nth_arg_value_type(1): pass
## TODO def vars(object: object = None) -> { str : object }: pass
## TODO __import__(name, globals=None, locals=None, fromlist=(), level=0)

# Type constructors must be declared at last
def bool(x: object = False) -> bool: pass
def bytearray(source: _any_of([int, [int], bytes]) = b"") -> bytearray: pass
def bytes(source: _any_of([int, [int], bytes]) = b"") -> bytes: pass
def complex(real: _numeric = 0, imag: _numeric = 0) -> complex: pass
def dict(mapping:_mapping = {}, **kwarg) -> { _nth_arg_key_type(1): _nth_arg_value_type(1) }: pass
def int(x: _any_of([int, float]) = 0) -> int: pass
def float(x: _any_of([int, float]) = 0) -> float: pass
def str(object: object = "") -> str: pass
