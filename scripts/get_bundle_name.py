import re
import subprocess
import os
import os.path
import sys

if 3 == sys.version_info.major:
    from urllib.parse import unquote
    def to_str(x):
        return x.decode("utf-8")
else:
    from urllib import unquote
    def to_str(x):
        return unicode(x)


def get_version_information(top_level_dir):
    assert isinstance(top_level_dir, str) or isinstance(top_level_dir, unicode)
    if os.path.exists(top_level_dir + os.path.sep + ".git"):
        version_info = ""
        try:
            version_info = to_str(subprocess.check_output(
                "git describe --abbrev=0 --tags --exact-match",
                shell=True,
                stderr=subprocess.PIPE
            )).strip()
        except subprocess.CalledProcessError:
            pass

        if version_info:
            return version_info

        branch_name = ""
        try:
            branch_name = to_str(subprocess.check_output(
                "git rev-parse --abbrev-ref HEAD",
                shell=True
            )).strip()
        except subprocess.CalledProcessError:
            pass
        if branch_name == "HEAD": # detached HEAD?
            branch_name = os.environ.get("CI_COMMIT_REF_NAME", "").strip()
        if not branch_name:
            branch_name = "NONE"

        hash_tag = ""
        try:
            hash_tag = to_str(subprocess.check_output(
                "git rev-parse --short=12 --verify HEAD",
                shell=True
            )).strip()
        except subprocess.CalledProcessError:
            pass
        if not hash_tag:
            hash_tag = os.environ.get("CI_COMMIT_SHA", "").strip()
        if not hash_tag:
            hash_tag = "0000000000000000000000000000000000000000"

        return branch_name + "-" + hash_tag

    else:
        dir_name = os.path.basename(top_level_dir)
        match = re.match(r"kumir2-(.+)", dir_name)
        version_info = match.group(1)

    return version_info


def main():
    version_name = get_version_information(os.getcwd())
    prefix = ""
    suffix = ""
    nl = ""
    outfile = sys.stdout
    for arg in sys.argv:
        if arg.startswith("--prefix="):
            prefix = unquote(arg[9:].replace('@', '%'))
        elif arg.startswith("--suffix="):
            suffix = unquote(arg[9:].replace('@', '%'))
        elif arg.startswith("--out="):
            outfile = open(arg[6:], 'w')
        elif "--nl" == arg:
            if os.name.startswith("nt"):
                nl = "\r\n"
            else:
                nl = "\n"
    output = prefix + version_name + suffix + nl
    outfile.write(output)
    outfile.close()


if __name__ == "__main__":
    main()
