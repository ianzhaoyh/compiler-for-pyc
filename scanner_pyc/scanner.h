#ifndef SCANNER_H
#define SCANNER_H
#include <stdio.h>
#include <stdlib.h>
typedef enum
{
  START,
  WAITING,
  TERMINAL,
  ERROR_STATE

} State;
typedef enum tokentype
{
  PLUS,
  MINUS,
  MUL,
  DIV,
  MOD,
  PPLUS,
  MMINUS,
  POWER,
  LT,
  GT,
  LTE,
  GTE,
  EQ,
  UNEQ,
  AAND,
  OR,
  OR1,
  NOT,
  ASSIGN,
  PLUS_ASSIGN,
  MINUS_ASSIGN,
  MUL_ASSIGN,
  DIV_ASSIGN,
  MOD_ASSIGN,
  POWER_ASSIGN,
  AND,
  LEFT_SHIFT,
  RIGHT_SHIFT, /*operators + - * / % ++ -- ** < > <= >= == != && || ! = += -= *= /= %= **= & << >> ,? : ,sizeof */
  INT,
  FRAC,
  STR,
  VOID,
  ID,
  INTL,
  FRACL,
  STRL, // literal
  LCUR,
  RCUR,
  LPAR,
  RPAR,
  LBRA,
  RBRA, // { } ( ) [ ]
  NEWLINE,
  COLON,
  SEMI,
  COMMA,   // \n : ; ,
  INDENT,  // 缩进
  COMMENT, // 注释
  DO,
  WHILE,
  FOR,
  DEF,
  RETURN,
  IN,
  IF,
  ELIF,
  ELSE,
  ERROR,
  END_OF_FILE // end of file
} TokenType;
extern const char *tokenTypeNames[];

typedef struct token
{
  TokenType type;
  char *info;
  int lineNum;
} Token;

typedef struct node
{
  Token *t; // Token* token;
  struct node *next;
  struct node *prev;
  char *info;
  TokenType type;
  int lineNum;
} Node;
typedef struct list
{
  Node *head; // pointer to the first node
  Node *tail; // pointer to the last node
  int size;   // number of elements in the list
} List;

void initList(List *list);                                   // 初始化链表
void addToken(List *list, TokenType type, const char *info); // 添加token
void tokenizeFile(List *list, FILE *file);                   // 识别文件中的token，并将其添加到链表中
TokenType identifyTokenType(const char *token); //// 识别Token类型（重点写的部分）DFA function
void printTokens(const List *list);             // 打印token
void freeList(List *list);                      // 释放链表
List* scanFile(const char *filename);
#endif // SCANNER_H

// Token* get_token(void); 没有参数，依赖某些全局变量（global variable）
// Token* get_token_2(ScanEnv* info); 有参数，不依赖全局变量
/*typedef struct scanEnv{
  char* fileString;
  int currentPos;
} ScanEnv; 把一个文件中的全部内容读到一个字符串里面
void print_token(const Token*);
TokenNode* scan(void); 读取文件中的所有token，返回一个链表
*/