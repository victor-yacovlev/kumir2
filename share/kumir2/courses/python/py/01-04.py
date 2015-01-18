# Прочитать с терминала одну строку, после этого:
# 
# 1. Сохранить в переменную third 
#    третий символ этой строки
# 2. Сохранить в переменную pre_last 
#    предпоследний символ
# 3. Сохранить в переменную start_5 
#    первые пять символов
# 4. Сохранить в переменную prefix
#    все, кроме последний двух символов
# 5. Сохранить в переменную length 
#    длину исходной строки
# 6. В исходной строке поменять первый 
#    и последний символ, после чего
#    вывести полученную строку на экран



# -*- kumir: begin hidden text -*-
def __pre_run__():
    import _kumir
    _kumir.simulate_input("0123456789abcdef")

def __post_run__():    
    result = 0
    try:
        if third=='2':
            result += 1
    except:
        pass
    try:
        if pre_last=='e':
            result += 2
    except:
        pass
    try:
        if start_5=="01234":
            result += 1
    except:
        pass
    try:
        if prefix=="0123456789abcd":
            result += 2
    except:
        pass
    try:
        if length==16:
            result += 1
    except:
        pass
    import _kumir
    lines = _kumir.get_output_buffer().strip().split('\n')
    last_line = lines[-1]
    if last_line.endswith("f123456789abcde0"):
        result += 3
    return result

# -*- kumir: protected lines: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 -*-
