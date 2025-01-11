#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "scanner.h"
#include "error_handler.h"

void handleError(ErrorCode code, const char *details)
{
    switch (code)
    {
    case ERR_MEMORY_ALLOCATION_FAILED:
        fprintf(stderr, "Error: Memory allocation failed! %s\n", details);
        break;
    case ERR_FILE_OPEN_FAILED:
        fprintf(stderr, "Error: Cannot open file! %s\n", details);
        break;
    case ERR_UNKNOWN_TOKEN:
        fprintf(stderr, "Error: Unknown token encountered! %s\n", details);
        break;
    case ERR_NULL_POINTER:
        fprintf(stderr, "Error: Null pointer dereference! %s\n", details);
        break;
    // 其他错误处理
    default:
        fprintf(stderr, "Error: Unknown error occurred! %s\n", details);
        break;
    }

    // 根据需要决定是否退出程序
    exit(EXIT_FAILURE);
}
void initList(List *list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void addToken(List *list, TokenType type, const char *info)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        handleError(ERR_MEMORY_ALLOCATION_FAILED, "Failed to allocate memory for new Node in addToken.");
        return;
    }

    newNode->type = type;
    if (type == ID || type == INTL || type == FRACL || type == STRL || type == COMMENT || type == INDENT)
        newNode->info = strdup(info); // 复制字符串以确保其独立存储
    else
        newNode->info = NULL;

    // 初始化新节点的指针
    newNode->next = NULL;
    newNode->prev = NULL;

    if (list->head == NULL)
    {
        list->tail = newNode;
        list->head = newNode;
    } // 如果链表为空，则新节点是链表的第一个节点
    else
    { // 否则将其添加到链表末尾
        list->tail->next = newNode;
        newNode->prev = list->tail;
        list->tail = newNode;
    }
    list->size++;
}

void tokenizeFile(List *list, FILE *file)
{
    int ch;
    char buffer[200];
    int i = 0;
    int indentLevel = 0;
    int isStartOfLine = 1; // 用于检测行首

    while ((ch = fgetc(file)) != EOF)
    {
        if (isStartOfLine)
        {
            // 行首开始计数缩进
            indentLevel = 0;
            while (ch == ' ' || ch == '\t') // 统计空格或tab作为缩进
            {
                if (ch == ' ')
                    indentLevel += 1; // 假设一个空格是一个缩进单位
                else if (ch == '\t')
                    indentLevel += 4; // 假设一个tab等于四个空格
                ch = fgetc(file);
            }
            if (indentLevel > 0)
            {
                // 将缩进添加为一个新的 token
                char indentInfo[10];
                snprintf(indentInfo, sizeof(indentInfo), "%d", indentLevel);
                addToken(list, INDENT, indentInfo);
            }
            isStartOfLine = 0; // 一旦行首处理完毕，取消标记
        }
        if (ch == '#')
        {
            ;
            while ((ch = fgetc(file)) != '\n')
            {
                if (ch == EOF)
                    break;
            }
            addToken(list, COMMENT, NULL);
            if (ch == '\n')
            {
                addToken(list, NEWLINE, NULL);
                isStartOfLine = 1;
            }
            continue;
        }
        if (ch == '/')
        {
            int next_ch = fgetc(file);
            if (next_ch == '/')
            {
                while ((ch = fgetc(file)) != '\n' && ch != EOF)
                    ;
                addToken(list, COMMENT, NULL);
                if (ch == '\n')
                {
                    addToken(list, NEWLINE, NULL);
                    isStartOfLine = 1;
                }
                continue;
            }
            else if (next_ch == '*')
            {
                while (1)
                {
                    while ((ch = fgetc(file)) != '*' && ch != EOF)
                        ;
                    if ((ch = fgetc(file)) == '/')
                    {
                        addToken(list, COMMENT, NULL);
                        break;
                    }
                    if (ch == EOF)
                        break;
                }
                continue;
            }
            else
            {
                ungetc(next_ch, file); // 将读取的字符放回文件流
            }
        }
        else if (ch == '"')
        {
            {
                i = 0;
                buffer[i] = ch;
                i++;
                while ((ch = fgetc(file)) != '"' && ch != EOF)
                {
                    buffer[i] = ch;
                    i++;
                }
                if (ch == EOF)
                {
                    handleError(ERR_UNKNOWN_TOKEN, "Unexpected end of file while reading string literal.");
                    return;
                }
                buffer[i] = ch;
                i++;
                buffer[i] = '\0';
                addToken(list, STRL, buffer);
                fgetc(file);
                continue;
            }
        }
        else if (!isspace(ch))
        {
            if (i < sizeof(buffer) - 1) // 确保不会缓冲区溢出
            {
                buffer[i] = ch;
                i++;
            }
        }
        else if (i > 0)
        {
            buffer[i] = '\0';
            addToken(list, identifyTokenType(buffer), buffer);
            i = 0;
            if (ch == '\n')
            {
                addToken(list, NEWLINE, NULL);
                isStartOfLine = 1; // 新行开始，重置标记
            }
        }
    }

    if (i > 0)
    {
        buffer[i] = '\0';
        addToken(list, identifyTokenType(buffer), buffer);
    }
    addToken(list, END_OF_FILE, NULL);
}
TokenType identifyTokenType(const char *token)
{
    switch (token[0])
    {
    case '#':
        return COMMENT;
        break;
    case '{':
        return LCUR;
        break;
    case '}':
        return RCUR;
        break;
    case '(':
        return LPAR;
        break;
    case ')':
        return RPAR;
        break;
    case '[':
        return LBRA;
        break;
    case ']':
        return RBRA;
        break;
    case '\n':
        return NEWLINE;
        break;
    case ':':
        return COLON;
        break;
    case ';':
        return SEMI;
        break;
    case ',':
        return COMMA;
        break; // single character tokens
    case '+':
        if (token[1] == '\0')
            return PLUS;
        if (token[1] == '+')
            return PPLUS;
        if (token[1] == '=')
            return PLUS_ASSIGN;
        break;
    case '-':
        if (token[1] == '\0')
            return MINUS;
        if (token[1] == '-')
            return MMINUS;
        if (token[1] == '=')
            return MINUS_ASSIGN;
        break;
    case '*':
        if (token[1] == '\0')
            return MUL;
        if (token[1] == '*')
            return POWER;
        if (token[1] == '=')
            return MUL_ASSIGN;
        break;
    case '/':
        if (token[1] == '\0')
            return DIV;
        if (token[1] == '=')
            return DIV_ASSIGN;
        if (token[1] == '/')
            return COMMENT;
        if (token[1] == '*')
            return COMMENT; // comment
        break;
    case '%':
        if (token[1] == '\0')
            return MOD;
        if (token[1] == '=')
            return MOD_ASSIGN;
        break;
    case '<':
        if (token[1] == '\0')
            return LT;
        if (token[1] == '=')
            return LTE;
        if (token[1] == '<')
            return LEFT_SHIFT;
        break;
    case '>':
        if (token[1] == '\0')
            return GT;
        if (token[1] == '=')
            return GTE;
        if (token[1] == '>')
            return RIGHT_SHIFT;
        break;
    case '=':
        if (token[1] == '\0')
            return ASSIGN;
        if (token[1] == '=')
            return EQ;
        break;
    case '!':
        if (token[1] == '\0')
            return NOT;
        if (token[1] == '=')
            return UNEQ;
        break;
    case '&':
        if (token[1] == '&')
            return AAND;
        if (token[1] == '\0')
            return AND;
        break;
    case '|':
        if (token[1] == '|')
            return OR;
        if (token[1] == '\0')
            return OR1;
        break;
    default:
        break;
    }

    /*if(strcmp(token,"-")==0)
    return MINUS;
    if(strcmp(token,"*")==0)
    return MUL;
    if(strcmp(token,"/")==0)
    return DIV;
    if(strcmp(token,"%")==0)
    return MOD;
    if(strcmp(token,"**")==0)
    return POWER;
    if(strcmp(token,"++")==0)
    return PPLUS;
    if(strcmp(token,"--")==0)
    return MMINUS;
    if(strcmp(token,"<")==0)
    return LT;
    if(strcmp(token,">")==0)
    return GT;
    if(strcmp(token,"<=")==0)
    return LTE;
    if(strcmp(token,">=")==0)
    return GTE;
    if(strcmp(token,"==")==0)
    return EQ;
    if(strcmp(token,"!=")==0)
    return UNEQ;
    if(strcmp(token,"&&")==0)
    return AAND;
    if(strcmp(token,"||")==0)
    return OR;
    if(strcmp(token,"!")==0)
    return NOT;
    if(strcmp(token,"=")==0)
    return ASSIGN;
    if(strcmp(token,"+=")==0)
    return PLUS_ASSIGN;
    if(strcmp(token,"-=")==0)
    return MINUS_ASSIGN;
    if(strcmp(token,"*=")==0)
    return MUL_ASSIGN;
    if(strcmp(token,"/=")==0)
    return DIV_ASSIGN;
    if(strcmp(token,"%=")==0)
    return MOD_ASSIGN;
    if(strcmp(token,"**=")==0)
    return POWER_ASSIGN;
    if(strcmp(token,"&")==0)
    return AND;
    if(strcmp(token,"<<")==0)
    return LEFT_SHIFT;
    if(strcmp(token,">>")==0)
    return RIGHT_SHIFT; */
    if (isdigit(token[0]))
    {
        int i = 1;
        while (token[i] != '\0')
        {
            if (token[i] == ':' && isdigit(token[i + 1]))
                return FRACL;
            i++;
        }
        return INTL;
    }
    // STRL
    if (token[0] == '"')
        return STRL;
    /*if(strcmp(token,"{")==0)
    return LCUR;
    if(strcmp(token,"}")==0)
    return RCUR;
    if(strcmp(token,"(")==0)
    return LPAR;
    if(strcmp(token,")")==0)
    return RPAR;
    if(strcmp(token,"[")==0)
    return LBRA;
    if(strcmp(token,"]")==0)
    return RBRA;
    if(strcmp(token,"\n")==0)
    return NEWLINE;
    if(strcmp(token,":")==0)
    return COLON;
    if(strcmp(token,";")==0)
    return SEMI;
    if(strcmp(token,",")==0)
    return COMMA;
    if(strcmp(token,"#")==0||strcmp(token,"//")==0||strcmp(token,"/*")==0)
    return COMMENT; */
    if (isalpha(token[0]))
    {
        if (strcmp(token, "int") == 0)
            return INT;
        else if (strcmp(token, "frac") == 0)
            return FRAC;
        else if (strcmp(token, "str") == 0)
            return STR;
        else if (strcmp(token, "void") == 0)
            return VOID;
        else if (strcmp(token, "do") == 0)
            return DO;
        else if (strcmp(token, "while") == 0)
            return WHILE;
        else if (strcmp(token, "for") == 0)
            return FOR;
        else
            // if(strcmp(token,"def")==0)
            // return DEF;
            // else
            if (strcmp(token, "return") == 0)
                return RETURN;
            else if (strcmp(token, "in") == 0)
                return IN;
            else if (strcmp(token, "if") == 0)
                return IF;
            else if (strcmp(token, "elif") == 0)
                return ELIF;
            else if (strcmp(token, "else") == 0)
                return ELSE;
            else
                return ID;
    }
    return ERROR;
}
void printTokens(const List *list)
{
    Node *current = list->head;
    while (current)
    {
        if (current->info == NULL)
            printf("%s\n", tokenTypeNames[current->type]);
        else
        {
            printf("%s", tokenTypeNames[current->type]);
            printf(":%s\n", current->info);
        }
        current = current->next;
    }
}
void freeList(List *list)
{

    if (list == NULL || list->tail == NULL)
    {
        return;
    }
    while (list->tail != NULL)
    {
        Node *temp = list->tail;
        list->tail = list->tail->prev;
        free(temp->info);
        free(temp);
        list->size--;
    }
    list->head = NULL;
}

List* scanFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file %s\n", filename);
        handleError(ERR_FILE_OPEN_FAILED, filename);
        exit(1);
    }
    List *tokenList = (List *)malloc(sizeof(List));
    if (!tokenList) {
        printf("Memory allocation failed for token list.\n");
        fclose(file);
        exit(1);
    }

    initList(tokenList);             // 初始化链表
    tokenizeFile(tokenList, file);   // 扫描文件，生成 Token 链表
    fclose(file);                    // 关闭文件

    return tokenList;                // 返回链表指针
}
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("Cannot open file %s\n", argv[1]);
        return 1;
    }
    if (file == NULL)
    {
        handleError(ERR_FILE_OPEN_FAILED, argv[1]);
        return 1;
    }
    List tokenList;
    initList(&tokenList);

    tokenizeFile(&tokenList, file);

    printTokens(&tokenList);

    fclose(file);
    freeList(&tokenList);

    return 0;
}