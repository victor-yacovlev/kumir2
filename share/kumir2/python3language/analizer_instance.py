"""
Analizer instance module
NOTE: Each analizer instance runs in it's own interpreter
"""

from kumir_constants import *

SOURCE_DIR_NAME = ""
SOURCE_TEXT = ""
ERRORS = []
LINE_PROPERTIES = []
LINE_RANKS = []


class Error:
    """ One error message """
    def __init__(self, line_no, start_pos, length, message):
        """
            line_no -- line number (int)
            start_pos -- start position from 0 (int)
            length -- error block length (int)
            message -- a string (ASCII symbols only) identificating error message
        """
        assert isinstance(line_no, int)
        assert isinstance(start_pos, int)
        assert isinstance(length, int)
        assert isinstance(message, str)
        self.line_no = line_no
        self.start_pos = start_pos
        self.length = length
        self.message = message


def set_source_dir_name(path):
    """
    Set the source file location (directory name) to help searching imported modules
        path -- directory path in platform dependent format (str)
    """
    assert isinstance(path, str)
    global SOURCE_DIR_NAME
    SOURCE_DIR_NAME = path


def set_source_text(text):
    """
    Set the source text and require complete analisis
        text -- complete python program source; line delimiter is '\n' (str)
    """
    assert isinstance(text, str)
    global SOURCE_TEXT
    SOURCE_TEXT = text


def get_errors():
    """
    Get a list of errors generated while 'set_source_text'
        returns a list of Error class instances
    """
    global ERRORS
    return ERRORS


def get_line_properties():
    """
    Get a list of line highlight properties generated while 'set_source_text'
        returns a list:
        - each item corresponds one text line
        - each item is a list:
            - each item corresponds one character in line
            - each item is an integer number, see 'kumir_constants.py'
    """
    global LINE_PROPERTIES
    return LINE_PROPERTIES


def get_line_ranks():
    """
    Get a list of line indentation ranks generated while 'set_source_text'
        returns a list:
        - each item corresponds one text line
        - each item is a tuple (start, end), where:
            - start is a line start indentation rank
            - end is a terminal indentation rank
    """
    global LINE_RANKS
    return LINE_RANKS


def get_line_property(line_no, line_text):
    """
    Get line property of one text line, currently editing and possible not complete
        line_no -- editable line number (from 0)
        line_text -- one text line while in edit progress
        returns a list:
        - each item corresponds one character in line
        - each item is an integer number, see 'kumir_constants.py'
    """
    assert isinstance(line_no, int)
    assert isinstance(line_text, str)
    return []