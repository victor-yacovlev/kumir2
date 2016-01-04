__author__ = 'velkerr'


def func1(param, value):
    """
    :param param:
    :type param: int
    :param value:
    :type value: str
    :return:
    :rtype: list[str]
    """
    def incr2(val):
        """
        :param val:
        :type val: int
        :return:
        :rtype: int
        """
        return val + 2

    return [param for s in range(0, incr2(value))]


class Clazz:
    """
    :type value: str
    :type param: int
    """
    def __init__(self, value, param):
        self.value = value
        self.param = param

    def test(self, val):
        self.value = val

        class FuckingInnerClass:
            def __init__(self, arg):
                self.arg = arg


class Vasya:
    def __init__(self, name, age):
        self.name = name
        self.age = age


class Kev(Vasya):
    """
    Der
    """
    def __init__(self, name, age, power):
        super.__init__(name, age)
        self.power = power


class Outer(object):
    outer_var = 1

    class Inner(object):
        def inner_var(self):
            return Outer.outer_var

    if outer_var == 1:
        elver_sa = 'nnjnj'
    else:
        elver_sa = 'nhjk'

    typ = []
    typ2 = {}
    typ3 = ()



if __name__ == '__main__':
    print(func1('str', 5))