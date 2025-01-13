#include "../parser/util.h"
#include "../scanner_pyc/scanner.h"
#include "../parser/parse.h"
#include "s_analyzer.h"
#include "symbol_table.h"


Bool A_debugAnalyzer = FALSE;  /* by default as false, do not print debug information of running the analyzer*/

static void top_symbtb_initialize(Analyzer * ana);

static void pre_traverse( TreeNode * t, SymbolTable * st, Bool * errorFound, SymbolTable* (* pre_proc) (TreeNode *, SymbolTable *, Bool *));
static void post_traverse( TreeNode * t, Bool * errorFound, void (* post_proc) (TreeNode *, Bool *) );
static SymbolTable* pre_proc(TreeNode * nd, SymbolTable * st, Bool * errorFound);
static void post_proc(TreeNode * nd, Bool * errorFound);
static void semantic_error(const TreeNode* nd, int errorNum, Bool * errorFound);
static Bool is_keyword(const char * name);

typedef struct analyzerInfo{
	SymbolTable * symbolTable; /* the symbol table on the top level. */
	Bool analyzerError; /* When TRUE, some error is found the analyzer */
	TreeNode* parseTree; /* the parse tree that the analyzer is working on*/
} AnalyzerInfo;


//SymbolTable* symbolTable;

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
static void top_symbtb_initialize(AnalyzerInfo * info){
	int j;
	/* add the read(), write(), and print() functions. Assume they appear at line 0.*/
	TreeNode * readNd  = new_dcl_node(FUN_DCL, 0);
	TreeNode * writeNd = new_dcl_node(FUN_DCL, 0);
	TreeNode * printNd = new_dcl_node(FUN_DCL, 0);
	TreeNode * p1, * p2, * p3; 

	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);

	info->symbolTable = st_initialize(TRUE); /* create an empty symbol table with id 0 */
	/*  The new_dcl_node will assign these fields to be NULL.
	inputNd->parent = outputNd->parent = inputNd->lSibling = inputNd->rSibling = outputNd->lSibling = outputNd->rSibling = NULL;
	 */
	readNd->attr.dclAttr.type = NUM_TYPE;
	writeNd->attr.dclAttr.type = VOID_TYPE;
	readNd->attr.dclAttr.name = "read";
	writeNd->attr.dclAttr.name = "write";
	p1 = new_param_node(VOID_PARAM, 0);
	p1->attr.dclAttr.type = VOID_TYPE;
	p1->attr.dclAttr.name = "void"; /*this is not required */
	/* new_param_node() has already assigned the lineNum with 0.
	 * p1->lineNum = 0;
	 */
	p2 = new_param_node(VAR_PARAM, 0);
	p2->attr.dclAttr.type = NUM_TYPE;
	p2->attr.dclAttr.name = "x";
	/* p2->lineNum = 0; */

	p3 = new_param_node(VAR_PARAM, 0);
	p3->attr.dclAttr.type = STRING_TYPE;
	p2->attr.dclAttr.name = "y";

	readNd->child[0] = p1;
	writeNd->child[0] = p2;
	printNd->child[0] = p2;
	st_insert_dcl(inputNd, info->symbolTable);
	st_insert_dcl(outputNd, info->symbolTable);
	st_insert_dcl(printNd, info->symbolTable);
}



/*  pre_traverse()
    [Computation]:  
    - It is a generic recursive syntax tree traversal routine: it applies pre_proc in preorder to the tree pointed to by t.
    [Preconditions]:
    - st is not NULL.
 */
static void pre_traverse( TreeNode * t, SymbolTable * st, Bool * errorFound, SymbolTable* (* pre_proc) (TreeNode *, SymbolTable *, Bool *)){
	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);
	if (t != NULL){
		SymbolTable* newSt = pre_proc(t, st, errorFound);
		int i;
		for (i=0; i < MAX_CHILDREN; i++)
			pre_traverse(t->child[i], newSt, errorFound, pre_proc);
		pre_traverse(t->rSibling, st, errorFound, pre_proc);
		/* siblings of the node t share the same scope with nd, since they are in the same same block. */
	}
}


/*  post_traverse()
    [Computation]:  
    - It is a generic recursive syntax tree traversal routine: it applies post_proc in post-order to the tree pointed to by t.
 */
static void post_traverse( TreeNode * t, Bool * errorFound, void (* post_proc) (TreeNode *, Bool *) ){
	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);
	if (t != NULL){
		int i;
		for (i=0; i < MAX_CHILDREN; i++)
			post_traverse(t->child[i],errorFound, post_proc );
		post_proc(t, errorFound);
		post_traverse(t->rSibling, errorFound, post_proc);
	}
}



static Bool is_keyword(const char * name){
	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);
	if( 	(strcmp(name, "if") == 0) ||
			(strcmp(name, "else") == 0) ||
			(strcmp(name, "while") == 0) ||
			(strcmp(name, "num") == 0) ||
			(strcmp(name, "void") == 0) ||
			(strcmp(name, "return") == 0) 
	)
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
static SymbolTable* pre_proc(TreeNode * nd, SymbolTable * st, Boolean * errorFound){
	/* !!!!!!!!! Please put your code here !!!!!!!!!!!!! */
	
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
static void post_proc(TreeNode * nd, Boolean * errorFound){
	/* !!!!!!!!! Please put your code here !!!!!!!!!!!!! */
}


/*  build_symbol_table()
    [Computation]:
    - constructs the symbol table by preorder traversal of the parse-tree that is known by the analyzer.
    - pre_proc is applied to each tree node.
 */
void build_symbol_table(AnalyzerInfo * info){
	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);
	/*initialize the symbol_table */
	top_symbtb_initialize(ana);
	pre_traverse(info->parseTree, info->symbolTable, &(info->analyzerError), pre_proc);
}


/* type_check()
   [Computation]:
   - type_check performs type checking by a post-order traversal on the parse-tree of an analyzer.
   - post_proc is applied to each tree node.
 */
void type_check(AnalyzerInfo * info){
	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);
	post_traverse(info->parseTree,  &(info->analyzerError), post_proc);
}