/* symtab.h
 *
 * Holds function definitions for the symbol table. The symbol table
 * is implemented as a global hash table that contains local symbol
 * tables for each function
 */

#ifndef SYMTAB_H
#define SYMTAB_H

#include "shared.h"
#include "usrdef.h"
#include <stdlib.h>
#include <string.h>



/* ----------------------------------------------------------------
 * Function declarations
 * ----------------------------------------------------------------
 */
//
// This is used as a return value to the insert method to tell the caller
// if there was an error or a warning when inserting a new id into our
// hash table.
#define	 HT_SUCCESS 0x03;  // successfully added new id and type.
#define	 HT_WARNING 0x24; // This id and type were added to the table but were also
                              // defined earlier by higher scope.
#define	 HT_ERROR  0x0;  // Could not add this id into the tables.

enum ht_types {
	scope_table, array,variable
};

struct ht_item_t {
    void *value; // data that is used for this scope or id.
    int value_type; // this is the type that the void * value represents
};

struct ht_node_t {
    char *key; // This is either the scope or the id of the element found in a scope table.
    struct ht_item_t *value;

    struct ht_node_t *next;
};

struct hash_table_t {
    int size;
    struct ht_node_t **table;
};

// This is a data structure used to define a scope table entry.
struct ht_scope_item_t {
	char* return_type; // This is only used for methods and is ignored by most scopes. ( null )
	// this holds the table for all elements that are defined within
	// this scope.
	struct hash_table_t *table;
	// This holds a pointer to the parent of this scope or null if this is the top
	// scope of the program.
	struct ht_scope_item_t *parent;
};

struct ht_array_item_t {
	char* type; // this is the type of this array;
	int min; // this is the minimum index for this array
	int max; // this is the maximum index for this array.
};
struct scope_path{
	char* scope;
	struct scope_path * parent;
};

struct ht_scope_item_t* moveDownToNewScope(char* scope);
struct scope_path *current_scope;
struct hash_table_t *global_table;
void removeCurrentScope();
char* checkMethodType(char* id, char* scope, char* method );
char* checkType(char* id1,char *scope1,char*scope2, char* id2);
void symtab_init();
void symtab_print(int numOfTabs);
struct hash_table_t* new_hash_table(int size);
int hash(struct hash_table_t *hashtable, char *key);
struct ht_item_t* get_hashtable_item(struct hash_table_t *hashtable, char *key);
struct ht_item_t* insert_item(struct hash_table_t *hashtable, char *key, struct ht_item_t *value);
struct ht_item_t* findElement(char* id, char* scope);
struct ht_item_t* remove_item(struct hash_table_t *hashtable, char *key);
int add_element(struct ht_item_t *value, char* id, char* scope);
struct ht_item_t * get_or_create_scope(char* scope, char* parent);
#endif
