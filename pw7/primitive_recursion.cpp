#include <iostream>
#include <limits>

const int MAX_VAL = std::numeric_limits<int>::max();
const int MIN_VAL = 0;

/* Примитивные функции */

// Функция обнуление
int z(int x){
    return 0;
}

// Функция последователь
int s(int x){
    return x + 1;
}


/* Суперпозиции примитивных функци */

// Функция предшественник
int pred(int x) {
    if (x == 0) {
        return 0;
    }
    return x - 1;
}

// Функция сумма
int add(int x, int y){
    if (y == 0) {
        return x;
    }
    return s(add(x, pred(y)));
}

// Функция умножение
int mult(int x, int y){
    if (y == 0) {
        return 0;
    }
    return add(x, mult(x, pred(y)));
}

// Функция факториал
int fact(int x){
    if (x == 0) {
        return 1;
    }
    return mult(x, fact(pred(x)));
}

// Функция возведение в степень
int pow(int x, int y){
    if (y == 0){
        return 1;
    }
    return mult(x, pow(x, pred(y)));
}

// Основная функция задания
int f(int x){
    if (x < MIN_VAL) {
        return -1;
    }
    return pow(2, fact(x));
}

int main(int argc, char* argv[]) {
    std::cout << "f(x) = 2^x! " << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << "f(" << i << ") = " << f(i) << std::endl;
    }

}