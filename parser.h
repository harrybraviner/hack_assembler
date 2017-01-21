#ifndef _PARSER_H_
#define _PARSER_H_

#include <regex.h>

typedef struct AInstruction {
    int   isLiteral;
    int   literal;
    char* symbol;
} AInstruction;

typedef enum Comp {
    COMP_zero,    COMP_one,     COMP_minusOne, COMP_D,       COMP_M, COMP_A,
    COMP_notD,    COMP_notM,    COMP_notA,     COMP_negD,    COMP_negM, COMP_negA,
    COMP_incD,    COMP_incM,    COMP_incA,     COMP_decD,    COMP_decM, COMP_decA,
    COMP_DplusA,  COMP_DplusM,  COMP_DminusA,  COMP_DminusM,
    COMP_AminusD, COMP_MminusD, COMP_DandA,    COMP_DandM,
    COMP_DorA,    COMP_DorM
} Comp;

typedef struct Jump {
    int j1;
    int j2;
    int j3;
} Jump;

typedef struct Dest {
    int d1;
    int d2;
    int d3;
} Dest;

typedef struct CInstruction {
    Dest dest;
    Comp comp;
    Jump jump;
} CInstruction;

typedef enum LineType { LINETYPE_C, LINETYPE_A, LINETYPE_Label} LineType;
typedef struct Line {
    LineType     lineType;
    CInstruction cInstruction;
    AInstruction aInstruction;
    char*        label;
} Line;

void splitToExpressionAndComment(char *line, char **expression, char **comment);

Line parseInstruction(char *instruction);

char* copyGroup(char* string, regmatch_t match);

void strip(char *str, char **out);
int isWhitespace(char c);

Comp matchCompPattern (char *compPattern);
Dest matchDestPattern (char *destPattern);
Jump matchJumpPattern (char *jumpPattern);

#endif
