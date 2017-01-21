#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "parser.h"
#include "lineToMachineCode.h"
#include "replaceSymbols.h"
#include "lineList.h"

void printBinary(FILE *stream, uint16_t x);

int main(int argc, char* argv[]) {
    // ToDo:
    // List to hold the lines (called 'program' or similar?)
    // Code to do symbolic conversion to addresses in list (will need hash table)
    // Conversion from 'program' to machine code.
    // Output of machine code.

    if (argc != 3) {
        fprintf(stderr, "Usage: assembler [input filename] [output filename]\n");
        return -1;
    }

    FILE *source = fopen(argv[1], "r");
    if(source == NULL) {
        fprintf(stderr, "Failed to open %s for reading.\n", argv[1]);
        return -1;
    }

    FILE *outfile = fopen(argv[2], "w");
    if(outfile == NULL) {
        fprintf(stderr, "Failed to open %s for writing.\n", argv[2]);
        return -1;
    }

    

    char* line = NULL;
    char *expression=NULL, *comment=NULL;
    size_t bytesRead = 0;
    LineList parsedLineList = emptyLineList();
    while (getline(&line, &bytesRead, source) != -1) {
        splitToExpressionAndComment(line, &expression, &comment);
        if (expression[0] != '\0') {
            Line parsedLine = parseInstruction(expression);
            addLine(&parsedLineList, parsedLine);
        }
    }

    LineList replacedLineList = replaceSymbols(parsedLineList);

    for (int i=0; i < replacedLineList.numberOfStoredLines; i++){
        uint16_t instruction = lineToMachineCode(replacedLineList.memory[i]);
        printBinary(outfile, instruction);
        fprintf(outfile, "\n");
    }

    fclose(source);
    fclose(outfile);

//    char *line = NULL;
//    size_t numCharsRead;
//    while(getline(&line, &numCharsRead, stdin) != -1) {
//        char *instruction=NULL, *comment=NULL;
//        splitToExpressionAndComment(line, &instruction, &comment);
//        fprintf(stderr, "Instruction: %s\nComment: %s\n\n", instruction, comment);
//        Line line = parseInstruction(instruction);
//    }
}

void printBinary(FILE *stream, uint16_t x) {
    for (int i=0; i<16; i++){
        fprintf(stream, "%d", (x >> (15-i))%2);
    }
}
