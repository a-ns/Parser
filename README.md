## Recursive Descent Parser
A simple RDP that works for text files. Program takes in one argument: the name of the inputFile to be checked.

Files required for compilation are: Lexan.c main.c Parser.c Lexan.h Parameters.h Parser.h

### Sample Input Files
##### Legal
```
begin

~I'm a comment!

a = (a + 5)/2 - b;

end
```

##### Illegal
```
begin

a = (a_ + 5)/2 - 6b__b;

end!
```
For the illegal example, output from the program reads:
```
Identifier cannot end in '_': a_. Exiting.
```
If the input inputFile is legal, output from the program reads:
```
Program is legal.
```
Followed by the list of identifiers. For the legal sample, this list would look like the following:
```
|-----Identifier--------------------Type----|
|       begin                       400
|       end                         401
|       a                           300
|       b                           300
|-------------------------------------------|
```
