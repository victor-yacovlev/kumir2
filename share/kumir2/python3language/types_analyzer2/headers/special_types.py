## Root type inherited by others
class object:
    def __dir__(self) -> [str]: pass
    def __repr__(self) -> str: pass
    def __str__(self) -> str: pass
    def __eq__(self, other: object) -> bool: pass
    def __ne__(self, other: object) -> bool: pass

class NoneType(object):
    def __bool__(self) -> bool: pass

## Special pseudo-types for collections
class _special_pseudo_type: pass
class _self_type(_special_pseudo_type): pass
class _key_type(_special_pseudo_type): pass
class _value_type(_special_pseudo_type): pass
class _any(_special_pseudo_type): pass

## Special pseudo-types for pattern-matching on resulting type resolution
class _any_of(_special_pseudo_type):
    def __init__(self, variants): self.variants = variants
class _nth_arg_type(_special_pseudo_type):
    def __init__(self, arg_number): self.arg_number = arg_number
class _nth_arg_key_type(_special_pseudo_type):
    def __init__(self, arg_number): self.arg_number = arg_number
class _nth_arg_value_type(_special_pseudo_type):
    def __init__(self, arg_number): self.arg_number = arg_number
class _nth_arg_match(_special_pseudo_type):
    def __init__(self, arg_number, match_table: dict): self.arg_number, self.match_table = arg_number, match_table
class _nth_arg_callable_rtype(_special_pseudo_type):
    def __init__(self, arg_number): self.arg_number = arg_number
class _match_after(_special_pseudo_type):
    def __init__(self, arg_numbers_required, predicate):
        self.arg_numbers_required, self.predicate = arg_numbers_required, predicate

class _scriptable_match(_special_pseudo_type):
    def evaluate(self, kwlist_args, kwdict_args):
        pass

## Special pseudo-types for higher-order functions
class _callable(object): pass
class _typed_callable(_special_pseudo_type, _callable):
    def __init__(self, return_type, arg_types): self.return_type, self.arg_types = return_type, arg_types