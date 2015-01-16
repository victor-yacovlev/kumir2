# Дан список нечетной длины с именем s.
#
# 1. Сохраните в переменной head
#    первый элемент списка
# 2. Сохраните в переменной tail
#    "хвост" списка, то есть
#    все элементы, кроме первого
# 3. Выведете на экран элемент списка,
#    который находится ровно посередине

s = [1, 2, 3, 4, 5]


# -*- kumir: begin hidden text -*-

def __pre_run__():
    import _kumir
    _kumir.force_global_variable_value("s", [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

def __post_run__():
    result = 0
    try:
        if head==0:
            result += 2
    except:
        pass
    try:
        if tail==[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]:
            result += 3
    except:
        pass
    import _kumir
    lines = _kumir.get_output_buffer().strip().split('\n')
    last_line = lines[-1]
    if last_line=="5":
        result += 5
    return result

# -*- kumir: protected lines: 0 1 2 3 4 5 6 7 8 9 10  -*-
