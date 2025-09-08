#include <iostream>
#include <array>
#include <string>

enum RESULT {
    UNKNOWN_SYMBOL_ERR      = 0,
    NOT_REACHED_FINAL_STATE = 1,
    REACHED_FINAL_STATE     = 2
};

// The set Q
enum DFA_STATES {
    q00000, q00001, q00010, q00011, q00100, q00101, q00110, q00111, 
    q01000, q01001, q01010, q01011, q01100, q01101, q01110, q01111, 
    q10000, q10001, q10010, q10011, q10100, q10101, q10110, q10111, 
    q11000, q11001, q11010, q11011, q11100, q11101, q11110, q11111,
};

constexpr int TOTAL_STATES = 32;
constexpr int ACCEPTED_STATES = 16;
constexpr int ALPHABET_CHARCTERS = 2;

// The set F
std::array<int, ACCEPTED_STATES> g_Accepted_states { 
    q10000, q10001, q10010, q10011, q10100, q10101, q10110, q10111, 
    q11000, q11001, q11010, q11011, q11100, q11101, q11110, q11111 
};

// The set Sigma
std::array<char, ALPHABET_CHARCTERS> g_alphabet { '0', '1' };

// Transition function
int g_Transition_Table[TOTAL_STATES][ALPHABET_CHARCTERS] = {};

//Start state of DFA
int g_Current_state = q00000;

void SetDFA_Transitions() {
    g_Transition_Table[q00000][0] = q00000; 
    g_Transition_Table[q00000][1] = q00001;
    g_Transition_Table[q00001][0] = q00010; 
    g_Transition_Table[q00001][1] = q00011;
    g_Transition_Table[q00010][0] = q00100; 
    g_Transition_Table[q00010][1] = q00101;
    g_Transition_Table[q00011][0] = q00110; 
    g_Transition_Table[q00011][1] = q00111;
    g_Transition_Table[q00100][0] = q01000; 
    g_Transition_Table[q00100][1] = q01001;
    g_Transition_Table[q00101][0] = q01010; 
    g_Transition_Table[q00101][1] = q01011;
    g_Transition_Table[q00110][0] = q01100; 
    g_Transition_Table[q00110][1] = q01101;
    g_Transition_Table[q00111][0] = q01110; 
    g_Transition_Table[q00111][1] = q01111;
    g_Transition_Table[q01000][0] = q10000; 
    g_Transition_Table[q01000][1] = q10001;
    g_Transition_Table[q01001][0] = q10010; 
    g_Transition_Table[q01001][1] = q10011;
    g_Transition_Table[q01010][0] = q10100; 
    g_Transition_Table[q01010][1] = q10101;
    g_Transition_Table[q01011][0] = q10110; 
    g_Transition_Table[q01011][1] = q10111;
    g_Transition_Table[q01100][0] = q11000; 
    g_Transition_Table[q01100][1] = q11001;
    g_Transition_Table[q01101][0] = q11010; 
    g_Transition_Table[q01101][1] = q11011;
    g_Transition_Table[q01110][0] = q11100; 
    g_Transition_Table[q01110][1] = q11101;
    g_Transition_Table[q01111][0] = q11110; 
    g_Transition_Table[q01111][1] = q11111;
    g_Transition_Table[q10000][0] = q00000; 
    g_Transition_Table[q10000][1] = q00001;
    g_Transition_Table[q10001][0] = q00010; 
    g_Transition_Table[q10001][1] = q00011;
    g_Transition_Table[q10010][0] = q00100; 
    g_Transition_Table[q10010][1] = q00101;
    g_Transition_Table[q10011][0] = q00110; 
    g_Transition_Table[q10011][1] = q00111;
    g_Transition_Table[q10100][0] = q01000; 
    g_Transition_Table[q10100][1] = q01001;
    g_Transition_Table[q10101][0] = q01010; 
    g_Transition_Table[q10101][1] = q01011;
    g_Transition_Table[q10110][0] = q01100; 
    g_Transition_Table[q10110][1] = q01101;
    g_Transition_Table[q10111][0] = q01110; 
    g_Transition_Table[q10111][1] = q01111;
    g_Transition_Table[q11000][0] = q10000; 
    g_Transition_Table[q11000][1] = q10001;
    g_Transition_Table[q11001][0] = q10010; 
    g_Transition_Table[q11001][1] = q10011;
    g_Transition_Table[q11010][0] = q10100; 
    g_Transition_Table[q11010][1] = q10101;
    g_Transition_Table[q11011][0] = q10110; 
    g_Transition_Table[q11011][1] = q10111;
    g_Transition_Table[q11100][0] = q11000; 
    g_Transition_Table[q11100][1] = q11001;
    g_Transition_Table[q11101][0] = q11010; 
    g_Transition_Table[q11101][1] = q11011;
    g_Transition_Table[q11110][0] = q11100; 
    g_Transition_Table[q11110][1] = q11101;
    g_Transition_Table[q11111][0] = q11110; 
    g_Transition_Table[q11111][1] = q11111;
}

int DFA(char current_symbol) {

    int symbol_index = -1;

    for (char symbol : g_alphabet) {
        if (symbol == current_symbol) {
            symbol_index = (symbol == '0') ? 0 : 1;
        }
    }

    if (symbol_index == -1)
        return UNKNOWN_SYMBOL_ERR;

    g_Current_state = g_Transition_Table[g_Current_state][symbol_index];

    for (int st : g_Accepted_states) {
        if (g_Current_state == st) return REACHED_FINAL_STATE;
    }

    return NOT_REACHED_FINAL_STATE;
}

int main()
{
    SetDFA_Transitions();
    std::cout << "Enter a string with '0' s and '1's:\nPress Enter Key to stop\n";

    char ch;
    int result = NOT_REACHED_FINAL_STATE;
    while(std::cin.get(ch) && ch != '\n') {
        result = DFA(ch);
        if (result == UNKNOWN_SYMBOL_ERR) break;
    }

    if (result == REACHED_FINAL_STATE) {
        std::cout << "\nAccepted! The fifth symbol from the right is 1\n";
    } else {
        std::cout << "\nRejected! The fifth symbol from the right is not 1\n";
    }

    return 0;
}