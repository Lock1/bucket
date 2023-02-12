# Miller-Rabin prime test
# n < 2047,   a = 2
# n <= 1 mil, a = 2, 3
# n <= 2^32,  a = 2, 7, 61

def sprp(a, n):
    if not (n & 1):
        return False
    s = 0
    while not (((n-1) >> s) & 1):
        s += 1
    d = (n-1) >> s
    return pow(a, d, n) == 1 or any([pow(a, d*(1 << r), n) == n-1 for r in range(s)])

