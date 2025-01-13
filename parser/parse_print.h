/****************************************************
 File: parse_print.h

 Programming designed by the teacher: Liang, Zhiyao
****************************************************/

#ifndef _PARSE_PRINT_H_
#define _PARSE_PRINT_H_

#include "parse.h"
/* TreeNode  is defined in parse.h.
 * If parse.h is not here, but some file includes parse_print.h before including
 * parse.h, then some strange error message appears.  */

void print_tree(Parser *, TreeNode *);

// void print_token_type(TokenType );

void print_expr_type(ExprType);
void print_stmt_type(StmtKind);
void print_op_type(TokenType);
void print_spaces(int);
void print_indent(int);
void print_token_type(TokenType);
void print_node(TreeNode *);
const char *token_type_to_string(TokenType token); // 添加函数原型声明

#endif
