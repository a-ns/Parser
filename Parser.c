/*********************************
 * Parser.c
 * Alex Lindemann
 * 1/30/2016
 * Recursive Descent Parser
 */
#include <stdlib.h>
#include <string.h>
#include "Parser.h"
/*
 *
 * This is the functionality for things like
 * a = 2 + 3;
 *
 */
void AssignStmt(paramsP params){
    char expr[SIZE];
    strcpy(expr, params->value);
    match(ID, params);
    if(params->lookahead != '='){ //error
        printf("Line %i contains error \"Expected '=' missing\". Exiting.\n", params->lineno);
        fclose(params->inputFile);
        fclose(params->outputFile);
        exit(0);
    }
    else{

        match(params->lookahead, params);

        expression(params);

        match(';', params);

    }

    //we have reached the end of the statement and can begin to print the final assignment and the postfix expression
   // printf("%s = R%i\n", expr, params->currentRegister - 1);
    fprintf(params->outputFile, "%s = R%i\n", expr, params->currentRegister - 1);
    params->postfix[strlen(params->postfix) -1] = '\0'; // this gets rid of the last comma in the postfix
   // printf("*********[%s]**********\n", params->postfix);
    fprintf(params->outputFile, "*********[%s]**********\n", params->postfix);
    strcpy(params->postfix , "");
    params->currentRegister = 0;
    return;
}
/*
 * Functionality for things like x + 3;
 */
void expression(paramsP params){

    term(params);
    while(params->lookahead == (char) '+' || params->lookahead == (char)'-'){
        char plusOrMinus[2]; // need to convert a char to a string for strcat later
        if(params->lookahead == '+'){
            plusOrMinus[0] = '+';
        }
        else if(params->lookahead == '-'){
            plusOrMinus[0] = '-';
        }

        match(params->lookahead, params);
        term(params);
        // Done with our expression, now to just print the final thing
        if(plusOrMinus[0] == '+'){
            params->currentRegister -= 1;
            //printf("R%i = R%i + R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);
            fprintf(params->outputFile, "R%i = R%i + R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);
        }
        else {
            params->currentRegister -= 1;
           // printf("R%i = R%i - R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);
            fprintf(params->outputFile, "R%i = R%i - R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);

        }
        plusOrMinus[1] = '\0';
        strcat(params->postfix, plusOrMinus);
        strcat(params->postfix, ",");
    }
    return;
}
/*
 * For things like 2*3
 */
void term(paramsP params){
    factor(params);

    while(params->lookahead == (char) '*' || params->lookahead == (char) '/'){
        char multOrDiv[2];
        if(params->lookahead == '*')
            multOrDiv[0] = '*';
        else if(params->lookahead == '/')
            multOrDiv[0] = '/';

        match(params->lookahead, params);
        factor(params);
        if(multOrDiv[0] == '*'){
            params->currentRegister -= 1;
          //  printf("R%i = R%i * R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);
            fprintf(params->outputFile,"R%i = R%i * R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);
        }
        else {
            params->currentRegister -= 1;
            //printf("R%i = R%i / R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);

            fprintf(params->outputFile,"R%i = R%i / R%i\n", params->currentRegister - 1, params->currentRegister -1, params->currentRegister);

        }
        multOrDiv[1] = '\0';
        strcat(params->postfix, multOrDiv);
        strcat(params->postfix, ",");
    }

    return;
}

 


/*
 * For things like (3 * 2(3+3))
 */

void factor(paramsP params){
    char ch[SIZE];
    if(params->lookahead == ID){
        match(ID, params);
        strcpy(ch, params->value);
       // printf("R%i = ", params->currentRegister);

        fprintf(params->outputFile, "R%i = ", params->currentRegister);
       // printf(" %s \n", params->value);
        fprintf(params->outputFile, " %s \n", params->value);
        strcat(params->postfix, params->value);
        strcat(params->postfix, ",");
        params->currentRegister +=1;
    }
    else if(params->lookahead == NUM){
        match(NUM, params);
       // printf("R%i = %s\n", params->currentRegister, params->value);

        fprintf(params->outputFile, "R%i = %s\n", params->currentRegister, params->value);
        strcat(params->postfix, params->value);
        strcat(params->postfix, ",");
        params->currentRegister += 1;
    }
    else if (params->lookahead == '('){
        match('(',params);
        expression(params);
        match(')', params);
    }
    else{//error
        printf("Line %i contains error \"Expected '(' missing\". Exiting.\n", params->lineno);
        fclose(params->inputFile);
        fclose(params->outputFile);
        exit(0);
    }
}
/*
 * This makes sure we get our expected things like BEGIN, END, semicolons and such
 * If we expect something that it's not there, we quit.
 */
void match(int t, paramsP params){
    if(t == params->lookahead)
        params->lookahead = lexan(params);
    else{//error
        if(params->lookahead == DONE){
            printf("Line %i contains error\"Expected 'end' not found\". Exiting.\n", params->lineno);
        }
        else if(t == BEGIN){
            printf("Line %i contains error \"Expected \"begin\" not found\". Exiting.\n", params->lineno);
        }
        else if(t == END){
            printf("Line %i contains error \"Expected \"end\" not found\". Exiting.\n", params->lineno);
        }
        else printf("Line %i contains error. Character '%c' unknown contextually. Exiting.\n", params->lineno, params->ch);
        fclose(params->inputFile);
        fclose(params->outputFile);
        exit(0);
    }
    return;
}

/*
 * the function that sets it all in motion.
 * matches BEGIN and END because a program must have those things at the very least.
 */
void parse( paramsP params){
    params->lookahead = lexan(params);
    match(BEGIN, params);
    while(params->lookahead != END){
        AssignStmt(params);
    }
    match(END, params);
    //check for stuff after end
    checkAfterEnd(params);

   // listIdentifiers(params);
    return;
}
/*
 * Prints the Symbol Table
 */
void listIdentifiers(paramsP params){
    int i;
        printf("|-----Identifier--------------------Type----|\n");
    for(i = 1; i < params->table.size; i++){
        printf("|       %-10s                  %i\n", params->table.entries[i].value, params->table.entries[i].type);
    }
        printf("|-------------------------------------------|\n");
    return;
}

/*
 * Checks for illegal tokens after the "end" keyword. Allows for comments. If it finds any type of white space or a comment,
 * it calls itself to advance lookahead. If it finds anything other than whitespace or a comment
 * it gives an error
 */
void checkAfterEnd(paramsP params){
    while(params->lookahead != DONE) {
        params->lookahead = lexan(params);
        if(params->lookahead == '~' || params->lookahead == ' ' || params->lookahead == '\t' || params->lookahead == '\r' || params->lookahead == '\n')
            checkAfterEnd(params);
        else{
            printf("Line %i contains error \"Unexpected tokens after \"end\"\". Exiting.\n", params->lineno);
            fclose(params->inputFile);
            fclose(params->outputFile);
            exit(0);
        }
    }
    return;
}
