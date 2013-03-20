import os
import re
import shutil
import subprocess
import sys


def print_usage_and_exit():
    sys.stderr.write("Usage: %s TARGET_ROOT [CUSTOM_BUILD_SDK_ROOT]\n" % sys.argv[0])
    sys.stderr.write("    TARGET_ROOT            a directory containing kumir dist\n")
    sys.stderr.write("    CUSTOM_BUILD_SDK_ROOT  a directory containing SDK libraries\n")
    sys.stderr.write("                           (default: /opt/sdk)\n\n")
    sys.exit(127)


def run_ldd(filename):
    out = subprocess.check_output(["ldd", filename])
    lines = out.split('\n')
    result = []
    rx = re.compile(r"^.+\s=>\s(.+)\s\(.+\)$")
    for line in lines:
        match = rx.match(line)
        if match:
            target = match.group(1)
            result += [target]
    return result


def deploy_library(filepath, target_dir, lib):
    filedir, filename = os.path.split(lib)
    target_filename = os.path.join(target_dir, filename)
    if not os.path.exists(target_filename):
        sys.stderr.write("  - copying %s to %s\n" % (lib, target_filename))
        shutil.copyfile(lib, target_filename)


def run_chrpath(filename, target_dir):
    filedir, name = os.path.split(filename)
    rpath = os.path.relpath(target_dir, filedir) + "/"
    sys.stderr.write("  - changing rpath to %s\n" % rpath)
    subprocess.call(["chrpath", "-r", rpath, filename])


def process_file(filename, target_dir, sdk_root):
    sys.stderr.write("Processing %s\n" % filename)
    dependencies = run_ldd(filename)
    libs_to_deploy = filter(lambda x: x.startswith(sdk_root), dependencies)
    for lib in libs_to_deploy:
        deploy_library(filename, target_dir, lib)
    run_chrpath(filename, target_dir)


def process_dir(dirname, suffix, target_dir, sdk_root):
    files = os.listdir(dirname)
    for filename in files:
        if filename[0] != '.' and filename.endswith(suffix):
            process_file(dirname + "/" + filename, target_dir, sdk_root)


def process_root(target_root, sdk_root):
    if os.path.exists(target_root + "/lib/kumir2"):
        process_dir(target_root + "/bin", "", target_root + "/lib/kumir2", sdk_root)
    elif os.path.exists(target_root + "/lib64/kumir2"):
        process_dir(target_root + "/bin", "", target_root + "/lib64/kumir2", sdk_root)
    if os.path.exists(target_root + "/lib/kumir2"):
        process_dir(target_root + "/lib/kumir2", ".so", target_root + "/lib/kumir2", sdk_root)
    if os.path.exists(target_root + "/lib64/kumir2"):
        process_dir(target_root + "/lib64/kumir2", ".so", target_root + "/lib64/kumir2", sdk_root)
    if os.path.exists(target_root + "/lib/kumir2/plugins"):
        process_dir(target_root + "/lib/kumir2/plugins", ".so", target_root + "/lib/kumir2", sdk_root)
    if os.path.exists(target_root + "/lib64/kumir2/plugins"):
        process_dir(target_root + "/lib64/kumir2/plugins", ".so", target_root + "/lib64/kumir2", sdk_root)

if __name__ == "__main__":
    if "--help" in sys.argv or len(sys.argv) == 1:
        print_usage_and_exit()
    target_root = sys.argv[1]
    if len(sys.argv) > 2:
        sdk_root = sys.argv[2]
    else:
        sdk_root = "/opt/sdk"
    process_root(target_root, sdk_root)
