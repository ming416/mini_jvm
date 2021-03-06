/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software 
for any purpose with or without fee is hereby granted, provided 
that the above copyright notice and this permission notice appear 
in all copies. 

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN      
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

 */

/* Hash table implementation */

#include <stdlib.h>
#include <stdio.h>
#include "hashtable.h"
#include "math.h"
#include "../jvm/garbage.h"


struct _HashtableEntry {
    HashtableKey key;
    HashtableValue value;
    HashtableEntry *next;
};

struct _Hashtable {
    HashtableEntry **table;
    unsigned long long int table_size;
    HashtableHashFunc hash_func;
    HashtableEqualFunc equal_func;
    HashtableKeyFreeFunc key_free_func;
    HashtableValueFreeFunc value_free_func;
    unsigned long long int entries;
    int prime_index;
};

/* This is a set of good hash table prime numbers, from:
 *   http://planetmath.org/encyclopedia/GoodHashtablePrimes.html
 * Each prime is roughly double the previous value, and as far as
 * possible from the nearest powers of two.
 *
 * Note: Primes after 1610612741 are added by finding the nearest prime to
 *       previous prime multiplied by 2.
 */

static const unsigned long long int hash_table_primes[] = {
        193UL, 389UL, 769UL, 1543UL, 3079UL, 6151UL, 12289UL, 24593UL, 49157UL, 98317UL,
        196613UL, 393241UL, 786433UL, 1572869UL, 3145739UL, 6291469UL,
        12582917UL, 25165843UL, 50331653UL, 100663319UL, 201326611UL,
        402653189UL, 805306457UL, 1610612741UL, 3221225479UL, 6442450967UL,
        12884901947UL, 25769803897UL, 51539607793UL, 103079215583UL, 206158431161UL,
};

static const int hash_table_num_primes
        = sizeof(hash_table_primes) / sizeof(unsigned long long int);

unsigned long long int getHashtableSizeWithIncrement(Hashtable *hash_table, int increment) {
    unsigned long long int size = hash_table_primes[0];

    // Target size is within range:
    if (hash_table->prime_index + increment < hash_table_num_primes) {
        size = hash_table_primes[hash_table->prime_index + increment];
    }        // Target size not in range:
    else {
        // Current in range, target not in range:
        if (hash_table->prime_index < hash_table_num_primes) {
            int diff = hash_table->prime_index + increment - (hash_table_num_primes - 1);

            size = hash_table_primes[hash_table_num_primes - 1];
            size = size * pow(2, diff);
        }            // Current is outside, target is outside; current < target:
        else if (hash_table->prime_index < hash_table->prime_index + increment) {
            size = size * pow(2, increment);
        }            // Current is outside, target is outside; current > target:
        else if (hash_table->prime_index > hash_table->prime_index + increment) {
            size = size / pow(2, increment);
        }
    }

    return size;
}

/* Internal function used to allocate the table on hash table creation
 * and when enlarging the table */

static int hash_table_allocate_table(Hashtable *hash_table, int increment) {
    unsigned long long int new_table_size = getHashtableSizeWithIncrement(hash_table, increment);

    // Update:
    hash_table->table_size = new_table_size;
    hash_table->prime_index = hash_table->prime_index + increment;

    /* Allocate the table and initialise to NULL for all entries */

    hash_table->table = jvm_alloc(hash_table->table_size *
                                  sizeof(HashtableEntry *));

    return hash_table->table != NULL;
}

/* Free an entry, calling the free functions if there are any registered */

static void hash_table_free_entry(Hashtable *hash_table, HashtableEntry *entry) {
    /* If there is a function registered for freeing keys, use it to free
     * the key */

    if (hash_table->key_free_func != NULL) {
        hash_table->key_free_func(entry->key);
    }

    /* Likewise with the value */

    if (hash_table->value_free_func != NULL) {
        hash_table->value_free_func(entry->value);
    }

    /* Free the data structure */

    jvm_free(entry);
}

unsigned long _DEFAULT_HashtableHash(HashtableKey kmer) {
    return (unsigned long) kmer;
}

int _DEFAULT_HashtableEquals(HashtableValue value1, HashtableValue value2) {
    return (value1 == value2);
}

Hashtable *hashtable_create(HashtableHashFunc hash_func,
                            HashtableEqualFunc equal_func) {
    Hashtable *hash_table;

    /* Allocate a new hash table structure */

    hash_table = (Hashtable *) jvm_alloc(sizeof(Hashtable));

    if (hash_table == NULL) {
        return NULL;
    }

    hash_table->hash_func = hash_func;
    hash_table->equal_func = equal_func;
    hash_table->key_free_func = NULL;
    hash_table->value_free_func = NULL;
    hash_table->entries = 0;
    hash_table->prime_index = 0;

    /* Allocate the table */

    if (!hash_table_allocate_table(hash_table, 0)) {
        jvm_free(hash_table);

        return NULL;
    }

    return hash_table;
}

void hashtable_destory(Hashtable *hash_table) {
    HashtableEntry *rover;
    HashtableEntry *next;
    unsigned long long int i;

    /* Free all entries in all chains */

    for (i = 0; i < hash_table->table_size; ++i) {
        rover = hash_table->table[i];
        while (rover != NULL) {
            next = rover->next;
            hash_table_free_entry(hash_table, rover);
            rover = next;
        }
    }

    /* Free the table */

    jvm_free(hash_table->table);

    /* Free the hash table structure */

    jvm_free(hash_table);
}

void hashtable_register_free_functions(Hashtable *hash_table,
                                       HashtableKeyFreeFunc key_free_func,
                                       HashtableValueFreeFunc value_free_func) {
    hash_table->key_free_func = key_free_func;
    hash_table->value_free_func = value_free_func;
}

int hashtable_put(Hashtable *hash_table, HashtableKey key, HashtableValue value) {
    HashtableEntry *rover;
    HashtableEntry *newentry;
    unsigned long long int index;

    /* If there are too many items in the table with respect to the table
     * size, the number of hash collisions increases and performance
     * decreases. Enlarge the table size to prevent this happening */

    if ((hash_table->entries * 2) / hash_table->table_size > 0) {

        /* Table is more than 1/2 full */

        if (!hashtable_resize(hash_table)) {

            /* Failed to enlarge the table */

            return 0;
        }
    }

    /* Generate the hash of the key and hence the index into the table */

    index = hash_table->hash_func(key) % hash_table->table_size;

    /* Traverse the chain at this location and look for an existing
     * entry with the same key */

    rover = hash_table->table[index];

    while (rover != NULL) {
        if (hash_table->equal_func(rover->key, key) != 0) {

            /* Same key: overwrite this entry with new data */

            /* If there is a value free function, free the old data
             * before adding in the new data */

            if (hash_table->value_free_func != NULL) {
                hash_table->value_free_func(rover->value);
            }

            /* Same with the key: use the new key value and free 
             * the old one */

            if (hash_table->key_free_func != NULL) {
                hash_table->key_free_func(rover->key);
            }

            rover->key = key;
            rover->value = value;

            /* Finished */

            return 1;
        }
        rover = rover->next;
    }

    /* Not in the hash table yet.  Create a new entry */

    newentry = (HashtableEntry *) jvm_alloc(sizeof(HashtableEntry));

    if (newentry == NULL) {
        return 0;
    }

    newentry->key = key;
    newentry->value = value;

    /* Link into the list */

    newentry->next = hash_table->table[index];
    hash_table->table[index] = newentry;

    /* Maintain the count of the number of entries */

    ++hash_table->entries;

    /* Added successfully */

    return 1;
}

HashtableValue hashtable_get(Hashtable *hash_table, HashtableKey key) {
    HashtableEntry *rover;
    unsigned long long int index;

    /* Generate the hash of the key and hence the index into the table */

    index = hash_table->hash_func(key) % hash_table->table_size;

    /* Walk the chain at this index until the corresponding entry is
     * found */

    rover = hash_table->table[index];

    while (rover != NULL) {
        if (hash_table->equal_func(key, rover->key) != 0) {

            /* Found the entry.  Return the data. */

            return rover->value;
        }
        rover = rover->next;
    }

    /* Not found */

    return HASH_TABLE_NULL;
}

int hashtable_remove(Hashtable *hash_table, HashtableKey key, int resize) {
    HashtableEntry **rover;
    HashtableEntry *entry;
    unsigned long long int index;
    unsigned long long int result;

    /* If there are too few items in the table with respect to the table
     * size, the table is taking up too much space. 
     * Shrink table to improve space efficiency. */

    if (resize && (hash_table->entries * 8) / hash_table->table_size <= 0) {

        /* Table is less than 1/8 full */

        if (!hashtable_resize(hash_table)) {

            /* Failed to enlarge the table */

            return 0;
        }
    }

    /* Generate the hash of the key and hence the index into the table */

    index = hash_table->hash_func(key) % hash_table->table_size;

    /* Rover points at the pointer which points at the current entry
     * in the chain being inspected.  ie. the entry in the table, or
     * the "next" pointer of the previous entry in the chain.  This
     * allows us to unlink the entry when we find it. */

    result = 0;
    rover = &hash_table->table[index];

    while (*rover != NULL) {

        if (hash_table->equal_func(key, (*rover)->key) != 0) {

            /* This is the entry to remove */

            entry = *rover;

            /* Unlink from the list */

            *rover = entry->next;

            /* Destroy the entry structure */

            hash_table_free_entry(hash_table, entry);

            /* Track count of entries */

            --hash_table->entries;

            result = 1;

            break;
        }

        /* Advance to the next entry */

        rover = &((*rover)->next);
    }

    return result;
}

unsigned long long int hashtable_num_entries(Hashtable *hash_table) {
    return hash_table->entries;
}

void hashtable_iterate(Hashtable *hash_table, HashtableIterator *iterator) {
    unsigned long long int chain;

    iterator->hash_table = hash_table;

    /* Default value of next if no entries are found. */

    iterator->next_entry = NULL;

    /* Find the first entry */

    for (chain = 0; chain < hash_table->table_size; ++chain) {

        if (hash_table->table[chain] != NULL) {
            iterator->next_entry = hash_table->table[chain];
            iterator->next_chain = chain;
            break;
        }
    }
}

int hashtable_iter_has_more(HashtableIterator *iterator) {
    return iterator->next_entry != NULL;
}

HashtableValue hashtable_iter_next(HashtableIterator *iterator) {
    HashtableEntry *current_entry;
    Hashtable *hash_table;
    HashtableValue result;
    unsigned long long int chain;

    hash_table = iterator->hash_table;

    /* No more entries? */

    if (iterator->next_entry == NULL) {
        return HASH_TABLE_NULL;
    }

    /* Result is immediately available */

    current_entry = iterator->next_entry;
    result = current_entry->value;

    /* Find the next entry */

    if (current_entry->next != NULL) {

        /* Next entry in current chain */

        iterator->next_entry = current_entry->next;

    } else {

        /* None left in this chain, so advance to the next chain */

        chain = iterator->next_chain + 1;

        /* Default value if no next chain found */

        iterator->next_entry = NULL;

        while (chain < hash_table->table_size) {

            /* Is there anything in this chain? */

            if (hash_table->table[chain] != NULL) {
                iterator->next_entry = hash_table->table[chain];
                break;
            }

            /* Try the next chain */

            ++chain;
        }

        iterator->next_chain = chain;
    }

    return result;
}

HashtableKey hashtable_iter_next_key(HashtableIterator *iterator) {
    HashtableEntry *current_entry;
    Hashtable *hash_table;
    HashtableKey result;
    unsigned long long int chain;

    hash_table = iterator->hash_table;

    /* No more entries? */

    if (iterator->next_entry == NULL) {
        return HASH_TABLE_NULL;
    }

    /* Result is immediately available */

    current_entry = iterator->next_entry;
    result = current_entry->key;

    /* Find the next entry */

    if (current_entry->next != NULL) {

        /* Next entry in current chain */

        iterator->next_entry = current_entry->next;

    } else {

        /* None left in this chain, so advance to the next chain */

        chain = iterator->next_chain + 1;

        /* Default value if no next chain found */

        iterator->next_entry = NULL;

        while (chain < hash_table->table_size) {

            /* Is there anything in this chain? */

            if (hash_table->table[chain] != NULL) {
                iterator->next_entry = hash_table->table[chain];
                break;
            }

            /* Try the next chain */

            ++chain;
        }

        iterator->next_chain = chain;
    }

    return result;
}

int hashtable_resize(Hashtable *hash_table) {
    HashtableEntry **old_table;
    unsigned long long int old_table_size;
    int old_prime_index;
    HashtableEntry *rover;
    HashtableEntry *next;
    unsigned long long int index;
    unsigned long long int i;

    int increment = 0;

    // GROW
    if ((hash_table->entries * 2) / hash_table->table_size > 0) {
        do {
            /* Table is more than 1/2 full */
            increment++;

        } while ((hash_table->entries * 2) / getHashtableSizeWithIncrement(hash_table, increment) > 0);
    }
        // SHRINK
    else if ((hash_table->entries * 8) / hash_table->table_size == 0) {
        do {
            // Are we already as small as possible?
            if (getHashtableSizeWithIncrement(hash_table, increment) <= hash_table_primes[0]) {
                break;
            }

            /* Table is less than 1/8 full */
            increment--;
        } while ((hash_table->entries * 8) / getHashtableSizeWithIncrement(hash_table, increment) <= 0);
    }

    if (increment != 0) {
        /* Store a copy of the old table */
        old_table = hash_table->table;
        old_table_size = hash_table->table_size;
        old_prime_index = hash_table->prime_index;

        if (!hash_table_allocate_table(hash_table, increment)) {

            /* Failed to allocate the new table */

            printf("CRITICAL: FAILED TO ALLOCATE HASH TABLE!\n");

            hash_table->table = old_table;
            hash_table->table_size = old_table_size;
            hash_table->prime_index = old_prime_index;

            return 0;
        }

        /* Link all entries from all chains into the new table */

        for (i = 0; i < old_table_size; ++i) {
            rover = old_table[i];

            while (rover != NULL) {
                next = rover->next;

                /* Find the index into the new table */

                index = hash_table->hash_func(rover->key) % hash_table->table_size;

                /* Link this entry into the chain */

                rover->next = hash_table->table[index];
                hash_table->table[index] = rover;

                /* Advance to next in the chain */

                rover = next;
            }
        }

        /* Free the old table */

        jvm_free(old_table);
    }

    return 1;
}

