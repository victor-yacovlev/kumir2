import re
import subprocess
import os
import os.path
import sys
import time

if 3 == sys.version_info.major:
    from urllib.parse import unquote

    def to_str(x):
        assert isinstance(x, str) or isinstance(x, bytes)
        if isinstance(x, str):
            return x
        else:
            return x.decode("utf-8")
else:
    from urllib import unquote

    def to_str(x):
        assert isinstance(x, str) or isinstance(x, unicode)
        return unicode(x)

OUT_FILE = sys.stdout


def get_version_information(top_level_dir):
    assert isinstance(top_level_dir, str) or isinstance(top_level_dir, unicode)
    if os.path.exists(top_level_dir + os.path.sep + ".git"):
        try:
            version_info = subprocess.check_output(
                "git describe --abbrev=0 --tags --exact-match",
                shell=True,
                stderr=subprocess.PIPE
            ).strip()
        except subprocess.CalledProcessError:
            version_info = to_str(subprocess.check_output(
                "git rev-parse --abbrev-ref HEAD",
                shell=True
            ).strip())
            version_info += "-" + to_str(subprocess.check_output(
                "git --no-pager log -1 --pretty=format:%H",
                shell=True
            ).strip())
    else:
        dir_name = os.path.basename(top_level_dir)
        match = re.match(r"kumir2-(.+)", dir_name)
        version_info = match.group(1)
    return to_str(version_info)


def get_timestamp(top_level_dir):
    assert isinstance(top_level_dir, str) or isinstance(top_level_dir, unicode)
    if os.path.exists(top_level_dir + os.path.sep + ".git"):
        return to_str(subprocess.check_output(
            "git --no-pager log -1 --pretty=format:%ct",
            shell=True,
            stderr=subprocess.PIPE
        )).strip()
    else:
        return "{}".format(int(time.time()))


def is_tag(version):
    return version.startswith("2")


def find_suitable_list_file_name(version_name):
    base = os.getcwd() + os.path.sep + "subdirs-disabled-{}.txt"
    name = base.format(version_name)
    if os.path.exists(name):
        return name
    match = re.match(r"(.+)-(alpha|beta|rc|pt|test)[0-9]+", version_name)
    if match:
        version_base = match.group(1)
        name = base.format(version_base)
        if os.path.exists(name):
            return name


def cmake_disabled_modules():
    version_name = get_version_information(os.getcwd())
    disabled_list_file_name = find_suitable_list_file_name(version_name)
    disabled_list = []
    if disabled_list_file_name:
        with open(disabled_list_file_name) as source:
            lines = source.read().split('\n')
            for line in lines:
                line = line.strip()
                if line and not line.startswith("#"):
                    disabled_list += ["[" + line + "]"]
    if disabled_list:
        OUT_FILE.write(" ".join(disabled_list))


def cmake_version_info():
    version_name = get_version_information(os.getcwd())
    timestamp = get_timestamp(os.getcwd())
    if is_tag(version_name):
        OUT_FILE.write("-DGIT_TAG=\"{}\";".format(version_name))
        OUT_FILE.write("-DGIT_BRANCH=\"unknown\";")
        OUT_FILE.write("-DGIT_HASH=\"unknown\";")
    else:
        OUT_FILE.write("-DGIT_TAG=\"unknown\";")
        branch, ghash = version_name.split("-")
        OUT_FILE.write("-DGIT_BRANCH=\"{}\";".format(branch))
        OUT_FILE.write("-DGIT_HASH=\"{}\";".format(ghash))
    OUT_FILE.write("-DGIT_TIMESTAMP=\"{}\";".format(timestamp))


def package_bundle_name():
    version_name = get_version_information(os.getcwd())
    prefix = ""
    suffix = ""
    nl = ""
    for arg in sys.argv:
        if arg.startswith("--prefix="):
            prefix = unquote(arg[9:].replace('@', '%'))
        elif arg.startswith("--suffix="):
            suffix = unquote(arg[9:].replace('@', '%'))
        elif "--nl" == arg:
            if os.name.startswith("nt"):
                nl = "\r\n"
            else:
                nl = "\n"
    output = prefix + version_name + suffix + nl
    OUT_FILE.write(output)


def main():
    global OUT_FILE
    mode = "package_bundle_name"
    for arg in sys.argv:
        if arg.startswith("--mode="):
            mode = arg[7:]
        elif arg.startswith("--out="):
            OUT_FILE = open(arg[6:], 'w')
    if mode in globals():
        globals()[mode]()
    OUT_FILE.close()


if __name__ == "__main__":
    main()
