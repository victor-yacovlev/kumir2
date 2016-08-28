#!/usr/bin/python

# coding=UTF-8
import subprocess

import sys
import os.path
import os
import kumirutils

TEST_DIRS = ["kumir_prog", "newTOS", "NewErrors", "new_files", "tOurNew"]

out = sys.stdout

COMPILE = kumirutils.compile_to_bytecode
RUN = kumirutils.run_bytecode
EXE_SUFFIX = ".kod"


def group_errors(lst):
    result = {}
    for e in lst:
        result[e.line] = e
    return result


def ask_differences(filename, old_errors, new_errors):
#    if old_errors:
#        print "Old: ", reduce(lambda x,y: str(x)+"\n"+str(y), old_errors)
#    if new_errors:
#        print "New: ", reduce(lambda x,y: str(x)+"\n"+str(y), new_errors)
    old_db = group_errors(old_errors)
    new_db = group_errors(new_errors)
    common = set(old_db.keys()) & set(new_db.keys())
    old_only = set(old_db.keys()) - common
    new_only = set(new_db.keys()) - common
    result = []
    has_changes = False
    for line in list(common):
        e_o = old_db[line]
        e_n = new_db[line]
        if e_o.pos != e_n.pos or e_o.end != e_n.end or e_o.text != e_n.text:
            sys.stderr.write("------------------\n")
            sys.stderr.write("Difference in " + filename + "\n")
            sys.stderr.write("OLD: " + str(e_o) + "\n")
            sys.stderr.write("NEW: " + str(e_n) + "\n")
            sys.stderr.write("--- What should I do? (use [o]ld, use [n]ew, just [s]kip this file): ")
            answer = _read_char()
            if answer == 's':
                sys.stderr.write("s\n")
                return None
            elif answer == 'n':
                sys.stderr.write("n\n")
                has_changes = True
                result += [e_n]
            else:
                sys.stderr.write("o\n")
                result += [e_o]

        else:
            result += [e_o]

    for line in list(old_only):
        e = old_db[line]
        sys.stderr.write("------------------\n")
        sys.stderr.write("Lost error in " + filename + "\n")
        sys.stderr.write(str(e) + "\n")
        sys.stderr.write(
            "--- What should I do? ([r]emove error from standard, [k]eep error in standard, [s]kip this file): ")
        answer = _read_char()
        if answer == 's':
            sys.stderr.write("s\n")
            return None
        elif answer == 'r':
            has_changes = True
            sys.stderr.write("r\n")
        else:
            sys.stderr.write("k\n")
            result += [e]

    for line in list(new_only):
        e = new_db[line]
        sys.stderr.write("------------------\n")
        sys.stderr.write("New error in " + filename + "\n")
        sys.stderr.write(str(e) + "\n")
        sys.stderr.write(
            "--- What should I do? ([a]dd error to standard, [k]eep standard without this error, [s]kip this file): ")
        answer = _read_char()
        if answer == 's':
            sys.stderr.write("s\n")
            return None
        elif answer == 'a':
            has_changes = True
            sys.stderr.write("a\n")
            result += [e]
        else:
            sys.stderr.write("k\n")

    if has_changes:
        return result


def _read_char():
    import tty, termios, sys

    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        ch = sys.stdin.read(1)
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    if ch == 'q':
        sys.stderr.write("q\nUser terminated.\n")
        sys.exit(0)
    return ch


def group_comp_errors(lst):
    result = {}
    for e in lst:
        result[e.line] = e
    return result


def print_comp_difference(filename, old_errors, new_errors):
    old_db = group_comp_errors(old_errors)
    new_db = group_comp_errors(new_errors)
    common = set(old_db.keys()) & set(new_db.keys())
    old_only = set(old_db.keys()) - common
    new_only = set(new_db.keys()) - common
    for line in list(common):
        e_o = old_db[line]
        e_n = new_db[line]
        if e_o.pos != e_n.pos or e_o.end != e_n.end or e_o.text != e_n.text:
            out.write("Difference in " + filename + "\n")
            out.write("OLD: " + str(e_o) + "\n")
            out.write("NEW: " + str(e_n) + "\n")
            out.write("------------------\n")
    for line in list(old_only):
        e = old_db[line]
        out.write("Lost error in " + filename + "\n")
        out.write(str(e) + "\n")
        out.write("------------------\n")
    for line in list(new_only):
        e = new_db[line]
        out.write("New error in " + filename + "\n")
        out.write(str(e) + "\n")
        out.write("------------------\n")


def print_difference(gsdata, data, title):
    if data:
        out.write("Difference in " + title + ":\n")
        out.write("===== OLD:\n")
        out.write(gsdata + '\n')
        out.write("===== NEW:\n")
        out.write(data + '\n')


def ask_difference(filename, data, title):
    f = open(filename, 'r')
    gsdata = f.read()
    f.close()
    if gsdata != data:
        print_difference(gsdata, data, title)
        sys.stderr.write("Write [n]ew or [k]eep old? ")
        answer = _read_char()
        if answer == 'n':
            sys.stderr.write("n\n")
            f = open(filename, 'w')
            f.write(data)
            f.close()
        else:
            sys.stderr.write("k\n")


def write_comp_standard_file(filename, errors):
    f = open(filename, 'w')
    for error in errors:
        f.write(str(error) + '\n')
    f.close()


def read_comp_standard_errors(filename):
    f = open(filename, 'r')
    lines = f.read().split('\n')
    f.close()
    result = []
    for line in lines:
        if line.strip() == "":
            continue
        if len(line) > 0 and line[0] == '#':
            continue
        terms = line.split(',', 4)
        terms = map(lambda x: x.strip(), terms)
        err = kumirutils.CompileError()
        err.line = int(terms[0][2:])
        err.pos = int(terms[1][2:])
        err.end = int(terms[2][2:])
        err.text = terms[3][2:].strip()
        result += [err]
    result.sort(key=lambda x: x.line)
    return result


def read_standard_errors(filename):
    f = open(filename, 'r')
    lines = f.read().split('\n')
    f.close()
    result = []
    for line in lines:
        if line.strip() == "":
            continue
        if len(line) > 0 and line[0] == '#':
            continue
        terms = line.split(',', 4)
        terms = map(lambda x: x.strip(), terms)
        err = kumirutils.CompileError()
        err.line = int(terms[0][2:])
        err.pos = int(terms[1][2:])
        err.end = int(terms[2][2:])
        err.text = terms[3][2:].strip()
        result += [err]
    result.sort(key=lambda x: x.line)
    return result


def write_standard_file(filename, errors):
    f = open(filename, 'w')
    for error in errors:
        f.write(str(error) + '\n')
    f.close()


def process_file(filename):
    if not filename.endswith(".kum"):
        return
    dirname, filename = os.path.split(filename)
    if dirname.startswith("../"):
        dirname = dirname[3:]
    kodfilename = "../" + dirname + "/" + filename[0:-4] + EXE_SUFFIX
    new_errors = COMPILE("../" + dirname + "/" + filename)
    gs_name = "../new_standards/" + dirname + "/" + filename + ".2"
    if os.path.exists(gs_name):
        old_errors = read_comp_standard_errors(gs_name)
        print_comp_difference("../new_standards/" + dirname + "/" + filename, old_errors, new_errors)
    else:
        out.write("No standard for " + dirname + "/" + filename + "\n")
        out.write("Suggestion:\n")
        for e in new_errors: out.write(str(e) + "\n")
        out.write("------------------\n")
    if not os.path.exists(kodfilename):
        out.write("Error creating " + kodfilename + '\n')
        return
    else:
        indata = ""
        if os.path.exists("../new_standards/" + dirname + "/" + filename + ".in"):
            f = open("../new_standards/" + dirname + "/" + filename + ".in", 'r')
            indata = f.read()
            f.close()
        sys.stderr.write("Starting: RUN(" + kodfilename + "," + indata)
        new_output, new_rterror = RUN(kodfilename, indata)

    out_gs_name = "../new_standards/" + dirname + "/" + filename + ".out"
    err_gs_name = "../new_standards/" + dirname + "/" + filename + ".rterr"

    if not os.path.exists(out_gs_name):
        out.write("No output standard for " + filename + "\n")
    else:
        f = open(out_gs_name, 'r')
        old_output = f.read()
        f.close()
        if old_output != new_output:
            print_difference(old_output, new_output, "output for " + filename)
            out.write("----------------------------\n")
    if not os.path.exists(err_gs_name):
        out.write("No runtime error standard for " + filename + "\n")
    else:
        f = open(err_gs_name, 'r')
        old_rterror = f.read()
        f.close()
        if old_rterror != new_rterror:
            print_difference(old_rterror, new_rterror, "runtime error for " + filename)
            out.write("----------------------------\n")


def chunkIt(seq, num):
    avg = len(seq) / float(num)
    out = []
    last = 0.0
    while last < len(seq):
        out.append(seq[int(last):int(last + avg)])
        last += avg
    return out


def process_dir_in_parallel(dirname, threadsCount=16):
    file_names = os.listdir("../" + dirname)
    file_names = filter(lambda name: name.endswith(".kum"), file_names)
    groups = chunkIt(file_names, threadsCount)
    processes = []
    for group in groups:
        args = ["./runtest_r.py", "--list", "--kumirdir=%s" % kumirutils.KUMIR_DIR]
        if "--llvm" in sys.argv:
            args += ["--llvm"]
        args += map(lambda x: "../" + dirname + "/" + x, group)
        # sys.stderr.write(str(args))
        proc = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        processes += [proc]
    for proc in processes:
        proc.wait()
    for proc in processes:
        out, err = proc.communicate()
        sys.stdout.write(out)
        # sys.stderr.write(err)


def process_dir(dirname):
    for filename in os.listdir("../" + dirname):
        if not filename.endswith(".kum"):
            continue
        kodfilename = "../" + dirname + "/" + filename[0:-4] + EXE_SUFFIX
        new_errors = COMPILE("../" + dirname + "/" + filename)
        gs_name = "../new_standards/" + dirname + "/" + filename + ".2"
        if "-i" in sys.argv:
            if not os.path.exists(gs_name):
                sys.stderr.write("Compile Errors Standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()
                if answer == 'y':
                    sys.stderr.write("y\n")
                    write_comp_standard_file(gs_name, new_errors)
                else:
                    sys.stderr.write("n\n")
            else:
                old_errors = read_comp_standard_errors(gs_name)
                new_standard_errors = ask_differences("../new_standards/" + dirname + "/" + filename, old_errors,
                                                      new_errors)
                if not new_standard_errors is None:
                    write_comp_standard_file(gs_name, new_standard_errors)
        else:
            if os.path.exists(gs_name):
                old_errors = read_comp_standard_errors(gs_name)
                print_comp_difference("../new_standards/" + dirname + "/" + filename, old_errors, new_errors)
            else:
                out.write("No standard for " + dirname + "/" + filename + "\n")
                out.write("Suggestion:\n")
                for e in new_errors: out.write(str(e) + "\n")
                out.write("------------------\n")

        if not os.path.exists(kodfilename):
            out.write("Error creating " + kodfilename + '\n')
            continue
        else:
            indata = ""
            if os.path.exists("../new_standards/" + dirname + "/" + filename + ".in"):
                f = open("../new_standards/" + dirname + "/" + filename + ".in", 'r')
                indata = f.read()
                f.close()
            sys.stderr.write("Starting: RUN(" + kodfilename + "," + indata)
            new_output, new_rterror = RUN(kodfilename, indata)

        out_gs_name = "../new_standards/" + dirname + "/" + filename + ".out"
        err_gs_name = "../new_standards/" + dirname + "/" + filename + ".rterr"

        if "-i" in sys.argv:
            if not os.path.exists(out_gs_name):
                sys.stderr.write(new_output + "\n")
                sys.stderr.write("Output standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()
                if answer == 'y':
                    sys.stderr.write("y\n")
                    f = open(out_gs_name, 'w')
                    f.write(new_output)
                    f.close()
                else:
                    sys.stderr.write("n\n")
            else:
                ask_difference(out_gs_name, new_output, "output for " + filename)
            if not os.path.exists(err_gs_name):
                sys.stderr.write(new_rterror + "\n")
                sys.stderr.write("Runtime error standard not exist. Create? ([y]es or [n]o): ")
                answer = _read_char()
                if answer == 'y':
                    sys.stderr.write("y\n")
                    f = open(err_gs_name, 'w')
                    f.write(new_rterror)
                    f.close()
                else:
                    sys.stderr.write("n\n")
            else:
                ask_difference(err_gs_name, new_rterror, "runtime error for " + filename)
        else:
            if not os.path.exists(out_gs_name):
                out.write("No output standard for " + filename + "\n")
            else:
                f = open(out_gs_name, 'r')
                old_output = f.read()
                f.close()
                if old_output != new_output:
                    print_difference(old_output, new_output, "output for " + filename)
                    out.write("----------------------------\n")
            if not os.path.exists(err_gs_name):
                out.write("No runtime error standard for " + filename + "\n")
            else:
                f = open(err_gs_name, 'r')
                old_rterror = f.read()
                f.close()
                if old_rterror != new_rterror:
                    print_difference(old_rterror, new_rterror, "runtime error for " + filename)
                    out.write("----------------------------\n")


if __name__ == "__main__":
    if "--llvm" in sys.argv:
        COMPILE = kumirutils.compile_to_native
        RUN = kumirutils.run_native
        if os.name == "nt":
            EXE_SUFFIX = ".exe"
        else:
            EXE_SUFFIX = ""

    if "--list" in sys.argv:
        sys.stderr.write("Running a list of files\n")
        filenames = filter(lambda x: not x.startswith("-"), sys.argv[1:])
        for filename in filenames:
            sys.stderr.write("Running %s... " % filename)
            process_file(filename)
    elif not "-i" in sys.argv and not "--debug" in sys.argv:
        process_dir_in_parallel(sys.argv[-1])
    else:
        testdirs = []
        for arg in sys.argv[1:]:
            if arg.startswith("--out="):
                out = open(arg[6:], 'w')
            elif not arg.startswith("-"):
                testdirs += [arg]
        if not testdirs:
            testdirs = TEST_DIRS
        for dirname in testdirs:
            process_dir(dirname)
    out.close()
