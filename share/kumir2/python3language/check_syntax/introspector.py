import inspect


def get_builtins_classes_and_functions():
    classes = []
    functions = []
    all = __builtins__.items()
    for name, obj in all:
        assert isinstance(name, str)
        if not name.startswith("__"):
            if inspect.isfunction(obj) or inspect.isbuiltin(obj):
                functions += [name]
            elif inspect.isclass(obj):
                classes += [name]
    return classes, functions


class Context:
    def __init__(self, range):
        self.range = None
        self.modules = []
        self.functions = []
        self.classes = []

    def get_module_names(self):
        return self.modules

    def get_function_names(self):
        return self.functions

    def get_class_names(self):
        return self.classes


class Introspector:
    def __init__(self):
        self.ast = None
        self.source_text = ""
        self.source_dir_name = ""
        self.global_context = Context(None)
        glob_types, glob_funs = get_builtins_classes_and_functions()
        self.global_context.functions = glob_funs
        self.global_context.classes = glob_types

    def set_source_dir_name(self, dir_name: str):
        self.source_dir_name = dir_name

    def set_source_text(self, text: str):
        self.source_text = text

    def get_global_context(self):
        return self.global_context
