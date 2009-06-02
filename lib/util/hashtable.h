#include "data.h"
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

/**
 * @addtogroup HashTable
 * @{
 */

/**
 * Hash table object definition.
 */
typedef struct _htable htable;

/**
 * Internal hash table node.
 */
struct _htable_node {
  /**> next node */
  struct _htable_node *next;
  /**> calculated hash for the node */
  unsigned long int hash; 
  /**> key */
  char *key; 
  /**> value */
  data *data; 
};

/**
 * Hash table internal structure.
 */
struct _htable {
  /**> maximum number of elements */
  int size; 
  /**> number of allocated elements */
  int num_elem; 
  /**> high density factor, default is 70% full */
  int high_fill_factor; 
  /**> low density factor, default is 30% full */
  int low_fill_factor; 
  /**> the table containing the nodes */
  struct _htable_node **table; 
};

htable * create_htable (int size);
void destroy_htable (htable *ht);

int htable_insert (htable *ht, char *key, data *v);
void htable_remove (htable *ht, char *key);
void htable_remove_entry (htable *ht, char *key);

struct _htable_node *_htable_lookup (htable *ht, char *key);
data *htable_lookup (htable *ht, char *key);
int htable_update (htable *ht, char *key, data *d);

void htable_print (htable *ht);

int htable_num_elements (htable *ht);

void htable_set_resize_high_density (htable *ht, int fill_factor);
void htable_set_resize_low_density (htable *ht, int fill_factor);

void _htable_shrink (htable *ht);
void _htable_expand (htable *ht);
void _htable_resize_size (htable *ht, int size);

/** @} */

#endif
