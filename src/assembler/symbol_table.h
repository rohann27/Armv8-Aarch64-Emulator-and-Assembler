#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define SYMBOL_TABLE_INITIAL_CAPACITY 4
#define BUCKET_INITIAL_CAPACITY 4

#define SYMBOL_TABLE_LOAD_FACTOR 0.75
#define SYMBOL_TABLE_RESIZE_RATE 2

#define BUCKET_RESIZE_RATE 2

#define STRING_HASH_BASE 255

#define NOT_FOUND (UINT32_MAX)
#define HASH_STRING_NULL_OUTPUT 0

/**
 * @struct pair
 * @brief A simple pair struct
 * 
 * Holds strings as keys and uint32_ts as values
 */

struct pair {
    char *key;
    uint32_t value;
};
typedef struct pair *pair;


/**
 * @struct bucket
 * @brief A container holding multiple pairs to handle collisions
 * 
 * Contains an array of pairs, its capacity, and current length
 */

struct bucket {
    int capacity;
    int len;
    pair pairs;
};
typedef struct bucket *bucket;


/**
 * @struct symbol_table
 * @brief The main hash map structure
 * 
 * Holds an array of buckets, with its total capacity and number of entries
 */

struct symbol_table {
    int capacity;
    int len;
    bucket buckets;
};
typedef struct symbol_table *symbol_table;

/**
 * @brief Creates a new, empty symbol table
 * 
 * Allocates and initializes the symbol table with default capacity
 * 
 * @return symbol_table Pointer to the newly created symbol table
 */

extern symbol_table symbol_table_create(void);

/**
 * @brief Inserts or updates a key-value pair in the symbol table
 * 
 * Copies the key string and associates it with the given value
 * Resizes the table if necessary
 * 
 * @param table Pointer to the symbol table
 * @param key Null-terminated string key
 * @param value Unsigned 32-bit integer value
 */

extern void symbol_table_append(symbol_table, char *, uint32_t);

/**
 * @brief Checks whether a key exists in the symbol table
 * 
 * @param table Pointer to the symbol table
 * @param key Null-terminated string key
 * @return true If the key exists in the table
 * @return false Otherwise or if key is NULL
 */

extern bool symbol_table_find(symbol_table, char *);

/**
 * @brief Retrieves the value associated with a key
 * 
 * @param table Pointer to the symbol table
 * @param key Null-terminated string key
 * @return uint32_t The value associated with the key or NOT_FOUND if key not found
 */

extern uint32_t symbol_table_get(symbol_table, char *);

/**
 * @brief Frees all resources used by the symbol table
 * 
 * Frees all keys, pairs, buckets, and finally the symbol table itself
 * 
 * @param table Pointer to the symbol table to free
 */

extern void symbol_table_free(symbol_table);

#endif
