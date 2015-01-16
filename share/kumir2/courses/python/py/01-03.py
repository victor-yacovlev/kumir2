# Заданы два числа с именами a и b.
#
# Создайте имя d, в котором хранится сумма
# чисел a и b, и имя f, в котором хранится
# их произведение.

a = 3
b = 4.5


# -*- kumir: begin hidden text -*-
def __pre_test__():
    import _kumir
    _kumir.set_test_run_count(3)
    _kumir.set_permanent_value("a_values", [1, 2, 3])
    _kumir.set_permanent_value("b_values", [2.5, 3.5, 4.5])
    
    
def __pre_run__():
    import _kumir
    a_values = _kumir.get_permanent_value("a_values")
    b_values = _kumir.get_permanent_value("b_values")
    assert len(a_values) == len(b_values)    
    index = 3 - _kumir.get_test_runs_left()
    a = a_values[index]
    b = b_values[index]
    _kumir.force_global_variable_value("a", a_values[index])
    _kumir.force_global_variable_value("b", b_values[index])

def __post_run__():    
    result = 0
    try:
        if d == a + b:
            result += 5
        else:
            result += 1
    except NameError:
        pass
    try:
        if f == a * b:
            result += 5
        else:
            result += 1
    except NameError:
        pass
    return result
    
    

# -*- kumir: protected lines: 0 1 2 3 4 5 6 7 -*-
