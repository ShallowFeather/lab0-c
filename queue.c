#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, l, list) {
        q_release_element(pos);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;
    tmp->value = strdup(s);
    if (!tmp->value) {
        free(tmp);
        return false;
    }
    list_add(&tmp->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;
    element_t *tmp = malloc(sizeof(element_t));
    if (!tmp)
        return false;
    tmp->value = strdup(s);
    if (!tmp->value) {
        free(tmp);
        return false;
    }
    list_add_tail(&tmp->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_entry(head->next, element_t, list);
    list_del(head->next);
    if (sp && bufsize != 0) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp && bufsize != 0) {
        strncpy(sp, tmp->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || head->next == NULL)
        return false;
    struct list_head *frontNode = head->next, *backNode = head->prev;
    while (frontNode != backNode && frontNode->next != backNode) {
        frontNode = frontNode->next;
        backNode = backNode->prev;
    }
    list_del(frontNode);
    q_release_element(list_entry(frontNode, element_t, list));
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (head == NULL)
        return false;
    bool checkDup = false;
    element_t *pos, *n;
    list_for_each_entry_safe (pos, n, head, list) {
        if (&n->list != head && strcmp(pos->value, n->value) == 0) {
            checkDup = true;
            list_del(&pos->list);
            q_release_element(pos);
        } else if (checkDup) {
            list_del(&pos->list);
            q_release_element(pos);
            checkDup = false;
        }
    }
    return true;
}

void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head)
        return;
    struct list_head *pos, *n;
    bool i = false;
    list_for_each_safe (pos, n, head) {
        if (i) {
            list_move(pos, pos->prev->prev);
        }
        i = !i;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL)
        return;
    struct list_head *pos, *n;
    list_for_each_safe (pos, n, head) {
        pos->next = pos->prev;
        pos->prev = n;
    }
    head->next = head->prev;
    head->prev = pos->next;
}

struct list_head *reverseK(struct list_head *head, int k, int cnt)
{
    if (head == NULL || cnt < k)
        return NULL;
    cnt -= k;
    struct list_head *now = head, *tmp = head->next;
    for (int i = 1; i < k; i++) {
        now->next = now->prev;
        now->prev = tmp;
        now = tmp;
        tmp = tmp->next;
    }
    tmp->next = reverseK(now, k, cnt);
    return now;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (head == NULL)
        return;
    reverseK(head, k, q_size(head));
    return;
}


void merge2list(struct list_head *left,
                struct list_head *right,
                struct list_head *result)
{
    while (!list_empty(left) && !list_empty(right)) {
        element_t *first = list_entry(left->next, element_t, list);
        element_t *second = list_entry(right->next, element_t, list);
        if (strcmp(first->value, second->value) <= 0)
            list_move_tail(&first->list, result);
        else
            list_move_tail(&second->list, result);
    }
    if (!list_empty(left))
        list_splice_tail(left, result);
    else
        list_splice_tail(right, result);
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *slow = head, *fast = head;
    do {
        fast = fast->next->next;
        slow = slow->next;
    } while (fast != head && fast->next != head);
    LIST_HEAD(left);
    LIST_HEAD(right);
    list_splice_tail_init(head, &right);
    list_cut_position(&left, &right, slow);
    q_sort(&left);
    q_sort(&right);
    merge2list(&left, &right, head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}