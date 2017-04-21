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

if "nt" == os.name:
    GIT_PATH_SEARCH = [
        r"C:\Git\bin",
        r"C:\Program Files\Git\bin",
        r"C:\Program Files (x86)\Git\bin"
    ]
else:
    GIT_PATH_SEARCH = [
        "/usr/bin",
        "/usr/local/bin"
    ]


def _add_path_env(value):
    if "nt" == os.name:
        value = '"' + value + '"'
        sep = ";"
    else:
        sep = ":"
    old_path = os.getenv('PATH')
    if old_path is None:
        new_path = value
    else:
        new_path = old_path + sep + value
    os.environ['PATH'] = new_path


for path_variant in GIT_PATH_SEARCH:
    candidate = path_variant + os.path.sep + "git"
    if "nt" == os.name:
        candidate += ".exe"
    if os.path.exists(candidate):
        _add_path_env(path_variant)
        break
    sys.stderr.write("Git executable not found!\n")
    sys.exit(1)


def get_version_information(top_level_dir):
    assert isinstance(top_level_dir, str) or isinstance(top_level_dir, unicode)
    result = {
        "taggedRelease": True,
        "version": None,
        "hash": None,
        "branch": None,
        "date": None
    }
    if os.path.exists(top_level_dir + os.path.sep + ".git"):
        try:
            version_info = subprocess.check_output(
                "git describe --abbrev=0 --tags --exact-match",
                shell=True,
                stderr=subprocess.PIPE
            ).strip()
            result["version"] = to_str(version_info)
        except subprocess.CalledProcessError:
            result["taggedRelease"] = False
            branch_name = to_str(subprocess.check_output(
                "git rev-parse --abbrev-ref HEAD",
                shell=True
            ).strip())
            result["branch"] = branch_name
            git_hash = to_str(subprocess.check_output(
                "git --no-pager log -1 --pretty=format:%H",
                shell=True
            ).strip())
            result["hash"] = git_hash
            result["date"] = get_date(top_level_dir)
    else:
        dir_name = os.path.basename(top_level_dir)
        match = re.match(r"kumir2-(.+)", dir_name)
        version_info = match.group(1)
        if version_info.startswith("2"):
            result["version"] = version_info
        else:
            result["taggedRelease"] = False
            result["date"] = get_date(top_level_dir)
    return result


def get_date(top_level_dir):
    timestamp = int(get_timestamp(top_level_dir))
    localtime = time.localtime(timestamp)
    assert isinstance(localtime, time.struct_time)
    return "{:04}{:02}{:02}".format(localtime.tm_year, localtime.tm_mon, localtime.tm_mday)


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


def find_suitable_list_file_name(version_info):
    base = os.getcwd() + os.path.sep + "subdirs-disabled-{}.txt"
    if version_info["taggedRelease"]:
        name = base.format(version_info["version"])
    else:
        name = base.format(version_info["branch"])
    if os.path.exists(name):
        return name
    if version_info["taggedRelease"]:
        match = re.match(r"(.+)-(alpha|beta|rc|pt|test)[0-9]+", version_info["version"])
        version_base = match.group(1)
    else:
        version_base = version_info["branch"]
    if version_base:
        name = base.format(version_base)
        if os.path.exists(name):
            return name


def disabled_modules():
    version_info = get_version_information(os.getcwd())
    disabled_list_file_name = find_suitable_list_file_name(version_info)
    disabled_list = []
    if disabled_list_file_name:
        with open(disabled_list_file_name) as source:
            lines = source.read().split('\n')
            for line in lines:
                line = line.strip()
                if line and not line.startswith("#"):
                    disabled_list += [line]
    return disabled_list


def cmake_disabled_modules():
    mods = ["[" + entry + "]" for entry in disabled_modules()]
    if mods:
        return " ".join(mods)
    else:
        return ""


def cmake_version_info():
    version_name = get_version_information(os.getcwd())
    assert isinstance(version_name, dict)
    timestamp = get_timestamp(os.getcwd())
    output = ""
    if version_name["taggedRelease"]:
        output += "-DGIT_TAG=\"{}\";".format(version_name["version"])
        output += "-DGIT_BRANCH=\"unknown\";"
        output += "-DGIT_HASH=\"unknown\";"
    else:
        output += "-DGIT_TAG=\"unknown\";"
        output += "-DGIT_BRANCH=\"{}\";".format(version_name["branch"])
        output += "-DGIT_HASH=\"{}\";".format(version_name["hash"])
    output += "-DGIT_TIMESTAMP=\"{}\";".format(timestamp)
    return output


def source_file_name(prefix: str, suffix: str):
    version_info = get_version_information(os.getcwd())
    if version_info["taggedRelease"]:
        version_name = version_info["version"]
    else:
        version_name = version_info["branch"] + "-"
        version_name += version_info["hash"] + "-"
        version_name += version_info["date"]
    return prefix + version_name + suffix


def package_bundle_name():
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
    output = source_file_name(prefix, suffix) + nl
    return output


def nsis_include_file():
    version_info = get_version_information(os.getcwd())
    data = ""
    if version_info["taggedRelease"]:
        data += "OutFile \"kumir2-" + version_info["version"] + "-install.exe\"\r\n"
        data += "Name \"Кумир " + version_info["version"] + "\"\r\n"
        data += "InstallDir \"$PROGRAMFILES\\Kumir-" + version_info["version"] + "\"\r\n"
        data += "!define VERSION_SUFFIX \"" + version_info["version"] + "\"\r\n"
    else:
        data += "OutFile \"kumir2-" + version_info["branch"] + "-"
        data += version_info["date"] + "-" + version_info["hash"] + "-install.exe\"\r\n"
        data += "Name \"Кумир 2.x-" + version_info["branch"] + "\"\r\n"
        data += "InstallDir \"$PROGRAMFILES\\Kumir2x-" + version_info["branch"] + "\"\r\n"
        data += "!define VERSION_SUFFIX \"" + version_info["branch"] + "\"\r\n"
    return data


def main():
    global OUT_FILE
    mode = "package_bundle_name"
    out_file_name = None
    for arg in sys.argv:
        if arg.startswith("--mode="):
            mode = arg[7:]
        elif arg.startswith("--out="):
            out_file_name = arg[6:]
    custom_encoding = False
    if out_file_name:
        if mode.startswith("nsis"):
            open_mode = "wb"
            custom_encoding = "CP1251"
        else:
            open_mode = "w"
        OUT_FILE = open(out_file_name, open_mode)
    if mode in globals():
        data = globals()[mode]()
        if custom_encoding:
            data = data.encode(custom_encoding)
        OUT_FILE.write(data)
    OUT_FILE.close()


if __name__ == "__main__":
    main()
