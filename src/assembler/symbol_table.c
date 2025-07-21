#include "symbol_table.h"


static uint64_t hash_string(char *str) {
    if (str == NULL) return HASH_STRING_NULL_OUTPUT;

    uint64_t result = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        result += (result * STRING_HASH_BASE + str[i]);
    }
    return result;  
}

static struct bucket bucket_create(void) {
    struct bucket new;
    new.capacity = BUCKET_INITIAL_CAPACITY;
    new.len = 0;
    new.pairs = malloc(sizeof(struct pair) * new.capacity);

    return new;
}

symbol_table symbol_table_create(void) {
    symbol_table new = malloc(sizeof(struct symbol_table));
    new->capacity = SYMBOL_TABLE_INITIAL_CAPACITY;
    new->len = 0; // 0 elements initially
    new->buckets = malloc(sizeof(struct bucket) * new->capacity);
    assert(new->buckets != NULL);

    for (int i = 0; i < new->capacity; i++) {
        new->buckets[i] = bucket_create();
    }

    return new;
}

static void bucket_resize(bucket buck) {
    if (buck == NULL) return;

    // Return if we don't need to resize
    if (buck->len < buck->capacity) return;

    buck->capacity *= BUCKET_RESIZE_RATE;
    buck->pairs = realloc(buck->pairs, sizeof(struct pair) * buck->capacity);

    return;
}

static void bucket_append(bucket buck, pair pr) {
    if (pr == NULL) return;

    bucket_resize(buck);

    for (int i = 0; i < buck->len; i++) {
        if (strcmp(buck->pairs[i].key, pr->key) == 0) {
            free(pr->key);
            buck->pairs[i].value = pr->value;
            return;
        }
    }

    buck->pairs[buck->len].key = pr->key;
    buck->pairs[buck->len].value = pr->value;
    buck->len++;

    return;
}

static bool bucket_find(bucket buck, char *key) {
    if (key == NULL) return false;

    for (int i = 0; i < buck->len; i++) {
        if (strcmp(buck->pairs[i].key, key) == 0) {
            return true;
        }
    }
    return false;
}

static uint32_t bucket_get(bucket buck, char *key) {
    if (key == NULL) return NOT_FOUND;

    for (int i = 0; i < buck->len; i++) {
        if (strcmp(buck->pairs[i].key, key) == 0) {
            return buck->pairs[i].value;
        }
    }
    return NOT_FOUND;
}

static void bucket_free(bucket buck) {
    if (buck == NULL) return;

    for(int i = 0; i < buck->len; i++) {
        free(buck->pairs[i].key);
    }
    free(buck->pairs);
    return;
}

static void symbol_table_resize(symbol_table table) {
    if (table == NULL) return;

    // Return if we don't need to resize
    if (table->len < table->capacity * SYMBOL_TABLE_LOAD_FACTOR) return;

    int newCapacity = table->capacity * SYMBOL_TABLE_RESIZE_RATE;

    bucket newBuckets = malloc(sizeof(struct bucket) * newCapacity);

    for (int i = 0; i < newCapacity; i++) {
        newBuckets[i] = bucket_create();
    }

    for (int i = 0; i < table->capacity; i++) {
        struct bucket curBucket = table->buckets[i];

        for (int j = 0; j < curBucket.len; j++) {
            int strHash = hash_string(curBucket.pairs[j].key);
            int bucketIndex = strHash % newCapacity;

            struct pair newPair = {
                .key = strdup(curBucket.pairs[j].key),
                .value = curBucket.pairs[j].value
            };

            bucket_append(&newBuckets[bucketIndex], &newPair);
        }
    }

    // Free the old buckets
    for (int i = 0; i < table->capacity; i++) {
        bucket_free(&table->buckets[i]);
    }
    free(table->buckets);


    table->capacity = newCapacity;
    table->buckets = newBuckets;

    return;
}

void symbol_table_append(symbol_table table, char *key, uint32_t value) {
    if (key == NULL) return;

    uint64_t strHash = hash_string(key);

    table->len++;
    uint64_t bucketIndex = strHash % table->capacity;

    struct pair newPair = {
        .key = strdup(key),
        .value = value
    };
    assert(newPair.key != NULL);

    bucket_append(&table->buckets[bucketIndex], &newPair);

    symbol_table_resize(table);
    return;
}

bool symbol_table_find(symbol_table table, char *key) {
    if (key == NULL) return false;

    uint64_t strHash = hash_string(key);
    uint64_t bucketIndex = strHash % table->capacity;

    if (bucket_find(&table->buckets[bucketIndex], key)) {
        return true;
    }

    return false;
}

uint32_t symbol_table_get(symbol_table table, char *key) {
    if (key == NULL) return NOT_FOUND;

    uint64_t strHash = hash_string(key);
    uint64_t bucketIndex = strHash % table->capacity;
    
    if (bucket_find(&table->buckets[bucketIndex], key)) {
            return bucket_get(&table->buckets[bucketIndex], key);
    }

    return NOT_FOUND;
}

void symbol_table_free(symbol_table table) {
    if (table == NULL) return;

    for (int i = 0; i < table->capacity; i++) {
        bucket_free(&table->buckets[i]);
    }
    free(table->buckets);
    free(table);

    return;
}
