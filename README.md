# C Minus Minus Compiler
This is a lesson project for *Principles of Compilers*, which implements a c-- compiler.

## 0. Features
Till now, this compiler has the function of:

* Lexical and syntax analysis for all ```.cmm``` files.
* Output a syntax tree for ```.cmm``` files without lexical or syntax error.
* Output the lexical and syntax errors for the files if they have some.
* The lexical analysis part can also deal with comments without nests, octal and hexademical numbers, and scientific notation.
* Some differences (mistakes?) in the implement of error output and error recovery.
* The analysis of all compulsory semantic errors.
* Implement of different scopes.
* Generation of intercode.

## 1. Compilation and Usage
For the convenience of presentation, I use ```$(DIR)``` to express the root directory of the folder in the following descriptions.

This project uses

* GNU Flex, *version 2.5.35*
* GNU Bison, *version 3.0.2*

to generate lexical and syntax analysis program.

So, as a prerequisite, you should install Flex and Bison.
```Bash
apt-get install flex
apt-get install bison
```

To compile all files, use command
```Bash
make
```

To perform tests on all part 2 ```.cmm``` files in folder ```$(DIR)/test```, use command
```Bash
make test
```
Note that the program can only identify ```.cmm``` source files in ```$(DIR)/test``` automatically, if you want to test on other files, please compile the project first and use command
```Bash
$(DIR)/parser $(YOUR_FILE_NAME) 
```
The result will be output both to the shell and ```$(DIR)/log.txt```.

You can comment out codes in ```$(DIR)/src/lexical.l``` to test the lexical analysis part.

To debug specific files in ```$(DIR)/test``` with gdb, modify the file directory below command  ```gdb``` in ```Makefile```. Then use command
```Bash
make gdb
```
The result will be output to ```$(DIR)/log.txt```.

To clean up all temporary files generated, use command
```Bash
make clean
```
 	
You can control the indent of the output syntax tree or always enable output regardless of the errors by modify values in file ```$(DIR)/include/syntax_tree.h```.

## 2. Data Structure of Lexical Analysis
I use a struct type "TreeNode" to implement a syntactic unit, the definition can be found in ```$(DIR)/include/syntax_tree.h```. For each syntactic unit, the program will dynamically allocate memory for the TreeNode. However, the members in TreeNode are all static. Consider that the ID name a user create can be very long, the robustness of the program is not good enough.

I define two functions in ```$(DIR)/src/syntax_tree.c``` to create a node and delete nodes. Function ```printTree``` is also defined in it, which is used to print the whole syntax tree.

Meanwhile, the assignment for specific terminal or nonterminal TreeNodes are done in ```$(DIR)/lexical.l``` and ```$(DIR)/syntax.y```.

## 3. Data Structure of Semantic Analysis
My symbol table is based on an orthogonal list and a hash table, the definition can be found in ```$(DIR)/include/symbol_table.h```. For each syntactic unit to be analyzed, I defined a function which can be found in ```$(DIR)/include/symbol_table.c```. Function ```procSymbolTable``` is the entrance of the semantic analysis and is called in ```$(DIR)/syntax.y```. Function ```buildSymbolTable``` is used to loop through the syntax tree recursively and the function will call other functions if met with a syntactical unit to analyze.

##4. Intercode Generation
For this part, you can output the intercode of ```in.cmm```  to file ```out.ir``` using the excutable file ```parser```. Command looks as follows:
```Bash
./parser in.cmm out.ir
```
I use a linked-list structure to implement the intercode. The structure is characterized in ```$(DIR)/include/ir.h```. Operations to the linked-list is described in ```$(DIR)/src/ir.c```. However, the generation of each line of intercode is done in ```$(DIR)/src/symbol_table.c```.

Some bugs in semantic analysis are fixed.
