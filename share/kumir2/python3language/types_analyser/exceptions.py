__author__ = 'velkerr'

class Base1StrException(Exception):

    """
    :type _name: str | int
    """
    def __init__(self, name):
        self._name = name


class UnexpectedEndOfDefinitionException(Base1StrException):
    """
    :type err_mess_parts: tuple[str]
    :type mess_part_num: int
    """
    err_mess_parts = ('before first', 'after last')

    def __init__(self, mess_part_num):
        super().__init__(mess_part_num)

    def __str__(self):
        return repr('No desciption ' + self.err_mess_parts[self._name] + ' comma.')


class TypeDoesNotExistsException(Base1StrException):

    def __init__(self, type_name):
        super().__init__(type_name)

    def __str__(self):
        return repr('The type '+ self._name + ' doesn\'t exists in the table of types.')


class InvalidDocStringException(Base1StrException):

    def __init__(self, name):
        super().__init__(name)

    def __str__(self):
        return repr('Invalid documentation of ' + self._name + '.')
