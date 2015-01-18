# Выведите на экран фрагмент любого стихотворения
#
# ВНИМАНИЕ: задание засчитывается, если выведено
#   не меньше четырех строк стихотворения! 


# -*- kumir: begin hidden text -*-
def __post_run__():    
    import _kumir
    answer = _kumir.get_output_buffer()[:-1]
    stripped = answer.lstrip(' \n').rstrip(' \n')
    if len(stripped)==0:
        return 0
    if stripped.count('\n') < 3:
        result = 3
    else:
        result = 10    
    if len(answer) != len(stripped):
        result -= 1
    return result
    
    

# -*- kumir: protected lines: 0 1 2 3 -*-
