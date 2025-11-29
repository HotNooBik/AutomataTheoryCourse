plus = "plus"
minus = "minus"
zero = "zero"
unknown = "unknown"

signs = {}

# --- Проверка и определение знаков чисел ---
def is_digit(value: str):
    try:
        int(value)
        return True
    except ValueError:
        return False

def sign_of_digit(value: int):
    if value > 0:
        return plus
    elif value < 0:
        return minus
    else:
        return zero


# --- Абстрактные правила для операций ---
def combine_signs(x, y, op):
    # Если хоть одно неизвестно
    if x == unknown or y == unknown:
        return unknown

    if op == "+":
        if x == zero:
            return y
        if y == zero:
            return x
        if x == y:
            return x
        return unknown

    if op == "-":
        if y == zero:
            return x
        if x == y:
            return unknown
        if x == plus and y == minus:
            return plus
        if x == minus and y == plus:
            return minus
        return unknown

    if op == "*":
        if x == zero or y == zero:
            return zero
        if (x == plus and y == plus) or (x == minus and y == minus):
            return plus
        if (x == plus and y == minus) or (x == minus and y == plus):
            return minus
        return unknown

    if op == "/":
        if y == zero:
            return unknown  # деление на ноль не определено
        if x == zero:
            return zero
        if (x == plus and y == plus) or (x == minus and y == minus):
            return plus
        if (x == plus and y == minus) or (x == minus and y == plus):
            return minus
        return unknown

    return unknown


# --- Рекурсивное вычисление знаков выражений ---
def determine_sign(expr):
    expr = expr.strip().replace(" ", "")

    # если это число
    if is_digit(expr):
        return sign_of_digit(int(expr))

    # если это просто переменная
    if expr in signs:
        return signs[expr]

    # если в скобках целиком
    if expr.startswith("(") and expr.endswith(")") and check_brackets(expr[1:-1]):
        return determine_sign(expr[1:-1])

    # ищем главную операцию (с учётом скобок)
    depth = 0
    main_op = None
    main_index = -1
    for i in range(len(expr) - 1, -1, -1):  # справа налево (чтобы + и - имели меньший приоритет)
        c = expr[i]
        if c == ')':
            depth += 1
        elif c == '(':
            depth -= 1
        elif depth == 0 and c in "+-*/":
            main_op = c
            main_index = i
            break

    if main_op is None:
        return unknown

    left = expr[:main_index]
    right = expr[main_index + 1:]
    x = determine_sign(left)
    y = determine_sign(right)
    return combine_signs(x, y, main_op)


def check_brackets(expr):
    """Проверяет сбалансированность скобок"""
    depth = 0
    for c in expr:
        if c == '(':
            depth += 1
        elif c == ')':
            depth -= 1
            if depth < 0:
                return False
    return depth == 0


# --- Основная функция анализа ---
def analyze(procedure):
    lines = [line.strip() for line in procedure.split("\n") if line.strip()]
    for line in lines:
        if "=" in line:
            left, right = line.split("=", 1)
            var = left.strip()
            expr = right.strip()
            signs[var] = determine_sign(expr)

    print("\nПроцедура:")
    print(procedure)
    print("\nЗнаки переменных:")
    for var, s in signs.items():
        print(f"{var}: {s}")


# --- Пример использования ---
def main():
    # пример процедуры
    procedure = """
a = 5
b = -2
c = a + b
d = a - b
e = a * b
f = d + e
g = f - b
h = g * 2
j = (a - b) * (b + 3)
k = (a + e) / b
    """
    analyze(procedure)


if __name__ == "__main__":
    main()
