# Scans for messages in subproject's *.cpp-files and updates
# corresponding messages table.

# This script is required for Kumir developers and not to
# be included into binary package

# usage:
#   scanmessages.py --db=CSV_FILE --out=CSV_FILE DIR1 [DIR2] ... [DIRn]
#
# Scans for messages in:
#  a) cpp-files using pattern: _("something")
#  b) rules-files using pattern: ("something")
#
# Updates db-file, creating if not exists and removing unused more keys.
# Then makes a copy of db into out-file.


import sys
import os
import os.path
import re



def dequote(s):
    """
    Removes quotation of string

    :param      s: source string
    :type       s: unicode
    :returns:   a string without leading and trailing quotation symbol
    :rtype:     unicode
    """
    s = s.strip()
    if s.startswith('"') and s.endswith('"') or s.startswith("'") and s.endswith("'"):
        return s[1:-1]
    else:
        return s


def parseTable(contents):
    """
    Parse CSV-file and read existing table

    :param      contents: CSV table data
    :type       contents: unicode
    :returns:   2-Dimensional dictionary: DATABASE := { Language -> DATA }; DATA := { Key -> Message }; VALUE := string
    :rtype:     dict
    """
    empty = dict()
    empty["ru"] = dict()
    empty["en"] = dict()
    empty["ua"] = dict()
    empty["by"] = dict()
    empty["kz"] = dict()
    database = empty
    lines = contents.split("\n")
    if len(lines) == 0:
        return database
    header = lines[0]
    data_rows = lines[1:]
    header_columns = header.split(";")
    if len(header_columns) < 2:
        return database
    languages = header_columns[1:]
    for row in data_rows:
        columns = row.split(";")
        if len(columns) < 2:
            continue
        key = dequote(columns[0])
        values = columns[1:]
        for i in range(0, min(len(languages), len(values))):
            language = dequote(languages[i])
            message = dequote(values[i])
            if language in database:
                data = database[language]
            else:
                data = dict()
            data[key] = message
            database[language] = data
    return database


def readCxx(filename):
    """
    Scans CPP-file for messages

    :param      filename: a file name of source
    :type       filename: str
    :returns:   a list of keys
    :rtype:     list
    """
    result = list()
    f = open(filename, 'r', encoding="utf-8")
    if f is None:
        return result
    cpp = f.read()
    f.close()
    norm = ""
    inLineComment = False
    inBlockComment = False
    for i in range(0, len(cpp)):
        if inLineComment:
            if cpp[i] == '\n':
                inLineComment = False
                norm += cpp[i]
        elif inBlockComment:
            if cpp[i] == '*' and i < len(cpp) - 1 and cpp[i + 1] == '/':
                inBlockComment = False
        else:
            if cpp[i] == '/' and i < len(cpp) - 1 and cpp[i + 1] == '/':
                inLineComment = True
            elif cpp[i] == '/' and i < len(cpp) - 1 and cpp[i + 1] == '*':
                inBlockComment = True
            else:
                norm += cpp[i]
    rx = re.compile(r"_\(\"(.*)\".*\)")
    iterator = rx.finditer(norm)
    for match in iterator:
        key = match.group(1)
        result += [key]
    return result


def readRules(filename):
    """
    Scans PD-Automata rules file for messages
    :param      filename: a file name of source
    :type       filename: str
    :returns:   a list of keys
    :rtype:     list
    """
    result = list()
    f = open(filename, 'r', encoding="utf-8")
    if f is None:
        return result
    if sys.version_info.major < 3:
        lines = unicode(f.read(), 'utf-8').split('\n')
    else:
        lines = f.read().split('\n')
    f.close()
    for line in lines:
        if len(line.strip()) == 0 or line.strip().startswith('#'):
            continue
        rule = line.split(':')
        if len(rule) < 2:
            continue
        scripts = rule[1].replace('{', '').replace('}', '').strip().split(';')
        for script in scripts:
            if '(' in script:
                brPos = script.index('(')
                argline = script[brPos + 1:-1]
                funcName = script[0:brPos]
                if funcName.lower() == "setcurrenterror":
                    args = argline.split(",")
                    for arg in args:
                        result += [dequote(arg)]
    return result


def cleanUnusedKeys(database, keys):
    """
    Removes non-existing more keys from database

    NOTE: Current implementation does nothing

    :param      database: database read
    :type       database: dict
    :param      keys: a list of existing keys
    :type       keys: list
    :returns:   number of removed entries
    :rtype:     int
    """
    count = 0
    return count

#    for lang in database.keys():
#        for old_key in database[lang].keys():
#            found = False
#            for new_key in keys:
#                u_new_key = unicode(new_key).strip()
#                u_old_key = unicode(old_key).strip()
#                if u_new_key==u_old_key:
#                    found = True
#                    break
#            if not found:
#                database[lang].pop(old_key)
#                count += 1
#    return count/len(database.keys())


def addNewKeys(database, keys):
    """
    Adds newly appeared keys to database

    :param      database: database read
    :type       database: dict
    :param      keys: a list of existing keys
    :type       keys: list
    :returns:   number of added entries
    :rtype:     int
    """
    count = 0
    for lang in database.keys():
        for key in keys:
            if not key in database[lang]:
                database[lang][key] = ""
                count += 1
    return count


def makeTable(database):
    """
    Writes database to CSV-table

    :param      database: database processed
    :type       database: dict
    :returns:   CSV table contents
    :rtype:     unicode
    """
    result = u'"Key";'
    for lang in database.keys():
        if len(lang) > 0:
            result += '"%s";' % lang
    result += '\n'
    allkeys = set()
    for lang in database.keys():
        if len(lang) > 0:
            allkeys |= set(database[lang].keys())
    allkeys = list(allkeys)
    allkeys.sort()
    for key in allkeys:
        result += '"%s";' % key
        for lang in database.keys():
            if len(lang) > 0:
                value = database[lang][key]
                result += '"%s";' % value
        result += '\n'
    return result


def printUsageAndExit():
    """
    Print script usage and exit
    """
    sys.stderr.write("Usage: " + sys.argv[0] + " --db=DB_FILE --out=OUT_FILE WORK_DIR1 WORK_DIR2 ... WORK_DIRn\n")
    sys.exit(127)


if __name__ == "__main__":
    db_file_name = None
    out_file_name = None
    work_dirs = []
    for arg in sys.argv[1:]:
        if arg.startswith("--db="):
            db_file_name = arg[5:]
        elif arg.startswith("--out="):
            out_file_name = arg[6:]
        else:
            work_dirs += [arg]
    if db_file_name is None or out_file_name is None or len(work_dirs) == 0:
        printUsageAndExit()    
    #sys.stderr.write("db_file: "+db_file_name+"\n")
    #sys.stderr.write("out_file: "+out_file_name+"\n")
    #sys.stderr.write("work_dirs: "+str(work_dirs)+"\n")

    allkeys = set()

    for work_dir in work_dirs:
        allfiles = os.listdir(work_dir)
        cppfiles = filter(lambda x: x.endswith(".cpp") and not x.startswith("moc_") and not x.startswith("ui_"),
                          allfiles)
        cpppaths = map(lambda x: work_dir + "/" + x, cppfiles)
        rulesfiles = filter(lambda x: x.endswith(".rules"), allfiles)
        relespaths = map(lambda x: work_dir + "/" + x, rulesfiles)

        for filename in cpppaths:
            allkeys |= set(readCxx(filename))
        for filename in relespaths:
            allkeys |= set(readRules(filename))

    db_file = open(db_file_name, 'r', encoding="utf-8")
    if sys.version_info.major < 3:
        source_csv_contents = unicode(db_file.read(), 'utf-8')
    else:
        source_csv_contents = db_file.read()
    db_file.close()

    database = parseTable(source_csv_contents)

    cleanUnusedKeys(database, allkeys)
    addNewKeys(database, allkeys)

    new_csv_contents = makeTable(database)

    if new_csv_contents != source_csv_contents:
        for file_name in [db_file_name, out_file_name]:
            f = open(file_name, 'w', encoding="utf-8")
            if sys.version_info.major < 3:
                f.write(new_csv_contents.encode('utf-8'))
            else:
                f.write(new_csv_contents)
            f.close()

    if not os.path.exists(out_file_name):
        f = open(out_file_name, 'w', encoding="utf-8")
        if sys.version_info.major < 3:
            f.write(new_csv_contents.encode('utf-8'))
        else:
            f.write(new_csv_contents)
        f.close()

