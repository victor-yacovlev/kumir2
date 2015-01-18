# Выведите на экран:
#       1. Сумму чисел 56 и 20
#       2. Произведение чисел 35 и 10
#       3. Частное от деления 5 на 8
#       4. Целую часть от деления 16 на 5
#       5. Число 2 в 10 степени
#       6. Куб суммы чисел 10 и 48


# -*- kumir: begin hidden text -*-
def __post_run__():
    correct_answers = [
        ("76", 1),
        ("350", 1),
        ("0.625", 1),
        ("3", 2),
        ("1024", 2),
        ("195112", 3)
    ]
    import _kumir
    answers = _kumir.get_output_buffer().strip().split('\n')
    result = 0
    for index, answer in enumerate(answers):
        right_answer, score = correct_answers[index]
        if answer==right_answer:
            result += score
    return result

# -*- kumir: protected lines: 0 1 2 3 4 5 6 -*-
