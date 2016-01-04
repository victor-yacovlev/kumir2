from pylint.test.input import func_noerror_staticmethod_as_decorator_py24

__author__ = 'velkerr'

from sys import argv
from termcolor import colored
import re

l = list()
l.append('yalle')
l.append(1)
l.append(True)

a = 0
b = 0.3

def print_colored(text, color='green'):
    print(colored(text, color))

def ch(param):
    """
    :type param: int
    :param param:
    :rtype int
    """
    param = param + 1
    return param


def ch2(param1: int, param2: bool) -> int:
    if param2:
        param1 = param1 + 1
    return param1, param2

def ch3():
    kiv = [1, 2, 3, 4, 5]
    [(print(k) if k == 4 else print("1")) for k in kiv]

def ch4():
    return 1 and 2 and 3 in (1,2,3)

def ch5():
    lis = list()
    [lis.append(name.strip()) for name in 'int, bool, char, List<>'.split(',')]
    return lis

def ch6(name):
    return name[name.find('[')+1:len(name)- 1]

def ch7(name):
    return name.index('1')

def ch8(name, param):
    assert isinstance(name, int)
    assert isinstance(param, str)
    k = name + 2
    assert isinstance(k, int)
    return k

class Test(object):
    static = 1
    static2 = static + 2
    st_str = 'str'
    st_bool = True
    st_tuple = (1, 2)
    """
    :type name: str
    :type elems: list
    """
    def __init__(self, name='', elems=list()):
        self.name = name
        """:type : str """
        self.elems = elems
        """:type : list """

    def __str__(self):
        repr(self.name)
        for el in self.elems:
            a = el + 1
            """
            :type : int
            """
            repr(a)

set_elem = {1, 2}

def check():
    a = set_elem
    print(a)
    # print(type(a))

def kjo(string):
    if ('class' or 'def') in string:
        return 1
    else:
        return 2

if __name__ == '__main__':
    string = 'class Vasya(Keer):'
    print(kjo(string))
    string = 'efvdvvf'
    print(kjo(string))
    # print(ch6('int[int, list[int]]'))
    # print(ch7('in1t1'))
    # t1 = Test('1')
    # t2 = Test('2')
    # print_colored(repr(t1.static) + ' ' + repr(t2.static))
    # t1.static = 2
    # print_colored(repr(t1.static) + ' ' + repr(t2.static))
    # Test.static = 3 # WHAT THE FUCK ?!
    # print_colored(repr(t1.static) + ' ' + repr(t2.static))
    # t1.v = 5
    #
    # # Output:
    # # 1 1
    # # 2 1
    # # 2 3
    #
    # def a():
    #     k = True
