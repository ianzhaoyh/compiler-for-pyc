#include "util.h"

#include "parse.h"


/* macros of  increase/decrease indentation */
//#define INDENT indentNum+=2
//#define UNINDENT indentNum-=2
#define INDENT_GAP 2

static void print_expr_type(ExprType t){
	switch(t) {
	case VOID_TYPE: printf( "void"); break;
	case INT_TYPE: printf( "int"); break;
	case ADDR_TYPE: printf( "address"); break;
	default: printf( "Error ExpType"); break;
	}
}

/* printSpaces indents by printing spaces */
static void print_spaces(int indentNum) {
	int i;
	for (i=0;i<indentNum;i++)
		printf(" ");
}

/* procedure print_tree prints a syntax tree to the
   listing file using indentation to indicate subtrees
   handle FOR_STMT  13/nov/2014
 */
void print_tree( TreeNode * tree ){
	int i;

	/* Variable indentNum is used by printTree to
	 * store current number of spaces to indent
	 */
	static int indentNum = 0;

	indentNum+= INDENT_GAP;
	while (tree != NULL) {
		print_spaces(indentNum); /* Each case only prints one line, If print more than one line, need use printSpaces() first.*/
		//printf("%d ",  tree->lineNum);
		if (tree->nodeKind == DCL_ND){
			printf( "Declare:  ");
			print_expr_type(tree->attr.dclAttr.type);
			printf( " %s ", tree->attr.dclAttr.name );
			// print the [size] only if it is an array.
			switch(tree->kind.dcl){
			case ARRAY_DCL:
				printf( "[%d]\n", tree->attr.dclAttr.size );
				break;
			case FUN_DCL:
				printf( "function with parameters :\n");
				// Function parameters will be saved as child[0] of the node
				break;
			case VAR_DCL:
				// do nothing
				printf( "\n");
				break;
			default:
				printf("Unknown DclNode kind\n");
				break;
			}
		}
		else if (tree->nodeKind==PARAM_ND){
			printf( "Parameter: ");
			print_expr_type(tree->attr.dclAttr.type);
			if(tree->attr.dclAttr.type != VOID_TYPE){
				printf(" %s", tree->attr.dclAttr.name);
				if (tree->kind.param == ARRAY_PARAM)
					printf( "[ ]");
			}
			printf( "\n");
		}
		else if(tree->nodeKind==STMT_ND) {
			switch (tree->kind.stmt) {
			case SLCT_STMT:
				printf("If ");
				if (tree->child[2] != NULL)  // has else part
					printf( " with ELSE \n");
				else
					printf( " without ELSE \n");
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
				//if there is a return value, it is  child[0].
				break;
			case NULL_STMT:
				printf( "Null statement:  ;\n");
				break;
			default:
				printf("Unknown StmtNode kind\n");
				break;
			}
		}
		else if(tree->nodeKind==EXPR_ND) {
			switch (tree->kind.expr) {
			case OP_EXPR:
				printf("Operator: ");
				if(tree->attr.exprAttr.op == LBR)
					printf( "[] index operator");
				else
					printf("%s", token_type_to_string(tree->attr.exprAttr.op));
				printf("\n");
				break;
			case CONST_EXPR:
				printf("Const: %d\n",tree->attr.exprAttr.val);
				break;
			case ID_EXPR:
				printf("ID: %s\n",tree->attr.exprAttr.name);
				break;
			/*
			case ARRAY_EXPR:
				printf("Array: %s, with member index:\n",tree->attr.exprAttr.name);
				break;
			*/
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
		else printf("Unknown node kind\n");
		for (i=0;i<MAX_CHILDREN;i++)
			print_tree(tree->child[i]);
		tree = tree->rSibling;
	}// end of while loop.
	indentNum -= INDENT_GAP;
}
