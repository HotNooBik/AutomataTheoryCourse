/*! \file    rd.cpp
 *  \brief   Recursive descent parser for logical expressions.
 *
 *  \details Recursive descent parser for LL(1) grammar recognizing sequences of logical expressions separated by ';'.
 *           Expressions include integer constants (decimal, binary, hex), variables (1-2 letters), operations (~, *, +, =),
 *           and parentheses. Priorities: ~ > * > + > = (decreasing). Associativity: left for * and +; right for ~ and =.
 *           Left side of assignment is only a variable name.
 *
 *           Grammar (in terms of tokens):
 *           S -> LIST
 *           LIST -> EXPR LIST_TAIL
 *           LIST_TAIL -> ; EXPR LIST_TAIL | ε
 *           EXPR -> ~ UNARY | LPAR EXPR RPAR EXPR_TAIL | CONST EXPR_TAIL | ID EXPR_TAIL_ID
 *           EXPR_TAIL -> MUL_TAIL ADD_TAIL
 *           EXPR_TAIL_ID -> = EXPR | MUL_TAIL ADD_TAIL
 *           ADD_TAIL -> + MUL ADD_TAIL | ε
 *           MUL_TAIL -> * UNARY MUL_TAIL | ε
 *           UNARY -> ~ UNARY | PRIMARY
 *           PRIMARY -> LPAR EXPR RPAR | CONST | ID
 *
 *           Tokens: ID, CONST, NEG(~), MUL(*), ADD(+), ASSIGN(=), LPAR((), RPAR()), SEMI(;), EOP
 *
 *           Formal proof of LL(1):
 *           The grammar is LL(1) because:
 *           1. No left recursion: All productions are right-recursive or terminal-starting.
 *           2. For each nonterminal, the FIRST sets of alternative productions are disjoint.
 *           3. For nullable productions (ε), the FOLLOW set is disjoint from FIRST of other alternatives.
 *
 *           FIRST sets:
 *           - S: {~, (, CONST, ID}
 *           - LIST: {~, (, CONST, ID}
 *           - LIST_TAIL: {;, ε}
 *           - EXPR: {~, (, CONST, ID}
 *           - EXPR_TAIL: {*, +, ε}
 *           - EXPR_TAIL_ID: {=, *, +, ε}
 *           - ADD_TAIL: {+, ε}
 *           - MUL_TAIL: {*, ε}
 *           - UNARY: {~, (, CONST, ID}
 *           - PRIMARY: {(, CONST, ID}
 *
 *           FOLLOW sets:
 *           - S: {$}
 *           - LIST: {$}
 *           - LIST_TAIL: {$}
 *           - EXPR: {;, ), $}
 *           - EXPR_TAIL: {;, ), $}
 *           - EXPR_TAIL_ID: {;, ), $}
 *           - ADD_TAIL: {;, ), $}
 *           - MUL_TAIL: {+, ;, ), $}
 *           - UNARY: {*, +, =, ;, ), $}
 *           - PRIMARY: {*, +, =, ;, ), $}
 *
 *           Select sets (predictive) for alternatives are disjoint:
 *           - For LIST_TAIL: ';' -> {;}, ε -> FOLLOW(LIST_TAIL) = {$} (disjoint).
 *           - For EXPR: ~ -> {~}, ( -> {(}, CONST -> {CONST}, ID -> {ID} (disjoint tokens).
 *           - For EXPR_TAIL_ID: = -> {=}, MUL_TAIL ADD_TAIL -> {*, + , ε} but since MUL_TAIL ADD_TAIL starts with * or + or ε, and ε's FOLLOW disjoint from =.
 *             Actually, the alternative for MUL_TAIL ADD_TAIL is chosen when lookahead in FIRST(MUL_TAIL ADD_TAIL) = {*, +, ε}, but since ε not first, it's {*, +}, and = is separate.
 *           - Similar for ADD_TAIL: + -> {+}, ε -> FOLLOW = {;, ), $} (disjoint).
 *           - MUL_TAIL: * -> {*}, ε -> FOLLOW = {+, ;, ), $} (disjoint).
 *           - UNARY: ~ -> {~}, PRIMARY -> {(, CONST, ID} (disjoint).
 *           - PRIMARY: ( -> {(}, CONST -> {CONST}, ID -> {ID} (disjoint).
 *           No multiple entries in the LL(1) parsing table, hence LL(1).
 *
 *           \license Distributed under the terms of the Zlib license,
 *                    see https://www.zlib.net/zlib_license.html
 */

#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cstring>

// Terminals (tokens)
const int NEG = 0;      // ~
const int MUL = 1;      // *
const int ADD = 2;      // +
const int ASSIGN = 3;   // =
const int LPAR = 4;     // (
const int RPAR = 5;     // )
const int SEMI = 6;     // ;
const int ID = 7;
const int CONST = 8;

// End of program
const int EOP = 9;

const int UNDEF = 10;

// Current lexeme (token)
int lexeme = 0;

// Non-terminals
void S();
void LIST();
void LIST_TAIL();
void EXPR();
void EXPR_TAIL();
void EXPR_TAIL_ID();
void ADD_TAIL();
void MUL_TAIL();
void UNARY();
void PRIMARY();

// Error handling
void error();

// Look ahead token
int get_token();

char g_inputBuffer[1024] = "";
char* g_prog = nullptr;

int main(int argc, char* argv[])
{
    if (1 == argc)
    {
        std::cout << "Enter your input line: ";
        std::cin.getline(g_inputBuffer, 1024);
    }
    else
    {
        std::strcpy(g_inputBuffer, argv[1]);
    }

    g_prog = g_inputBuffer;

    lexeme = get_token();
    S();
    if (lexeme == EOP)
        std::cout << "Accepted.\n";
    else
        std::cout << "Rejected.\n";

    return 0;
}

void S()
{
    LIST();
}

void LIST()
{
    EXPR();
    LIST_TAIL();
}

void LIST_TAIL()
{
    if (lexeme == SEMI)
    {
        lexeme = get_token();
        EXPR();
        LIST_TAIL();
    }
    // else ε (if lexeme in FOLLOW(LIST_TAIL) = EOP)
    else if (lexeme != EOP)
        error();
}

void EXPR()
{
    if (lexeme == NEG)
    {
        lexeme = get_token();
        UNARY();
    }
    else if (lexeme == LPAR)
    {
        lexeme = get_token();
        EXPR();
        if (lexeme != RPAR)
            error();
        lexeme = get_token();
        EXPR_TAIL();
    }
    else if (lexeme == CONST)
    {
        lexeme = get_token();
        EXPR_TAIL();
    }
    else if (lexeme == ID)
    {
        lexeme = get_token();
        EXPR_TAIL_ID();
    }
    else
        error();
}

void EXPR_TAIL()
{
    MUL_TAIL();
    ADD_TAIL();
}

void EXPR_TAIL_ID()
{
    if (lexeme == ASSIGN)
    {
        lexeme = get_token();
        EXPR();
    }
    else
    {
        MUL_TAIL();
        ADD_TAIL();
    }
}

void ADD_TAIL()
{
    if (lexeme == ADD)
    {
        lexeme = get_token();
        UNARY();
        ADD_TAIL();
    }
    // else ε
}

void MUL_TAIL()
{
    if (lexeme == MUL)
    {
        lexeme = get_token();
        UNARY();
        MUL_TAIL();
    }
    // else ε
}

void UNARY()
{
    if (lexeme == NEG)
    {
        lexeme = get_token();
        UNARY();
    }
    else
        PRIMARY();
}

void PRIMARY()
{
    if (lexeme == LPAR)
    {
        lexeme = get_token();
        EXPR();
        if (lexeme != RPAR)
            error();
        lexeme = get_token();
    }
    else if (lexeme == CONST)
    {
        lexeme = get_token();
    }
    else if (lexeme == ID)
    {
        lexeme = get_token();
    }
    else
        error();
}

int get_token()
{
    char token[132] = "";
    char* tok = token;

    // Skip spaces
    while (std::isspace(*g_prog))
        ++g_prog;

    // End of input
    if (*g_prog == '\0')
        return EOP;

    // Operators and punctuation
    if (*g_prog == '~')
    {
        ++g_prog;
        return NEG;
    }
    if (*g_prog == '*')
    {
        ++g_prog;
        return MUL;
    }
    if (*g_prog == '+')
    {
        ++g_prog;
        return ADD;
    }
    if (*g_prog == '=')
    {
        ++g_prog;
        return ASSIGN;
    }
    if (*g_prog == '(')
    {
        ++g_prog;
        return LPAR;
    }
    if (*g_prog == ')')
    {
        ++g_prog;
        return RPAR;
    }
    if (*g_prog == ';')
    {
        ++g_prog;
        return SEMI;
    }

    // ID: 1-2 letters (a-zA-Z)
    if (std::isalpha(*g_prog))
    {
        *tok++ = *g_prog++;
        if (std::isalpha(*g_prog))
            *tok++ = *g_prog++;
        *tok = '\0';
        // Check length: must be 1 or 2
        if (std::strlen(token) < 1 || std::strlen(token) > 2)
            return UNDEF;
        return ID;
    }

    // CONST: decimal, binary, hex
    if (std::isdigit(*g_prog))
    {
        bool valid = false;
        if (*g_prog == '0')
        {
            *tok++ = *g_prog++;
            if (std::tolower(*g_prog) == 'b')  // binary 0b[01]+
            {
                *tok++ = *g_prog++;
                if (*g_prog == '0' || *g_prog == '1')
                {
                    valid = true;
                    while (*g_prog == '0' || *g_prog == '1')
                        *tok++ = *g_prog++;
                }
            }
            else if (std::tolower(*g_prog) == 'x')  // hex 0x[0-9a-fA-F]+
            {
                *tok++ = *g_prog++;
                if (std::isdigit(*g_prog) || std::tolower(*g_prog) >= 'a' && std::tolower(*g_prog) <= 'f')
                {
                    valid = true;
                    while (std::isdigit(*g_prog) || std::tolower(*g_prog) >= 'a' && std::tolower(*g_prog) <= 'f')
                        *tok++ = *g_prog++;
                }
            }
            else  // decimal starting with 0, optional more digits
            {
                valid = true;
                while (std::isdigit(*g_prog))
                    *tok++ = *g_prog++;
            }
        }
        else  // decimal [1-9][0-9]*
        {
            valid = true;
            while (std::isdigit(*g_prog))
                *tok++ = *g_prog++;
        }
        *tok = '\0';
        // Must have at least one digit, and valid
        if (std::strlen(token) < 1 || !valid)
            return UNDEF;
        return CONST;
    }

    return UNDEF;
}

void error()
{
    std::cout << "Rejected.\n";
    std::exit(EXIT_FAILURE);
}