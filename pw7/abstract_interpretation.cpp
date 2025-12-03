#include <iostream>
#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

const string PLUS = "plus";
const string MINUS = "minus";
const string ZERO = "zero";
const string UNKNOWN = "unknown";

// Глобальная карта для хранения знаков переменных
map<string, string> signs;

using TransitionTable = string[4][4];

// Таблица сложения 
const TransitionTable ADD_TABLE = {
    //           PLUS    MINUS    ZERO    UNKNOWN
    /* PLUS */  {PLUS,   UNKNOWN, PLUS,   UNKNOWN},
    /* MINUS */ {UNKNOWN, MINUS,  MINUS,  UNKNOWN},
    /* ZERO */  {PLUS,   MINUS,   ZERO,   UNKNOWN},
    /* UNKNOWN */{UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN}
};

// Таблица вычитания
const TransitionTable SUB_TABLE = {
    //           PLUS     MINUS   ZERO    UNKNOWN
    /* PLUS */  {UNKNOWN, PLUS,   PLUS,   UNKNOWN},
    /* MINUS */ {MINUS,  UNKNOWN, MINUS,  UNKNOWN},
    /* ZERO */  {MINUS,  PLUS,    ZERO,   UNKNOWN},
    /* UNKNOWN */{UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN}
};

// Таблица умножения
const TransitionTable MUL_TABLE = {
    //           PLUS    MINUS   ZERO    UNKNOWN
    /* PLUS */  {PLUS,   MINUS,  ZERO,   UNKNOWN},
    /* MINUS */ {MINUS,  PLUS,   ZERO,   UNKNOWN},
    /* ZERO */  {ZERO,   ZERO,   ZERO,   ZERO},
    /* UNKNOWN */{UNKNOWN, UNKNOWN, ZERO,  UNKNOWN}
};

// Таблица деления
const TransitionTable DIV_TABLE = {
    //           PLUS    MINUS   ZERO     UNKNOWN
    /* PLUS */  {PLUS,   MINUS,  UNKNOWN, UNKNOWN},
    /* MINUS */ {MINUS,  PLUS,   UNKNOWN, UNKNOWN},
    /* ZERO */  {ZERO,   ZERO,   UNKNOWN, UNKNOWN},
    /* UNKNOWN */{UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN}
};

// Вспомогательная функция для получения индекса
int get_state_index(const string& s) {
    if (s == PLUS) return 0;
    if (s == MINUS) return 1;
    if (s == ZERO) return 2;
    return 3; // UNKNOWN
}

// Абстрактные правила для операций
string abstract_combine(const string& x, const string& y, char op) {
    int idx_x = get_state_index(x);
    int idx_y = get_state_index(y);

    switch (op) {
        case '+':
            return ADD_TABLE[idx_x][idx_y];
        case '-':
            return SUB_TABLE[idx_x][idx_y];
        case '*':
            return MUL_TABLE[idx_x][idx_y];
        case '/':
            return DIV_TABLE[idx_x][idx_y];
        default:
            return UNKNOWN;
    }
}


/* Вспомогательные функции для работы со строками и числами */

// Определяет знак целого числа
string get_sign_of_digit(int value) {
    if (value > 0) {
        return PLUS;
    } else if (value < 0) {
        return MINUS;
    } else {
        return ZERO;
    }
}

// Проверяет, является ли строка числом (с учетом знака)
bool is_digit(const string& value) {
    if (value.empty()) {
        return false;
    }
    string s = value;
    size_t start = 0;
    if (s[0] == '+' || s[0] == '-') {
        start = 1;
    }
    return !s.substr(start).empty() && all_of(s.begin() + start, s.end(), ::isdigit);
}

// Проверяет сбалансированность скобок
bool check_brackets(const string& expr) {
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
string determine_sign(const string& expr) {
    string clean_expr = expr;
    clean_expr.erase(remove_if(clean_expr.begin(), clean_expr.end(), ::isspace), clean_expr.end());

    if (clean_expr.empty()) {
        return UNKNOWN;
    }

    if (is_digit(clean_expr)) {
        try {
            return get_sign_of_digit(stoi(clean_expr));
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

    string operators = "+-*/";
    char main_operator = '\0';
    int index_of_main = -1;

    for (int i = clean_expr.length() - 1; i >= 0; --i) {
        char c = clean_expr[i];

        if (c == ')') {
            depth++;
        } else if (c == '(') {
            depth--;
        } else if (depth == 0 && operators.find(c) != string::npos) {
            bool is_unary = false;
            
            if (i == 0) {
                is_unary = true;
            } 
            else {
                char prev = clean_expr[i - 1];
                if (operators.find(prev) != string::npos || prev == '(') {
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

    string left = clean_expr.substr(0, index_of_main);
    string right = clean_expr.substr(index_of_main + 1);

    string x = determine_sign(left);
    string y = determine_sign(right);

    return abstract_combine(x, y, main_operator);
}

// Основная функция анализа
void analyze(const string& procedure) {
    signs.clear();
    stringstream ss(procedure);
    string line;

    while (getline(ss, line, '\n')) {
        line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
        if (line.empty() || line.find('=') == string::npos) {
            continue;
        }

        size_t eq_pos = line.find('=');
        string left = line.substr(0, eq_pos);
        string right = line.substr(eq_pos + 1);

        string var = left;
        string expr = right;

        signs[var] = determine_sign(expr);
    }

    cout << "\nProcedure:" << procedure << endl;
    cout << "Signs of variables:" << endl;
    for (const auto& pair : signs) {
        cout << pair.first << ": " << pair.second << endl;
    }
}

int main() {
    string procedure = R"(
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