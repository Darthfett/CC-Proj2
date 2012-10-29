/* symtab.c
 *
 * Implements the symbol table
 *
 * A flat symbol table is the root of all evil.
 */

#include <assert.h>
#include "symtab.h"
#include "shared.h"


#define SYMBOL_VARIABLE 1
#define SYMBOL_FUNCTION 2

void printTable(struct hash_table_t *table);


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

/* ------------------------------------------------------------
 * Initializes the symbol table
 * ------------------------------------------------------------
 */
void symtab_init()
{
	// TODO we need to define a gloabal hash table to use for holding all the tag IDs and types.
	global_table = new_hash_table(10);

}


/* ------------------------------------------------------------
 * Prints the contents of the symbol table
 * ------------------------------------------------------------
 */
void symtab_print(int numOfTabs)
{
    int i;
    // Not sure what numOfTabs argument is -- print every item instead.
    if ( global_table->table == NULL ) {
    	printf("global_table is null\n");
    }
    printf("{\n");

    for (i = 0; i < global_table->size; i++) {
        struct ht_node_t *node = global_table->table[i];
        while (node != NULL) {
            printf("\"%s\": ", node->key);
            // TODO - Update value to be actual value rather than type.
            printf("%d\n", node->value->value_type);
        }
        node = node->next;
    }

    printf("}\n");

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
		}
		node = node->next;
	}

	printf("}\n");
}
struct ht_item_t * findElement(char* id, char* scope)
{
	struct ht_item_t *rval = NULL;
	struct ht_scope_item_t *scope_item;
	struct ht_item_t * T;
	do
	{
		if ( id == NULL ) {
			// we cannot find a null id
			break;
		}
		if ( scope == NULL ) {
			// we will assume that we should look in the current scope
			scope = current_scope->scope;
		}
		T = get_hashtable_item(global_table,scope);

		// check that we got a valid value back
		if ( T == NULL ) {
			// Could not find hash table for the given scope
			break;
		} else if ( T->value_type == scope_table ) {
			scope_item = T->value;
		} else {
			// We are in trouble because we have found a table entry in the
			// scope layer that is not a scope.
			assert(!"Hash Table structure error: found a none scope_table element in top level\n");
		}
		// TODO get the next hash table for the scope that was returned.
		struct hash_table_t *t_scope = scope_item->table;
		rval = get_hashtable_item(t_scope,id);

	} while (0);

	return rval;
}
struct ht_item_t * get_or_create_scope(char* scope, char* parent)
{
	struct ht_item_t * r_scope = NULL;
	struct ht_item_t * p_scope = NULL;
	struct ht_scope_item_t *scope_item;
	do {
		if ( scope == NULL ) {
			// we cannot create nor open a scope that has no value.
			assert(!"Cannot open or create a NULL scope\n");
			break;
		}
		struct ht_item_t * T = get_hashtable_item(global_table,scope);

		// check that we got a valid value back
		if ( T == NULL ) {

			// we do not have a table defined for this scope yet so we want to
			// create that table now.
			scope_item = (struct ht_scope_item_t *)malloc(sizeof(struct ht_scope_item_t));
			CHECK_MEM_ERROR(scope_item);

			scope_item->parent = NULL;
			// find the parent scope if provided
			if ( parent != NULL ) {
				p_scope = get_hashtable_item(global_table,parent);
				if ( p_scope != NULL && p_scope->value_type == scope_table ) {
					scope_item->parent = p_scope->value;
				} // else the default value is already null
			} // else the default value is already null
			scope_item->return_type = NULL;
			scope_item->table = new_hash_table(15); // we might want to make this number bigger

			// create the hash table value for this scope
			r_scope = (struct ht_item_t *)malloc(sizeof(struct ht_item_t));
			CHECK_MEM_ERROR(r_scope);

			r_scope->value = scope_item;
			r_scope->value_type = scope_table;
			insert_item(global_table,scope,r_scope);
		} else {
			// we found a valid scope element so we are good
			r_scope = T;
		}
	} while (0);
	return r_scope;
}
struct ht_item_t * findInParentScope(struct ht_scope_item_t *scope_item, char *id)
{
	// The default return value is NULL
	struct ht_item_t * rval = NULL;
	while ( scope_item != NULL ) {
		struct ht_item_t * element = get_hashtable_item(scope_item->table,id);
		if ( element != NULL ) {
			// we have found this tag in an ancestor scope so we should return
			// this item
			rval = element;
			break;
		} // else we move to the next parent scope untill we hit the Global scope
		scope_item = scope_item->parent;
	}
	return rval;
}

int add_element(struct ht_item_t *value, char* id, char* scope)
{
	// The default return value is ERROR
	int rval = HT_ERROR;
	struct ht_scope_item_t *scope_item;
	do {
		if ( id == NULL || value == NULL ) {
			// we cannot add a new node into the hash table if it is null
			break;
		}
		if ( scope == NULL ) {
			// we will assume that we should look in the current scope
			scope = current_scope->scope;
		}
		struct ht_item_t * T = get_hashtable_item(global_table,scope);

		// check that we got a valid value back
		if ( T == NULL ) {
			// We do not have this scope defined yet so that is an error
			assert(!"Error Scope not defined yet");
			break;
		}
		// make sure that we have found a scope element and not something else
		if ( T->value_type != scope_table ) {
			// we are in trouble!!! our hash table is not set up correctly
			assert(!"Hash Table is not structured correctly");
			break;
		}
		scope_item = T->value;

		// check to see if this item is already delcared within this scope
		struct ht_item_t * element = get_hashtable_item(scope_item->table,scope);
		if ( element == NULL ) {
			// yeah we can add this element into the hash table and all is well
			insert_item(scope_item->table,id,value);
			rval = HT_SUCCESS;

			// Make sure that this element is not defined in a parent scope
			if ( findInParentScope(scope_item->parent,id) != NULL ) {
				// This variable is already declared in a larger scope
				// this is either an error or a warning.
				rval = HT_WARNING;
			}

			break;
		} // else
		// This element is already defined by this scope so we do not add it and we
		// return the default value of error.

	} while (0);
	return rval;
}
// This method is called to change the global scope variable so that we
// are pointing to the correct scopes.
struct ht_scope_item_t* moveDownToNewScope(char* scope)
{
	char* current = NULL;
	if ( current_scope != NULL ) {
		current = current_scope->scope;
	}
	// The default return value is NULL if this scope has already been
	// defined.
	struct ht_scope_item_t* rval = NULL;
	if ( get_hashtable_item(global_table,scope) == NULL ) {
		struct ht_item_t * t = get_or_create_scope(scope,current);
		if ( t != NULL && t->value_type == scope_table ) {
			if ( current_scope == NULL ) {
				current_scope = (struct scope_path * )malloc(sizeof(struct scope_path));
				current_scope->parent=NULL;
				current_scope->scope = (char*) malloc((strlen(scope) + 1));
			}
			strcpy(current_scope->scope, scope);
			rval = t->value;

		}
	}
	return rval;
}
char* getType(struct ht_item_t* element)
{
	char *type = NULL;
	switch ( element->value_type ) {
	case array:
	{
		struct ht_array_item_t * a = element->value;
		type = a->type;
	}
	break;
	case variable:
	{
		type = element->value;
	}
	break;
	case scope_table:
	{
		// this is the return type if we are a method
		struct ht_scope_item_t *s = element->value;
		type = s->return_type;
	}
	break;
	default:
		break;
	}
	return type;
}

// This method checks if the two IDs have the same type and returns that
// type or NULL if they are not the same.
char* checkType(char* id1,char *scope1,char*scope2, char* id2)
{
	// default return value is NULL
	char * rval = NULL;
	char * type1 = NULL;
	char * type2 = NULL;
	struct ht_item_t * t1;
	struct ht_item_t * t2;
	struct ht_scope_item_t* s1;
	struct ht_scope_item_t* s2;

	if ( scope1 == NULL ) {
		scope1 = current_scope->scope;
	}
	if ( scope2 == NULL ) {
		scope2 = current_scope->scope;
	}

	do {
		t1 = get_hashtable_item(global_table,scope1);
		t2 = get_hashtable_item(global_table,scope2);
		if ( t1 == NULL || t2 == NULL ) {
			printf("checkType(): unknown scope used: scope1 = %s, scope2 = %s ",scope1,scope2);
			break;
		}
		if ( t1->value_type != scope_table || t2->value_type != scope_table ) {
			// Structure problem we should not have this happen.
			assert(!"checkType(): Hash Table is not structured correctly");
			break;
		}
		s1 = t1->value;
		s2 = t1->value;

		if ( s1 == NULL || s2 == NULL ) {
			// Structure problem we should not have this happen.
			assert(!"checkType(): scope table was lost");
			break;
		}
		// search in all the parent scopes as well to find the elements for these ids
		t1 = findInParentScope(s1,id1);
		t2 = findInParentScope(s2,id2);

		if ( t1 == NULL || t2 == NULL ) {
			// we do not have this identifier yet so we cannot say they are the same
			break;
		}
		type1 = getType(t1);
		type2 = getType(t2);
		if ( strcmp(type1,type2) == 0 ) {
			// These are the same type Yeah!!!
			rval = type1;
		}


	} while (0);
	return rval;
}
char* checkMethodType(char* id, char* scope, char* method )
{
	// default return value is NULL
	char * rval = NULL;
	char * type1 = NULL;
	char * type2 = NULL;
	struct ht_item_t * t1;
	struct ht_item_t * t2;
	struct ht_scope_item_t* s1;
	struct ht_scope_item_t* s2;

	if ( scope == NULL ) {
		// assume that the user is using the current scope
		scope = current_scope->scope;
	}
	do {
		t1 = get_hashtable_item(global_table,scope);
		t2 = get_hashtable_item(global_table,method);
		if ( t1 == NULL || t2 == NULL ) {
			printf("checkMethodType(): unknown scope or method used: scope1 = %s, method = %s ",scope,scope);
			break;
		}
		if ( t1->value_type != scope_table || t2->value_type != scope_table ) {
			// Structure problem we should not have this happen.
			assert(!"checkMethodType(): Hash Table is not structured correctly");
			break;
		}
		s1 = t1->value;
		s2 = t1->value;

		if ( s1 == NULL || s2 == NULL ) {
			// Structure problem we should not have this happen.
			assert(!"checkMethodType(): scope table was lost");
			break;
		}
		// search in all the parent scopes as well to find the elements for these ids
		t1 = findInParentScope(s1,id);

		if ( t1 == NULL ) {
			// we do not have this identifier yet so we cannot say they are the same
			break;
		}
		type1 = getType(t1);
		type2 = getType(t2);
		if ( strcmp(type1,type2) == 0 ) {
			// These are the same type Yeah!!!
			rval = type1;
		}


	} while (0);
	return rval;
}

void removeCurrentScope()
{
	struct scope_path* temp;
	// find the parent scope of the current scope
	if ( current_scope->parent != NULL ) {
		temp = current_scope;
		current_scope = current_scope->parent;
		free(temp);
	}
}
