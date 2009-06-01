#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "data.h"
#include "hash.h"

#ifdef DEBUG
#define DEBUGP(fmt,...) \
	printf ("%s:%d ", __FILE__, __LINE__); \
	printf (fmt, ##__VA_ARGS__);
#else
#define DEBUGP(fmt,...)
#endif

/**
 * @defgroup HashTable Hash table implementation
 * @ingroup Util
 *
 * @{
	 */

/**
 * Initial hash value
 */
#define HASH_INIT_VAL 103424

/**
 * Create a new hash table object.
 *
 * @param size the size of the initial hash table.
 * @return a newlly-allocated hash table object.
 */
htable * create_htable (int size) {

	htable *ht;
	
	if (size < 1) 
		return NULL;

	if ( (ht = malloc (sizeof (htable))) == NULL )
		return NULL;

	if ( (ht->table = calloc (size, sizeof (struct _htable_node))) == NULL )
		return NULL;

	ht -> size = size;
	ht -> num_elem = 0;
	ht -> high_fill_factor = 70;
	ht -> low_fill_factor = 30;

	return ht;

}

/**
 * Destroy the hash table and its content.
 *
 * @param ht a hash table object.
 */
void destroy_htable (htable *ht) {

	int i;
	struct _htable_node *hnode, *temp;

	if (ht == NULL) 
		return;

	for (i=0; i<ht->size; i++) {
		hnode = ht -> table [i]; 
		while (hnode != NULL) {
			temp = hnode;
			hnode = hnode -> next;
			free (temp->key);
			destroy_data (temp->data);
			free (temp);
		}
	}

	free (ht -> table);
	free (ht);

}

/**
 * Insert a pair key -> value to the hash table. 
 *
 * @param ht a hash table object.
 * @param key the key.
 * @param v a data object as value.
 */
int htable_insert (htable *ht, char *key, data *v) {

	struct _htable_node *new_hnode;
	struct _htable_node *cur_hnode;
	unsigned long int hashval = hash ((unsigned char *)key, sizeof (unsigned long int), HASH_INIT_VAL);
	unsigned long int hashpos = hashval % ht->size;

	DEBUGP ("+++ insert key[%s] as [%lu] [%lu]\n", key, hashpos, hashval );

	new_hnode = calloc (1, sizeof (struct _htable_node));

	if (new_hnode == NULL)
		return 1;

	//printf ("looking up...\n"); 

	cur_hnode = _htable_lookup (ht, key);

	if (cur_hnode != NULL)
		return 2;

	//printf ("inserting...\n");

	new_hnode->key = strdup (key);
	new_hnode->hash = hashval;
	new_hnode->data = v;
	new_hnode->next = ht->table [hashpos];

	ht->table [hashpos] = new_hnode; 
	ht->num_elem++;

	// tries to expand the hashtable
	_htable_expand (ht);

	return 0;

}

/**
 * Removes a key->value pair from the hash table.
 *
 * @param ht a hash table object.
 * @param key the key to be removed.
 */
void htable_remove (htable *ht, char *key) {

	struct _htable_node *cur_hnode;
	unsigned long int hashval = hash ((unsigned char *)key, sizeof (unsigned long int), HASH_INIT_VAL);
	unsigned long int hashpos = hashval % ht -> size;

	cur_hnode = _htable_lookup (ht, key);

	if (cur_hnode == NULL)
		return;

	free (cur_hnode -> key); 
	free (cur_hnode -> data);
	free (cur_hnode);

	ht->table [hashpos] = NULL;
	ht->num_elem--;

	// tries to shrink the hashtable
	_htable_shrink (ht);

	return;

}

/**
 * Internal hash table lookup implementation.
 * 
 * @param ht a hash table object.
 * @param key the key to be found in the hash table.
 * @return an _htable_node object.
 */
struct _htable_node *_htable_lookup (htable *ht, char *key) {

	struct _htable_node *cur_hnode;
	unsigned long int hashval = hash ((unsigned char *)key, sizeof (unsigned long int), HASH_INIT_VAL);
	unsigned long int hashpos = hashval % ht -> size;

	DEBUGP ("+++ looking up key[%s] with pos [%lu] - hash [%lu]\n", key, hashpos, hashval );

	for ( cur_hnode = ht -> table [hashpos];
	     cur_hnode != NULL;
	     cur_hnode = cur_hnode -> next ) {

		     if (strcmp (key, cur_hnode -> key) == 0)
			     return cur_hnode;
	     }

	return NULL;

}

/**
 * Looks up for a key in the given hash table.
 *
 * @param ht a hash table object.
 * @param key the key to be found in the hash table.
 * @return returns the data object if the key exists in the hash table, otherwise returns null.
 */
data *htable_lookup (htable *ht, char *key) {

	struct _htable_node *cur_hnode;

	DEBUGP ("key=[%s]\n", key);

	cur_hnode = _htable_lookup (ht, key);

	if (cur_hnode != NULL) 
		return cur_hnode -> data;

	return NULL;

}

/**
 * Prints the content of a hash table.
 *
 * @param ht a hash table object.
 */
void htable_print (htable *ht) {

	struct _htable_node *hnode;
	int i;

	printf ("{\n");

	for (i=0; i<ht->size; i++) {
		hnode = ht -> table [i]; 
		while (hnode != NULL) {
			printf ("\t[%s]->", hnode -> key);
			switch (hnode -> data -> type) {
				case STRING:
					printf ("[%s]\n", hnode->data->value.u_str);
					break;
				case INTEGER:
					printf ("[%d]\n", hnode->data->value.u_int);
					break;
				case FLOAT:
					printf ("[%f]\n", hnode->data->value.u_double);
			}
			hnode = hnode -> next;
		}
	}

	printf ("}\n");

}

/**
 * Returns the number elements stored in the hash table.
 *
 * @param ht a hash table object.
 * @return number of elements stored.
 */
int htable_num_elements (htable *ht) {
	//printf ("htable_num_elements pointer ht=[%p]\n", ht);
	//printf ("%x\n", (&ht) );
	return ht -> num_elem;
}

/**
 * Set the high density factor, that is, when should the hash table expand
 * its maximum number of stored elements
 * 
 *
 * @param ht a hash table object.
 * @param fill_factor the high density factor.
 */
void htable_set_resize_high_density (htable *ht, int fill_factor) {
	ht -> high_fill_factor = fill_factor;
}

/**
 * Set the low density factor, that is, when should the hash table shrink
 * its maximum number of stored elements
 * 
 *
 * @param ht a hash table object.
 * @param fill_factor the low density factor.
 */
void htable_set_resize_low_density (htable *ht, int fill_factor) {
	ht -> low_fill_factor = fill_factor;
}

/**
 * Internal expand function. If the number of stored elements exceeds 
 * one of the density factors it expands the hashtable size by a
 * factor of 2.
 *
 * @param ht a hash table object.
 */
void _htable_expand (htable *ht) {

	float fill_factor;

	fill_factor = (((float)ht->num_elem) / (ht -> size)) * 100.0;

	//printf ("fill_factor [%f]\n", fill_factor);

	if ( fill_factor > (float) ht -> high_fill_factor) {

		//printf ("==>expand\n");
		_htable_resize_size (ht, ht->size * 2);

	}

}

/**
 * Internal shrink function. If the number of stored elements exceeds 
 * one of the density factors it shrink the hashtable size by a
 * factor of 2.
 *
 * @param ht a hash table object.
 */
void _htable_shrink (htable *ht) {

	float fill_factor;

	fill_factor = (((float)ht->num_elem) / (ht -> size)) * 100.0;

	//printf ("fill_factor [%f]\n", fill_factor);

	if ( fill_factor < (float) ht -> low_fill_factor) {

		//printf ("==>shrink\n");
		_htable_resize_size (ht, (int) ht->size / 2);

	}

}

/**
 * Internal resize function, called by _htable_resize.
 *
 * @param ht a hash table object.
 * @param size the maximum number of elements in the new hash table.
 */
void _htable_resize_size (htable *ht, int size) {

	int i;
	unsigned long int hashval;
	unsigned long int hashpos;
	struct _htable_node **old_table;
	struct _htable_node *cur_hnode, *temp, *next_temp;

	old_table = ht->table;

	ht->num_elem = 0;
	ht->table = malloc (sizeof (struct _htable_node) * size );

	// reset the new table
	for (i=0; i<size; i++) {
		ht->table[i] = NULL;
	}

	// move the data to the new table
	for (i=0; i<ht->size; i++) {
		cur_hnode = old_table [i];
		while (cur_hnode != NULL) {
			//printf ("i=>[%d] adding to new ht!\n", i);

			temp = cur_hnode;
			next_temp = (cur_hnode -> next) ? cur_hnode -> next : NULL;

			hashval = hash ((unsigned char *)temp->key, sizeof (unsigned long int), HASH_INIT_VAL);
			hashpos = hashval % size;

			temp->next = ht->table [hashpos];

			ht->table [hashpos] = temp;
			ht->num_elem++;

			cur_hnode = next_temp;

		}
	}

	ht->size = size;
	//printf ("new_ht - num_elem [%d] - size [%d]\n", ht -> num_elem, ht -> size);
	free (old_table);

}

/** @} */