expression：表达式，用来计算产生一个值
statement：是程序的“命令单元”，组织和控制程序的执行流程，可以包含表达式

BNF Grammar  of the Syntax of Pyc
```
program --> declaration-list
declaration-list --> declaration-list declaration | declaration
declaration --> var-declaration | fun-declaration
var-declaration --> type-specifier ID | type-specifier ID [NUM] 
type-specifier --> int | frac | void 
fun-declaration --> type-specifier ID ( param-list ) compound-stmt | def ID (param-list): compound-stmt
param-list --> param-list, param | param | empty
param --> type-specifier ID | type-specifier ID[] | ID
compound-stmt --> { local-declarations statement-list } 
local-declarations --> local-declarations var-declaration | empty
statement-list --> statement-list statement | empty
statement --> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt
expression-stmt --> expression ; | ;
selection-stmt --> if ( expression ) statement | if ( expression ) statement else statement | if expression : statement | if expression : statement else : statement
iteration-stmt --> while expression : statement | while ( expression ) statement
return-stmt --> return expression ; | return ;
expression --> var = expression | simple-expression
var --> ID | ID [expression]
simple-expression --> additive-expression relop additive-expression | additive-expression
relop --> < | <= | > | >= | == | !=
additive-expression --> additive-expression addop term ｜ term
addop --> + | -
term --> term mulop factor | factor
mulop --> * | /
factor --> (expression) | var | call | num | Str
num --> INTL | FRACL
Str --> STRL
call --> ID(args)
args --> arg-list | empty
arg-list --> arg-list , expression | expression
```

​    
