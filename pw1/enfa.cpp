#include <iostream>
#include <vector>
#include <array>
#include <stack>
#include <unordered_set>

enum RESULT
{
    UNKNOWN_SYMBOL_ERR = 0,
    NOT_REACHED_FINAL_STATE = 1,
    REACHED_FINAL_STATE = 2
};

// The set Q
enum NFA_STATES
{
    q0, q1, q2,
};

constexpr int TOTAL_STATES = 3;
constexpr int ACCEPTED_STATES = 2;
constexpr int ALPHABET_CHARCTERS = 2;

// The set F
std::array<int, ACCEPTED_STATES> g_Accepted_states{q1, q2};

// The set Sigma (a - 0, b - 1)
std::array<char, ALPHABET_CHARCTERS> g_Alphabet{'a', 'b'};

// Transition functions
// (сначала пустой со всеми возможными переходами, потом будем его заполнять)
std::vector<int> g_Transitions[TOTAL_STATES][ALPHABET_CHARCTERS];
std::vector<int> g_Epsilon[TOTAL_STATES];

// Current states 
// (может быть несколько из-за NFA)
std::unordered_set<int> g_CurrentStates;

// Symbol transition function
void AddSymbolTransition(int from, char symbol, int to)
{
    int idx = (symbol == 'a') ? 0 : 1;
    g_Transitions[from][idx].push_back(to);
}

// Epsilon transition function
void AddEpsilonTransition(int from, int to)
{
    g_Epsilon[from].push_back(to);
}

// Computing ε-closure of set os States
// Нужно для того, чтобы вычислить все доступные состояния из состояний в S, 
// в которые можно попасть с помощью ε-перехода - ε-замыкание
std::unordered_set<int> EpsilonClosure(const std::unordered_set<int> &S)
{
    std::unordered_set<int> closure = S;
    std::stack<int> st;
    for (int s : S)
        st.push(s);

    while (!st.empty())
    {
        int v = st.top();
        st.pop();
        for (int nxt : g_Epsilon[v])
        {
            if (!closure.count(nxt))
            {
                closure.insert(nxt);
                st.push(nxt);
            }
        }
    }
    return closure;
}

// Checking for states in Accepted_states
// (по условию NFA нужно, чтобы хотя бы одно состояние из текущих было принимающим)
bool IsAccepting(const std::unordered_set<int> &states)
{
    for (int s : states)
    {
        for (int acc : g_Accepted_states)
        {
            if (s == acc)
                return true;
        }
    }
    return false;
}

void BuildENFA()
{
    AddEpsilonTransition(q0, q1);
    AddSymbolTransition(q0, 'b', q2);
    AddSymbolTransition(q1, 'a', q1);
    AddSymbolTransition(q2, 'b', q2);
    AddSymbolTransition(q2, 'a', q1);
}

int ENFA_Step(char current_symbol)
{
    int symbol_index = -1;

    for (char symbol : g_Alphabet) {
        if (symbol == current_symbol) {
            symbol_index = (symbol == 'a') ? 0 : 1;
        }
    }

    if (symbol_index == -1)
    {
        g_CurrentStates.clear();
        return UNKNOWN_SYMBOL_ERR;
    }

    std::unordered_set<int> nextStates;
    for (int s : g_CurrentStates)
    {
        for (int tgt : g_Transitions[s][symbol_index])
        {
            nextStates.insert(tgt);
        }
    }

    nextStates = EpsilonClosure(nextStates);

    g_CurrentStates = std::move(nextStates);

    if (IsAccepting(g_CurrentStates))
        return REACHED_FINAL_STATE;
    return NOT_REACHED_FINAL_STATE;
}

int main(int argc, char *argv[])
{

    BuildENFA();

    g_CurrentStates.clear();
    g_CurrentStates.insert(q0);
    g_CurrentStates = EpsilonClosure(g_CurrentStates);

    std::cout << "Enter a string with 'a's and 'b's:\nPress Enter Key to stop\n";

    char ch;
    int result = IsAccepting(g_CurrentStates) ? REACHED_FINAL_STATE : NOT_REACHED_FINAL_STATE;

    while (std::cin.get(ch) && ch != '\n')
    {
        result = ENFA_Step(ch);
        if (result == UNKNOWN_SYMBOL_ERR || g_CurrentStates.empty())
        {
            break;
        }
    }

    bool accepted = IsAccepting(g_CurrentStates);

    if (result == REACHED_FINAL_STATE)
    {
        std::cout << "\nAccepted! The string belongs to the language b* a* ({a^n : n >= 1} U {b^m a^k : m,k >= 0}).\n";
    }
    else
    {
        std::cout << "\nRejected! The string does not belong the language b* a* (({a^n : n >= 1} U {b^m a^k : m,k >= 0}).\n";
    }

    return 0;
}