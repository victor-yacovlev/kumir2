#Built-in functions (using callable as one of argument) calls
filter_1 = filter(callable, [ascii])  # expects: [ _typed_callable<(object) -> str> ]
filter_2 = filter(callable, [ascii, bin])  # expects: [ _callable ]
filter_3 = filter(callable, [chr, hex])  # expects: [ _typed_callable<(int) -> str> ]

map_1 = map(chr, [1, 2, 3])  # expects: [ str ]
map_2 = map(abs, [1, 2, 3])  # expects: [ int ]
map_3 = map(abs, [0.5, 0.7])  # expects: [ float ]
map_4 = map(abs, [1, 0.5])  # expects: [ _numeric ]