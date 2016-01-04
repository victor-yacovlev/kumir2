__author__ = 'velkerr'

# For testing type analysis

from types_analyser.initial_checking import main

path = 'types_analyser/tests/testing_code/'

files = ('types_analyser/2.py',
         'types_analyser/exceptions.py',
         'types_analyser/tests/testing_code/Clazz.py',
         'types_analyser/tests/testing_code/example.py',
         'types_analyser/initial_checking.py')

main(path + 'vy.py')

