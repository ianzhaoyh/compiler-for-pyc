/***************************************************
 File: symbol_table.c
 Symbol table implementation for the C-Minus compiler
 Symbol table is implemented as a chained   hash table 
 Symbol tables are linked together according to scope information     
 Based on the textbook
 Compiler Construction: Principles and Practice   
 Provided by Zhiyao Liang
 MUST compiler  2024 Fall
 ****************************************************/


#include "../parser/parse.h"
#include "symbol_table.h"

/* SHIFT is the power of two used as multiplier in hash function  */
#define SHIFT 4

/* hash()
   [computation]: The hash function. It generates an integer according to the key, which is a string.
 */
static int hash( const char * key ){
	int temp = 0;
	int i = 0;
	while (key[i] != '\0'){
		temp = ((temp << SHIFT) + key[i]) % ST_SIZE;
		++i;
	}
	return temp;
}



/* The "something" field in a tree node has the following meaning: 
  - for a declaration node, something is a pointer to the bucket-list-record in the symbol table. 
  - for a reference node (where a name is used), something is a pointer to the line-list-record int he symbol table. 
  - for other kind of nodes, the something field is meaningless and should not be used 
 */
/* st_insert_dcl():
   [parameters]
   - dclND: an address of a tree node. 
   - st : an address of a symbole table. 
   [precondition]
   - dclND should be a declaration node or parameter node. It should not be NULL.
   - st should not be NULL. st must correspond to the scope (block) where the declaration or parameter of dclNd belongs to. Remember that each scope has a unique symbol table.
   [computation]:
   1) Make a BucketListRec according to dclNd. The st field of the bucket-list-record should be the parameter st. 
   2) Insert the bucket-list-record into the symbol table st. this process has two steps:
      2.1)  Use the hash function to find the has value, say v,  of the name attribute of the node dclNd. 
      2.2)  Insert the bucket-list-record at beginning of the list located at the index v (the hash value) of the array of the symbol-table st. 
   3) Associate the node dclNd and the bucket-list-record with each other. I.e., assign the nd field of the buckt-list-record, and the something field of the tree-node dclNd, should be each other's address. 
   [Implementation notes]:
   insert the record at the beginning of the list, not the end. Since the order of records in a bucket-list do not correspond to their appearance order in the scource file, the order of the buck list records does not matter.
   ----- */ 

 
void st_insert_dcl(TreeNode *dclNd, SymbolTable *st){
/* !!!!!!!!! Please put your code here !!!!!!!!!!!!! */
	
}


/* ----------------
   st_insert_ref()
   [parameters]
   - refNd: the address of a tree node. 
   - bk: the address of a bucket-list-record. 
   [precondition]
   - refNd is not NULL, and it is a reference to the declaration of the other parameter bk.
   - bk is not NULL, and it should correspond to the declaration for the reference of refND. This pre-condition suggests bk should be the result of calling the look-up function with refNd. 
   [computation]:
   1) Make a line-list-record according to refNd. 
   2) Insert the line-list-record at the end of the line-list of the bucket-list-record bk.
   3) Associate the tree-node refNd and the line-list-record with each other. I.e, the something field of refNd, and nd field of the line-list-record should be the addressses of the record and the node, respectively.
   [Implementation notes]:
   - The order of the records in the line list may correspond to the order of their appearance in the program, that is why insert the record at the end of the list.
 --------------*/
void  st_insert_ref(TreeNode *refNd, struct BucketListRec* bk){
/* !!!!!!!!! Please put your code here !!!!!!!!!!!!! */

}


/* 
   st_lookup()
   [parameters]
   - st: the address of a symbol-table.
   - name: a string. 
   [preconditions]:
   - name should not be NULL.  name is the search key, the name of a variable, array, or function. 
   - st should not be NULL.   st is the starting point of searching. When the function is called st should the symbol table corresponding to the block where the name appears. 
   [computation]: 
   Find the bucket list record of the declaration associated with the name.  The search of the name starts from st. If the name is not found in st, then continue the search in the upper level table of st, ... until it is found or the top-most symbol table (for the global names) is reached used in the searching. 
   [return]
   -  returns NULL if the name not found. 
   -  returns the bucket-list-record that corresponds to the declaration of the name.
 */
struct BucketListRec*  st_lookup (SymbolTable* st,  const char * name ){
/* !!!!!!!!! Please put your code here !!!!!!!!!!!!! */

}

/* st_print():
   [computation]: 
   - prints formatted symbol table contents to  the screen (stdout)
   - Table head is printed on top.
   - Prints out error message if the symbol table is wrongly built.
   [implementation]:
   - uses a static variable to avoid printing the table head in recursive calls. 
 */
void st_print( SymbolTable* st){
	int i;
	static int flag = 0; /* flag is used to control that the table head is only printed once. */
	if (st==NULL) return ;
	if(flag == 0) { /* only print the head of the table once, at the top of the table.*/
		printf("%-6s%-15s%-12s%-5s%-9s\n","Table", "Name","Kind","Dcl","Ref");
		printf("%-6s%-15s%-12s%-5s%-9s\n","ID", "","","line","lines");
		printf("%-6s%-15s%-12s%-5s%-9s\n","----","----","----","----","----");
	}
	for (i=0;i<ST_SIZE;++i){
		BucketList bl = st->hashTable[i];
		while (bl != NULL){
			LineList lines;
			TreeNode * nd = bl->nd;
			printf( "%-6d", st->id);
			/* both parameter and declaration store name in attr.dclAttr.name */
			printf("%-15s",nd->attr.dclAttr.name);
			if(nd->nodeKind == DCL_ND) /* a declaration node */
				switch(nd->kind.dcl){
				case VAR_DCL: printf("%-12s","Var"); break;
				case ARRAY_DCL: printf("%-12s","Array"); break;
				case FUN_DCL: printf("%-12s","Func"); break;
				default:
					printf("\n%s\n","st_print(): wrong declaration type, the symbol table records a wrong node.");
					//errorFound = TRUE;
					exit(1);
					break;
				}
			else if(nd->nodeKind == PARAM_ND)
				switch(nd->kind.param){
				case VAR_PARAM: printf("%-12s","Var_Param"); break;
				case ARRAY_PARAM: printf("%-12s","Array_Param"); break;
				case VOID_PARAM: printf( "%-12s", "Void_Param"); break;
				default:
					printf("\n%s\n","st_print(): wrong parameter type, the symbol table records a wrong node.");
					//errorFound = TRUE;
					exit(1);
					break;
				}
			else { // not a declaration node or parameter node
				printf("\n%s\n","st_print(): wrong node type, the symbol table records a wrong node.");
				//errorFound = TRUE;
				exit(1);
			}
			printf("%-5d",nd->lineNum);
			lines = bl->lines;
			while (lines != NULL){
				printf("%d ",lines->nd->lineNum);
				lines = lines->next;
			}
			printf("\n");
			bl = bl->next;
		}
	}
	/* now print the lower level scope tables.*/
	flag++;
	st_print( st->lower); /* print tables of  nested scope */
	st_print( st->next);  /* print tables of sibling scopes */
	flag--;
}

/* st_initialize()
   [computation]:  
   Returns the pointer to an initialized empty symbol table. 
    If the parameter restart is TRUE, then the id of the symbol table is 0, otherwise,
   the id of the symbol table is accumulating (one plus the latest value ).
 */
SymbolTable*  st_initialize(Bool restart){
	int i;
	SymbolTable* tab;
	/* A counter of the tables. This number will increase each time a table is created. */
	static int tabId = 0; /* initially it is 0 */

	/* also need to reset it to 0, otherwise, the table id will accumulate, fixed an error 12/27/2015 */
	if(restart == TRUE)
		tabId = 0;

	if(A_debugAnalyzer)
			printf( "%20s \n", __FUNCTION__);

	/*  sizeof(SymbolTable),  not:  sizeof SymbolTable*/
	tab = (SymbolTable*) checked_malloc(sizeof(SymbolTable));
	tab->id = tabId++;
	tab->nd = NULL;
	tab->upper = NULL;
	tab->lower = NULL;
	tab->next = NULL;
	tab->prev = NULL;
	for(i = 0; i<ST_SIZE; i++)
		tab->hashTable[i] = NULL;
	return tab;
}

/*  st_attach()
    [computation]: 
    - Attach an initialized empty symbol table at the end of st->lower
    - Returns the pointer to the newly added empty symbol table.     
    [Precondition]: st is not NULL
 */
SymbolTable * st_attach(SymbolTable* st){
	SymbolTable* newSt = st_initialize(FALSE);
	SymbolTable* last = st->lower;

	//if(A_debugAnalyzer)
	//		printf( "%20s \n", __FUNCTION__);

	newSt->upper = st;
	if (last == NULL)
		st->lower = newSt;
	else{
		while(last->next != NULL) /* move lower to the last record in the list.*/
			last = last->next;
		last->next = newSt; /* attach newSt to the end of the list.*/
		newSt->prev = last;
	}
	return newSt;
}

static void LineList_free(LineList lis){
	if (lis==NULL)
		return;
	lis->nd->something = NULL; /*detach the line list record with the tree node */
	LineList_free(lis->next);
	free(lis);
}

static void BucketList_free(BucketList lis){
	if(lis==NULL)
		return;
	BucketList_free(lis->next);
	lis->nd->something = NULL; /*detach the line list record with the tree node */
	LineList_free(lis->lines);
	free(lis);
}

/* stj_free()
 * release the space occupied by a symbol table
 */
void st_free(SymbolTable * st){
	int j;
	if(st == NULL)
		return;
	for(j=0; j<ST_SIZE; j++){
		BucketList_free(st->hashTable[j]);
	}
	st_free(st->lower);
	st_free(st->next);
	free(st);
}

