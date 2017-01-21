#ifndef _LINELIST_H_
#define _LINELIST_H_

#include "parser.h"

typedef struct LineList {
    size_t numberOfStoredLines;
    size_t capacityOfMemory;
    Line* memory;
} LineList;

LineList emptyLineList();

void addLine(LineList *list, Line newLine);

#endif
