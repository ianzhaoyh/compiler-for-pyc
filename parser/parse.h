#ifndef _PARSE_H_
#define _PARSE_H_


#include "../scanner_pyc/scanner.h"
#include "libs.h"

typedef struct symbolTable SymbolTable;
// No need for ARG_ND, since an argument is just an expression

/* Naming convention. If the word is no more than 5 character use the full word, like param, array, otherwise use, 3 characters, like varaible -> var, assign -> asn.
 */ 

typedef enum {DCL_ND, PARAM_ND, STMT_ND, EXPR_ND,ROOT} NodeKind; //枚举类型，有五种节点类型，分别是声明节点，参数节点，语句节点，表达式节点,ROOT
typedef enum {VAR_DCL,  ARRAY_DCL, FUN_DCL} DclKind; //
typedef enum {VAR_PARAM, ARRAY_PARAM, VOID_PARAM} ParamKind;//参数类型，分别是变量参数，数组参数，空参数
//No Need for ArgKind, since every argument is just an expression, 
//typedef enum {VAR_ARG, ARRAY_ARG} ArgKind;
//typedef enum {SLCT_STMT, ITER_STMT, EXPR_STMT, CMPD_STMT, RTN_STMT, NULL_STMT} StmtKind;
// On 11152013 replace ITER_STMT, with WHILE_STMT and DO_WHILE_STMT
// 12/nov/2014 add FOR_STMT
typedef enum {SLCT_STMT, WHILE_STMT, DO_WHILE_STMT, FOR_STMT, EXPR_STMT, CMPD_STMT, RTN_STMT, NULL_STMT} StmtKind;

// Distuiguish ID_EXPR and Array_EXPR
//typedef enum {OP_EXPR, CONST_EXPR, ID_EXPR, ARRAY_EXPR, CALL_EXPR,  ASN_EXPR} ExprKind;
// Since for loop is used, the comma expression is considered, since without it for loop cannot do much. Put it as  bonus for student. 
/* remove COMMA_EXPR, since commar is an operator */
/* remove array_expr, since [] is introduced as an operator */
typedef enum {OP_EXPR, CONST_EXPR, ID_EXPR, ARRAY_EXPR,CALL_EXPR,ASN_EXPR} ExprKind; // 

/* The type of the value of an expression
   ExprType is used for type checking 
*/
/* No need for Boolean type, since C- uses integer as
   boolean valus */

typedef enum {VOID_TYPE, INT_TYPE, FRAC_TYPE,STR_TYPE} ExprType;

/* changed MAX_CHILDREN from 3 to 4 */
#define MAX_CHILDREN 4

/**************************************************/
/***********   Syntax tree for parsing ************/
/************************** ************************/

typedef struct treeNode {
  struct treeNode * child[MAX_CHILDREN]; // children of the node
  struct treeNode * lSibling; // left sibling
  struct treeNode * rSibling; // right sibling
  // sibling is useful for declaration_list, param_list, local_declarations, statement_list, arg_list.
  struct treeNode * parent; // parent of the node
  /* parent is useful to check the containing structure of a node during parsing. So, the connected tree nodes can be found in all directions, up (to parents), down (to children), and horizontally (left and right to siblings).  */
  /* LineNum:  At the momemt in parsing, when this treeNode is constructed, what is the line number of the token being handled. */
  int lineNum; 
  NodeKind nodeKind; //节点的分类（大类）
  union {DclKind dcl; ParamKind param; StmtKind stmt; ExprKind expr;} kind; // 小类
  union{
    union {
      TokenType op; // used by Op_EXPR
      int val;      // used by Const_EXPR,
      const char * name;  // used by ID_EXPR, Call_EXPR, Array_EXPR
    } exprAttr;
    struct { // it is a struct, not union, because an array declaration need all the three fields.
      ExprType type; // used by all dcl and param
      const char * name;  // used by all dcl and param
      int size;     // used by array declaration
      /* size is only used for and array declaration; i.e., when  Dcl_Kind is Array_DCL. The requirement that size must be a constant should be checked by semantic analyzer.   For parameters, for Array_PARAM, size is ignored. For array element argument, the index is a child of the node, and should not be considered as dclAttr. */
      //SymbolTable* symbol;
      Token* token;
    } dclAttr; // for declaration and parameters.
  }attr;
  ExprType type;
  /* type is for type-checking of exps, will be updated by type-checker,  the parser does not touch it.  */
  Token * token; // the token that the node represents
  void * something; //can carry something possibly useful for other tasks of compiling
  SymbolTable * symbol; // the symbol table that the node represents
} TreeNode;

/*  Not the best design
typedef struct {
	TreeNode * top; // top of a tree.
	Boolean ok;	 // TRUE means fine.  FALSE means error found, then nd could be a partially built tree
}ParseResult;
*/

struct Parser; 

/* Each function has a parameter p, that is a pointer to the parser itself, in order to use the resources belong to the parser */
typedef struct Parser {
    TreeNode *(*parse)(struct Parser *p);
    void (*set_token_list)(struct Parser *p, List tokenList);
    void (*print_tree)(struct Parser *p, TreeNode *tree);
    void (*free_tree)(struct Parser *p, TreeNode *tree);
    void *info;
} Parser;
//typedef struct parser{
	//TreeNode * (* parse)(Parser * p); /* returning a parse tree, based on the tokenList that the parser knows */
	//void (* set_token_list)(Parser * p, List tokenList); /* let the parser remember some tokenList */
	//void (* print_tree)(Parser * p,  TreeNode * tree); /* can print some parser tree */
	//void (* free_tree)(Parser *p, TreeNode * tree); /* free the space of a parse tree */
	//void * info; /* Some data belonging to this parser object. It can contain the tokenList that the parser knows. */
//} Parser;



typedef struct ParserInfo {
    Node *currentTokenNode;
    List tokenList;
    int errorCount;
} ParserInfo;

// 基本解析器操作
Parser *createParser(void);
void destroyParser(Parser *p);

// 语法分析相关
TreeNode *parse_program(Parser *p);
TreeNode *parse(Parser *p);
void set_token_list(Parser *p, List tokenList);
void free_tree(Parser *p, TreeNode *tree);

// 辅助函数
Token *currentToken(ParserInfo *info);
Bool checkType(Token *token, TokenType type);
Bool moveTokenNext(ParserInfo *info);
Bool checkMove(ParserInfo *info, TokenType type);
TreeNode *newNode(NodeKind nodeKind);
void removeNode(TreeNode *node);
Bool canStartDeclaration(TokenType t);
Bool looksLikeFunDeclaration(ParserInfo *f);
void skipNewlines(ParserInfo *info);


// 语法规则解析函数
TreeNode *declaration_list(ParserInfo *f, Bool *status);
TreeNode *declaration(ParserInfo *f, Bool *status);
TreeNode *var_declaration(ParserInfo *f, Bool *status);
TreeNode *fun_declaration(ParserInfo *f, Bool *status);
TreeNode *param_list(ParserInfo *f, Bool *status);
TreeNode *param(ParserInfo *f, Bool *status);
TreeNode *compound_stmt(ParserInfo *f, Bool *status);
TreeNode *local_declarations(ParserInfo *f, Bool *status);
TreeNode *statement_list(ParserInfo *f, Bool *status);
TreeNode *statement(ParserInfo *f, Bool *status);
TreeNode *expression_stmt(ParserInfo *f, Bool *status);
TreeNode *selection_stmt(ParserInfo *f, Bool *status);
TreeNode *iteration_stmt(ParserInfo *f, Bool *status);
TreeNode *return_stmt(ParserInfo *f, Bool *status);
TreeNode *expression(ParserInfo *f, Bool *status);
TreeNode *var(ParserInfo *f, Bool *status);
TreeNode *simple_expression(ParserInfo *f, Bool *status);
TreeNode *relop(ParserInfo *f, Bool *status);
TreeNode *additive_expression(ParserInfo *f, Bool *status);
TreeNode *addop(ParserInfo *f, Bool *status);
TreeNode *term(ParserInfo *f, Bool *status);
TreeNode *mulop(ParserInfo *f, Bool *status);
TreeNode *factor(ParserInfo *f, Bool *status);
TreeNode *num(ParserInfo *f, Bool *status);
TreeNode *Str(ParserInfo *f, Bool *status);
TreeNode *call(ParserInfo *f, Bool *status);
TreeNode *args(ParserInfo *f, Bool *status);
TreeNode *arg_list(ParserInfo *f, Bool *status);

/*
extern TreeNode * syntaxTree;

extern TokenNode * thisTokenNode;
*/

#endif
