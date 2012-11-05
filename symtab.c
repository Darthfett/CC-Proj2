/* symtab.c
 *
 * Implements the symbol table
 *
 * A flat symbol table is the root of all evil.
 */

#include <assert.h>
#include "symtab.h"
#include "shared.h"

#define INT 1

struct hash_table_t *name_table;
int new_name_val;
char **hash_names;
int hash_names_size;

struct hash_table_t* new_hash_table(int size)
{
    struct hash_table_t *table;
    int i;
    
    if (size <= 0) {
        return NULL;
    }
    
    table = (struct hash_table_t*) malloc(sizeof(struct hash_table_t));
    CHECK_MEM_ERROR(table);
    
    table->size = size;
    
    table->table = (struct ht_node_t **) malloc(sizeof(struct ht_node_t *) * size);
    CHECK_MEM_ERROR(table);
    
    for(i = 0; i < size; i++) {
        table->table[i] = NULL;
    }
    
    return table;
}

int hash(struct hash_table_t *hashtable, char *key)
{
    return makekey(key, hashtable->size);
}

struct ht_item_t* get_hashtable_item(struct hash_table_t *hashtable, char *key)
{
    struct ht_node_t *node;
    int hashed_key = hash(hashtable, key);
    
    for(node = hashtable->table[hashed_key]; node != NULL; node = node->next) {
        if (strcmp(key, node->key) == 0) {
            return node->value;
        }
    }
    
    return NULL;
}

struct ht_item_t* insert_item(struct hash_table_t *hashtable, char *key, struct ht_item_t *value)
{
    // Insert a value into the hashtable.  If a value already exists for the given key, the old value is returned, and the user must take responsibility for freeing that memory.
    struct ht_node_t *node;
    int hashed_key = hash(hashtable, key);
    struct ht_item_t *item = get_hashtable_item(hashtable, key);
    if (item != NULL) {
        // value already exists, overwrite value in table and return old value to user
        for(node = hashtable->table[hashed_key]; node != NULL; node = node->next) {
            if (strcmp(key, node->key) == 0) {
                node->value = value;
                return item;
            }
        }
    }

    // Implicit else

    node = (struct ht_node_t*) malloc(sizeof(struct ht_node_t));
    node->key = (char*) malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = value;

    node->next = hashtable->table[hashed_key];
    hashtable->table[hashed_key] = node;
    return NULL;
}

struct ht_item_t *remove_item(struct hash_table_t* table, char *key)
{
    /* Remove an item from a hashtable, and return the item to the user (user becomes responsible for memory of the item */
    int hashed_key = hash(table, key);
    struct ht_node_t *prev = NULL;
    struct ht_node_t *node = table->table[hashed_key];
    struct ht_item_t *value = NULL;

    while (node != NULL) {
        if (strcmp(key, node->key) == 0) {
            value = node->value;
            if (prev == NULL) {
                table->table[hashed_key] = node->next;
            } else {
                prev->next = node->next;
            }

            free(node->key);
            free(node);
            break;
        }
        prev = node;
        node = node->next;
    }
    return value;
}

void printTable(struct hash_table_t *table)
{

	int i;
	int size = table->size;
	// Not sure what numOfTabs argument is -- print every item instead.

	printf("{\n");

	for (i = 0; i < size; i++) {
		struct ht_node_t *node = table->table[i];
		while (node != NULL) {
			printf("\"%s\": ", node->key);
			// TODO - Update value to be actual value rather than type.
			printf("%d\n", node->value->value_type);
                        node = node->next;
		}
	}

	printf("}\n");
}

int get_name_hashval(char *name)
{
    struct ht_item_t *item = get_hashtable_item(name_table, name);
    int *new_val;
    if (item == NULL) {
        /* Add item to name_table */
        item = (struct ht_item_t*) malloc(sizeof(struct ht_item_t));
        new_val = (int*) malloc(sizeof(int));
        *new_val = new_name_val;

        item->value_type = INT;
        item->value = new_val;

        /* Index name in hash_names */
        if (new_name_val >= hash_names_size) {
            /* Table is full, double the size and copy the old stuff in */
            char **new_hash_names = (char**) malloc(sizeof(char*) * hash_names_size * 2);
            memcpy(new_hash_names, hash_names, sizeof(char*) * hash_names_size);
            free(hash_names);

            hash_names = new_hash_names;
            hash_names_size *= 2;
        }
        hash_names[new_name_val] = name;
        new_name_val++;
        
        insert_item(name_table, name, item);
        return new_name_val - 1;
    } else {
        return *((int*) item->value);
    }
}

char* get_hashval_name(int hashval)
{
    char *buffer;
    if (hashval < hash_names_size) {
        return hash_names[hashval];
    }

    // Buffer to hold invalid 16 chars + 10 for integer
    buffer = (char*) malloc(sizeof(char) * 26);
    snprintf(buffer, 26, "Invalid name <%d>", hashval);
    return buffer;
}

/* ------------------------------------------------------------
 * Initializes the symbol table
 * ------------------------------------------------------------
 */
void symtab_init(void)
{
        new_name_val = 0;
        hash_names_size = 50;
        name_table = new_hash_table(20);
        hash_names = (char**) malloc(sizeof(char*) * hash_names_size);

}
