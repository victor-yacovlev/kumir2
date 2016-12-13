alist = ["a", "b", "c"]
blist = [1, 2, 3]

for entry_1 in alist:
    s_1 = entry_1  # expects: str

for entry_2_a, entry_2_b in zip(alist, blist):
    a_2 = entry_2_a  # expects: str
    b_2 = entry_2_b  # expects: int

for index_3, entry_3 in enumerate(alist):
    i_3 = index_3  # expects: int
    s_3 = entry_3  # expects: str