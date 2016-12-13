from .base_types_0 import _any, _any_of
from .base_types_0 import _nth_arg_type
from .base_types_0 import _iterable, str, bytearray, bytes
from .helper_annotation_classes import _min_max_return_type, _zip_return_type, _dict_from_iterable_constructor_type, \
    _list_from_iterable_constructor_type, _tuple_from_iterable_constructor_type


def max(arg1: _any, arg2: _any, *args: [_any]) -> _min_max_return_type: pass
def min(arg1: _any, arg2: _any, *args: [_any]) -> _min_max_return_type: pass
def zip(*iterables: [_iterable]) -> _zip_return_type: pass
def bytearray(source: str, encoding: str, errors: str = "strict") -> bytearray: pass
def bytes(source: str, encoding: str, errors: str = "strict") -> bytes: pass
def complex(x: _any_of([str, bytes, bytearray])) -> complex: pass
def dict(iterable: [(_any(), _any())] = [], **kwarg) -> _dict_from_iterable_constructor_type: pass
def int(x: _any_of([str, bytes, bytearray]), base: int = 10) -> int: pass
def float(x: _any_of([str, bytes, bytearray])) -> float: pass
def str(object: _any_of([bytearray, bytes]), encoding: str = "utf-8", errors: str = "strict") -> str: pass
def list(iterable: [] = []) -> _list_from_iterable_constructor_type: pass
def tuple(iterable: [] = []) -> _tuple_from_iterable_constructor_type: pass
