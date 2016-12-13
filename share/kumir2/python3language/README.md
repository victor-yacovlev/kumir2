# python3_types_checker
A part of more complex syntax analizer

## What implemented now
Expression's type deduction for:
 * built-in functions usage
 * basic types and operations
 * 2-nd order functionals (like map and reduce)
 * lambda-expressions
 * annotated functions
 * not annotated functions under some good conditions

## Test suites
Located in types_analizer2/test

## How to run test
`python3 check_syntax/types_analyzer_wrapper.py FILE_NAME_TO_TEST.py`

The result will be displayed to stdout as source text with special comments starting with `###`
