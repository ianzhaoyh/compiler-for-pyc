#include "analyzer.h"
#include "s_analyzer.h"
#include "symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
	/* returns the symbol table that is built by the semantic analyzer */
    SymbolTable * get_symbol_table(Analyzer * self){
        AnalyzerInfo *info = (AnalyzerInfo *)self->info;
        return info->symbolTable;
    }
    
	/* Using a function to access the internal data (error information) known by the analyzer
	 * Returning TRUE means semantic error is found*/
	
    Bool check_semantic_error(Analyzer *self) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    return info->analyzerError;
}
	/* Clear all data that the analyzer remembers (in info), resume the default initial values */
	
    void clear(Analyzer *self) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    if (info) {
        st_free(info->symbolTable);
        free(info);
        self->info = NULL;
    }
}
	/* Reset the internal error record, set that status as no error is found*/
	
    void clear_error_status(Analyzer *self) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    info->analyzerError = FALSE;
}
	void set_parse_tree(Analyzer *self, TreeNode *tree) {
    AnalyzerInfo *info = (AnalyzerInfo *)self->info;
    info->parseTree = tree;
} /*Let the analyzer know the parse-tree, on which the semantic analysis will be carried out */

Analyzer *new_s_analyzer(TreeNode *parseTree) {
    Analyzer *analyzer = (Analyzer *)malloc(sizeof(Analyzer));
    if (!analyzer) {
        fprintf(stderr, "Error: Failed to allocate memory for Analyzer.\n");
        exit(1);
    }

    AnalyzerInfo *info = (AnalyzerInfo *)malloc(sizeof(AnalyzerInfo));
    if (!info) {
        fprintf(stderr, "Error: Failed to allocate memory for AnalyzerInfo.\n");
        free(analyzer);
        exit(1);
    }

    info->parseTree = parseTree;
    info->symbolTable = NULL;
    info->analyzerError = FALSE;

    analyzer->info = info;

    /* 绑定各个功能函数 */
    analyzer->build_symbol_table = build_symbol_table;
    analyzer->get_symbol_table = get_symbol_table;
    analyzer->type_check = type_check;
    analyzer->check_semantic_error = check_semantic_error;
    analyzer->clear = clear;
    analyzer->clear_error_status = clear_error_status;
    analyzer->set_parse_tree = set_parse_tree;

    return analyzer;
}

void destroyAnalyzer(Analyzer *self) {
    if (self) {
        self->clear(self);
        free(self);
    }
}
