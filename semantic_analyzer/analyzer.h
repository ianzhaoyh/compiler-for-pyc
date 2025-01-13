/*
 * A general interface of the analyzer is described by analyzer.h. A user of the analyzer does not need to know
 * the details of the implementation is hidden to the user. Different analyzers could be implemented
 * sharing the same interface, and how to use these analyzers are the same.
 *
 */

#ifndef  _ANALYZER_H_
#define  _ANALYZER_H_

#include "../parser/parse.h"
#include "symbol_table.h"


typedef struct analyzer Analyzer;

/*st stands for symbol table*/
struct analyzer{
	/* build a symbol table based on a parse-tree that the analyzer knows. */
	void (* build_symbol_table)(Analyzer * self);
	/* returns the symbol table that is built by the semantic analyzer */
    SymbolTable * (* get_symbol_table)(Analyzer * self);
	void (* type_check)(Analyzer * self);
	/* Using a function to access the internal data (error information) known by the analyzer
	 * Returning TRUE means semantic error is found*/
	Bool (*check_semantic_error)(Analyzer * self);
	/* Clear all data that the analyzer remembers (in info), resume the default initial values */
	void (*clear)(Analyzer * self);
	/* Reset the internal error record, set that status as no error is found*/
	void (*clear_error_status)(Analyzer * self);
	void (*set_parse_tree)(Analyzer * self, TreeNode * tree); /*Let the analyzer know the parse-tree, on which the semantic analysis will be carried out */
	void * info; /* All of the data that the analyzer need to know is included in info
	                including the parse-tree, the symbol-table, and the flag of whether error is found
	                by the semantic analyzer */
};


#endif /* _ANALYZER_H_ */
