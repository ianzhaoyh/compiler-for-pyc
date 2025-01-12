#include "libs.h"
#include "../scanner_pyc/scanner.h"
#include "parse.h"
#include "util.h"

TreeNode *parse(Parser *p)
{
    if (!p->info)
    {
        fprintf(stderr, "parse: p->info is NULL\n");
        exit(1);
    }
    ParserInfo *info = (ParserInfo *)p->info;
    info->errorCount = 0;
    info->currentTokenNode = info->tokenList.head;

    TreeNode *tree = parse_program(p); // start form program
    if (info->errorCount > 0)
    {
        fprintf(stderr, "There are %d syntax errors in the program\n", info->errorCount);
    }
    return tree;
}

void set_token_list(Parser *p, List tokenList)
{
    if (!p->info)
    {
        p->info = malloc(sizeof(ParserInfo));
        memset(p->info, 0, sizeof(ParserInfo));
    }
    ParserInfo *info = (ParserInfo *)p->info;
    info->tokenList = tokenList;
    info->currentTokenNode = tokenList.head;
}

void free_tree(Parser *p, TreeNode *tree)
{
    if (!tree)
        return;
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        free_tree(p, tree->child[i]);
    }
    free_tree(p, tree->rSibling);
    free(tree);
}

Parser *createParser()
{
    Parser *p = (Parser *)malloc(sizeof(Parser));
    p->parse = parse;
    p->set_token_list = set_token_list;
    p->free_tree = free_tree;
    p->info = NULL;
    // p->print_tree = print_tree;
    return p;
}
void destroyParser(Parser *p)
{
    if (p)
    {
        if (p->info)
        {
            free(p->info);
        }
        free(p);
    }
}
/****************************
 * 下面是具体的解析函数框架 *
 ****************************/

/* 获取当前token类型和lexeme的帮助函数 */
Token *currentToken(ParserInfo *info)
{
    if (info->currentTokenNode)
    {
        return (info->currentTokenNode->t);
    }
    return NULL; // 获得info中当下指向的Node结构体中的token
} // 获得当前
Bool checkType(Token *token, TokenType type)
{
    if (token)
    {
        return token->type == type;
    }
    return FALSE;
} // 检查当下info指向的token的与预期的token类型是否匹配

Bool moveTokenNext(ParserInfo *info)
{
    if (info->currentTokenNode && info->currentTokenNode->next)
    {
        info->currentTokenNode = info->currentTokenNode->next;
        return TRUE;
    }
    return FALSE;
} // 移动token链表的指针，指向下一个节点

Bool checkMove(ParserInfo *info, TokenType type)
{
    Token *t = info->currentTokenNode->t;
    if (t && t->type == type)
    {
        if (info->currentTokenNode && info->currentTokenNode->next)
        {
            info->currentTokenNode = info->currentTokenNode->next;
            return TRUE;
        }
        else
        {
            info->currentTokenNode = NULL;
            return TRUE;
        } // last token
    }
    else
    {
        return FALSE;
    }
} // checkType+moveTokenNext，token类型匹配之后移动指针到下一个
TreeNode *newNode(NodeKind nodeKind)
{
    TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
    if (!node)
    {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    node->nodeKind = nodeKind;
    for (int i = 0; i < MAX_CHILDREN; i++)
    {
        node->child[i] = NULL;
    }
    node->lSibling = NULL;
    node->rSibling = NULL;
    node->parent = NULL;
    node->lineNum = 0;
    switch (nodeKind)
    {
    case DCL_ND:
        node->kind.dcl = 0;
        node->attr.dclAttr.type = VOID_TYPE;
        node->attr.dclAttr.name = NULL;
        node->attr.dclAttr.size = 0;
        break;
    case STMT_ND:
        node->kind.stmt = 0;
        break;
    case EXPR_ND:
        node->kind.expr = 0;
        node->attr.exprAttr.op = 0;
        node->attr.exprAttr.val = 0;
        node->attr.exprAttr.name = NULL;
    case PARAM_ND:
        node->kind.param = 0;
        node->attr.dclAttr.type = VOID_TYPE;
        node->attr.dclAttr.name = NULL;
        node->attr.dclAttr.size = 0;
        break;
    case ROOT:
        break;
    default:
        fprintf(stderr, "Unknown node kind\n");
        exit(1);
    }
    node->type = VOID_TYPE;      // 初始化表达式类型（用于类型检查）
    node->something = NULL; // 额外字段（可选扩展）
    return node;
}
Bool canStartDeclaration(TokenType t)
{
    return (t == INT || t == FRAC || t == VOID || t == DEF);
}
Bool looksLikeFunDeclaration(ParserInfo *f)
{
    if (checkType(f->currentTokenNode->t, INT) || checkType(f->currentTokenNode->t, FRAC) || checkType(f->currentTokenNode->t, VOID) || checkType(f->currentTokenNode->t, DEF))
    {
        if (checkType(f->currentTokenNode->next->t, ID))
        {
            if (checkType(f->currentTokenNode->next->next->t, LPAR))
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}
void removeNode(TreeNode *node)
{
    if (node)
    {
        free(node);
    }
}
/****************************
 * 文法解析函数 *
 ***************************/
/*program --> declaration-list*/

/*declaration_list -> declaration_list declaration | declaration*/
TreeNode *parse_program(Parser *p)
{
    TreeNode *root = newNode(ROOT);
    Bool s;
    if ((root->child[0] = declaration_list(p->info, &s)), s == TRUE)
    {
        return root;
    }
    else
    {
        printf("Error:Failed to parse declaration_list in program. \n");
        removeNode(root);
        return NULL;
    }
    return root;
}
TreeNode *declaration_list(ParserInfo *f, Bool *status)
{
    TreeNode *result = newNode(DCL_ND);
    Bool s;
    TreeNode *firstDecl = declaration(f, &s);
    if (s == FALSE)
    {
        printf("Error: expected at least one declaration.\n");
        removeNode(result);
        *status = FALSE;
        return NULL;
    }
    result->child[0] = firstDecl;

    TreeNode *currentNode = firstDecl;
    while (canStartDeclaration(currentToken(f)->type))
    {
        TreeNode *nextDecl = declaration(f, &s);
        if (s == FALSE)
        {
            break;
        }
        result->child[1] = nextDecl; // 有问题
        currentNode->rSibling = nextDecl;
        nextDecl->lSibling = currentNode;
        currentNode = nextDecl;
    }
    *status = TRUE;
    return result;
}
/*declaration -> var_declaration | fun_declaration*/
TreeNode *declaration(ParserInfo *f, Bool *status)
{

    TreeNode *node = NULL;
    Bool s;
    if (looksLikeFunDeclaration(f))
    {
        node = fun_declaration(f, &s);
    }
    else
    {
        node = var_declaration(f, &s);
    }
    *status = s;
    return node;
}
/*var_declaration -> type_specifier ID  | type_specifier ID LBRA INTL RBRA*/
TreeNode *var_declaration(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(DCL_ND);
    Bool s;
    Token *typeToken = currentToken(f);
    if (checkMove(f, INT) || checkMove(f, FRAC) || checkMove(f, VOID) || checkMove(f, STR))
    {
        switch (typeToken->type)
        {
        case INT:
            node->attr.dclAttr.type = INT_TYPE;
            break;
        case FRAC:
            node->attr.dclAttr.type = FRAC_TYPE;
            break;
        case VOID:
            node->attr.dclAttr.type = VOID_TYPE;
            break;
        case STR:
            node->attr.dclAttr.type = STR_TYPE;
            break;
        default:
            fprintf(stderr, "Unknown type\n");
            exit(1);
            removeNode(node);
            return NULL;
        }
        Token *idToken = currentToken(f);
        if (checkMove(f, ID))
        {
            node->attr.dclAttr.name = idToken->info;
            if (checkType(currentToken(f), LBRA))
            {
                Token *sizeToken = currentToken(f);
                moveTokenNext(f);
                if (checkType(currentToken(f), INTL))
                {
                    node->kind.dcl = ARRAY_DCL;
                    node->attr.dclAttr.size = atoi(sizeToken->info);
                    if (!checkMove(f, RBRA))
                    {
                        fprintf(stderr, "Error: missing ']' in array declaration.\n");
                        *status = FALSE;
                        removeNode(node);
                        return NULL;
                    }
                }
                else
                {
                    fprintf(stderr, "Error: missing array size in array declaration.\n");
                    *status = FALSE;
                    removeNode(node);
                    return NULL;
                }
            }
            else
            {
                node->kind.dcl = VAR_DCL;
                node->attr.dclAttr.size = 0;
            }
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*fun-declaration --> type-specifier ID ( param-list ) compound-stmt | def ID (param-list): compound-stmt*/
TreeNode *fun_declaration(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(DCL_ND);
    node->kind.dcl = FUN_DCL;
    Bool s;
    Token *typeToken = currentToken(f); // 获取函数返回类型
    if (checkType(typeToken, INT) || checkType(typeToken, FRAC) || checkType(typeToken, VOID))
    {
        switch (typeToken->type)
        {
        case INT:
            node->attr.dclAttr.type = INT_TYPE;
            break;
        case FRAC:
            node->attr.dclAttr.type = FRAC_TYPE;
            break;
        case VOID:
            node->attr.dclAttr.type = VOID_TYPE;
            break;
        default:
            fprintf(stderr, "Unknown return type in function declaration.\n");
            *status = FALSE;
            removeNode(node);
            return NULL;
        }
        moveTokenNext(f);
        Token *idToken = currentToken(f); // 获取函数名
        if (checkMove(f, ID))
        {
            node->attr.dclAttr.name = idToken->info;
            if (checkMove(f, LPAR))
            {
                if ((node->child[0] = param_list(f, &s)), s == TRUE)
                {
                    if (checkMove(f, RPAR))
                    {
                        if ((node->child[1] = compound_stmt(f, &s)), s == TRUE)
                        {
                            *status = TRUE;
                            return node;
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Error: missing ')' in function declaration.\n");
                    }
                }
                else
                {
                    fprintf(stderr, "Error: missing parameters in function declaration.\n");
                }
            }
        }
    }
    else if (checkMove(f, DEF))
    {
        Token *idToken = currentToken(f); // 获取函数名
        if (checkMove(f, ID))
        {
            node->attr.dclAttr.name = idToken->info;
            node->attr.dclAttr.type = VOID_TYPE;
            if (checkMove(f, LPAR))
            {
                if ((node->child[0] = param_list(f, &s)), s == TRUE)
                {
                    if (checkMove(f, RPAR))
                    {
                        if (checkMove(f, COLON))
                        {
                            if ((node->child[1] = compound_stmt(f, &s)), s == TRUE)
                            {
                                *status = TRUE;
                                return node;
                            }
                            else
                            {
                                fprintf(stderr, "Error: failed to parse function body.\n");
                            }
                        }
                        else
                        {
                            fprintf(stderr, "Error: missing ':' in function declaration.\n");
                        }
                    }
                }
            }
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*param-list --> param-list, param | param | empty*/
TreeNode *param_list(ParserInfo *f, Bool *status)
{
    Bool s;
    TreeNode *firstParam = param(f, &s);
    if (s == FALSE)
    {
        *status = FALSE;
        return NULL; // 无参数，返回 NULL
    }

    TreeNode *currentNode = firstParam;
    while (checkMove(f, COMMA))
    {
        TreeNode *nextParam = param(f, &s);
        if (s == FALSE)
        {
            fprintf(stderr, "Error: failed to parse parameter after ','.\n");
            *status = FALSE;
            return firstParam;
        }
        currentNode->rSibling = nextParam;
        nextParam->lSibling = currentNode;
        currentNode = nextParam;
    }
    *status = TRUE;
    return firstParam;
}
/*param --> type-specifier ID | type-specifier ID[] | ID*/
TreeNode *param(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(PARAM_ND);
    Bool s;
    Token *typeToken = currentToken(f);
    if (checkType(typeToken, INT) || checkType(typeToken, FRAC) || checkType(typeToken, VOID) || checkType(typeToken, STR))
    {
        switch (typeToken->type)
        {
        case INT:
            node->attr.dclAttr.type = INT_TYPE;
            break;
        case FRAC:
            node->attr.dclAttr.type = FRAC_TYPE;
            break;
        case VOID:
            node->attr.dclAttr.type = VOID_TYPE;
            break;
        case STR:
            node->attr.dclAttr.type = STR_TYPE;
            break;
        default:
            fprintf(stderr, "Unknown type in parameter declaration.\n");
            *status = FALSE;
            removeNode(node);
            return NULL;
        }
        moveTokenNext(f);
        Token *idToken = currentToken(f);
        if (checkMove(f, ID))
        {
            node->attr.dclAttr.name = idToken->info;
            if (checkType(currentToken(f), LBRA))
            {
                moveTokenNext(f);
                if (checkType(currentToken(f), RBRA))
                {
                    node->kind.param = ARRAY_PARAM;
                    moveTokenNext(f);
                    *status = TRUE;
                    return node;
                }
                else
                {
                    fprintf(stderr, "Error: Missing ']' in array parameter.\n");
                    *status = FALSE;
                    removeNode(node);
                    return NULL;
                }
            }
            else
            {
                node->kind.param = VAR_PARAM;
            }
            *status = TRUE;
            return node;
        }
    }
    else if (checkMove(f, ID))
    {
        Token *idToken = currentToken(f);
        node->attr.dclAttr.name = idToken->info;
        node->attr.dclAttr.type = INT_TYPE;
        node->kind.param = VAR_PARAM;
        *status = TRUE;
        return node;
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*compound-stmt --> { local-declarations statement-list }*/
TreeNode *compound_stmt(ParserInfo *f, Bool *status)
{
    TreeNode *root = NULL;
    Bool s;
    if (!checkMove(f, LCUR))
    {
        fprintf(stderr, "Error: expected '{' at the start of compound statement.\n");
        *status = FALSE;
        removeNode(root);
        return NULL;
    }
    root = newNode(STMT_ND);
    root->kind.stmt = CMPD_STMT;
    if ((root->child[0] = local_declarations(f, &s)), s == TRUE)
    {
        if ((root->child[1] = statement_list(f, &s)), s == TRUE)
        {
            if (checkMove(f, RCUR))
            {
                *status = TRUE;
                return root;
            }
            else
            {
                fprintf(stderr, "Error: expected '}' at the end of compound statement.\n");
                *status = FALSE;
                removeNode(root);
                return NULL;
            }
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(root);
    return NULL;
}

/*local-declarations --> local-declarations var-declaration | empty*/
TreeNode *local_declarations(ParserInfo *f, Bool *status)
{
    TreeNode *head = NULL;
    TreeNode *tail = NULL;

    while (canStartDeclaration(currentToken(f)->type))
    {
        Bool s;
        TreeNode *newNode = var_declaration(f, &s);
        if (s == FALSE)
        {
            printf("Error: expected at least one declaration.\n");
            *status = FALSE;
            return head;
        }
        if (head == NULL)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->rSibling = newNode;
            newNode->lSibling = tail;
            tail = newNode;
        }
    }
    *status = TRUE;
    return head;
}

/*statement-list --> statement-list statement | empty*/
TreeNode *statement_list(ParserInfo *f, Bool *status)
{
    TreeNode *head = NULL;
    TreeNode *tail = NULL;
    while (TRUE)
    {
        Bool s;
        TreeNode *newNode = statement(f, &s);
        if (s == FALSE)
        {
            printf("Error: expected at least one statement.\n");
            *status = FALSE;
            return head;
        }
        if (head == NULL)
        {
            head = newNode;
            tail = newNode;
        }
        else
        {
            tail->rSibling = newNode;
            newNode->lSibling = tail;
            tail = newNode;
        }
    }
    *status = TRUE;
    return head;
}
/*statement --> expression-stmt | compound-stmt | selection-stmt | iteration-stmt | return-stmt*/
TreeNode *statement(ParserInfo *f, Bool *status)
{
    TreeNode *node = NULL;
    Bool s;
    Token *t = currentToken(f);
    if (!t)
    {
        *status = FALSE;
        return NULL;
    }
    if (checkType(t, LCUR))
        node = compound_stmt(f, &s);
    else if (checkType(t, IF))
    {
        node = selection_stmt(f, &s);
    }
    else if (checkType(t, WHILE) || checkType(t, DO) || checkType(t, FOR))
    {
        node = iteration_stmt(f, &s);
    }
    else if (checkType(t, RETURN))
    {
        node = return_stmt(f, &s);
    }
    else
    {
        node = expression_stmt(f, &s);
    }
    *status = s;
    return node;
}
/*expression-stmt --> expression ; | ;*/
TreeNode *expression_stmt(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(STMT_ND);
    node->kind.stmt = EXPR_STMT;
    node->lineNum = currentToken(f)->lineNum;
    node->type = VOID_TYPE;
    Bool s;
    if (checkType(currentToken(f), SEMI))
    {
        moveTokenNext(f);
        *status = TRUE;
        return node;
    }
    else
    {
        if ((node->child[0] = expression(f, &s)), s == TRUE)
        {
            if (checkMove(f, SEMI))
            {
                *status = TRUE;
                return node;
            }
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*selection-stmt --> if ( expression ) statement | if ( expression ) statement else statement | if expression : statement | if expression : statement else : statement*/
TreeNode *selection_stmt(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(STMT_ND);
    Bool s;
    node->kind.stmt = SLCT_STMT;
    node->lineNum = currentToken(f)->lineNum;
    if (!checkMove(f, IF))
    {
        printf("missing if in selection statement");
        *status = FALSE;
        removeNode(node);
        return NULL;
    }

    Token *t = currentToken(f);
    if (!t)
    {
        printf("unexpected end of tokens after 'if'");
        removeNode(node);
        *status = FALSE;
        return NULL;
    }

    if (checkType(t, LPAR))
    {
        moveTokenNext(f);
        if (node->child[0] = expression(f, &s), s == TRUE)
        {
            if (checkMove(f, RPAR))
            {
                if (node->child[1] = statement(f, &s), s == TRUE)
                {
                    if (checkType(currentToken(f), ELSE))
                    {
                        moveTokenNext(f);
                        if (node->child[2] = statement(f, &s), s == TRUE)
                        {
                            *status = TRUE;
                            return node;
                        }
                    }
                    else
                    {
                        *status = TRUE;
                        return node;
                    }
                }
            }
        }
    }
    else
    {
        if (node->child[0] = expression(f, &s), s == TRUE)
        {
            if (checkMove(f, COLON))
            {
                if (node->child[1] = statement(f, &s), s == TRUE)
                {
                    if (checkType(currentToken(f), ELSE))
                    {
                        moveTokenNext(f);
                        if (checkMove(f, COLON))
                        {
                            if (node->child[2] = statement(f, &s), s == TRUE)
                            {
                                *status = TRUE;
                                return node;
                            }
                        }
                    }
                    else
                    {
                        *status = TRUE;
                        return node;
                    }
                }
            }
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*iteration-stmt --> while expression : statement | while ( expression ) statement | do statement while (expression) | do: statement while expression | for（expression；expression；expression）statement ｜ for ID in expression : statement*/
TreeNode *iteration_stmt(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(STMT_ND);
    node->lineNum = currentToken(f)->lineNum; // 记录行号
    Bool s;

    /* ---------- WHILE 循环 ---------- */
    if (checkType(currentToken(f), WHILE))
    {
        node->kind.stmt = WHILE_STMT;
        moveTokenNext(f);

        // C风格：while ( condition ) { statement }
        if (checkType(currentToken(f), LPAR))
        {
            moveTokenNext(f);
            if ((node->child[0] = expression(f, &s)), s == TRUE) // 条件表达式
            {
                if (checkMove(f, RPAR)) // 匹配右括号
                {
                    if ((node->child[1] = statement(f, &s)), s == TRUE) // 循环体
                    {
                        *status = TRUE;
                        return node;
                    }
                }
            }
        }
        // Python风格：while condition : statement
        else if ((node->child[0] = expression(f, &s)), s == TRUE)
        {
            if (checkMove(f, COLON))
            {
                if ((node->child[1] = statement(f, &s)), s == TRUE)
                {
                    *status = TRUE;
                    return node;
                }
            }
        }
    }

    /* ---------- DO-WHILE 循环 ---------- */
    else if (checkType(currentToken(f), DO))
    {
        node->kind.stmt = DO_WHILE_STMT;
        moveTokenNext(f);

        if ((node->child[0] = statement(f, &s)), s == TRUE) // 循环体
        {
            if (checkMove(f, WHILE))
            {
                if (checkMove(f, LPAR))
                {
                    if ((node->child[1] = expression(f, &s)), s == TRUE) // 条件表达式
                    {
                        if (checkMove(f, RPAR))
                        {
                            *status = TRUE;
                            return node;
                        }
                    }
                }
            }
        }
    }

    /* ---------- FOR 循环 ---------- */
    else if (checkType(currentToken(f), FOR))
    {
        node->kind.stmt = FOR_STMT;
        moveTokenNext(f);

        // C风格：for (expr1; expr2; expr3) statement
        if (checkMove(f, LPAR))
        {
            node->child[0] = expression(f, &s); // 初始化表达式
            if (!checkMove(f, SEMI))
                goto ERROR;

            node->child[1] = expression(f, &s); // 条件表达式
            if (!checkMove(f, SEMI))
                goto ERROR;

            node->child[2] = expression(f, &s); // 更新表达式
            if (!checkMove(f, RPAR))
                goto ERROR;

            if ((node->child[3] = statement(f, &s)), s == TRUE) // 循环体
            {
                *status = TRUE;
                return node;
            }
        }
        // Python风格：for ID in expression : statement
        else if (checkType(currentToken(f), ID))
        {
            TreeNode *iterVar = newNode(EXPR_ND); // 迭代变量
            iterVar->kind.expr = ID_EXPR;
            iterVar->attr.exprAttr.name = currentToken(f)->info;
            iterVar->lineNum = currentToken(f)->lineNum;
            node->child[0] = iterVar;
            moveTokenNext(f);

            if (checkMove(f, IN)) // 匹配 `in`
            {
                if ((node->child[1] = expression(f, &s)), s == TRUE) // 迭代对象
                {
                    if (checkMove(f, COLON))
                    {
                        if ((node->child[2] = statement(f, &s)), s == TRUE) // 循环体
                        {
                            *status = TRUE;
                            return node;
                        }
                    }
                }
            }
        }
    }

ERROR:
    printf("Error: Invalid iteration statement.\n");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*return-stmt --> return expression ; | return ;*/
TreeNode *return_stmt(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(STMT_ND);
    node->kind.stmt = RTN_STMT;               // 标记为 return 语句
    node->lineNum = currentToken(f)->lineNum; // 记录行号
    Bool s;

    // 匹配 "return" 关键字
    if (checkMove(f, RETURN))
    {
        // 情况 1：无返回值的 return;
        if (checkType(currentToken(f), SEMI))
        {
            moveTokenNext(f); // 消耗 ";"
            *status = TRUE;
            return node;
        }
        else
        {
            // 情况 2：有返回值的 return expression;
            if ((node->child[0] = expression(f, &s)), s == TRUE)
            {
                if (checkMove(f, SEMI)) // 检查 ";"
                {
                    *status = TRUE;
                    return node;
                }
            }
        }
    }

    // 如果上述条件都不满足，说明语法有误
    printf("Syntax error: Invalid return statement\n");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*expression --> var = expression | simple-expression*/
TreeNode *expression(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);
   
    Bool s;
    if (node->child[0] = var(f, &s), s == TRUE)
    {   
       
        if (checkMove(f, ASSIGN))
        {   
             node->kind.expr = ASN_EXPR;
            if (node->child[1] = expression(f, &s), s == TRUE)
            {
                *status = TRUE;
                return node;
            }
        }
    }
    else
    {
        if (node->child[0] = simple_expression(f, &s), s == TRUE)
        {   
            node->kind.expr = OP_EXPR;
            *status = TRUE;
            return node;
        }
    }
    printf("something wrong");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*var --> ID | ID [ expression ]*/
TreeNode *var(ParserInfo *f, Bool *status)
{   
    TreeNode *node = newNode(EXPR_ND); 
    node->lineNum = currentToken(f)->lineNum;
    Bool s;
    if (checkMove(f, ID))
    {   
        node->kind.expr = ID_EXPR;
        node->attr.exprAttr.name = currentToken(f)->info;
        if (checkType(currentToken(f), LBRA))
        {
            moveTokenNext(f);
            node->kind.expr = ARRAY_EXPR;
            if (node->child[0] = expression(f, &s), s == TRUE)
            {
                if (checkMove(f, RBRA))
                {
                    *status = TRUE;
                    return node;
                }
            }
            printf("Syntax error: Incomplete array access\n");
            *status = FALSE;
            removeNode(node);
            return NULL;
        }
        *status = TRUE;
        return node;
    }
    printf("Syntax error: Expected variable identifier\n");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*simple-expression --> additive-expression relop additive-expression | additive-expression*/
TreeNode *simple_expression(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);
    node->lineNum = currentToken(f)->lineNum;
    Bool s;
    if (node->child[0] = additive_expression(f, &s), s == TRUE)
    {
        if (node->child[1] = relop(f, &s), s == TRUE)
        {   
            node->kind.expr = OP_EXPR;
            node->attr.exprAttr.op = currentToken(f)->type;
            if (node->child[2] = additive_expression(f, &s), s == TRUE)
            {
                *status = TRUE;
                return node;
            }
            else{
                 printf("Error: Invalid right-hand side in relational expression.\n");
                *status = FALSE;
                removeNode(node);
                return NULL;
            }
        }
        node->kind.expr = OP_EXPR;
        *status = TRUE;
        return node;
    }
    printf("Error: Invalid simple expression.\n");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*relop --> < | <= | > | >= | == | !=*/
TreeNode *relop(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);
    node->lineNum = currentToken(f)->lineNum;
    Bool s;
    Token *t = currentToken(f);
    if (checkMove(f, LT) || checkMove(f, LTE) || checkMove(f, GT) || checkMove(f, GTE) || checkMove(f, EQ) || checkMove(f, UNEQ))
    {   
        node->kind.expr = OP_EXPR;
        node->attr.exprAttr.op = t->type;
        *status = TRUE;
        return node;
    }
    printf("Error: Expected relational operator.\n");
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*additive-expression --> additive-expression addop term | term*/
TreeNode *additive_expression(ParserInfo *f, Bool *status)
{
    Bool s;
    TreeNode *root=NULL;
    TreeNode *t=term(f,&s);
    if(s==FALSE){
        *status = FALSE;
        return NULL;
    }
    root=t;
    while(5==5){
        TokenType tp = currentToken(f)->type;
        if(tp==PLUS || tp==MINUS){
            TreeNode *newRoot = newNode(EXPR_ND);
            newRoot->kind.expr=OP_EXPR;
            newRoot->lineNum=currentToken(f)->lineNum;
            newRoot->attr.exprAttr.op=tp;
            newRoot->child[0]=root;
            moveTokenNext(f);
            newRoot->child[1]=term(f,&s);
           
           if (s == FALSE) {
                printf("Error: invalid term after '+' or '-'\n");
                *status=FALSE;
                removeNode(newRoot);
                return NULL;
            
        }
           root=newRoot;
    }  else {
        break;
    }
}  
    *status=TRUE;
    return root;
}
/*addop --> + | -*/
TreeNode *addop(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);
    node->kind.expr=OP_EXPR;
    node->lineNum=currentToken(f)->lineNum;
    TokenType tp = currentToken(f)->type;
     if (tp == PLUS || tp == MINUS)
    {
        node->attr.exprAttr.op = tp;      // 记录操作符类型
        moveTokenNext(f);                 // 成功匹配，移动到下一个 Token
        *status = TRUE;
        return node;
    }

    // 如果不是 + 或 -，报错并释放节点
    printf("Syntax Error: expected '+' or '-', but got token type %d\n", tp);
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/*term --> term mulop factor | factor*/

TreeNode *term(ParserInfo *f, Bool *status)
{
    Bool s;
    TreeNode *root = NULL;

    // 1. 解析第一个 factor
    TreeNode *t = factor(f, &s);
    if (s == FALSE) {
        *status = FALSE;
        return NULL;
    }
    root = t;

    // 2. 处理连续的 * 或 / 运算
    while (TRUE) {
        TokenType tp = currentToken(f)->type;

        if (tp == MUL || tp == DIV) {
            // 创建新的操作符节点
            TreeNode *newRoot = newNode(EXPR_ND);
            newRoot->kind.expr = OP_EXPR;               // 标记为运算符节点
            newRoot->lineNum = currentToken(f)->lineNum; // 保存行号
            newRoot->attr.exprAttr.op = tp;             // 保存操作符（* 或 /）

            // 左子树指向当前根
            newRoot->child[0] = root;

            // 移动到下一个 token，并解析右侧 factor
            moveTokenNext(f);
            newRoot->child[1] = factor(f, &s);

            if (s == FALSE) {
                printf("Error: invalid factor after '*' or '/'\n");
                removeNode(newRoot);
                *status = FALSE;
                return NULL;
            }

            // 更新根节点
            root = newRoot;
        } else {
            break;  // 不是 * 或 /，退出循环
        }
    }

    *status = TRUE;
    return root;
}

/*mulop --> * | / */
TreeNode *mulop(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);
    node->kind.expr = OP_EXPR;
    node->lineNum = currentToken(f)->lineNum;

    TokenType tp = currentToken(f)->type;
    Bool s;
    if (tp == MUL || tp == DIV)
    {
        node->attr.exprAttr.op = tp;      // 记录操作符类型 (* 或 /)
        moveTokenNext(f);                 // 匹配成功，移动到下一个 Token
        *status = TRUE;
        return node;
    }

    // 错误处理：不是 * 或 / 则报错
    printf("Syntax Error: expected '*' or '/', but got token type %d\n", tp);
    *status = FALSE;
    removeNode(node);                     // 释放节点，防止内存泄漏
    return NULL;
}
/* factor --> ( expression ) | var | call | NUM | STR */
TreeNode *factor(ParserInfo *f, Bool *status)
{
    TreeNode *node = NULL;
    Bool s;

    Token *t = currentToken(f);
    if (!t) {
        printf("Syntax Error: Unexpected end of input in factor.\n");
        *status = FALSE;
        return NULL;
    }

    switch (t->type) {
        case LPAR:  // ( expression )
            moveTokenNext(f);  // Consume '('
            node = expression(f, &s);
            if (s == TRUE) {
                if (checkMove(f, RPAR)) {  // Consume ')'
                    *status = TRUE;
                    return node;
                } else {
                    printf("Syntax Error: Expected ')' after expression.\n");
                }
            }
            break;

        case ID:    // var or call
            // 先尝试解析为函数调用
            node = call(f, &s);
            if (s == FALSE) {
                // 失败则尝试解析为变量
                node = var(f, &s);
            }
            if (s == TRUE) {
                *status = TRUE;
                return node;
            }
            break;

        case INTL:  // 整数常量
        case FRACL: // 小数常量
            node = num(f, &s);
            if (s == TRUE) {
                *status = TRUE;
                return node;
            }
            break;

        case STRL:  // 字符串常量
            node = Str(f, &s);
            if (s == TRUE) {
                *status = TRUE;
                return node;
            }
            break;

        default:
            printf("Syntax Error: Unexpected token '%s' in factor.\n", t->info);
            break;
    }

    // 错误处理
    if (node) {
        removeNode(node);
    }
    *status = FALSE;
    return NULL;
}
/* num --> INTL | FRACL */
TreeNode *num(ParserInfo *f, Bool *status)
{
    Token *t = currentToken(f);  // 获取当前 Token
    if (!t) {
        printf("Syntax Error: Unexpected end of input in num.\n");
        *status = FALSE;
        return NULL;
    }

    // 创建新节点
    TreeNode *node = newNode(EXPR_ND);
    node->lineNum = t->lineNum;
    node->kind.expr = CONST_EXPR;  // 常量表达式

    if (checkMove(f, INTL))  // 处理整数常量
    {
        node->type = INT_TYPE;  // 设置类型为整数
        node->attr.exprAttr.val = atoi(t->info);  // 将字符串转换成整数
        *status = TRUE;
        return node;
    }
    else if (checkMove(f, FRACL))  // 处理浮点数常量
    {
        node->type = FRAC_TYPE;  // 设置类型为浮点数
        node->attr.exprAttr.val = atof(t->info);  // 将字符串转换成浮点数
        *status = TRUE;
        return node;
    }

    // 错误处理：既不是 INTL 也不是 FRACL
    printf("Syntax Error: Expected INTL or FRACL but got '%s'\n", t->info);
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/* Str --> STRL */
TreeNode *Str(ParserInfo *f, Bool *status)
{
    Token *t = currentToken(f);  // 获取当前 Token
    if (!t) {
        printf("Syntax Error: Unexpected end of input in string literal.\n");
        *status = FALSE;
        return NULL;
    }

    // 创建新的表达式节点
    TreeNode *node = newNode(EXPR_ND);
    node->lineNum = t->lineNum;       // 记录行号
    node->kind.expr = CONST_EXPR;     // 设置为常量表达式
    node->type = STR_TYPE;            // 设置表达式类型为字符串

    // 匹配字符串字面量
    if (checkMove(f, STRL))
    {
        node->attr.exprAttr.name = strdup(t->info);  // 保存字符串内容
        *status = TRUE;
        return node;
    }

    // 错误处理
    printf("Syntax Error: Expected string literal (STRL), but got '%s'\n", t->info);
    *status = FALSE;
    removeNode(node);
    return NULL;
}
/* call --> ID ( args ) */
TreeNode *call(ParserInfo *f, Bool *status)
{
    Token *t = currentToken(f);  // 获取当前Token
    if (!t || !checkType(t, ID)) {
        printf("Syntax Error: Expected function name (ID) at line %d.\n", t ? t->lineNum : -1);
        *status = FALSE;
        return NULL;
    }

    // 创建函数调用节点
    TreeNode *node = newNode(EXPR_ND);
    node->kind.expr = CALL_EXPR;         // 设置节点类型为函数调用
    node->lineNum = t->lineNum;          // 记录行号
    node->attr.exprAttr.name = strdup(t->info);  // 保存函数名称

    moveTokenNext(f);  // 消耗 ID

    // 匹配左括号 '('
    if (!checkMove(f, LPAR)) {
        printf("Syntax Error: Expected '(' after function name '%s' at line %d.\n", t->info, t->lineNum);
        *status = FALSE;
        removeNode(node);
        return NULL;
    }

    // 解析参数列表
    Bool s;
    node->child[0] = args(f, &s);
    if (s == FALSE) {
        printf("Syntax Error: Invalid argument list in function call '%s' at line %d.\n", t->info, t->lineNum);
        *status = FALSE;
        removeNode(node);
        return NULL;
    }

    // 匹配右括号 ')'
    if (!checkMove(f, RPAR)) {
        printf("Syntax Error: Expected ')' after arguments in function call '%s' at line %d.\n", t->info, t->lineNum);
        *status = FALSE;
        removeNode(node);
        return NULL;
    }

    *status = TRUE;
    return node;
}
/* args --> arg-list | empty */
TreeNode *args(ParserInfo *f, Bool *status)
{
    TreeNode *node = newNode(EXPR_ND);      // 参数是表达式，节点类型改为 EXPR_ND
    node->kind.expr = CALL_EXPR;            // 设置节点为函数调用参数
    node->lineNum = currentToken(f)->lineNum;

    Bool s;
    node->child[0] = arg_list(f, &s);       // 尝试解析参数列表

    if (s == TRUE)
    {
        *status = TRUE;
        return node;
    }

    // 如果参数为空，直接返回空的 ARGS 节点
    *status = TRUE;
    return node;
}
/* arg-list --> arg-list , expression | expression */
TreeNode *arg_list(ParserInfo *f, Bool *status)
{
    Bool s;
    TreeNode *head = NULL;
    TreeNode *tail = NULL;

    // 解析第一个表达式
    TreeNode *expr = expression(f, &s);
    if (s == FALSE)
    {
        printf("Syntax Error: Failed to parse the first argument expression at line %d.\n", currentToken(f)->lineNum);
        *status = FALSE;
        return NULL;
    }

    head = expr;
    tail = expr;

    // 解析后续的逗号分隔的参数
    while (checkType(currentToken(f), COMMA))
    {
        moveTokenNext(f);  // 消耗逗号

        expr = expression(f, &s);
        if (s == FALSE)
        {
            printf("Syntax Error: Failed to parse argument after ',' at line %d.\n", currentToken(f)->lineNum);
            *status = FALSE;
            removeNode(head);  // 释放之前成功解析的节点
            return NULL;
        }

        // 将新解析的参数挂到兄弟节点上
        tail->rSibling = expr;
        expr->lSibling = tail;
        tail = expr;
    }

    *status = TRUE;
    return head;
}