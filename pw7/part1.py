import sys
sys.setrecursionlimit(100000)

# --- Примитивные функции ---

def zero(x):
    return 0

def succ(x):
    return x + 1

# --- Суперпозиции и примитивные рекурсии ---

def add(x, y):
    if y == 0:
        return x
    return succ(add(x, y - 1))

def mul(x, y):
    if y == 0:
        return 0
    return add(x, mul(x, y - 1))

def pow_rec(a, b):
    if b == 0:
        return 1
    return mul(a, pow_rec(a, b - 1))

def fact(x):
    if x == 0:
        return 1
    return mul(x, fact(x - 1))

def f(x):
    """Искомая функция f(x) = 2^(x!)"""
    return pow_rec(2, fact(x))

def main():
    # --- Примеры вычислений ---
    for i in range(4):
        print(f"f({i}) = 2^({i}!) = {f(i)}")

if __name__ == "__main__":
    main()
