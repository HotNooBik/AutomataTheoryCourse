#include <iostream>
#include <limits>

const int MAX_VAL = std::numeric_limits<int>::max();
const int MIN_VAL = 0;

using pos = unsigned int;

/* Примитивные функции */

// Функция обнуление
pos z(pos x){
    return 0;
}

// Функция последователь
pos s(pos x){
    return x + 1;
}


/* Суперпозиции примитивных функци */

// Функция предшественник
pos pred(pos x) {
    if (x == z(x)) {
        return z(x);
    }
    return x - 1;
}

// Функция сумма
pos add(pos x, pos y){
    if (y == z(y)) {
        return x;
    }
    return s(add(x, pred(y)));
}

// Функция умножение
pos mult(pos x, pos y){
    if (y == z(y)) {
        return z(x);
    }
    return add(x, mult(x, pred(y)));
}

// Функция факториал
pos fact(pos x){
    if (x == z(x)) {
        return 1;
    }
    return mult(x, fact(pred(x)));
}

// Функция возведение в степень
pos pow(pos x, pos y){
    if (y == z(y)){
        return 1;
    }
    return mult(x, pow(x, pred(y)));
}

// Основная функция задания
pos f(pos x){
    if (x < MIN_VAL) {
        return -1;
    }
    return pow(2, fact(x));
}

int main(int argc, char* argv[]) {
    std::cout << "f(x) = 2^x! " << std::endl;
    for (pos i = 0; i < 4; i++) {
        std::cout << "f(" << i << ") = " << f(i) << std::endl;
    }

}