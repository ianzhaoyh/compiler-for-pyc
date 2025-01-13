#include <stdio.h>
#include <stdlib.h>
#include "./parse.h"
#include "../scanner_pyc/scanner.h"
#include "parse_print.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // 1词法分析：获取 Token 链表（指针）
    List *tokenList = scanFile(filename);
    if (!tokenList || tokenList->head == NULL) {
        fprintf(stderr, "Lexical analysis failed.\n");
        return 1;
    }
    printf("Lexical analysis completed successfully.\n");

    
    printf("----- Token List -----\n");
    printTokens(tokenList);  // 打印所有Token
    printf("---------------------\n");
    //  创建 Parser
    Parser *parser = createParser();
    printf("Parser created.\n");
    parser->set_token_list(parser, *tokenList);  // 传递链表内容

    // 语法分析：生成语法树
    TreeNode *syntaxTree = parser->parse(parser);
    if (!syntaxTree) {
        fprintf(stderr, "Parsing failed.\n");
        destroyParser(parser);
        freeList(tokenList);
        free(tokenList);
        return 1;
    }

    printf("Parsing completed successfully.\n");

    //  打印语法树
    printf("\n==== Syntax Tree ====\n");
    print_tree(parser, syntaxTree);

    // 释放资源
    parser->free_tree(parser, syntaxTree);
    destroyParser(parser);
    freeList(tokenList);
    free(tokenList);

    printf("\nFinished.\n");
    return 0;
}