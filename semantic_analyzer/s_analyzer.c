#include "../scanner_pyc/scanner.h"
#include "../parser/parse.h"
#include "s_analyzer.h"
#include "symbol_table.h"

Bool A_debugAnalyzer = FALSE; /* by default as false, do not print debug information of running the analyzer*/

TreeNode *new_param_node(NodeKind kind, int lineno)
{
	TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
	if (node == NULL)
	{
		fprintf(stderr, "Out of memory error at line %d\n", lineno);
		exit(EXIT_FAILURE);
	}
	node->nodeKind = kind;
	node->lineNum = lineno;
	// 初始化其他字段
	for (int i = 0; i < MAX_CHILDREN; i++)
		node->child[i] = NULL;
	node->lSibling = NULL;
	node->rSibling = NULL;
	node->attr.dclAttr.name = NULL;
	return node;
}

/*  top_symbtb_initialize(void)
	[Computation]:
	- initialize the top-level symbol table and save it in the info of the analyzer, which is a parameter
	- Add the two dummy functions input and output
	  int read(void)
	  void write(int x)
	  The TreeNode of these to functions are not on the syntax tree. And the body of these two functions are missing.
	  We want all built-in functions can be found by the lookup function at the global symbol table.
   [Note]:
   This function must be called before any other symbol table is initialized, and it should only be called once during the whole program.
 */

void top_symbtb_initialize(AnalyzerInfo *info)
{    if (!info) {
        fprintf(stderr, "[Error] AnalyzerInfo is NULL.\n");
        exit(EXIT_FAILURE);
    }
	TreeNode *nd = info->parseTree;

	info->symbolTable = st_initialize(TRUE); /* create an empty symbol table with id 0 */

	/* add the read(), write(), and print() functions. Assume they appear at line 0. */
	TreeNode *readNd = newNode(DCL_ND);
	TreeNode *writeNd = newNode(DCL_ND);
	TreeNode *printNd = newNode(DCL_ND);
	if (readNd == NULL || writeNd == NULL || printNd == NULL)
	{
		fprintf(stderr, "Out of memory error\n");
		exit(EXIT_FAILURE);
	}
	if (nd == NULL)
	{
		return;
	}
	if (A_debugAnalyzer)
		printf("%20s \n", __FUNCTION__);

	

	readNd->attr.dclAttr.type = INT_TYPE;
	writeNd->attr.dclAttr.type = VOID_TYPE;
	readNd->attr.dclAttr.name = "read";
	writeNd->attr.dclAttr.name = "write";

	TreeNode *p1 = new_param_node(PARAM_ND, 0);
	p1->attr.dclAttr.type = VOID_TYPE;
	p1->attr.dclAttr.name = "void"; /* this is not required */

	TreeNode *p2 = new_param_node(PARAM_ND, 0);
	if (nd->child[0]->type != INT_TYPE)
	{
		p2->attr.dclAttr.type = INT_TYPE;
	}
	else
	{
		p2->attr.dclAttr.type = FRAC_TYPE;
	}
	p2->attr.dclAttr.name = "x";

	TreeNode *p3 = new_param_node(PARAM_ND, 0);
	p3->attr.dclAttr.type = STR_TYPE;
	p3->attr.dclAttr.name = "y";

	readNd->child[0] = p1;
	writeNd->child[0] = p2;
	printNd->child[0] = p3;

	st_insert_dcl(readNd, info->symbolTable);
	st_insert_dcl(writeNd, info->symbolTable);
	st_insert_dcl(printNd, info->symbolTable);
}

/*  pre_traverse()
	[Computation]:
	- It is a generic recursive syntax tree traversal routine: it applies pre_proc in preorder to the tree pointed to by t.
	[Preconditions]:
	- st is not NULL.
 */
void pre_traverse(TreeNode *t, SymbolTable *st, Bool *errorFound, SymbolTable *(*pre_proc)(TreeNode *, SymbolTable *, Bool *))
{
	if (A_debugAnalyzer)
		printf("%20s \n", __FUNCTION__);
	if (t != NULL)
	{
		SymbolTable *newSt = pre_proc(t, st, errorFound);
		for (int i = 0; i < MAX_CHILDREN; i++)
			pre_traverse(t->child[i], newSt, errorFound, pre_proc);
		pre_traverse(t->rSibling, st, errorFound, pre_proc);
	}
}

/*  post_traverse()
	[Computation]:
	- It is a generic recursive syntax tree traversal routine: it applies post_proc in post-order to the tree pointed to by t.
 */
void post_traverse(TreeNode *t, Bool *errorFound, void (*post_proc)(TreeNode *, Bool *))
{
	if (A_debugAnalyzer)
		printf("%20s \n", __FUNCTION__);
	if (t != NULL)
	{
		for (int i = 0; i < MAX_CHILDREN; i++)
			post_traverse(t->child[i], errorFound, post_proc);
		post_proc(t, errorFound);
		post_traverse(t->rSibling, errorFound, post_proc);
	}
}

Bool is_keyword(const char *name)
{
	if (A_debugAnalyzer)
		printf("%20s \n", __FUNCTION__);
	if ((strcmp(name, "if") == 0) ||
		(strcmp(name, "else") == 0) ||
		(strcmp(name, "while") == 0) ||
		(strcmp(name, "num") == 0) ||
		(strcmp(name, "void") == 0) ||
		(strcmp(name, "return") == 0))
		return TRUE;
	else
		return FALSE;
}

/* pre_proc()
[Parameters]:
- nd is node in the syntax tree.
- st is the symbol table that corresponds to the block where nd appears. st is not NULL (it is initialized).
[Computation]:
   Detailed description like c-minus.pdf
   [Updating the syntax tree]:
   - Attach a new symbol table when a new block is reached (it node is a compound statement, or a function definition).
   - if the node is a declaration node, insert a bucket list record for this declaration into the symbol table st.
   - if the node is reference of a name, look up in the symbol table st to find the bucket-list-record of the declaration of the name, and insert a line list record into the bucket list record.
   [Errors that should be detected]
   -  For a declaration node, the name to be declared is already declared in st.
   -  For a reference of a name, the name cannot be found (lookup) in the symbol table (st, and the upper ones of st), or the name can be found but is not proper (like the name of an array is found to be a function )
   - If some error is found, set the parameter * errorFound to be TRUE.
[Return]
   - If a new symbol table is attached, return it. Otherwise, return the parameter st.
 */
SymbolTable *pre_proc(TreeNode *nd, SymbolTable *st, Bool *errorFound)
{
	if (A_debugAnalyzer)
		printf("%20s \n", __FUNCTION__);

	if (nd == NULL)
		return st;

	switch (nd->nodeKind)
	{
	case STMT_ND:
		switch (nd->kind.stmt)
		{
		case CMPD_STMT:					  // Compound statement
			st = st_attach(st);			  // Create a new symbol table for the new block
			nd->symbol = st; // Attach the new symbol table to the node
			break;
		default:
			break;
		}
		break;
	case DCL_ND:
		switch (nd->kind.dcl)
		{
		case VAR_DCL:
			if (st_lookup(st, nd->attr.dclAttr.name) != NULL)
			{
				fprintf(stderr, "Error: '%s' already declared in this scope (Line %d)\n",
						nd->attr.dclAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				st_insert_dcl(nd, st);
			}
			break;
		case ARRAY_DCL:
			if (st_lookup(st, nd->attr.dclAttr.name) != NULL)
			{
				fprintf(stderr, "Error: '%s' already declared in this scope (Line %d)\n",
						nd->attr.dclAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				st_insert_dcl(nd, st);
			}
			break;
		case FUN_DCL:
			if (st_lookup(st, nd->attr.dclAttr.name) != NULL)
			{
				fprintf(stderr, "Error: '%s' already declared in this scope (Line %d)\n",
						nd->attr.dclAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				st_insert_dcl(nd, st);
			}
			break;
		default:
			break;
		}
		break;
	case EXPR_ND:
		switch (nd->kind.expr)
		{
		case ID_EXPR:
			if (is_keyword(nd->attr.exprAttr.name))
			{
				fprintf(stderr, "Error: '%s' is a keyword (Line %d)\n", nd->attr.exprAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else if (st_lookup(st, nd->attr.exprAttr.name) == NULL)
			{
				fprintf(stderr, "Error: Identifier '%s' not declared (Line %d)\n", nd->attr.exprAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				st_insert_ref(nd, st_lookup(st, nd->attr.exprAttr.name));
			}
			break;
		case ARRAY_EXPR:
			if (st_lookup(st, nd->attr.exprAttr.name) == NULL)
			{
				fprintf(stderr, "Error: Array '%s' not declared (Line %d)\n", nd->attr.exprAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				if (nd->child[0]->type != INT_TYPE)
				{
					fprintf(stderr, "Error: Array index must be an integer (Line %d)\n", nd->lineNum);
					*errorFound = TRUE;
				}
				st_insert_ref(nd, st_lookup(st, nd->attr.exprAttr.name));
			}
			break;
		case CALL_EXPR:
			if (st_lookup(st, nd->attr.exprAttr.name) == NULL)
			{
				fprintf(stderr, "Error: Identifier '%s' not declared (Line %d)\n", nd->attr.exprAttr.name, nd->lineNum);
				*errorFound = TRUE;
			}
			else
			{
				st_insert_ref(nd, st_lookup(st, nd->attr.exprAttr.name));
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	// Process children nodes
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		st = pre_proc(nd->child[i], st, errorFound);
	}

	// Process sibling nodes
	st = pre_proc(nd->rSibling, st, errorFound);

	return st;
}

/* post_proc()
[Parameters]:
- nd is node in the syntax tree.
- errorFound is a pointer to a Boolean value. It will be set with TRUE when error is found
[Computation]:
   Detailed description in c-minus.pdf
   [Updating the syntax tree]:
   -  Update the type field of an expression node.
   [Errors that should be detected]
   -  improper type. For example， for an assignment like x = e, x is num variable, the type of the type of RHS expression e is not num.
 */
void post_proc(TreeNode *nd, Bool *errorFound)
{
	if (nd == NULL)
		return;

	// 遍历子节点
	for (int i = 0; i < MAX_CHILDREN; i++)
	{
		post_proc(nd->child[i], errorFound);
	}

	// 根据节点类型进行处理
	switch (nd->nodeKind)
	{
	case EXPR_ND:
		switch (nd->kind.expr)
		{
		case ASN_EXPR:
			// 检查赋值表达式的左右类型是否匹配
			if (!checkType(nd->attr.dclAttr.token, nd->attr.exprAttr.op))
			{
				printf("Error: Type mismatch in assignment at line %d\n", nd->lineNum);
			}
			break;
		case OP_EXPR:
			// 根据操作符更新表达式类型
			if (nd->attr.exprAttr.op == PLUS || nd->attr.exprAttr.op == MINUS)
			{
				nd->type = INT_TYPE;
			}
			else if (nd->attr.exprAttr.op == LT || nd->attr.exprAttr.op == GT ||
					 nd->attr.exprAttr.op == EQ || nd->attr.exprAttr.op == UNEQ)
			{
				nd->type = INT_TYPE;
			}
			break;
		default:
			break;
		}
		break;
	case STMT_ND:
		switch (nd->kind.stmt)
		{
		case RTN_STMT:
			// 检查返回语句的类型是否正确
			if (!checkType(nd->token, RETURN))
			{
				printf("Error: Type mismatch in return statement at line %d\n", nd->lineNum);
			}
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	// 处理兄弟节点
	post_proc(nd->rSibling, errorFound);
}

/*  build_symbol_table()
	[Computation]:
	- constructs the symbol table by preorder traversal of the parse-tree that is known by the analyzer.
	- pre_proc is applied to each tree node.
 */
void build_symbol_table(Analyzer *self) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    if (!info->parseTree) {
        fprintf(stderr, "Error: Parse tree not set.\n");
        return;
    }
    top_symbtb_initialize(info);
    pre_traverse(info->parseTree, info->symbolTable, &info->analyzerError, pre_proc);
}


/* type_check()
   [Computation]:
   - type_check performs type checking by a post-order traversal on the parse-tree of an analyzer.
   - post_proc is applied to each tree node.
 */
void type_check(Analyzer *self) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    if (!info->parseTree) {
        fprintf(stderr, "Error: Parse tree not set.\n");
        return;
    }
    post_traverse(info->parseTree, &info->analyzerError, post_proc);
}
