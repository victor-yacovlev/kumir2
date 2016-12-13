import abc

a = abc  # expects: module
b = abc.__builtins__  # expects: { str: object }
c = abc.__cached__  # expects: str
d = abc.__doc__  # expects: str
e = abc.__loader__  # expects: object
f = abc.__name__  # expects: str
g = abc.__package__  # expects: str
h = abc.__spec__  # expects: object
