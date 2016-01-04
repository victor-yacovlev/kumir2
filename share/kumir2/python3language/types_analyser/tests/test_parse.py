from _ast import excepthandler

__author__ = 'velkerr'

import unittest
from types_analyser.analysis_node_visitor import AnalysisNodeVisitor
from types_analyser.exceptions import InvalidDocStringException


class TestParse (unittest.TestCase):

    # def test1(self):
    #     string = 'type1 [type2'
    #     self.assertFalse(_is_terminal(string))
    #
    # def test2(self):
    #     string = 'type'
    #     self.assertTrue(_is_terminal(string))
    #
    # def test3(self):
    #     string = 'type] type3'
    #     self.assertFalse(_is_terminal(string))
    #
    # def test4(self):
    #     string = 'type1[type2,type3]'
    #     self.assertFalse(_is_terminal(string))

    def test_doc(self):
        strings = [':type a: list[str]',
                   ':type b: str | int',
                   ':type err_mess_parts: tuple[str]',
                   ':type _num: int',
                   ':type c: ']
        expected_results = [('a', 'list[str]'),
                            ('b', 'str | int'),
                            ('err_mess_parts', 'tuple[str]'),
                            ('_num', 'int')]
        for num, str in enumerate(strings):
            if num < 4:
                self.assertEqual(AnalysisNodeVisitor._parse_args_from_doc(strings[num]), expected_results[num])
            else:
                flag = False
                try:
                    AnalysisNodeVisitor._parse_args_from_doc(strings[num])
                except InvalidDocStringException:
                    flag = True
                self.assertTrue(flag)