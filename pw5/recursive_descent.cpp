/*! 
 *      LIST -> ASSIGN LIST_TAIL
 *      LIST_TAIL -> ε
 *      LIST_TAIL -> ; ASSIGN LIST_TAIL
 *      ASSIGN -> EXPR ASSIGN_TAIL
 *      ASSIGN_TAIL -> = ASSIGN
 *      ASSIGN_TAIL -> ε
 * 
 *      EXPR -> NEXT_EXPR ADD
 *      ADD -> | NEXT_EXPR ADD
 *      ADD -> ε
 * 
 *      NEXT_EXPR -> UNARY MUL
 *      MUL -> & UNARY MUL
 *      MUL -> ε
 * 
 *      UNARY -> ~ UNARY
 *      UNARY -> PRIMARY
 * 
 *      PRIMARY -> ID
 *      PRIMARY -> CONST
 *      PRIMARY -> ( EXPR )
 * 
 *      ID -> ONE_SYM SEC_SYM
 *      SEC_SYM -> ONE_SYM
 *      SEC_SYM -> ε
 *      ONE_SYM -> a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v ,w ,x, y, z
 * 
 *      CONST -> 0 CONST_TAIL
 *      CONST -> NZDIGIT DIGIT_TAIL
 *      CONST_TAIL -> b BINARY
 *      CONST_TAIL -> x HEX
 *      CONST_TAIL -> DIGIT_TAIL
 * 
 *      BINARY -> BIT BIT_TAIL
 *      BIT_TAIL -> BIT BIT_TAIL
 *      BIT_TAIL -> ε
 *      BIT -> 0, 1
 * 
 *      HEX -> HEXDIGIT HEX_TAIL
 *      HEX_TAIL -> HEXDIGIT HEX_TAIL
 *      HEX_TAIL -> ε
 *      HEXDIGIT -> DIGIT
 *      HEXDIGIT -> a, b, c, d, e, f
 * 
 *      DIGIT_TAIL -> DIGIT DIGIT_TAIL
 *      DIGIT_TAIL -> ε
 * 
 *      NZDIGIT -> 1, 2, 3, 4, 5, 6, 7, 8, 9
 *      DIGIT -> NZDIGIT
 *      DIGIT -> 0
 */

#include <iostream>
#include <cctype>
#include <cstdlib>
#include <cstring>

// Terminals
const int NEG_SIGN = 0;      // ~
const int MUL_SIGN = 1;      // &
const int ADD_SIGN = 2;      // |
const int ASSIGN_SIGN = 3;   // =
const int LPAR_SIGN = 4;     // (
const int RPAR_SIGN = 5;     // )
const int SEMI_SIGN = 6;     // ;
const int ID_SIGN = 7;       // 1-2 symbols [a-zA-Z] (excep a,b,c,d,e,f,g,x)
const int DIGIT_SIGN = 8;    // 2, 3, 4, 5, 6, 7, 8, 9 (excep 0,1)
const int ZERO_SIGN = 9;     // 0
const int BIT_SIGN = 10;     // 1
const int HEX_SIGN = 11;     // a, c, d, e, f (except b)
const int B_SIGN = 12;       // b
const int X_SIGN = 13;       // x

// End of program
const int EOP = 14;

const int UNDEF = 15;

// Lexeme class (token)
int lexeme = 0;

// Non terminals
void LIST();
void LIST_TAIL();
void ASSIGN();
void ASSIGN_TAIL();
void EXPR();
void ADD();
void NEXT_EXPR();
void MUL();
void UNARY();
void PRIMARY();
void ID();
void ONE_SYM();
void SEC_SYM();
void CONST();
void CONST_TAIL();
void BINARY();
void BIT();
void BIT_TAIL();
void HEX();
void HEXDIGIT();
void HEX_TAIL();
void DIGIT_TAIL();
void DIGIT();
void NZDIGIT();

// Reject input line
void error();

// Look ahead lexeme
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
    LIST();
    if (lexeme == EOP)
        std::cout << "Accepted.\n";
    else
        std::cout << "Rejected.\n";

    return 0;
}

void LIST()
{
    ASSIGN();
    LIST_TAIL();
}

void LIST_TAIL()
{
    if (lexeme == SEMI_SIGN)
    {
        lexeme = get_token();
        ASSIGN();
        LIST_TAIL();
    }
    // Или ε
}

void ASSIGN()
{
    EXPR();
    ASSIGN_TAIL();
}

void ASSIGN_TAIL()
{
    if (lexeme == ASSIGN_SIGN)
    {
        lexeme = get_token();
        ASSIGN();
    }
    // Или ε
}

void EXPR()
{
    NEXT_EXPR();
    ADD();
}

void ADD()
{
    if (lexeme == ADD_SIGN)
    {
        lexeme = get_token();
        NEXT_EXPR();
        ADD();
    }
    // Или ε
}

void NEXT_EXPR()
{
    UNARY();
    MUL();
}

void MUL()
{
    if (lexeme == MUL_SIGN)
    {
        lexeme = get_token();
        UNARY();
        MUL();
    }
    // Или ε
}

void UNARY()
{
    if (lexeme == NEG_SIGN)
    {
        lexeme = get_token();
        UNARY();
    }
    else
    {
        PRIMARY();
    }
}

void PRIMARY()
{
    if (lexeme == ID_SIGN || lexeme == HEX_SIGN || lexeme == B_SIGN || lexeme == X_SIGN)
    {   
        ID();
    } 
    else if (lexeme == ZERO_SIGN || lexeme == BIT_SIGN || lexeme == DIGIT_SIGN)
    {   
        CONST();
    }
    else if (lexeme == LPAR_SIGN)
    {
        lexeme = get_token();
        EXPR();
        if (lexeme != RPAR_SIGN)
        {
            error();
        }
        lexeme = get_token();
    }
    else
    {
        error();
    }
}

void ID()
{
    ONE_SYM();
    SEC_SYM();
}

void ONE_SYM(){
    if (lexeme != ID_SIGN && lexeme != HEX_SIGN && lexeme != B_SIGN && lexeme != X_SIGN) {
        error();
    }
    lexeme = get_token();
}

void SEC_SYM()
{
    if (lexeme == ID_SIGN || lexeme == HEX_SIGN || lexeme == B_SIGN || lexeme == X_SIGN) {
        ONE_SYM();
    }
    // Или ε
}

void CONST()
{
    if (lexeme == ZERO_SIGN)
    {
        lexeme = get_token();
        CONST_TAIL();
    }
    else if (lexeme == DIGIT_SIGN || lexeme == BIT_SIGN)
    {
        lexeme = get_token();
        DIGIT_TAIL();
    }
    else
    {
        error();
    }
}

void CONST_TAIL()
{
    if (lexeme == B_SIGN)
    {
        lexeme = get_token();
        BINARY();
    }
    else if (lexeme == X_SIGN)
    {
        lexeme = get_token();
        HEX();
    }
    else
    {
        DIGIT_TAIL();
    }
}

void BINARY()
{
    BIT();
    BIT_TAIL();
}

void BIT()
{
    if (lexeme == ZERO_SIGN || lexeme == BIT_SIGN)
    {
        lexeme = get_token();
    }
    else
    {
        error();
    }
}

void BIT_TAIL()
{
    if (lexeme == ZERO_SIGN || lexeme == BIT_SIGN)
    {
        BIT();
        BIT_TAIL();
    }
    // Или ε
}


void HEX()
{
    HEXDIGIT();
    HEX_TAIL();
}

void HEXDIGIT()
{
    if (lexeme == DIGIT_SIGN || lexeme == BIT_SIGN || lexeme == ZERO_SIGN || lexeme == HEX_SIGN || lexeme == B_SIGN)
    {
        lexeme = get_token();
    }
    else
    {
        error();
    }
}

void HEX_TAIL()
{
    if (lexeme == DIGIT_SIGN || lexeme == BIT_SIGN || lexeme == ZERO_SIGN || lexeme == HEX_SIGN || lexeme == B_SIGN)
    {
        HEXDIGIT();
        HEX_TAIL();
    }
    // Или ε
}


void DIGIT()
{
    if (lexeme == ZERO_SIGN)
    {
        lexeme = get_token();
    }
    else
    {
        NZDIGIT();
    }
}

void DIGIT_TAIL()
{
    if (lexeme == DIGIT_SIGN || lexeme == BIT_SIGN || lexeme == ZERO_SIGN)
    {
        DIGIT();
        DIGIT_TAIL();
    }
    // Или ε
}

void NZDIGIT()
{
    if (lexeme == DIGIT_SIGN || lexeme == BIT_SIGN)
    {
        lexeme = get_token();
    }
    else
    {
        error();
    }
}


int get_token()
{
    char token[132] = "";
    char* tok = token;

    // Skip spaces
    while (std::isspace(*g_prog))
        ++g_prog;

    // End line marker
    if (*g_prog == '\0')
        return EOP;

    // Keywords
    if (*g_prog == '~')
    {
        ++g_prog;
        return NEG_SIGN;
    }
    if (*g_prog == '&')
    {
        ++g_prog;
        return MUL_SIGN;
    }
    if (*g_prog == '|')
    {
        ++g_prog;
        return ADD_SIGN;
    }
    if (*g_prog == '=')
    {
        ++g_prog;
        return ASSIGN_SIGN;
    }
    if (*g_prog == '(')
    {
        ++g_prog;
        return LPAR_SIGN;
    }
    if (*g_prog == ')')
    {
        ++g_prog;
        return RPAR_SIGN;
    }
    if (*g_prog == ';')
    {
        ++g_prog;
        return SEMI_SIGN;
    }
    if ((*g_prog >= 'h' && *g_prog < 'x') || (*g_prog >= 'y' && *g_prog <= 'z'))
    {
        ++g_prog;
        return ID_SIGN;
    }
    if (*g_prog >= '2' && *g_prog <= '9')
    {
        ++g_prog;
        return DIGIT_SIGN;
    }
    if (*g_prog == '0')
    {
        ++g_prog;
        return ZERO_SIGN;
    }
    if (*g_prog == '1')
    {
        ++g_prog;
        return BIT_SIGN;
    }
    if (*g_prog == 'a' || (*g_prog >= 'c' && *g_prog <= 'f'))
    {
        ++g_prog;
        return HEX_SIGN;
    }
    if (*g_prog == 'b')
    {
        ++g_prog;
        return B_SIGN;
    }
    if (*g_prog == 'x')
    {
        ++g_prog;
        return X_SIGN;
    }
    return UNDEF;
}

void error()
{
    std::cout << "Rejected.\n";
    std::exit(EXIT_FAILURE);
}