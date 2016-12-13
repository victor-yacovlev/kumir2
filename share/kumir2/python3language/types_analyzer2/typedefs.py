import ast
import copy
import functools
import inspect


class TypeDef:
    def __init__(self, name, supertypes, keytype=None, valuetype=None, tupleitems=[]):
        assert isinstance(name, str) or name is None
        assert isinstance(supertypes, list)
        assert isinstance(keytype, TypeDef) or keytype is None
        assert isinstance(valuetype, TypeDef) or valuetype is None
        self.name = name
        self.supertypes = supertypes
        self.keytype = keytype
        self.valuetype = valuetype
        self.parent_table = None
        self.tupleitems = tupleitems
        self.parametrizable = False
        self.properties = {}

    def copy(self):
        if not self.parametrizable:
            return self
        supertypes = copy.copy(self.supertypes)
        if self.keytype is None:
            keytype = None
        elif self.keytype == self:
            keytype = self
        else:
            keytype = self.keytype.copy()
        if self.valuetype is None:
            valuetype = None
        elif self.valuetype == self:
            valuetype = self
        else:
            valuetype = self.valuetype.copy()
        tupleitems = [x.copy() for x in self.tupleitems]
        result = TypeDef(self.name, supertypes, keytype, valuetype, tupleitems)
        result.parametrizable = self.parametrizable
        result.parent_table = self.parent_table
        result.properties = self.properties
        if result.keytype == self:
            result.keytype = result
        if result.valuetype == self:
            result.valuetype = result
        return result

    def __repr__(self):
        if self.name:
            r = self.name
            if "tuple" == self.name:
                r = "(" + ", ".join([repr(x) for x in self.tupleitems]) + ")"
            elif "_sequence" == self.name:
                if self.valuetype:
                    r = "[ " + repr(self.valuetype) + " ]"
                else:
                    r = "[]"
            elif "dict" == self.name:
                if self.keytype and self.valuetype:
                    r = "{ " + repr(self.keytype) + ": " + repr(self.valuetype) + " }"
                else:
                    r = "{}"
            elif self.valuetype and self.name not in ["str", "bytes", "bytearray"]:
                r += "<"
                if self.keytype and self.name not in ["tuple", "list"]:
                    r += repr(self.keytype) + ", "
                r += repr(self.valuetype)
                r += ">"
            return r
        else:
            return "<unnamed>"

    def __eq__(self, other):
        if not isinstance(other, TypeDef):
            return False
        return repr(self) == repr(other)

    def __hash__(self):
        r = repr(self)
        return hash(r)

    def match_method(self, method_name: str, arguments: [], raise_error=True):
        return self.methods_table.match_method(self, method_name, arguments, raise_error)

    def is_parametrizable(self):
        return self.parametrizable or any([x.is_parametrizable() for x in self.supertypes])
