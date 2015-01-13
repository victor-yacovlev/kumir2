# coding=utf-8
"""
Analizer instance module
NOTE: Each analizer instance runs in it's own interpreter
"""
import os

from kumir_constants import *

import color_marking
import static_analisys

SOURCE_DIR_NAME = ""
SOURCE_TEXT = ""
LINE_PROPERTIES = []
LINE_RANKS = []


class Error:
    # VY: В соответствии с PEP-8 и Google Coding Standards, имена классов - CamelCase
    # В PyCharm есть функция "умного" переименования имен: Refactor -> Rename... (Shift+F6)
    # Проверь также и другие файлы
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

    def __eq__(self, other):
        return self.line_no == other.line_no and self.start_pos == other.start_pos


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

    global LINE_PROPERTIES
    global LINE_RANKS
    static_analisys.clear_errors()
    static_analisys.run_static_analisys(text)
    color_marking.set_color_marks_and_ranks(SOURCE_TEXT)
    LINE_RANKS = color_marking.get_ranks()
    # VY: Результат -- это список пар чисел
    # каждое число -- это не количество отступов, а ожидаемое (логически, а не по количеству подсчитанных отступов)
    # изменение отступа. В качестве признака можно использовать, например, двоеточие в конце строки.
    # Эти значения используются редактором для того, чтобы автоматически вставлять нужное количество отступов при
    # нажатии Enter.
    #
    # Пример:
    # def sign(x):          # rank = ( 0, +1) # увеличиваем отступ следующей строки, поскольку в конце строки двоеточие
    # print(x)          # rank = ( 0,  0) # отступы не меняются
    #     if x < 0:         # rank = ( 0, +1) # увеличиваем отступ следующей строки, поскольку в конце строки двоеточие
    #         return -1     # rank = ( 0, -1) # в блоке кода после return не может быть ничего, поэтому уменьшаем отступ
    #     elif x > 0: return 1  # rank = ( 0,  0) # в данном случае elif ... : и return компенсируют друг друга
    #     else:             # rank = ( 0, +1) # увеличиваем отступ следующей строки, поскольку в конце строки двоеточие
    #         return 0      # rank = ( 0, -1) # в блоке кода после return не может быть ничего, поэтому уменьшаем отступ
    #
    # Таким образом, для данной программы ranks должно быть:
    # line_ranks = [ (0,1), (0,0), (0,1), (0,-1), (0,0), (0,1), (0,-1) ]
    #
    # Замечание. Похоже, в Python первое число во всех парах, равно 0. Тем не менее, необходимо соблюдать именно такой
    # интерфейс, который разработан с учетом других ЯП. Например, в Паскале, возможны такие числа:
    #
    # program MySuperProgram;  { rank = ( 0,  0) }
    # var                      { rank = (-1, +1)  -- почему в начале -1 -- смотри ниже, станет понятно }
    #     i: Integer;          { rank = ( 0,  0) }
    #     s: String;           { rank = ( 0,  0) }
    # begin                    { rank = (-1, +1) -- begin должен сместиться влево на 1 уровень с var и program,
    #                                               но следующая строка -- уже с отступом на +1. Аналогично и с var,
    #                                               поскольку, в общем случае, описание переменных может следовать
    #                                               после группы описания const.
    #    WriteLn('Hello!');    { rank = ( 0,  0) }
    # end.                     { rank = (-1,  0) -- end сам по себе сдвигается влево (-1), следующие -- на том же уровне


    LINE_PROPERTIES = color_marking.get_colors()
    for error in get_errors():
        if 0 <= error.line_no <= len(LINE_PROPERTIES):
            props = LINE_PROPERTIES[error.line_no]
            start = error.start_pos
            end = start + error.length
            for i in range(start, end):
                props[i] |= LxTypeError


def get_errors():
    """
    Get a list of errors generated while 'set_source_text'
        returns a list of Error class instances
    """
    return static_analisys.ERROR_LIST


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
    global LINE_RANKS
    changes = color_marking.is_change_rank(line_text)
    print(LINE_RANKS[line_no])
    LINE_RANKS[line_no] = (changes[0] + LINE_RANKS[line_no][0], changes[1] + LINE_RANKS[line_no][1])
    print(LINE_RANKS[line_no])

    return [LxTypeEmpty] * len(str)


def __run_test(test_name):
    base = os.path.dirname(os.path.abspath(__file__)) + "/"
    source_file = open(base + test_name, 'r')
    set_source_text(source_file.read())
    source_file.close()

    class LinePropPrintHex(int):
        def __repr__(self):
            return "0x%x" % self

    lines = SOURCE_TEXT.split('\n')
    ranks = get_line_ranks()
    props = [[LinePropPrintHex(item) for item in row] for row in get_line_properties()]
    errors = get_errors()

    assert len(lines) == len(ranks) == len(props)
    print("\nBegin test ", test_name, "========================")
    for no in range(0, len(lines)):
        out = "{:2d}: {!s:<30} # {!s:<8} {}".format(no+1, lines[no], ranks[no], props[no])
        print(out)
    if errors:
        print("\n")
    for error in errors:
        print("Error: ", error.message)
        line = lines[error.line_no]
        out = "{:2d}: {:s}".format(error.line_no+1, line)
        print(out)
        out = "    " + " " * error.start_pos + "^" * error.length
        print(out)
    print("End test ", test_name, "==========================")

if __name__ == "__main__":
    TESTS = [
        "MyTests/test1.py",
        "MyTests/test2.py",
        "MyTests/test3.py",
        "MyTests/test4.py",
        "MyTests/test5.py",
        "MyTests/undefined_names.py"
    ]
    for test in TESTS:
        __run_test(test)
