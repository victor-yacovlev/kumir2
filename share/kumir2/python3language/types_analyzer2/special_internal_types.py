from .typedefs import TypeDef
from .typetable import TypesTable


class ModuleTypeDef(TypeDef):

    def __init__(self, types_table):
        assert isinstance(types_table, TypesTable)
        object_type = types_table.lookup_by_name("object")
        str_type = types_table.lookup_by_name("str")
        str_object_dict_type = types_table.lookup_or_create_parametrized_dict(str_type, object_type)
        super().__init__("module", [object_type])
        self.properties["__builtins__"] = str_object_dict_type
        self.properties["__cached__"] = str_type
        self.properties["__doc__"] = str_type
        self.properties["__loader__"] = object_type
        self.properties["__name__"] = str_type
        self.properties["__package__"] = str_type
        self.properties["__spec__"] = object_type


