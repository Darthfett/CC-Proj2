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


int hash(struct hash_table_t *hashtable, char *key);
struct hash_table_t* new_hash_table(int size);
struct ht_item_t* get_hashtable_item(struct hash_table_t *hashtable, char *key);
struct ht_item_t* insert_item(struct hash_table_t *hashtable, char *key, struct ht_item_t *value);
struct ht_item_t* remove_item(struct hash_table_t *hashtable, char *key);

void printTable(struct hash_table_t *table);

int get_name_hashval(char *name);
void symtab_init();

#endif
