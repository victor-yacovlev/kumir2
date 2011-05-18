# Scans for messages in subproject's *.cpp-files and updates
# corresponding messages table.

# This script is required for Kumir developers and not to
# be included into binary package


import sys
import os
import os.path
import re

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
    lines = f.read().split("\n")
    f.close()
    if len(lines)==0:
        return database
    header = lines[0]
    data_rows = lines[1]
    header_columns = header.split(",")
    if len(header_columns)<2:
        return database
    languages = header_columns[1:]
    for row in data_rows:
        columns = row.split(",")
        if len(columns)<2:
            continue
        key = columns[0]
        values = columns[1:]
        for i in range( 0, min(len(languages),len(values)) ):
            language = languages[i]
            message = values[i]
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

def cleanUnusedKeys(database, keys):
    "Removes non-existing more keys from database"
    count = 0
    for lang in database.keys():
        for old_key in database[lang].keys():
            if not old_key in keys:
                database[lang].pop(old_key)
                count += 1
    return count

def addNewKeys(database, keys):
    "Adds newly appeared keys to database"
    count = 0
    for lang in database.keys():
        for key in keys:
            if not database[lang].has_key(key):
                database[lang][key] = "";
                count += 1
    return count

def writeTable(database, filename):
    "Writes database to CSV-table"
    f = open(filename, "w")
    assert not f is None
    f.write("\"Key\",")
    for lang in database.keys():
        f.write("\""+lang+"\",")
    f.write("\n")
    allkeys = []
    for lang in database.keys():
        allkeys += database[lang].keys()
    for key in allkeys:
        f.write("\""+key+"\",")
        for lang in database.keys():
            value = database[lang][key]
            f.write("\""+value+"\",")
        f.write("\n")
    f.close()

if __name__=="__main__":
    if len(sys.argv)<4:
        sys.stderr.write("Usage: "+sys.argv[0]+" DB_FILE OUT_FILE WORK_DIR\n")
        sys.exit(127)
    db_file = sys.argv[1]
    out_file = sys.argv[2]
    work_dir = sys.argv[3]
    #sys.stderr.write("db_file: "+db_file+"\n")
    #sys.stderr.write("out_file: "+out_file+"\n")
    #sys.stderr.write("work_dir: "+work_dir+"\n")
    allfiles = os.listdir(work_dir)
    cppfiles = filter(lambda x: x.endswith(".cpp") and not x.startswith("moc_") and not x.startswith("ui_"), allfiles)
    cpppaths = map(lambda x: work_dir+"/"+x, cppfiles)
    allkeys = []
    for filename in cpppaths:
        allkeys += readCxx(filename)
    database = readTable(db_file)

    changes = 0
    changes += cleanUnusedKeys(database, allkeys)
    changes += addNewKeys(database, allkeys)

    if changes>0:
        writeTable(database, db_file)

    writeTable(database, out_file)

