#include <stdlib.h>
#include <string.h>
#include "lineList.h"

LineList emptyLineList() {
    LineList empty = {numberOfStoredLines: 0,
            capacityOfMemory: 5,
            memory: malloc(sizeof(Line)*5)};
    return empty;
}

void addLine(LineList *list, Line newLine) {
    if (list->numberOfStoredLines < list->capacityOfMemory) {
        list->memory[list->numberOfStoredLines] = newLine;
        list->numberOfStoredLines += 1;
    }
    else {
        size_t newSize = list->capacityOfMemory * 2;
        Line* newMemory = malloc(sizeof(Line)*newSize);
        memcpy(newMemory, list->memory, sizeof(Line)*list->capacityOfMemory);
        list->capacityOfMemory = newSize;
        if(list->memory != NULL) free(list->memory);
        list->memory=newMemory;

        addLine(list, newLine);
    }
}
