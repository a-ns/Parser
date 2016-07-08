/*********************************
 * Lexan.h
 * Alex Lindemann
 * 1/30/2016
 * Recursive Descent Parser
 */

#ifndef PARSER_LEXAN_H
#define PARSER_LEXAN_H
#include <stdio.h>
#include "Parser.h"
#include "Parameters.h"


#define ID 300
#define NUM 301
#define BEGIN 400
#define END 401
#define DONE 402
#define TRUE 1
#define FALSE 0

int lexan(paramsP);
void buildTable(paramsP, char*);
paramsP newParams(char*);
int find(char * , SymbolTable);
void addValueToSymbolTable(char *, paramsP);
int legalIdentifier(char *, paramsP);
void getBufferIntoTable(char *, paramsP);
void readInDecls(char *, paramsP);
void readInID(paramsP);
void undeclaredError(paramsP);
void changeExtension(char *);

#endif //PARSER_LEXAN_H
