/*********************************
 * Lexan.c
 * Alex Lindemann
 * 1/30/2016
 * Recursive Descent Parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Lexan.h"
#include <ctype.h>

/*
 * Gets chars from the inputFile and gives them to the parser.
 */

int lexan(paramsP params){
    while(1){
        params->ch = fgetc(params->inputFile);
        if(params->ch == ' ' || params->ch == '\t' || params->ch == '\r'); //do nothing; advances by getting next char
        else if( params->ch == '~'){ //this is a comment, just keep getting chars until you get to a new line
            while(params->ch != '\n') {
                params->ch = fgetc(params->inputFile);
                if(params->ch == -1)
                    return DONE;
            }
            params->lineno += 1;
        }
        else if(params->ch == '\n')//increment the line number
            params->lineno += 1;
        else if(isdigit(params->ch)){
            char cToStr[2];
            cToStr[0] = params->ch;
            cToStr[1] = '\0';
            strcpy(params->value, cToStr);
            while(isdigit(params->ch = fgetc(params->inputFile))){ //just keeps getting char so long as they are digits
                //printf(" %i ", params->ch);
                cToStr[0] = params->ch;
                cToStr[1] = '\0';
                if(cToStr[0] == '\n'){
                    printf("UH OH ABORT");
                }
                strcat(params->value, cToStr);

            }

            if(isalpha(params->ch)){//if it wasn't a space or new line or something like that; makes sure there isn't something like 643a
                printf("Unexpected token before token \"%c\" on line %i. Exiting.\n", params->ch, params->lineno);
                fclose(params->inputFile);
                fclose(params->outputFile);
                exit(0);
            }

            ungetc(params->ch, params->inputFile); //puts the character back for the next round
            return NUM;
        }
        else if(isalpha(params->ch)){
            readInID(params);
            if(strcmp(params->value, "int") == 0){ //if we're going to get a declaration
                //read in declarations and add them to the symbol table
                if(params->finishedDeclarations == 1){
                    printf("\nLine %i contains error: Illegal declaration(s) after executable code! Exiting!\n", params->lineno);
                    fclose(params->inputFile);
                    fclose(params->outputFile);
                    exit(0);
                }
                char buffer[SIZE];
                readInDecls(buffer, params);
                getBufferIntoTable(buffer, params);
                buffer[0] = '\0';
            }
            else {
                params->pos = find(params->value, params->table);
                if(params->pos == -1) //undeclared variable
                    undeclaredError(params);
                else if (params->pos == 1)
                    return BEGIN;
                else if (params->pos == 2)
                    return END;
		        params->finishedDeclarations = 1; // we know we finished finding declarations. setting this to 1 will ensure that we spit out an error if we find keyword "int" again.
                return ID;
             }
        }
        else if( params->ch == EOF)
            return DONE;
        else
            return params->ch;
    }
}
/*
 * Checks to find the given token value in the Symbol Table
 */
int find(char *value, SymbolTable table){
    int i;
    for(i = 1; i < table.size; i++)
        if(strcmp(table.entries[i].value, value) == 0) return i;
    return -1; //defaults to -1 for not found
}
/*
 * inserts a string token into the Symbol Table
 */
void addValueToSymbolTable(char *value, paramsP params){
    strcpy(params->table.entries[params->table.size].value, value);
    params->table.entries[params->table.size].type = ID;
    params->table.size += 1;
    return;
}


/*
 * builds the symbol table and params with default values
 */
void buildTable(paramsP params, char * fileName){
    params->fileName = fileName;

	params->currentRegister = 0;
    params->finishedDeclarations = 0;
    params->lineno = 1;
    params->pos = 1;
    params->ch = 0;
    params->inputFile = fopen(fileName, "r");
    if(params->inputFile == NULL) {
        fprintf(stderr, "File not found! Exiting.\n");
        exit(0);
    }
    int i;
    i = strlen(params->fileName);
    char outputFileName[i + 1];
    strcpy(outputFileName, params->fileName);
    changeExtension(outputFileName);
    params->outputFile = fopen(outputFileName, "w");
    params->table.size = 3;
    strcpy(params->table.entries[1].value , "begin"); params->table.entries[1].type = BEGIN;
    strcpy(params->table.entries[2].value, "end"); params->table.entries[2].type = END;
    return;
}
/*
 * A constructor of sorts that returns a pointer to a symbol table, char, lookahead, etc.
 */
paramsP newParams(char *fileName){
    paramsP tmp = malloc(sizeof(struct Parameters));
    buildTable(tmp, fileName);
    char cToStr[2]; //convert a char into a string for string function
    cToStr[0] = '\0';
    strcpy(tmp->value, cToStr); //just manually sets value to null character
    return tmp;
}
/*
 * Returns TRUE (1) if the identifier is legal, FALSE (0) if it was not. Identifiers are legal if it does not end in an '_' and it does not have two consecutive '_'s
 */
int legalIdentifier(char *value, paramsP params){
    char *pos = value;
    if(*pos == '_') return FALSE; //it started with an underscore, which is not allowed
    while(*pos != '\0'){
        if(*pos == '_' && *(pos + 1) == '_'){ //consecutive underscores
            printf("Too many '_' in identifier: %s. Exiting.\n", value);
            fclose(params->inputFile);
            fclose(params->outputFile);
            exit(0);
        }
        pos++;
    }
    if(*(pos-1) == '_'){ //it ended in an underscore
        printf("Identifier cannot end in '_': %s. Exiting.\n", value);
        fclose(params->inputFile);
        fclose(params->outputFile);
        exit(0);
    }
    return TRUE;
}

/*
 * Inserts the buffer of int decls into the symbol table
 */
void getBufferIntoTable(char * buffer, paramsP params){
    int i = 0;
    int j = 0;
    int k = 0;
    char tokens[SIZE][SIZE];
    while(buffer[i] != '\0'){
        if(buffer[i] == '~') while(buffer[i] != '\n') i++; //this is a comment
        if(buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n' || buffer[i] == '\r'){
            if(buffer[i] == '\n')
                params->lineno += 1;
            i++;
        }
        else if(buffer[i] == ',') { // new token
            if(buffer[i + 1] == ','){
                printf("Line %i contains error: Error in declaration: %s!\n", params->lineno, buffer);
                fclose(params->inputFile);
                fclose(params->outputFile);
                exit(0);
            }
            j++;
            tokens[j - 1][k] = '\0';
            k = 0;
            i++;
        }
        else {
            tokens[j][k] = buffer[i];
            k++;
            i++;
            tokens[j][k] = '\0';
        }
    }
    int l = 0;
    for(l = 0; l<= j; l++){ //insert all the words we found into table
        params->pos = find(tokens[l], params->table);
        if(params->pos == -1){//symbol was not found
            //insert value into symboltable
            if(legalIdentifier(tokens[l], params))
                addValueToSymbolTable(tokens[l], params);
            else{//it was not a legal identifier
                printf("Unknown token \"%s\" found on line %i. Exiting.\n", tokens[l], params->lineno);
                fclose(params->inputFile);
                fclose(params->outputFile);
                exit(0);
            }
        }
        else {
            if(strcmp(tokens[l], "") == 0)
                printf("\nLine %i contains error: Problem with declarations!\n", params->lineno);
            else printf("\nLine %i contains error: \"Redeclaration of variable '%s'.\"\n", params->lineno, tokens[l]);
            fclose(params->inputFile);
            fclose(params->outputFile);
            exit(0);
        }
    }
    int m = 0;
    while(m <=j){
        tokens[m][0] = '\0';
        m++;
    }
}

/*
 * This reads in the declarations and has the functionality for detecting a missing semicolon.
 */

void readInDecls(char *buffer, paramsP params){
    int i = 0;
    while(params->ch != ';') {
        if(params->ch == EOF || params->ch == '=' || params->ch == '+' || params->ch == '/' ||  params->ch == '*'){
            printf("Line %i contains error: Expected ';' not found! Exiting.\n", params->lineno);
            fclose(params->inputFile);
            fclose(params->outputFile);
            exit(0);
        }
        if (params->ch == ' ' || params->ch == '\r' || params->ch == '\n' || params->ch == '\t') {
            if (params->ch == '\n')
                params->lineno += 1;

            params->ch = fgetc(params->inputFile);
        }

        else {
            char cToStr[2];
            cToStr[0] = params->ch;
            cToStr[1] = '\0';
            strcat(buffer, cToStr);
            params->ch = fgetc(params->inputFile);
            i++;
        }
    }
    buffer[i] = '\0';
    return;
}

/*
 * Function for quitting the program if there's an undeclared variable.
 */
void undeclaredError(paramsP params){
    printf("Line %i contains error 'Undeclared variable: %s'\n", params->lineno, params->value);
    fclose(params->inputFile);
    fclose(params->outputFile);
    exit(0);
}
/*
 * this reads in identifiers.
 */
void readInID(paramsP params){
    char cToStr[2]; //need to make a char into a string for string functions
    cToStr[0] = (char) params->ch;
    cToStr[1] = '\0';
    strcpy(params->value, cToStr);
    params->ch = fgetc(params->inputFile);
    while(isalpha(params->ch) || params->ch == '_' || isdigit(params->ch)){
        cToStr[0] = (char) params->ch; //need to turn the char into a string for string functions
        cToStr[1] = '\0';
        strcat(params->value, cToStr);
        params->ch = fgetc(params->inputFile);
    }
    cToStr[0] = '\0';
    strcat(params->value, cToStr);
    ungetc(params->ch, params->inputFile);
    return;
}
/*
 * changes changes the extension on the file name to ".out"
 */
void changeExtension(char *fileName){
    char *p = fileName;
    while(*p != '.'){
        p++;
    }
    p++;
    strcpy(p, "out");
    return;
}
