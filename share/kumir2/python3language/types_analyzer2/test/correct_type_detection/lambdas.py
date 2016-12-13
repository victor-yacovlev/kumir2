a = ["a", "b", "c"]  # expects: list<str>
b = map(lambda x: x + "1", a)  # expects: [ str ]
c = map(lambda x: len(x), a)  # expects: [ int ]
d = filter(lambda x: len(x) > 0, a)  # expects [ str ]