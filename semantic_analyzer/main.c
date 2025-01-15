#include <stdio.h>
#include <stdlib.h>
#include "../parser/parse.h"
#include "../scanner_pyc/scanner.h"
#include "../parser/parse_print.h"
#include "../semantic_analyzer/analyzer.h"  // 引入语义分析器

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // 1. 词法分析：获取 Token 链表（指针）
    List *tokenList = scanFile(filename);
    if (!tokenList || tokenList->head == NULL) {
        fprintf(stderr, "Lexical analysis failed.\n");
        return 1;
    }
    printf("Lexical analysis completed successfully.\n");

    printf("----- Token List -----\n");
    printTokens(tokenList);  // 打印所有 Token
    printf("---------------------\n");

    // 2. 创建 Parser
    Parser *parser = createParser();
    printf("Parser created.\n");
    parser->set_token_list(parser, *tokenList);  // 传递链表内容

    // 3. 语法分析：生成语法树
    TreeNode *syntaxTree = parser->parse(parser);
    if (!syntaxTree) {
        fprintf(stderr, "Parsing failed.\n");
        destroyParser(parser);
        freeList(tokenList);
        free(tokenList);
        return 1;
    }
    printf("Parsing completed successfully.\n");

    // 4. 打印语法树
    printf("\n==== Syntax Tree ====\n");
    print_tree(parser, syntaxTree);

    // 5. **语义分析阶段**
    printf("\n==== Semantic Analysis ====\n");

    // 5.1 创建语义分析器
    Analyzer *analyzer = new_s_analyzer(syntaxTree);

    // 5.2 构建符号表
    analyzer->build_symbol_table(analyzer);
    printf("Symbol Table built successfully.\n");

    // 5.3 类型检查
    analyzer->type_check(analyzer);
    printf("Type checking completed.\n");

    // 5.4 检查是否有语义错误
    if (analyzer->check_semantic_error(analyzer)) {
        printf("Semantic errors detected.\n");
    } else {
        printf("No semantic errors.\n");
    }

    // 6. 释放资源
    analyzer->clear(analyzer);   // 清空语义分析器数据
    destroyAnalyzer(analyzer);  // 销毁语义分析器

    parser->free_tree(parser, syntaxTree);  // 释放语法树
    destroyParser(parser);                  // 销毁解析器
    freeList(tokenList);                   // 释放 Token 列表
    free(tokenList);

    printf("\nFinished.\n");
    return 0;
}