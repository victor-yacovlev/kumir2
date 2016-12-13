# resolve function return type in case of everything is known, but list value type
def ranged_summ_if_positive(a: int, b: int, c: list):
    result = 0
    for i in range(a, b):
        item = c[i]
        if item > 0:
            result = result + item
    return result


ranged_summ_if_positive_1 = ranged_summ_if_positive(0, 3, [0.5, 0.6, 0.7, 0.8, 0.9])  # expects: float
ranged_summ_if_positive_2 = ranged_summ_if_positive(0, 3, [1, 2, 3, 4, 5])  # expects: int