#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"

void initList(List *list)
{
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
}

void addToken(List *list, TokenType type, const char *info)
{
  Node *newNode = (Node *)malloc(sizeof(Node));
  if (newNode == NULL)
  {
    fprintf(stderr, "Memory allocation failed!\n");
    return;
  }
                            
  newNode->t.type = type;
  if(type==ID||type==INTL||type==FRACL||type==STRL||type==COMMENT||type==INDENT)
  newNode->t.info = strdup(info);
  else 
  newNode->t.info=NULL;
  // 复制字符串以确保其独立存储

  // 初始化新节点的指针
  newNode->next = NULL; 
  newNode->prev = NULL;
  if (list->head == NULL)
  {
    list->tail = newNode;
    list->head = newNode;
  } // 如果链表为空，则新节点是链表的第一个节点
  else
  { // 否则将其添加到链表末尾
    list->tail->next = newNode;
    newNode->prev = list->tail;
    list->tail = newNode;
  }
  list->size++;
}

void tokenizeFile(List *list, FILE *file)
{
  int ch;
  char buffer[200];
  int i=0;
  while((ch=fgetc(file))!=EOF)
    if(!isspace(ch)){
      buffer[i]=ch;
      i++;
    }
    else if(i>0){
      buffer[i]='\0';
      addToken(list,identifyTokenType(buffer),buffer);
      //if(list->tail->t.type==COLON )
      // indent
      i=0;
      if(ch=='\n')
      addToken(list,NEWLINE,NULL);
    }
    addToken(list,END_OF_FILE,NULL);
  } 
  
  

TokenType identifyTokenType(const char *token)
{
  switch(token[0])
  {
    case '#':
    return COMMENT;
    break;
    case '{':
    return LCUR;
    break;
    case '}':
    return RCUR;
    break;
    case '(':
    return LPAR;
    break;
    case ')':
    return RPAR;
    break;
    case '[':
    return LBRA;
    break;
    case ']':
    return RBRA;
    break;
    case '\n':
    return NEWLINE;
    break;
    case ':':
    return COLON;
    break;
    case ';':
    return SEMI;
    break;
    case ',':
    return COMMA;
    break;  //single character tokens
    case '+':
    if(token[1]=='\0')
    return PLUS;
    if(token[1]=='+')
    return PPLUS;
    if(token[1]=='=')
    return PLUS_ASSIGN;
    break;
    case '-':
    if(token[1]=='\0')
    return MINUS;
    if(token[1]=='-')
    return MMINUS;
    if(token[1]=='=')
    return MINUS_ASSIGN;
    break;
    case '*':
    if(token[1]=='\0')
    return MUL;
    if(token[1]=='*')
    return POWER;
    if(token[1]=='=')
    return MUL_ASSIGN;
    break;
    case '/':
    if(token[1]=='\0')
    return DIV;
    if(token[1]=='=')
    return DIV_ASSIGN;
    if(token[1]=='/')
    return COMMENT;
    if(token[1]=='*')
    return COMMENT;// comment
    break;
    case '%':
    if(token[1]=='\0')
    return MOD;
    if(token[1]=='=')
    return MOD_ASSIGN;
    break;
    case '<':
    if(token[1]=='\0')
    return LT;
    if(token[1]=='=')
    return LTE;
    if(token[1]=='<')
    return LEFT_SHIFT;
    break;
    case '>':
    if(token[1]=='\0')
    return GT;
    if(token[1]=='=')
    return GTE;
    if(token[1]=='>')
    return RIGHT_SHIFT;
    break;
    case '=':
    if(token[1]=='\0')
    return ASSIGN;
    if(token[1]=='=')
    return EQ;
    break;
    case '!':
    if(token[1]=='\0')
    return NOT;
    if(token[1]=='=')
    return UNEQ;
    break;
    case '&':
    if(token[1]=='&')
    return AAND;
    if(token[1]=='\0')
    return AND;
    break;
    case '|':
    if(token[1]=='|')
    return OR;
    if(token[1]=='\0')
    return OR1;
    break;
    default:
    break;
  }


  /*if(strcmp(token,"-")==0)
  return MINUS;
  if(strcmp(token,"*")==0)
  return MUL;
  if(strcmp(token,"/")==0) 
  return DIV;
  if(strcmp(token,"%")==0)
  return MOD;
  if(strcmp(token,"**")==0)
  return POWER;
  if(strcmp(token,"++")==0)
  return PPLUS;
  if(strcmp(token,"--")==0)
  return MMINUS;
  if(strcmp(token,"<")==0)
  return LT;
  if(strcmp(token,">")==0)
  return GT;
  if(strcmp(token,"<=")==0)
  return LTE;
  if(strcmp(token,">=")==0)
  return GTE;
  if(strcmp(token,"==")==0)
  return EQ;
  if(strcmp(token,"!=")==0)
  return UNEQ;
  if(strcmp(token,"&&")==0)
  return AAND;
  if(strcmp(token,"||")==0)
  return OR;
  if(strcmp(token,"!")==0)
  return NOT;
  if(strcmp(token,"=")==0)
  return ASSIGN;
  if(strcmp(token,"+=")==0)
  return PLUS_ASSIGN;
  if(strcmp(token,"-=")==0)
  return MINUS_ASSIGN;
  if(strcmp(token,"*=")==0)
  return MUL_ASSIGN;
  if(strcmp(token,"/=")==0)
  return DIV_ASSIGN;
  if(strcmp(token,"%=")==0)
  return MOD_ASSIGN;
  if(strcmp(token,"**=")==0)
  return POWER_ASSIGN;
  if(strcmp(token,"&")==0)
  return AND;
  if(strcmp(token,"<<")==0)
  return LEFT_SHIFT;
  if(strcmp(token,">>")==0)
  return RIGHT_SHIFT; */
  if(isdigit(token[0]))
  { int i=1;
    while(token[i]!='\0'){
    if(token[i]==':'&&isdigit(token[i+1]))
    return FRACL;
    i++;
  }  
    return INTL;
  }
  // STRL

  if(token[0]=='"')
  return STRL;
  /*if(strcmp(token,"{")==0)
  return LCUR;
  if(strcmp(token,"}")==0)
  return RCUR;
  if(strcmp(token,"(")==0)
  return LPAR;
  if(strcmp(token,")")==0)
  return RPAR;
  if(strcmp(token,"[")==0)
  return LBRA;
  if(strcmp(token,"]")==0)
  return RBRA;
  if(strcmp(token,"\n")==0)
  return NEWLINE; 
  if(strcmp(token,":")==0)
  return COLON;
  if(strcmp(token,";")==0)
  return SEMI;
  if(strcmp(token,",")==0)
  return COMMA;
  if(strcmp(token,"#")==0||strcmp(token,"//")==0||strcmp(token,"/*")==0)
  return COMMENT; */
 if(isalpha(token[0])){
  if(strcmp(token,"int")==0)
  return INT;
  else
  if(strcmp(token,"frac")==0)
  return FRAC;
  else
  if(strcmp(token,"str")==0)
  return STR;
  else
  if(strcmp(token,"void")==0)
  return VOID;
  else
  if(strcmp(token,"do")==0)
  return DO;
  else
  if(strcmp(token,"while")==0)
  return WHILE;
  else
  if(strcmp(token,"for")==0)
  return FOR;
  else
 // if(strcmp(token,"def")==0)
  //return DEF;
  //else
  if(strcmp(token,"return")==0)
  return RETURN;
  else
  if(strcmp(token,"in")==0)
  return IN;
  else 
  if(strcmp(token,"if")==0)
  return IF;
  else
  if(strcmp(token,"elif")==0)
  return ELIF;
  else 
  if(strcmp(token,"else")==0)
  return ELSE;
  else 
  return ID;
  }
  return ERROR;
}
void printTokens(const List *list)
{
  Node *current = list->head;
  while (current)
  { if (current->t.info == NULL)
    printf("%s\n", tokenTypeNames[current->t.type]);
    else
    { printf("%s", tokenTypeNames[current->t.type]);
      printf(":%s\n", current->t.info);
    }
    current = current->next;
  }
}
void freeList(List *list)
{

  if (list == NULL || list->tail == NULL)
  {
    return;
  }
  while (list->tail != NULL)
  {
    Node *temp = list->tail;
    list->tail = list->tail->prev;
    free(temp->t.info);
    free(temp);
    list->size--;
  }
  list->head = NULL;
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: %s <input file>\n", argv[0]);
    return 1;
  }
  FILE *file = fopen(argv[1], "r");
  if (file == NULL)
  {
    printf("Cannot open file %s\n", argv[1]);
    return 1;
  }
  List tokenList;
  initList(&tokenList);

  tokenizeFile(&tokenList, file);

  printTokens(&tokenList);

  fclose(file);
  freeList(&tokenList);

  return 0;
}