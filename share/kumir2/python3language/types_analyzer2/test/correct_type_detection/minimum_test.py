# Scalar values assignment
i = 1  # expects: int
r = 1.4  # expects: float
b = True  # expects: bool
n = None  # expects: NoneType
s = "Hello"  # expects: str

# List value assignments
alist = []  # expects: list
ilist = [1, 2]  # expects: list<int>
rlist = [1.1, 1.2]  # expects: list<float>
nlist = [1, 2.5]  # expects: list<_numeric>
vlist = [1, None]  # expects: list

# Tuples assignment
x, y = 1, 2  # expects: x: int, y: int
xx, yy = r, i  # expects: xx: float, yy: int

# Arithmetics operations
i_plus_i = 1 + 2  # expects: int
i_plus_r = 1 + r  # expects: float
s_mul_i = s * 2  # expects: str

# Value methods call
sl = s.lower()  # expects: str
sil = s.islower()  # expects: bool
