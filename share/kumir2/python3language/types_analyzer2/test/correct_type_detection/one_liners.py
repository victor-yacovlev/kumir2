# if-expressions
a = 3 if True else 5  # expects: int
b = 3 if True else 5.0  # expects: _numeric
c = 3 if True else "aasdf"  # expects: object

# list comprehetions
numbers = [i for i in range(0, 10)]  # expects list<int>
pairs = [(x,y) for x in range(10) for y in range(5)]  # expects list<(int, int)>
prods = [x*y for x in range(10) for y in range(5)]  # expects list<int>
prods_non_zero = [x*y for x in range(10) for y in range(5) if x>0 and y>0]  # expects list<int>
two_dim = [[y for y in range(5)] for _ in range(4)]  # expects list<list<int>>