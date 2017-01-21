#ifndef _REPLACESYMBOLS_H_
#define _REPLACESYMBOLS_H_

#include "parser.h"
#include "lineList.h"

LineList replaceSymbols(LineList lineList);

int haveAlreadySeenSymbol(char** symbolList, int numSymbols, char* symbol);

#endif
