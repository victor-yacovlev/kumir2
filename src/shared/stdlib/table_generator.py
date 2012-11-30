#!/usr/bin/python2.7
#encoding=utf-8

TABLES = ["cp866", "koi8-r", "windows-1251"]

for table in TABLES:
    alph = map(lambda x: chr(x), range(128,255))
    ualph = []
    for elem in alph:
        try:
            symbol = unicode(elem, table)
        except:
            symbol = u"\0"
        ualph += [symbol]

    ucodes = map(lambda x: ord(x), ualph)
    to = open("wchar_"+table+".table", "w")
    fr = open(table+"_wchar.table", "w")
    for index, ucode in enumerate(ucodes):
        if ucode != 0:
            to.write("case "+str(ucode)+": return "+str(index+128)+"; break;\n")
            fr.write("case "+str(index+128)+": return "+str(ucode)+"; break; \n")
    to.close()
    fr.close()
