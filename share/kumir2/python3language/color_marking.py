# coding=utf-8
from tokenize import generate_tokens, tokenize, TokenError
import keyword
import token
from io import BytesIO
from kumir_constants import *
import parser
import analizer_instance
from static_analisys import GLOBAL_SYMBOL_LIST, Function, Module, Class, run_static_analisys, Variable

SECONDARY_KWD = ("in", "as", "is", "and", "or", "not", "pass", "break", "continue", "return", "else", "elif",
                 "if", "except", "finally", "try", "raise")
PRIMARY_KWD = ("def", "for", "class", "import", "from", "with", "global", "None", "while", "yield", "nonlocal", "lambda", "assert", "del")
#True, False -- отдельно, почему None не отдельно?

color_marks = [[]]
line_ranks = [0]

def set_color_marks_and_ranks(source_code_str, syntax_only = False):
    if not syntax_only:
        run_static_analisys(source_code_str)
    global color_marks
    global line_ranks
    color_marks = [[]]
    pair_line_ranks = []
    line_ranks = [0]
    previous_tok_ecol = 0
    i = 0
    tokens = tokenize(BytesIO(source_code_str.encode('utf-8')).readline)
    previous_tok_ecol = 0
    try:
        for tok in tokens:
            # tok[0] -- token type
            # tok[1] -- token string
            # tok[2] -- (srow, scol)
            # tok[3] -- (erow, ecol)
            # tok[4] -- "logical" line
            if tok[2] != tok[3] or tok[0] == token.DEDENT or tok[0] == token.NEWLINE:
                if tok[0] == token.NAME:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    if tok[1] in SECONDARY_KWD:
                        color_marks[i].extend([LxTypeSecondaryKwd]*len(tok[1]))
                    elif tok[1] in PRIMARY_KWD:
                        color_marks[i].extend([LxTypePrimaryKwd]*len(tok[1]))
                    elif tok[1] == "True":
                        color_marks[i].extend([LxConstBoolTrue]*len(tok[1]))
                    elif tok[1] == "False":
                        color_marks[i].extend([LxConstBoolFalse]*len(tok[1]))
                    else:
                        for s in GLOBAL_SYMBOL_LIST:
                            if isinstance(s, Function) and s.name == tok[1]:
                                color_marks[i].extend([LxNameAlg & LxTypeName]*len(tok[1]))
                                break
                            elif isinstance(s,Module) and s.name == tok[1]:
                                color_marks[i].extend([LxNameModule & LxTypeName]*len(tok[1]))
                                break
                            elif isinstance(s,Class) and s.name == tok[1]:
                                color_marks[i].extend([LxNameClass & LxTypeName]*len(tok[1]))
                                break
                            elif isinstance(s,Variable) and s.name == tok[1]:
                                color_marks[i].extend([LxNameVar & LxTypeName]*len(tok[1]))
                                break
                        else:
                            color_marks[i].extend([LxTypeName]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.STRING:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    multi_line_list = tok[1].split('\n')
                    for line in multi_line_list:
                        color_marks.append([])
                        color_marks[i].extend([LxConstLiteral]*len(line))
                        line_ranks.append(0)
                        i+=1
                    del color_marks[len(color_marks)-1]
                    del line_ranks[len(line_ranks)-1]
                    i-=1
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.OP:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    color_marks[i].extend([LxTypeOperator]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.N_TOKENS: #comments
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    color_marks[i].extend([LxTypeComment]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.INDENT:
                    color_marks[i].extend([LxTypeEmpty]*len(tok[1]))
                    line_ranks[len(line_ranks)-1] += 1
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.DEDENT:
                    line_ranks[len(line_ranks)-1] -= 1
                    #previous_tok_ecol = tok[3][1]
                elif tok[0] == token.NEWLINE or tok[0] == 55: #55 -- '\n'
                    color_marks.append([])
                    line_ranks.append(line_ranks[len(line_ranks)-1])
                    i += 1
                    previous_tok_ecol = 0
                elif tok[0] == token.NUMBER:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    color_marks[i].extend([LxConstInteger]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
                elif tok[0] == token.COLON:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    color_marks[i].extend([LxConstInteger]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
                    line_ranks[len(line_ranks)-1] -= 1
                elif tok[0] == token.ERRORTOKEN:
                        color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                        color_marks[i].extend([LxTypeError]*len(tok[1]))
                        previous_tok_ecol = tok[3][1]

                else:
                    color_marks[i].extend([LxTypeEmpty]*(tok[2][1] - previous_tok_ecol))
                    color_marks[i].extend([LxTypeEmpty]*len(tok[1]))
                    previous_tok_ecol = tok[3][1]
            #global pair_line_ranks
    except TokenError as err:
        #print(err)
        if(err.args[0] == 'EOF in multi-line string'):
            ml = source_code_str.split('\n')

            analizer_instance.ERRORS.append(analizer_instance.Error(err.args[1][0],err.args[1][1],1,"Нет парной \"\"\" " ))
            color_marks[i].extend((len(ml[i])-err.args[1][0]+3)*[LxTypeError | LxConstLiteral])
            for k in range(err.args[1][0],len(ml)):
                #print(ml[k])
                color_marks.append(len(ml[k])*[LxTypeError | LxConstLiteral])
        elif err.args[0] == 'EOF in multi-line statement':
            for j in range(len(color_marks[i])):
                color_marks[i][j] = color_marks[i][j] | LxTypeError


        #print((err.args[1]))
        #color_marks[i].extend([LxTypeError]*len(tok[1]))
        previous_tok_ecol = tok[3][1]





    for i in range(len(line_ranks)):
        if i<len(line_ranks)-1:
            pair_line_ranks.append((0, line_ranks[i+1]-line_ranks[i]))
        else:
            pair_line_ranks.append((line_ranks[i], 0))
    line_ranks = []
    line_ranks.extend(pair_line_ranks)
    if __name__ == "__main__":
        print(line_ranks)


def get_colors():
    global color_marks
    return color_marks

def get_ranks():
    global line_ranks
    return line_ranks


GLOBAL_CONSTANT_KWD_COMMA = ("for","if","try","while","def","class","with") #ключевые слова двоеточие после которых повышает rank end у строки
GLOBAL_CONSTANT_KWD_COMMED = ("else","elif","except","finally")          # уменьшают rank begin у строки






def is_change_rank(text):
    tokens = [x for x in tokenize(BytesIO(text.encode('utf-8')).readline)]
    if tokens[1][1] in GLOBAL_CONSTANT_KWD_COMMA and tokens[len(tokens)-3][1]==':':
        return (0,1)
    elif tokens[1][1] in GLOBAL_CONSTANT_KWD_COMMED and tokens[len(tokens)-3][1]==':':
        return (-1,1)
    elif tokens[1][1] == "return":
        return (0,-1)
    else:
        return (0,0)

if __name__ == "__main__":
    import os
    base = os.path.dirname(os.path.abspath(__file__)) + "/"
    test_name = "MyTests/test1.py"
    source_file = open(base + test_name, 'r')
    source_code_str = source_file.read()
    source_file.close()

    str_list = source_code_str.split('\n')
    set_color_marks_and_ranks(source_code_str)
    print( len(str_list) )
    print( len(color_marks) )
    print( len(line_ranks)  )
    for i in range(len(str_list)):
        if len(str_list[i])!=len(color_marks[i]):
            print("bad news..." + str(i))

            print(len(str_list[i]))
            print(len(color_marks[i]))


    print(get_colors())
    print(get_ranks())

    #tokens = [x for x in tokenize(BytesIO(source_code_str.encode('utf-8')).readline)]
    #for i in range(len(tokens)):
    #    print(is_change_rank(tokens[i][4]))



