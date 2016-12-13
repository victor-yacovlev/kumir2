sep = str()
supports_unicode_filenames = bool()

def abspath(path: str) -> str: pass
def basename(path: str) -> str: pass
def commonpath(paths: [str]) -> str: pass
def commonprefix(paths: [str]) -> str: pass
def dirname(path: str) -> str: pass
def exists(path: str) -> bool: pass
def lexists(path: str) -> bool: pass
def expandvars(path: str) -> str: pass
def getatime(path: str) -> float: pass
def getmtime(path: str) -> float: pass
def getctime(path: str) -> float: pass
def getsize(path: str) -> int: pass
def isabs(path: str) -> bool: pass
def isfile(path: str) -> bool: pass
def isdir(path: str) -> bool: pass
def islink(path: str) -> bool: pass
def ismount(path: str) -> bool: pass
def join(path: str, *objects) -> bool: pass
def normcase(path: str) -> str: pass
def normpath(path: str) -> str: pass
def realpath(path: str) -> str: pass
def relpath(path: str, start: str = ".") -> str: pass
def samefile(path1: str, path2: str) -> bool: pass
def sameopenfile(fp1: int, fp2: int) -> bool: pass
def samestat(stat1: tuple(), stat2: tuple()) -> bool: pass
def split(path: str) -> (str, str): pass
def splitdrive(path: str) -> (str, str): pass
def splitext(path: str) -> (str, str): pass
def splitunc(path: str) -> (str, str): pass
