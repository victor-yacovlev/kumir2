import stat
a = stat.S_IFBLK  # expects: int
b = stat.S_ISLNK(123)  # expects: bool
