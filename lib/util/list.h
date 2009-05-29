#ifndef LIST_H
#define LIST_H

/**
 * @addtogroup List
 * @{
 */

/**
 * List node object
 */
typedef struct _list_node list_node;

/**
 * List node structure
 */
struct _list_node {
  /**> */
  list_node *next;
  /**> */
  list_node *prev;
  /**> */
  char *data;
};

/**
 * List object
 */
typedef struct _list list;

/**
 * List structure
 */
struct _list {
  /**> */
  list_node *l_head;
  /**> */
  list_node *l_tail;
  /**> */
  int l_count;
};

/**
 * List iterator object
 */
typedef struct _list_iter list_iter;

/**
 * List iterator structure
 */
struct _list_iter {
  /**> */
  list *i_list;
  /**> */
  list_node *i_cur;
};

list*      list_create (void);
list_node* list_node_create (void);

void       list_free (list *l);
void       list_node_free (list_node *ln);
void       list_iter_free (list_iter *li);

int        list_append_node (list *l, list_node *ln);
int        list_remove_node (list *l, list_node *ln);

int        list_get_count (list *);

list_node* list_get_first_node (list *l);

list_iter* list_get_iterator (list *l);
int        list_iter_has_next (list_iter *li);
int        list_iter_has_previous (list_iter *li);
list_node* list_iter_get_next (list_iter *li);
list_node* list_iter_get_previous (list_iter *li);
void       list_iter_go_to_head (list_iter *li);
void       list_iter_go_to_tail (list_iter *li);

/** @} */

#endif
