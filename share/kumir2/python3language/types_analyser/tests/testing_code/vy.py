__author__ = 'velkerr'

from math import sqrt


# def func1(a, b):
#     c = a - b
#     d = a * b
#     return a + b


# def func2(a, b, c):
#     return a * b + c


# def perverted_func(a, b):
#     if isinstance(a, str):
#         return len(a)
#     return a + b
#
#
# def triangle_func1(a, b, c):
#     mul = 1 if c else -1
#     return sqrt(a ** 2 + mul * b ** 2)


def long_expression(a, b, c):
    return ((a + b) * (a - b) + a * b) ** c == c
