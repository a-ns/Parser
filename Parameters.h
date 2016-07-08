/*********************************
 * Parameters.h
 * Alex Lindemann
 * 1/30/2016
 * Recursive Descent Parser
 */

#ifndef PARSER_PARAMETERS_H
#define PARSER_PARAMETERS_H
#include <stdio.h>
#define SIZE 500
typedef struct Entry{
    char value[SIZE];
    int type;
}Entry;


typedef struct SymbolTable{
    Entry entries[SIZE]; //SIZE
    int size; //how many are in it
}SymbolTable;

typedef struct Parameters {
    char *fileName;
    int currentRegister;
    FILE *inputFile;
    FILE *outputFile;
    int finishedDeclarations;
    int ch;
    int lineno;
    int pos;
    struct SymbolTable table;
    int lookahead;
    char value[SIZE];
    char postfix[SIZE];
} *paramsP;



#endif //PARSER_PARAMETERS_H
