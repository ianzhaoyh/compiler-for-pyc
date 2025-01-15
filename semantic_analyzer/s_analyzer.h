/* implements a semantic analyzer, according to the interface analyzer.h
 */

#ifndef _S_ANALYZER_H_
#define _S_ANALYZER_H_

// #include "symbol_table.h"
#include "analyzer.h"
extern Bool A_debugAnalyzer;

// the symbol table on the top level.
// extern SymbTab * symbolTable;

/* Function buildSymtab constructs the symbol
 * table by preorder traversal of the syntax tree
 */
// void build_symb_tab(TreeNode *);

/* Procedure typeCheck performs type checking
 * by a postorder syntax tree traversal
 */
// void type_check(TreeNode *);
typedef struct analyzerInfo
{
	SymbolTable *symbolTable; /* the symbol table on the top level. */
	Bool analyzerError;		  /* When TRUE, some error is found the analyzer */
	TreeNode *parseTree;	  /* the parse tree that the analyzer is working on*/
} AnalyzerInfo;

extern Bool A_debugAnalyzer;

/*  build_symbol_table()
    [Computation]:
    - constructs the symbol table by preorder traversal of the parse-tree that is known by the analyzer.
    - pre_proc is applied to each tree node.
    */
// void _table(AnalyzerInfo *info);

Analyzer *new_s_analyzer(TreeNode *parseTree);

void top_symbtb_initialize(AnalyzerInfo *info);
//void semantic_analysis(Analyzer *analyzer);
 void pre_traverse(TreeNode *t, SymbolTable *st, Bool *errorFound, SymbolTable *(*pre_proc)(TreeNode *, SymbolTable *, Bool *));
 void post_traverse(TreeNode *t, Bool *errorFound, void (*post_proc)(TreeNode *, Bool *));
 SymbolTable *pre_proc(TreeNode *nd, SymbolTable *st, Bool *errorFound);
void post_proc(TreeNode *nd, Bool *errorFound);
//void semantic_error(const TreeNode *nd, int errorNum, Bool *errorFound);
Bool is_keyword(const char *name);
void type_check(Analyzer *self);
void build_symbol_table(Analyzer* self);
#endif
