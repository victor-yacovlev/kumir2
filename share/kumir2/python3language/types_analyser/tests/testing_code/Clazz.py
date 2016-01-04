__author__ = 'velkerr'

class Test(object):
    static = 1
    static2 = static + 2
    st_int = 21
    st_float = 0.25
    st_str = 'str'
    st_bool = True
    st_bytes = bytes(1)
    st_tuple = (1, 2)
    st_list = [1, 2]
    st_set = {1, 4}
    crazy_static_elem = {1, 'mipt', ['Java', 6, True, (0, 4)]}
    crazy_static_elem2 = (1, 'int', {1:'python', 2:'java', 3:'c++'})
    """
    :type name: str
    :type elems: list
    """
    def __init__(self, name='', elems=list()):
        self.name = name
        """:type : str """
        self.elems = elems
        """:type : list """


if __name__ == '__main__':
    t1 = Test('1')
    t2 = Test('2')
    print(repr(t1.static) + ' ' + repr(t2.static))
    t1.static = 2
    print(repr(t1.static) + ' ' + repr(t2.static))
    Test.static = 3
    print(repr(t1.static) + ' ' + repr(t2.static))
