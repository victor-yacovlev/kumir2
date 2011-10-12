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
    s = s.strip()
    if s.startswith('"') and s.endswith('"') or s.startswith("'") and s.endswith("'"):
        return s[1:-1]
    else:
        return s 

def readTable(filename):
    """Open CSV-file and read existing table.
    Returns 2-Dimensional dictionary:
        DATABASE := { Language -> DATA }
        DATA := { Key -> Message }
        VALUE := string
    """
    empty = dict()
    empty["ru"]=dict()
    empty["en"]=dict()
    empty["ua"]=dict()
    empty["by"]=dict()
    empty["kz"]=dict()
    database = empty
    try:
        f = open(filename,'r')
    except:
        return database
    lines = unicode(f.read(),"utf-8").split("\n")
    f.close()
    if len(lines)==0:
        return database
    header = lines[0]
    data_rows = lines[1:]
    header_columns = header.split(";")
    if len(header_columns)<2:
        return database
    languages = header_columns[1:]
    for row in data_rows:
        columns = row.split(";")
        if len(columns)<2:
            continue
        key = dequote(columns[0])
        values = columns[1:]
        for i in range( 0, min(len(languages),len(values)) ):
            language = dequote(languages[i])           
            message = dequote(values[i])
            if database.has_key(language):
                data = database[language]
            else:
                data = dict()
            data[key] = message
            database[language] = data
    return database

def readCxx(filename):
    """Open CPP-file and scans for messages.
    Returns a list of keys
    """
    result = list()
    f = open(filename, 'r')
    if f is None:
        return result
    cpp = f.read()
    f.close()
    norm = ""
    inLineComment = False
    inBlockComment = False
    for i in range(0, len(cpp)):
        if inLineComment:
            if cpp[i]=='\n':
                inLineComment = False
                norm += cpp[i]
        elif inBlockComment:
            if cpp[i]=='*' and i<len(cpp)-1 and cpp[i+1]=='/':
                inBlockComment = False
        else:
            if cpp[i]=='/' and i<len(cpp)-1 and cpp[i+1]=='/':
                inLineComment = True
            elif cpp[i]=='/' and i<len(cpp)-1 and cpp[i+1]=='*':
                inBlockComment = True
            else:
                norm += cpp[i]
    rx = re.compile(r"_\(\"(.*)\".*\)")
    iterator = rx.finditer(norm)
    for match in iterator:
        key = match.group(1)
        result += [ key ]
    return result

def readRules(filename):
    """Open rules-file and scans for messages.
    Returns a list of keys
    """
    result = list()
    f = open(filename, 'r')
    if f is None:
        return result
    lines = unicode(f.read(),'utf-8').split('\n')
    f.close()
    norm = ""
    for line in lines:
        if len(line.strip())==0 or line.strip().startswith('#'):
            continue
        rule = line.split(':')
        if len(rule)<2:
            continue
        scripts = rule[1].replace('{','').replace('}','').strip().split(';')
        for script in scripts:
            if '(' in script:
                brPos = script.index('(')
                argline = script[brPos+1:-1]
                funcName = script[0:brPos]
                if funcName.lower()=="setcurrenterror":
                    args = argline.split(",")
                    for arg in args:
                        result += [dequote(arg)]
    return result

def cleanUnusedKeys(database, keys):
    "Removes non-existing more keys from database"
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
    "Adds newly appeared keys to database"
    count = 0
    for lang in database.keys():
        for key in keys:
            if not database[lang].has_key(key):
                database[lang][key] = "";
                count += 1
    return count/len(database.keys())

def writeTable(database, filename):
    "Writes database to CSV-table"
    f = open(filename, "w")
    assert not f is None
    f.write("\"Key\";")
    for lang in database.keys():
        if len(lang)>0:
            f.write("\""+lang+"\";")
    f.write("\n")
    allkeys = set()
    for lang in database.keys():
        if len(lang)>0:
            allkeys |= set(database[lang].keys())
    for key in allkeys:
        f.write("\""+key+"\";")
        for lang in database.keys():
            if len(lang)>0:
                value = database[lang][key]
                f.write("\""+value.encode('utf-8')+"\";")
        f.write("\n")
    f.close()

def printUsageAndExit():
    sys.stderr.write("Usage: "+sys.argv[0]+" --db=DB_FILE --out=OUT_FILE WORK_DIR1 WORK_DIR2 ... WORK_DIRn\n")
    sys.exit(127)

if __name__=="__main__":
    db_file = None
    out_file = None
    work_dirs = []
    for arg in sys.argv[1:]:
        if arg.startswith("--db="):
            db_file = arg[5:]
        elif arg.startswith("--out="):
            out_file = arg[6:]
        else:
            work_dirs += [ arg ]
    if db_file is None or out_file is None or len(work_dirs)==0:
        printUsageAndExit()
    #sys.stderr.write("db_file: "+db_file+"\n")
    #sys.stderr.write("out_file: "+out_file+"\n")
    #sys.stderr.write("work_dirs: "+str(work_dirs)+"\n")

    allkeys = set()

    for work_dir in work_dirs:
        allfiles = os.listdir(work_dir)
        cppfiles = filter(lambda x: x.endswith(".cpp") and not x.startswith("moc_") and not x.startswith("ui_"), allfiles)
        cpppaths = map(lambda x: work_dir+"/"+x, cppfiles)
        rulesfiles = filter(lambda x: x.endswith(".rules"), allfiles)
        relespaths = map(lambda x: work_dir+"/"+x, rulesfiles)

        for filename in cpppaths:
            allkeys |= set(readCxx(filename))
        for filename in relespaths:
            allkeys |= set(readRules(filename))

    database = readTable(db_file)


    unusedKeysCount = cleanUnusedKeys(database, allkeys)
    newKeysCount = addNewKeys(database, allkeys)
    changes = unusedKeysCount + newKeysCount

    if changes>0:
        writeTable(database, db_file)

    writeTable(database, out_file)

