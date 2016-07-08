#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"




int main(int argc, char *argv[]){

    paramsP params = newParams(argv[1]);
    printf("Compiling %s\n", argv[1]);
    parse(params);

    fclose(params->inputFile);
    fclose(params->outputFile);
    return 0;
}


