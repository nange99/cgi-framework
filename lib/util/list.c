#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/**
 * @defgroup List List implementation
 * @ingroup Util
 *
 * @{
 */


/**
 * Allocates a new list object.
 * 
 * @return a list object.
 */
list* list_create (void) {
  list *l;

  l = calloc (1,sizeof (list));

  l->l_head = NULL;
  l->l_tail = NULL;
  l->l_count = 0;

  return l;
}

/**
 * Allocates a new list node object.
 *
 * @return a list node object.
 */
list_node* list_node_create (void) {
  list_node *ln;
  
  ln = calloc (1,sizeof (list_node));

  ln->data = NULL;
  ln->next = NULL;
  ln->prev = NULL;

  return ln;
}

/**
 * List iterator go to head node.
 *
 * @param li a list iterator object.
 */
void list_iter_go_to_head (list_iter *li) {
  li->i_cur = li->i_list->l_head;
}

/**
 * List iterator go to tail node.
 *
 * @param li a list iterator object.
 */
void list_iter_go_to_tail (list_iter *li) {
  li->i_cur = li->i_list->l_tail;
}

/**
 * List has next node.
 *
 * @param li a list iterator object.
 * @return returns 1 when true, otherwise returns 0.
 */
int list_iter_has_next ( list_iter *li ) {
  if (li->i_cur != NULL) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * List has previous node.
 *
 * @param li a list iterator object.
 * @return returns 1 when true, otherwise returns 0.
 */
int list_iter_has_previous ( list_iter *li ) {
  if (li->i_cur != NULL) {
    return 1;
  } else {
    return 0;
  }
}

/**
 * Get the next list_node object.
 *
 * @param li a list iterator object.
 * @return returns the next list_node object.
 */
list_node* list_iter_get_next ( list_iter *li ) {
  list_node *tmp;
  
  tmp = li->i_cur;
  li->i_cur = li->i_cur->next;

  return tmp;
}

/**
 * Get the previous list_node object.
 *
 * @param li a list iterator object.
 * @return returns the previous list_node object.
 */
list_node* list_iter_get_previous ( list_iter *li ) {
  list_node *tmp;

  tmp = li->i_cur;
  li->i_cur = li->i_cur->prev;

  return tmp;
}

/**
 * Frees the given list iterator.
 *
 * @param li list iterator object.
 */
void list_iter_free (list_iter *li) {

  li->i_list = NULL;
  li->i_cur = NULL;

  free (li);

}

/**
 * Frees a list and its contents.
 * 
 * @param l a list object.
 */
void list_free (list *l) {

  list_iter *li;
  list_node *ln;

  if (l->l_count > 0) {

    //printf("free the nodes first\n");
    li = list_get_iterator (l);

    list_iter_go_to_tail (li);

    while ( list_iter_has_previous (li) ) {
      ln = list_iter_get_previous (li);
      list_node_free (ln);
    }    

    list_iter_free (li);
  } 

  free (l);
  
}

/**
 * Frees a list node object.
 *
 * @param ln a list node object.
 */
void list_node_free (list_node *ln) {

  //printf ("freeing node\n");

  ln->data = NULL;
  ln->next = NULL;
  ln->prev = NULL;

  free (ln);

}

/**
 * Appends a list_node object to the given list.
 *
 * @param l a list object.
 * @param ln a list node object.
 */
int list_append_node (list *l, list_node *ln) {
  
  // list is empty
  if (l->l_head == NULL) {
    //printf ("list is empty\n");
    l->l_head = ln;
    l->l_tail = ln;
    l->l_count = 1;
    return 0;
  } 
  
  if (l->l_head->next == NULL) {
    //printf ("adding first node!\n");
    l->l_head->next = ln;
    ln->prev = l->l_head;
  } else {
    //printf ("adding other nodes!\n");
    l->l_tail->next = ln;
    ln->prev = l->l_tail;
  }
   
  l->l_tail = ln;
  l->l_count++;

  return 0;

}

int list_remove_node (list *l, list_node *ln) {
  
  if ( ln->next == NULL ) {
    // last node on the list

    //printf ("*** removing the last node on the list...\n");

    l->l_tail = ln->prev;
    l->l_count--;
    
    if (ln->prev == NULL) {
      l->l_head = NULL;
    } else {
      if (ln->prev->next == ln)
	ln->prev->next = NULL;
    }

    list_node_free (ln);

  } else if (ln->prev == NULL) {
    // first node on the list

    //printf ("*** removing the first node on the list...\n");

    l->l_head = ln->next;
    l->l_count--;

    if (ln->next == NULL) {
      l->l_tail = NULL;
    } else {
      if (ln->next->prev == ln)
	ln->next->prev = NULL;
    }

    list_node_free (ln);

  } else {
    // middle node

    //printf ("*** removing a middle node from the list...\n");

    ln->prev->next = ln->next;
    ln->next->prev = ln->prev;

    l->l_count--;

    list_node_free (ln);

  }

  if (l->l_count == 0) {
    //printf ("*** the list is now empty\n");
    l->l_head = NULL;
    l->l_tail = NULL;
  }

  return 0;

}

int list_get_count (list *l) {
  return l->l_count;
}

/**
 * Get the first node of a list
 */
list_node* list_get_first_node (list *l) {
  if (l->l_count >= 1) {
    return l->l_head;
  } 
  return NULL;
}

/**
 * Creates a list iterator object.
 *
 * @param l a list object.
 * @return a list iterator object.
 */
list_iter* list_get_iterator (list *l) {

  list_iter *l_iter;

  l_iter = malloc (sizeof (list_iter));

  l_iter->i_list = l;
  l_iter->i_cur = list_get_first_node (l);

  return l_iter;

}

/** @} */
