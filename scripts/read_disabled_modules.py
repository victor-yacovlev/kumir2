import re
import subprocess
import os
import os.path
import string
import sys


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
            version_info = subprocess.check_output(
                "git rev-parse --abbrev-ref HEAD",
                shell=True
            ).strip()
    else:
        dir_name = os.path.basename(top_level_dir)
        match = re.match(r"kumir2-(.+)", dir_name)
        version_info = match.group(1)
    return version_info


def find_suitable_list_file_name(version_name):
    base = os.getcwd() + os.path.sep + "subdirs-disabled-{}.txt"
    name = base.format(version_name)
    if os.path.exists(name):
        return name
    match = re.match(r"(.+)-(alpha|beta|rc)[0-9]+", version_name)
    if match:
        version_base = match.group(1)
        name = base.format(version_base)
        if os.path.exists(name):
            return name


def main():
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
        sys.stdout.write(string.join(disabled_list))


if __name__ == "__main__":
    main()