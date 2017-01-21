#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "parser.h"

void splitToExpressionAndComment(char *line, char **expression, char **comment) {
    if (*expression != NULL) {
        free(*expression);
        *expression = NULL;
    }
    if (*comment != NULL) {
        free(*comment);
        *comment = NULL;
    }

    int instructionStart = 0;
    int instructionEnd = 0;
    int commentStart = -1;
    int commentEnd = -1;

    int prevCharSlash = 0;
    int inComment = 0;
    for(int i=0; 1; i++) {
        if (!inComment) {
            if (line[i] == '/') {
                if (prevCharSlash == 1) {
                    // We've hit a comment
                    instructionEnd = i-1;
                    commentStart = i+1;
                    inComment = 1;
                }
                else prevCharSlash = 1;
            } else prevCharSlash = 0;

            if (line [i] == '\0' || line[i] == '\n') {
                instructionEnd = i;
                break;
            }
        } else {
            if (line [i] == '\0' || line[i] == '\n') {
                commentEnd = i;
                break;
            }
        }
    }

    char *unstrippedExpression, *unstrippedComment;
    unstrippedExpression = malloc(sizeof(char)*(instructionEnd - instructionStart + 1));
    unstrippedComment = malloc(sizeof(char)*(commentEnd - commentStart + 1));
    strncpy(unstrippedExpression, &(line[instructionStart]), instructionEnd - instructionStart);
    (unstrippedExpression)[instructionEnd - instructionStart] = '\0';
    if (commentStart == commentEnd) {
        (unstrippedComment)[0] = '\0';
    } else {
        strncpy(unstrippedComment, &(line[commentStart]), commentEnd - commentStart + 1);
        (unstrippedComment)[commentEnd - commentStart + 1] = '\0';
    }

    strip(unstrippedExpression, expression);
    strip(unstrippedComment, comment);
}

Line parseInstruction(char *instruction) {
    char *strIns = NULL;
    strip(instruction, &strIns);
    
    char *labelPattern = "^\\s*\\(([a-zA-Z_][a-zA-Z0-9_]*)\\)\\s*$";
    char *literalAInsPattern = "^\\s*@([[:digit:]]+)\\s*$";
    char *symbolicAInsPattern = "^\\s*@([a-zA-Z_][a-zA-Z0-9_]*)\\s*$";
    char *regPattern = "[DMA]";
    char *destPattern = "(null|D|M|A|MD|AM|AD|AMD)";
    char *jmpPattern = "(null|JGT|JEQ|JGE|JLT|JNE|JLE|JMP)";
    char *compPattern = malloc(sizeof(char)*300);
    sprintf(compPattern, "(0|1|-1|%s|!%s|-%s|%s\\+1|%s-1|D\\+A|D\\+M|D-A|D-M|A-D|M-D|D&A|D&M|D\\|A|D\\|M)", regPattern, regPattern, regPattern, regPattern, regPattern);
    char *cInsPattern = malloc(sizeof(char)*300);
    sprintf(cInsPattern, "^\\s*(%s=)?(%s)(;%s)?\\s*$", destPattern, compPattern, jmpPattern);

    regex_t literalAInsRegex, symbolicAInsRegex, labelRegex, cInsRegex;
    regcomp(&literalAInsRegex, literalAInsPattern, REG_EXTENDED);
    regcomp(&symbolicAInsRegex, symbolicAInsPattern, REG_EXTENDED);
    regcomp(&labelRegex, labelPattern, REG_EXTENDED);
    regcomp(&cInsRegex, cInsPattern, REG_EXTENDED);

    regmatch_t groups[7];

    if (regexec(&literalAInsRegex, strIns, 2, groups, 0) != REG_NOMATCH) {
        char *aValue = copyGroup(strIns, groups[1]);

        Line line = {lineType: LINETYPE_A, aInstruction: {isLiteral: 1, literal: atoi(aValue), symbol: NULL}, label: NULL};
        printf("A-instruction. Value: %d\n", atoi(aValue));

        return line;
    }

    if (regexec(&symbolicAInsRegex, strIns, 2, groups, 0) != REG_NOMATCH) {
        char *aValue = copyGroup(strIns, groups[1]);

        Line line = {lineType: LINETYPE_A, aInstruction: {isLiteral: 0, literal: 0, symbol: aValue}, label: NULL};
        printf("A-instruction. Symbol: %s\n", aValue);

        return line;
    }
    
    if (regexec(&labelRegex, strIns, 2, groups, 0) != REG_NOMATCH) {
        char* label = copyGroup(strIns, groups[1]);

        Line line = {lineType: LINETYPE_Label, label: label};
        printf("Label. Name: %s\n", label);
        return line;
    }

    if (regexec(&cInsRegex, strIns, 7, groups, 0) != REG_NOMATCH) {
        char* dest = copyGroup(strIns, groups[2]);
        char* comp = copyGroup(strIns, groups[4]);
        char* jump = copyGroup(strIns, groups[6]);
        // FIXME - not sure groups are the right ones - matchJumpPattern is complaining
        // FIXME - deal with case where there isn't a ;jump instruction at the end
        Line line = {lineType: LINETYPE_C, cInstruction: {dest: matchDestPattern(dest), comp: matchCompPattern(comp), jump: matchJumpPattern(jump) }, label: NULL};
        printf("C-instruction.\n");

        return line;
    }

    printf("Failed to parse instruction: %s\n", strIns);
    exit(-1);
}

void strip(char *str, char **out) {
    int i, j;
    for(i=0; isWhitespace(str[i]) && str[i] != '\0' ; i++) {}
    for(j=strlen(str)-1; isWhitespace(str[j]) && j >= i; j--) {}
    j++;
    
    char *newStr = malloc(sizeof(char)*(j - i + 1));
    strncpy(newStr, &(str[i]), (j-i));
    newStr[j-i] = '\0';
    *out = newStr;
}

int isWhitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n');
}

char* copyGroup(char* string, regmatch_t match) {
        size_t start = match.rm_so;
        size_t end   = match.rm_eo;
        char *str = malloc(sizeof(char)*(end-start+1));
        strncpy(str, &(string[start]), end-start);
        str[(end-start)] = '\0';
        return str;
}

Comp matchCompPattern (char *compPattern) {
    if(strcmp(compPattern, "0"  ) == 0) return COMP_zero;
    if(strcmp(compPattern, "1"  ) == 0) return COMP_one;
    if(strcmp(compPattern, "-1" ) == 0) return COMP_minusOne;
    if(strcmp(compPattern, "D"  ) == 0) return COMP_D;
    if(strcmp(compPattern, "M"  ) == 0) return COMP_M;
    if(strcmp(compPattern, "A"  ) == 0) return COMP_A;
    if(strcmp(compPattern, "!D" ) == 0) return COMP_notD;
    if(strcmp(compPattern, "!M" ) == 0) return COMP_notM;
    if(strcmp(compPattern, "!A" ) == 0) return COMP_notA;
    if(strcmp(compPattern, "-D" ) == 0) return COMP_negD;
    if(strcmp(compPattern, "-M" ) == 0) return COMP_negM;
    if(strcmp(compPattern, "-A" ) == 0) return COMP_negA;
    if(strcmp(compPattern, "D+1") == 0) return COMP_incD;
    if(strcmp(compPattern, "M+1") == 0) return COMP_incM;
    if(strcmp(compPattern, "A+1") == 0) return COMP_incA;
    if(strcmp(compPattern, "D-1") == 0) return COMP_decD;
    if(strcmp(compPattern, "M-1") == 0) return COMP_decM;
    if(strcmp(compPattern, "A-1") == 0) return COMP_decA;
    if(strcmp(compPattern, "D+A") == 0) return COMP_DplusA;
    if(strcmp(compPattern, "D+M") == 0) return COMP_DplusM;
    if(strcmp(compPattern, "D-A") == 0) return COMP_DminusA;
    if(strcmp(compPattern, "D-M") == 0) return COMP_DminusM;
    if(strcmp(compPattern, "A-D") == 0) return COMP_AminusD;
    if(strcmp(compPattern, "M-D") == 0) return COMP_MminusD;
    if(strcmp(compPattern, "D&A") == 0) return COMP_DandA;
    if(strcmp(compPattern, "D&M") == 0) return COMP_DandM;
    if(strcmp(compPattern, "D|A") == 0) return COMP_DorA;
    if(strcmp(compPattern, "D|M") == 0) return COMP_DorM;

    fprintf(stderr, "matchCompPattern was asked to interpret unknown pattern %s\n", compPattern);
    exit(-1);
}

Dest matchDestPattern (char* destPattern) {
    if(strcmp(destPattern, "null") == 0) return (Dest){0, 0, 0}; // d1, d2, d3
    if(strcmp(destPattern, ""    ) == 0) return (Dest){0, 0, 0};
    if(strcmp(destPattern, "D"   ) == 0) return (Dest){0, 1, 0};
    if(strcmp(destPattern, "M"   ) == 0) return (Dest){0, 0, 1};
    if(strcmp(destPattern, "A"   ) == 0) return (Dest){1, 0, 0};
    if(strcmp(destPattern, "MD"  ) == 0) return (Dest){0, 1, 1};
    if(strcmp(destPattern, "AM"  ) == 0) return (Dest){1, 0, 1};
    if(strcmp(destPattern, "AD"  ) == 0) return (Dest){1, 1, 0};
    if(strcmp(destPattern, "AMD" ) == 0) return (Dest){1, 1, 1};

    fprintf(stderr, "matchDestPattern was asked to interpret unknown pattern %s\n", destPattern);
    exit(-1);
}

Jump matchJumpPattern (char* jumpPattern) {
    if(strcmp(jumpPattern, "null") == 0) return (Jump){0, 0, 0}; // j1, j2, j3
    if(strcmp(jumpPattern, ""    ) == 0) return (Jump){0, 0, 0};
    if(strcmp(jumpPattern, "JGT" ) == 0) return (Jump){0, 0, 1};
    if(strcmp(jumpPattern, "JEQ" ) == 0) return (Jump){0, 1, 0};
    if(strcmp(jumpPattern, "JGE" ) == 0) return (Jump){0, 1, 1};
    if(strcmp(jumpPattern, "JLT" ) == 0) return (Jump){1, 0, 0};
    if(strcmp(jumpPattern, "JNE" ) == 0) return (Jump){1, 0, 1};
    if(strcmp(jumpPattern, "JLE" ) == 0) return (Jump){1, 1, 0};
    if(strcmp(jumpPattern, "JMP" ) == 0) return (Jump){1, 1, 1};

    fprintf(stderr, "matchJumpPattern was asked to interpret unknown pattern %s\n", jumpPattern);
    exit(-1);
}
