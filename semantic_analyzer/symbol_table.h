#ifndef _SYMBOL_TABLE_H_
#define _SYMBOL_TABLE_H_
#include "../parser/parse.h"

/* ST_SIZE is the size of the hash table, usually a prime number */
/*#define SIZE 211 */
/* assuming commonly there are no more than 71 declarations in a block of the program.
 *
 */
#define ST_SIZE 71 

typedef struct LineListRec  //一个链表用来存储一个变量多次引用的行号 依附于一个BucketListRec
{ 
  TreeNode * nd;      // pointing to the node where the name is referenced
  struct LineListRec * prev ; // links to left neighbor
  struct LineListRec * next; // links to right neighbor
  struct BucketListRec * bk; // pointing back to the containing bucket record, which corresponds to the declaration
} * LineList; 

typedef struct BucketListRec
{   
  struct symbolTable * st; /* pointer to the containing symbol table.*/
  TreeNode * nd;  /*pointer to the declaration node or parameter node in the syntax tree.*/
  LineList lines;  /* pointer to the list of records of reference (line-list-record) of the declaration.*/
  struct BucketListRec* prev; 
  struct BucketListRec* next; 
  void * something;
  /* something is added for possible usage in code generation. For semantic analysis it is not used.
   * For function declaration: it is the address of the function in the code memory
   * For global data: it is the offset to the beginning of global data (in the data memory), which is the highest address of data memory, and is pointed by register gp
   * For parameters and local data: it is the offset to the fp (frame pointer), in the data memory.
   * */
} * BucketList;


typedef struct symbolTable SymbolTable;
struct symbolTable{
	int id;
	TreeNode * nd;
	SymbolTable * upper;
	SymbolTable * lower;
	SymbolTable * prev;
	SymbolTable * next;
	BucketList hashTable[ST_SIZE];
};
/*数据结构关系
hashTable[1] --> BucketList(a) --> NULL
                            |
                            v
                     LineListRec(2) --> LineListRec(4) --> NULL

hashTable[3] --> BucketList(b) --> NULL
                            |
                            v
                     LineListRec(4) --> NULL
*/
/* st_insert_dcl():
   [computation]:
   Make a BucketListRec according to a declaration or parameter tree node, then insert the BucketListRec into the symbol table st. Associate the tree node and the Bucket List record with each other.
   [Preconditions]: 
   --  st is not NULL, 
   --  dclNd is not NULL 
   --  dclNd  must be a declaration node or parameter node.
   --  st is a (pointer to) symbol table, which must correspond to the scope where the declaration or parameter of dclNd belongs to.
   ----- */ 
void st_insert_dcl(TreeNode * dclNd, SymbolTable * st);

/* st_insert_ref()
   [computation]:
   Make a line list rec according to refNd, and insert it into the end of the line list of a  Bucket list record bk. The order of the records in the line list may correspond to the order of their appearance in the program.
   [Preconditions]:
   - refNd is not NULL, and it is a reference to the declaration of bk. 
   - bk is not NULL, and it should correspond to the declaration for the reference of refND.  
   - The above two conditions  mean that bk should be the result of looking up refNd. 
*/
void st_insert_ref(TreeNode *refNd, struct BucketListRec* bk );


/* st_lookup()
   [computation]: 
   Find the bucket list record of the declaration associated with the name.  The search of the name starts from st. If the name is not found in st, then continue the search in the upper level table of st. It returns NULL if not found.
   [parameters]:
   - name is the search key, the name of a variable, parameter, array, or function.
   - st is the starting point of searching. When the function is called st should the symbol table corresponding to the block where the node appears. 
   [preconditions]:
   - st should not be NULL. 
   - name should not be NULL.
*/
struct BucketListRec*  st_lookup (SymbolTable* st, const char * name );


/* st_print():
   [computation]: 
   - prints formatted symbol table contents to the stdout stream.
   - Table head is printed on top.
   - Prints out error message if the symbol table is wrongly built.
 */
void st_print(SymbolTable * st);

/* st_initialize()
   [computation]:  
   Returns the pointer to an initialized empty symbol table, which is newly created.
   If the parameter restart is TRUE, then the id of the symbol table is 0, otherwise,
   the id of the symbol table is accumulating (one plus the latest value ).
*/
SymbolTable * st_initialize(Boolean restart);


/*  st_attach()
    [computation]: 
    - Attach an initialized empty symbol table at the end of st->lower
    - Returns the pointer to the newly added empty symbol table.     
    [Precondition]: st is not NULL
*/
SymbolTable * st_attach(SymbolTable* st);


/* stj_free()
 * release the space occupied by a symbol table
 */
void st_free(SymbolTable * st);


#endif
