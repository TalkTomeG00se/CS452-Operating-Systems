#include "ThreadsafeBoundedList.h"
#include <stdlib.h>
#include <pthread.h>

/**
 * Structure supplied by project instructions. Struct contains pointer to underlying list and additional variables to implement the monitor.
 */ 
struct tsb_list {
 struct list *list;
 int capacity;
 Boolean stop_requested;
 pthread_mutex_t mutex;
 pthread_cond_t listNotFull;
 pthread_cond_t listNotEmpty;
};

/**
 * Constructor: Allocates a new listist object and initializes its members. Initialize
 * the mutex and condition variables associated with the bounded list monitor. Set the
 * capacity of the list.
 *
 * @return a pointer to the allocated list.
 */
struct tsb_list * tsb_createList(int (*equals)(const void *, const void *),  char * (*toString)(const void *), void (*freeObject)(void *), int capacity) {



}

/**
 * Frees all elements of the given list and the <code>struct *list</code> itself.
 * Does nothing if list is <code>NULL</code>. Also frees the associated mutex and
 * condition varibles and the wrapper structure.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_freeList(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // initial lock, pointer list to variable name mutex

    freeList(list->list); // pointer list, variable name list

    pthread_mutex_unlock(&(list->mutex)); // unlocking, pointer list to variable name mutex

}

/**
 * Returns the size of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The current size of the list.
 */
int tsb_getSize(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // lock

    int size = getSize(list->list); // pointer list to variable name list

    return size;

    pthread_mutex_unlock(&(list->mutex)); // unlocking
}

/**
 * Returns the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @return The macimum capacity of the list.
 */
int tsb_getCapacity(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // lock

    return list -> capacity; // pointer list to variable name capacity

    pthread_mutex_unlock(&(list->mutex)); // unlocking
}

/**
 * Sets the maximum capacity of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 * @param capacity the maximum allowed capacity of the list
 * @return none
 */
void tsb_setCapacity(struct tsb_list * list, int capacity) {

    pthread_mutex_lock(&(list->mutex)); // initial lock, pointer list to variable name mutex

    list->capacity = capacity; // pointer list to capacity variable, set capacity

    pthread_mutex_unlock(&(list->mutex)); // unlocking, pointer list to variable name mutex

}

/**
 * Checks if the list is empty.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is empty; false otherwise.
 */
Boolean tsb_isEmpty(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // lock

    int size = tsb_getSize(list); // get size of list

    return size == 0; // using relational operator, returns yes if true, else no

    pthread_mutex_unlock(&(list->mutex)); // unlocking
}

/**
 * Checks if the list is full.
 *
 * @param  list a pointer to a <code>List</code>.
 * @return true if the list is full to capacity; false otherwise.
 */
Boolean tsb_isFull(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // initial lock

    int size = tsb_getSize(list); // get size of list

    return size >= tsb_getCapacity(list); // relational operator, comparing size to current capacity. True if full, else no

    pthread_mutex_unlock(&(list->mutex)); // unlocking

}

/**
 * Adds a node to the front of the list. After this method is called,
 * the given node will be the head of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtFront(struct tsb_list * list, NodePtr node) {

    pthread_mutex_lock(&(list->mutex)); // lock

    struct node* temp = createNode(node); // creates a new node, using the createNode function

    addAtFront(list->list, temp); // adds at front using existing function

    pthread_cond_broadcast(&(list->listNotEmpty)); // broadcasting that the list is not empty now, ensuring consumers are going

    pthread_mutex_unlock(&(list->mutex)); // unlocking

}

/**
 * Adds a node to the rear of the list. After this method is called,
 * the given node will be the tail of the list. (Node must be allocated
 * before it is passed to this function.) If the list and/or node are NULL,
 * the function will do nothing and return.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to add.
 */
void tsb_addAtRear(struct tsb_list * list, NodePtr node) {

    pthread_mutex_lock(&(list->mutex)); // lock

    struct node* temp = createNode(node); // creates a new node, using the createNode function

    addAtRear(list->list, temp); // adds at rear using existing function

    pthread_cond_broadcast(&(list->listNotEmpty)); // broadcasting that the list is not empty now, ensuring consumers are going

    pthread_mutex_unlock(&(list->mutex)); // unlocking

}

/**
 * Removes the node from the front of the list (the head node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeFront(struct tsb_list * list) {

   

}

/**
 * Removes the node from the rear of the list (the tail node) and returns
 * a pointer to the node that was removed. If the list is NULL or empty,
 * the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeRear(struct tsb_list * list) {


}

/**
 * Removes the node pointed to by the given node pointer from the list and returns
 * the pointer to it. Assumes that the node is a valid node in the list. If the node
 * pointer is NULL, the function will do nothing and return NULL.
 *
 * @param list a pointer to a <code>List</code>.
 * @param node a pointer to the node to remove.
 * @return a pointer to the node that was removed.
 */
NodePtr tsb_removeNode(struct tsb_list * list, NodePtr node) {


}

/**
 * Searches the list for a node with the given key and returns the pointer to the
 * found node.
 *
 * @param list a pointer to a <code>List</code>.
 * @param the object to search for.
 * @return a pointer to the node that was found. Or <code>NULL</code> if a node 
 * with the given key is not found or the list is <code>NULL</code> or empty.
 */
NodePtr tsb_search(struct tsb_list * list, const void *obj) {

    pthread_mutex_lock(&(list -> mutex)); // initial lock

    NodePtr search = search(list -> list, obj); // Using NodePtr struct defined in Node.h. Running search, pointer list to variable name list

    pthread_mutex_unlock(&(list -> mutex)); // unlocking

    return search; // returns the pointer for node

}

/**
 * Reverses the order of the given list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_reverseList(struct tsb_list *  list) {

    pthread_mutex_lock(&(list -> mutex)); // initial lock

    reverseList(list -> list); // reverses the list, using existing function

    pthread_mutex_unlock(&(list -> mutex)); // unlocking

}

/**
 * Prints the list.
 *
 * @param list a pointer to a <code>List</code>.
 */
void tsb_printList(struct tsb_list * list) {

    pthread_mutex_lock(&(list -> mutex)); // initial lock

    printList(list -> list); // prints the list, using existing function

    pthread_mutex_unlock(&(list -> mutex)); // unlocking

}

/**
 * Finish up the monitor by broadcasting to all waiting threads
 */
void tsb_finishUp(struct tsb_list * list) {

    pthread_mutex_lock(&(list->mutex)); // initial lock

    list->stop_requested = 1; // setting value to 1, requesting stop

    pthread_cond_broadcast(&(list->listNotEmpty)); // broadcasting that the list is not empty, ensures all consumers are going

    pthread_mutex_unlock(&(list->mutex)); // unlocking
}