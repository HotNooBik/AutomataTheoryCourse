#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

const std::string PLUS = "plus";
const std::string MINUS = "minus";
const std::string ZERO = "zero";
const std::string UNKNOWN = "unknown";

// Глобальная карта для хранения знаков переменных
std::map<std::string, std::string> signs;


// Абстрактные правила для операций
std::string abstract_combine(const std::string& x, const std::string& y, char op) {
    if (x == UNKNOWN || y == UNKNOWN) {
        return UNKNOWN;
    }

    if (op == '+') {
        if (x == y) return x;
        if (x == ZERO) return y;
        if (y == ZERO) return x;
        return UNKNOWN;
    }

    if (op == '-') {
        if (x == PLUS && y == MINUS) return PLUS;
        if (x == MINUS && y == PLUS) return MINUS;
        if (y == ZERO) return x;
        if (x == ZERO && y == PLUS) return MINUS;
        if (x == ZERO && y == MINUS) return PLUS;
        if (x == y) return ZERO;
        return UNKNOWN;
    }

    if (op == '*') {
        if (x == y) return PLUS;
        if (x == ZERO || y == ZERO) return ZERO;
        return MINUS;
    }

    if (op == '/') {
        if (x == y) return PLUS;
        if (y == ZERO) return UNKNOWN;
        if (x == ZERO) return ZERO;
        return MINUS;
    }

    return UNKNOWN;
}


/* Вспомогательные функции для работы со строками и числами */

// Определяет знак целого числа
std::string get_sign_of_digit(int value) {
    if (value > 0) {
        return PLUS;
    } else if (value < 0) {
        return MINUS;
    } else {
        return ZERO;
    }
}

// Проверяет, является ли строка числом (с учетом знака)
bool is_digit(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    std::string s = value;
    size_t start = 0;
    if (s[0] == '+' || s[0] == '-') {
        start = 1;
    }
    return !s.substr(start).empty() && std::all_of(s.begin() + start, s.end(), ::isdigit);
}

// Проверяет сбалансированность скобок
bool check_brackets(const std::string& expr) {
    int depth = 0;
    for (char c : expr) {
        if (c == '(') {
            depth++;
        } else if (c == ')') {
            depth--;
        }
        if (depth < 0) {
            return false;
        }
    }
    return depth == 0;
}

// Рекурсивное вычисление знаков выражений
std::string determine_sign(const std::string& expr) {
    std::string clean_expr = expr;
    clean_expr.erase(std::remove_if(clean_expr.begin(), clean_expr.end(), ::isspace), clean_expr.end());

    if (clean_expr.empty()) {
        return UNKNOWN;
    }

    if (is_digit(clean_expr)) {
        try {
            return get_sign_of_digit(std::stoi(clean_expr));
        } catch (...) {
            return UNKNOWN;
        }
    }

    if (signs.count(clean_expr)) {
        return signs[clean_expr];
    }

    if (clean_expr.front() == '(' && clean_expr.back() == ')' && check_brackets(clean_expr.substr(1, clean_expr.length() - 2))) {
        return determine_sign(clean_expr.substr(1, clean_expr.length() - 2));
    }

    int depth = 0;

    std::string operators = "+-*/";
    char main_operator = '\0';
    int index_of_main = -1;

    for (int i = clean_expr.length() - 1; i >= 0; --i) {
        char c = clean_expr[i];

        if (c == ')') {
            depth++;
        } else if (c == '(') {
            depth--;
        } else if (depth == 0 && operators.find(c) != std::string::npos) {
            bool is_unary = false;
            
            if (i == 0) {
                is_unary = true;
            } 
            else {
                char prev = clean_expr[i - 1];
                if (operators.find(prev) != std::string::npos || prev == '(') {
                    is_unary = true;
                }
            }

            if (is_unary) {
                continue;
            }

            bool current_is_low_prio = (c == '+' || c == '-');
            bool existing_is_high_prio = (main_operator == '*' || main_operator == '/');
            
            if (main_operator == '\0') {
                main_operator = c;
                index_of_main = i;
            } else {
                if (current_is_low_prio) {
                    main_operator = c;
                    index_of_main = i;
                    break; 
                }
            }
        }
    }

    if (main_operator == '\0' || index_of_main == -1) {
        return UNKNOWN;
    }

    std::string left = clean_expr.substr(0, index_of_main);
    std::string right = clean_expr.substr(index_of_main + 1);

    std::string x = determine_sign(left);
    std::string y = determine_sign(right);

    return abstract_combine(x, y, main_operator);
}

// Основная функция анализа
void analyze(const std::string& procedure) {
    signs.clear();
    std::stringstream ss(procedure);
    std::string line;

    while (std::getline(ss, line, '\n')) {
        line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line.find('=') == std::string::npos) {
            continue;
        }

        size_t eq_pos = line.find('=');
        std::string left = line.substr(0, eq_pos);
        std::string right = line.substr(eq_pos + 1);

        std::string var = left;
        std::string expr = right;

        signs[var] = determine_sign(expr);
    }

    std::cout << "\nProcedure:" << procedure << std::endl;
    std::cout << "Signs of variables:" << std::endl;
    for (const auto& pair : signs) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }
}

int main() {
    std::string procedure = R"(
a = 1
b = -1
c = a + b
d = a - b
e = d * d + a
f = 3
f = (f + e) * (b - d)
g = c + c
h = b - (0 - f)
j = -2 / -2
k = (h - j) / (b - 0)
)";
    analyze(procedure);
    return 0;
}