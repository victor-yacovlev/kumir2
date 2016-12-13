from ..special_types import NoneType

import __os_path as path
name = str()
environ = dict()
environb = dict()
PRIO_PROCESS = 0
PRIO_PGRP = 1
PRIO_USER = 2
supports_bytes_environ = bool()


def ctermid() -> str : pass
def chdir(path: str) -> NoneType : pass
def fchdir(fd: int) -> NoneType : pass
def getcwd() -> str : pass
def fsencode(filename: str) -> bytes: pass
def fsdecode(filename: bytes) -> str: pass
def getenv(key: str, default = None ) -> str : pass
def getenvb(key: str, default = None ) -> bytes: pass
def get_exec_path(env: dict = None) -> [str] : pass
def getegid() -> int : pass
def geteuid() -> int : pass
def getgid() -> int : pass
def getuid() -> int : pass
def getgrouplist(user: str, group: str) -> [int]: pass
def getgroups() -> [int] : pass
def getlogin() -> str: pass
def getpgid(pid: int) -> int : pass
def getpgrp() -> int : pass
def getpid() -> int : pass
def getppid() -> int : pass
def getpriority(which: int, who: int) -> int : pass
def getresuid() -> int : pass
def getresgid() -> int : pass
def getuid() -> int : pass
def initgroups(username: str, gid: int) -> NoneType: pass
def putenv(key: str, value: str) -> NoneType: pass
def setegid(egid: int) -> NoneType : pass
def seteuid(euid: int) -> NoneType : pass
def setgid(gid: int) -> NoneType : pass
def setgroups(groups: [int]) -> NoneType : pass
def setpgrp() -> NoneType: pass
def setpgid(pid: int, pgrp: int) -> NoneType : pass
def setpriority(which: int, who: int, priority: int) -> NoneType : pass
def setregid(rgid: int, egid: int) -> NoneType : pass
def setresgid(rgid: int, egid: int, sgid: int) -> NoneType : pass
def setresuid(ruid: int, euid: int, suid: int) -> NoneType : pass
def setreuid(ruid: int, euid: int) -> NoneType : pass
def getsid(pid: int) -> int : pass
def setsid() -> NoneType : pass
def setuid(uid: int) -> NoneType : pass
def strerror(code: int) -> str : pass
def umask(mask: int) -> int : pass
def uname() -> object : pass   # TODO implement posix.uname_result structure

# TODO implement all other functions...