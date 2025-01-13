/****************************************************
 File: parse_print.c

 Programming designed by the teacher: Liang, Zhiyao
 ****************************************************/
#include "parse.h"
#include "parse_print.h"
#include <stdio.h>
#include "../scanner_pyc/scanner.h"
#include "libs.h"

/* macros of  increase/decrease indentation */
// #define INDENT indentNum+=2
// #define UNINDENT indentNum-=2
#define INDENT_GAP 2

void print_expr_type(ExprType t)
{
	switch (t)
	{
	case VOID_TYPE:
		printf("void");
		break;
	case INT_TYPE:
		printf("int");
		break;
	case FRAC_TYPE:
		printf("frac");
		break;
	case STR_TYPE:
		printf("string");
		break;
	default:
		printf("Error ExpType");
		break;
	}
}

/* printSpaces indents by printing spaces */
void print_spaces(int indentNum)
{
	int i;
	for (i = 0; i < indentNum; i++)
		printf(" ");
}

/* procedure print_tree prints a syntax tree to the
   listing file using indentation to indicate subtrees
   handle FOR_STMT  13/nov/2014
 */
void print_tree(Parser *p, TreeNode *tree)
{
	int i;

	/* Variable indentNum is used by printTree to
	 * store current number of spaces to indent
	 */
	static int indentNum = 0;

	indentNum += INDENT_GAP;
	while (tree != NULL)
	{
		print_spaces(indentNum); /* Each case only prints one line, If print more than one line, need use printSpaces() first.*/
		// printf("%d ",  tree->lineNum);
		if (tree->nodeKind == DCL_ND)
		{
			printf("Declare:  ");
			print_expr_type(tree->attr.dclAttr.type);
			printf(" %s ", tree->attr.dclAttr.name);
			// print the [size] only if it is an array.
			switch (tree->kind.dcl)
			{
			case ARRAY_DCL:
				printf("[%d]\n", tree->attr.dclAttr.size);
				break;
			case FUN_DCL:
				printf("function with parameters :\n");
				// Function parameters will be saved as child[0] of the node
				break;
			case VAR_DCL:
				// do nothing
				printf("\n");
				break;
			default:
				printf("Unknown DclNode kind\n");
				break;
			}
		}
		else if (tree->nodeKind == PARAM_ND)
		{
			printf("Parameter: ");
			print_expr_type(tree->attr.dclAttr.type);
			if (tree->attr.dclAttr.type != VOID_TYPE)
			{
				printf(" %s", tree->attr.dclAttr.name);
				if (tree->kind.param == ARRAY_PARAM)
					printf("[ ]");
			}
			printf("\n");
		}
		else if (tree->nodeKind == STMT_ND)
		{
			switch (tree->kind.stmt)
			{
			case SLCT_STMT:
				printf("If ");
				if (tree->child[2] != NULL) // has else part
					printf(" with ELSE \n");
				else
					printf(" without ELSE \n");
				break;
				//  case ITER_STMTMT:
			case WHILE_STMT:
				printf("while stmt: \n");
				break;
			case FOR_STMT:
				printf("for stmt: \n");
				break;
			case DO_WHILE_STMT:
				printf("do while stmt: \n");
				break;
			case EXPR_STMT:
				printf("Expression stmt: \n");
				break;
			case CMPD_STMT:
				printf("Compound Stmt:\n");
				break;
			case RTN_STMT:
				printf("Return \n");
				// if there is a return value, it is  child[0].
				break;
			case NULL_STMT:
				printf("Null statement:  ;\n");
				break;
			default:
				printf("Unknown StmtNode kind\n");
				break;
			}
		}
		else if (tree->nodeKind == EXPR_ND)
		{
			switch (tree->kind.expr)
			{
			case OP_EXPR:
				printf("Operator: ");
				if (tree->attr.exprAttr.op == LBRA)
					printf("[] index operator");
				else
					printf("%s", token_type_to_string(tree->attr.exprAttr.op));
				printf("\n");
				break;
			case CONST_EXPR:
				printf("Const: %d\n", tree->attr.exprAttr.val);
				break;
			case ID_EXPR:
				printf("ID: %s\n", tree->attr.exprAttr.name);
				break;

			case ARRAY_EXPR:
				printf("Array: %s, with member index:\n", tree->attr.exprAttr.name);
				break;

			case CALL_EXPR:
				printf("Call function: %s, with arguments:\n", tree->attr.exprAttr.name);
				break;
				/* arguments are listed as  child[0]
				  remove ASN_EXP, since it is just an operator expression 13/NOV/2014
			case ASN_EXP:
			printf("Assignment, with LHS and RHS:\n");
			break;
				 */
			default:
				printf("Unknown ExpNode kind\n");
				break;
			}
		}
		else
			printf("Unknown node kind\n");
		for (i = 0; i < MAX_CHILDREN; i++)
			print_tree(p, tree->child[i]);
		tree = tree->rSibling;
	} // end of while loop.
	indentNum -= INDENT_GAP;
}

void print_stmt_type(StmtKind t)
{
	switch (t)
	{
	case SLCT_STMT:
		printf("if");
		break;
	case FOR_STMT:
		printf("for");
		break;
	case DO_WHILE_STMT:
		printf("do while");
		break;
	case EXPR_STMT:
		printf("expression");
		break;
	case CMPD_STMT:
		printf("compound");
		break;
	case RTN_STMT:
		printf("return");
		break;
	case NULL_STMT:
		printf("null");
		break;
	default:
		printf("Error StmtKind");
		break;
	}
}
void print_op_type(TokenType t)
{
	switch (t)
	{
	case PLUS:
		printf("+");
		break;
	case MINUS:
		printf("-");
		break;
	case MUL:
		printf("*");
		break;
	case DIV:
		printf("/");
		break;
	case LT:
		printf("<");
		break;
	case LTE:
		printf("<=");
		break;
	case LEFT_SHIFT:
		printf("<<");
		break;
	case GT:
		printf(">");
		break;
	case GTE:
		printf(">=");
		break;
	case RIGHT_SHIFT:
		printf(">>");
		break;
	case EQ:
		printf("==");
		break;
	case UNEQ:
		printf("!=");
		break;
	case AND:
		printf("&&");
		break;
	case OR:
		printf("||");
		break;
	case ASSIGN:
		printf("=");
		break;
	case LBRA:
		printf("[");
		break;
	case RBRA:
		printf("]");
		break;
	default:
		printf("Error TokenType");
		break;
	}
}
void print_indent(int indentNum)
{
	int i;
	for (i = 0; i < indentNum; i++)
		printf(" ");
}
void print_token_type(TokenType tokentype)
{
	switch (tokentype)
	{
	case INT:
		printf("int");
		break;
	case FRAC:
		printf("frac");
		break;
	case VOID:
		printf("void");
		break;
	case STR:
		printf("string");
		break;
	case ID:
		printf("ID");
		break;
	case INTL:
		printf("INTL");
		break;
	case FRACL:
		printf("FRACL");
		break;
	case STRL:
		printf("STRL");
		break;
	case LPAR:
		printf("(");
		break;
	case RPAR:
		printf(")");
		break;
	case LCUR:
		printf("{");
		break;
	case RCUR:
		printf("}");
		break;
	case LBRA:
		printf("[");
		break;
	case RBRA:
		printf("]");
		break;
	case SEMI:
		printf(";");
		break;
	case COMMA:
		printf(",");
		break;
	case ASSIGN:
		printf("=");
		break;
	case PLUS:
		printf("+");
		break;
	case MINUS:
		printf("-");
		break;
	case MUL:
		printf("*");
		break;
	case DIV:
		printf("/");
		break;
	case LT:
		printf("<");
		break;
	case LTE:
		printf("<=");
		break;
	case LEFT_SHIFT:
		printf("<<");
		break;
	case GT:
		printf(">");
		break;
	case GTE:
		printf(">=");
		break;
	case RIGHT_SHIFT:
		printf(">>");
		break;
	case EQ:
		printf("==");
		break;
	case UNEQ:
		printf("!=");
		break;
	case AND:
		printf("&&");
		break;
	case OR:
		printf("||");
		break;
	case NOT:
		printf("!");
		break;
	case ERROR:
		printf("ERROR");
		break;
	case END_OF_FILE:
		printf("END_OF_FILE");
		break;
	default:
		printf("Unknown token type");
		break;
	}
}
void print_node(TreeNode *TreeNode)
{
	if (TreeNode == NULL)
		return;
	switch (TreeNode->nodeKind)
	{
	case DCL_ND:
		printf("Declaration Node\n");
		break;
	case STMT_ND:
		printf("Statement Node\n");
		break;
	case EXPR_ND:
		printf("Expression Node\n");
		break;
	case PARAM_ND:
		printf("Parameter Node\n");
		break;
	case ROOT:
		printf("Root Node\n");
		break;
	default:
		printf("Unknown Node Kind\n");
		break;
	}
	print_tree(NULL, TreeNode); // 这里传递 NULL，因为 print_tree 需要两个参数
}
const char *token_type_to_string(TokenType token)
{
	switch (token)
	{
	case PLUS:
		return "+";
	case MINUS:
		return "-";
	case MUL:
		return "*";
	case DIV:
		return "/";
	case MOD:
		return "%";
	case PPLUS:
		return "++";
	case MMINUS:
		return "--";
	case POWER:
		return "**";
	case LT:
		return "<";
	case GT:
		return ">";
	case LTE:
		return "<=";
	case GTE:
		return ">=";
	case EQ:
		return "==";
	case UNEQ:
		return "!=";
	case AAND:
		return "&&";
	case OR:
		return "||";
	case OR1:
		return "|";
	case NOT:
		return "!";
	case ASSIGN:
		return "=";
	case PLUS_ASSIGN:
		return "+=";
	case MINUS_ASSIGN:
		return "-=";
	case MUL_ASSIGN:
		return "*=";
	case DIV_ASSIGN:
		return "/=";
	case MOD_ASSIGN:
		return "%=";
	case POWER_ASSIGN:
		return "**=";
	case AND:
		return "&";
	case LEFT_SHIFT:
		return "<<";
	case RIGHT_SHIFT:
		return ">>";
	case INT:
		return "int";
	case FRAC:
		return "frac";
	case STR:
		return "string";
	case VOID:
		return "void";
	case ID:
		return "ID";
	case INTL:
		return "INTL";
	case FRACL:
		return "FRACL";
	case STRL:
		return "STRL";
	case LCUR:
		return "{";
	case RCUR:
		return "}";
	case LPAR:
		return "(";
	case RPAR:
		return ")";
	case LBRA:
		return "[";
	case RBRA:
		return "]";
	case NEWLINE:
		return "\n";
	case COLON:
		return ":";
	case SEMI:
		return ";";
	case COMMA:
		return ",";
	case INDENT:
		return "INDENT";
	case COMMENT:
		return "COMMENT";
	case DO:
		return "do";
	case WHILE:
		return "while";
	case FOR:
		return "for";
	case DEF:
		return "def";
	case RETURN:
		return "return";
	case IN:
		return "in";
	case IF:
		return "if";
	case ELIF:
		return "elif";
	case ELSE:
		return "else";
	case ERROR:
		return "ERROR";
	case END_OF_FILE:
		return "EOF";
	default:
		return "Unknown	TokenType";
	}
}

// 读取文件内容
char *read_file(const char *filename)
{
	FILE *file = fopen(filename, "r");
	if (!file)
	{
		perror("无法打开文件");
		exit(EXIT_FAILURE);
	}

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *content = (char *)malloc(length + 1);
	if (!content)
	{
		perror("内存分配失败");
		exit(EXIT_FAILURE);
	}

	fread(content, 1, length, file);
	content[length] = '\0';

	fclose(file);
	return content;
}
// 生成解析树
TreeNode *generate_parse_tree(const char *filename)
{
	Parser *parser = createParser();
	List *tokenList = scanFile(filename); // 使用 scanFile 函数
	parser->set_token_list(parser, *tokenList);
	TreeNode *tree = parser->parse(parser);
	freeList(tokenList); // 释放 tokenList
	free(tokenList);	 // 释放 tokenList 指针
	destroyParser(parser);
	return tree;
}