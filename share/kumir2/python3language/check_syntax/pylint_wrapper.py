# coding=utf-8
from astroid import MANAGER
from astroid.builder import AstroidBuilder
from pylint import lint

description = {
    "generic": "PyLint error checker",
    "russian": "Анализатор PyLint"
}

priority = 10.0

ERRORS = []

def set_source_text(text):
    global ERRORS
    ERRORS.clear()

    builder = AstroidBuilder()
    try:
        astroid_module = builder.string_build(text)
    except:
        astroid_module = None
    

def get_errors():
    return ERRORS
