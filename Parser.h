/*********************************
 * ParserP.h
 * Alex Lindemann
 * 1/30/2016
 * Recursive Descent Parser
 */

#ifndef PARSER_PARSER_H
#define PARSER_PARSER_H
#include <stdio.h>
#include "Lexan.h"
#include "Parameters.h"



void AssignStmt(paramsP );
void expression(paramsP );
void term(paramsP);
void factor(paramsP);
void match(int, paramsP);
void parse(paramsP);
void listIdentifiers(paramsP);
void checkAfterEnd(paramsP);
#endif //PARSER_PARSER_H
