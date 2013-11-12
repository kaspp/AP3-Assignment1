#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "tldlist.h"
#include "date.h"


/*
  self defined function. 
  static to prevent misused of function in other file.
 */

static int createNewNode(TLDList * tld, char * hostname, TLDNode * node);
static TLDNode * makeNode(char * hostname);
static TLDNode * findDeepestNode(TLDNode * node);
static long findHeight(TLDNode * node);
static void RR (TLDNode * node);
static void LL (TLDNode * node);
static void balance(TLDNode * node);

//create the struct of tldlist, tldnode and iterator

struct tldlist {
    TLDNode *root;
    Date *begin;
    Date *end;

    long size;
};


struct tldnode {
    TLDNode * parent, * left, * right;
    char * content;
    long count, height;
};

struct tlditerator {
    TLDList * tld;
    long size;
    TLDNode * nodes;
};


TLDList *tldlist_create(Date *begin, Date *end) {
    TLDList *tld = (TLDList *)malloc(sizeof(TLDList));
    //if mem allocation fail
    if (tld == NULL)
        return NULL;
    
    //set the range and initialize the rest of the value of tldlist
    tld->begin = date_duplicate(begin);
    tld->end = date_duplicate(end);
    tld->root = NULL;
    tld->size = 0;
    
    return tld;
}

//destroy everything in tldlist.
void tldlist_destroy(TLDList *tld) {
    
	TLDIterator *it = tldlist_iter_create(tld);
	TLDNode *node;
    
	while ((node = tldlist_iter_next(it)) != NULL) {
		free(node->content);
		free(node);
	}
    
    date_destroy(tld->begin);
    date_destroy(tld->end);
	free(tld);
	tldlist_iter_destroy(it);

    
}

//add into tldlist
int tldlist_add(TLDList *tld, char *hostname, Date *d) {
    int forR = 1;
    long i;
   
    //change all to lowercase
    for (i = 0; (hostname[i] != '\0'); i++) {
        hostname[i] = tolower(hostname[i]);
    }
    
    //compare if the date is between the range
    if ((date_compare(tld->begin, d) >= 0) && (date_compare(tld->end, d) >= 0))
        return 0;
    
    //set a memory location to the hostname
    char *s = strrchr(hostname, '.') + 1;
    char *content = (char *)malloc(sizeof(s));
    strcpy(content, s);
    
    
    //put the node into the list
    if (createNewNode(tld, content, tld->root) != 0) {
        //add to the total size of the tldlist
        tld->size++;
        
    } else {
        //if not free the content
        free(content);

    }
    
    return forR;
}

//make the node
static TLDNode * makeNode(char *hostname) {
    TLDNode * cNode = (TLDNode *) malloc (sizeof(TLDNode) + 1);
    
    if (cNode==NULL)
        return NULL;
    cNode->parent = NULL;
    cNode->content = hostname;
    cNode->left = NULL;
    cNode->right = NULL;
    cNode->height = 1;
    cNode->count = 1;
    
    
    return cNode;
}

//create the ndoe. Recursive method
static int createNewNode(TLDList * tld, char * hostname, TLDNode * node) {
    int r = 0;
    //add to the root. r is for the value to return.
    if (tld->root == NULL) {
        TLDNode *n = makeNode(hostname);
        n->parent = NULL;
        tld->root = n;
        r = 1;
    } else {
        
        int cmp = strcmp(hostname, node->content); //compare between the str to see which is the bigger one.

        if (cmp == 0) {
            //node already exist
            node->count++;
            r = 1;
            free(hostname); //free the memory allocated hostname because this is a duplication
        } else if (cmp < 0) {
            if (node->left != NULL) //turn left
                return createNewNode(tld, hostname, node->left); //recursive
             else {
                //add node if blank
                TLDNode *n = makeNode(hostname);
                n->parent = node;
                node->left = n;

                balance(node->left); //create the balance
                r = 1;
            }
        } else if (cmp > 0) {
            if (node->right != NULL)
                return createNewNode(tld, hostname, node->right);
            else {
                TLDNode *n = makeNode(hostname);
                n->parent = node;
                node->right = n;

                balance(node->right);
                r = 1;
            }
        }
    }
    return r;

}

//return the total size of tldlist
long tldlist_count(TLDList *tld) {
    return tld->size;
}


TLDIterator *tldlist_iter_create(TLDList *tld) {
    
    TLDIterator *iter = (TLDIterator *)malloc(sizeof(TLDIterator));
    //if memory allocation fail
    if (iter == NULL)
        return NULL;
    
    //set the deepest node to be the start of the iterator list.
    iter->nodes = findDeepestNode(tld->root);
    iter->size = 0;
    
    return iter;
}


//function to look for the deepest node in the list.
static TLDNode *findDeepestNode(TLDNode *node) {
    
    TLDNode * test = node;
    
    //check if node is empty
    if (test != NULL) {
        // recurse to find which node is the last node in the tree.
        if (((test = findDeepestNode(node->left)) != NULL) || (test = findDeepestNode(node->right)) != NULL) {
            //return the found node
            return test;
        } else {
            return node;
        }
    } else {
        
        return node;
    }
    
    
}


//get the next node in the list
TLDNode *tldlist_iter_next(TLDIterator *iter) {
    TLDNode * get = iter->nodes;
    
    //if it is the last node.
    if (get == NULL)
        return NULL;
    
    //if it is the root
    if(iter->nodes->parent == NULL) {
        iter->nodes = NULL;
        return get;
    }
    
    //check for the deepest node and get the next node.
    if(iter->nodes->parent->right != iter->nodes && iter->nodes->parent->right != NULL)
        iter->nodes = findDeepestNode(iter->nodes->parent->right);
    else
        iter->nodes = iter->nodes->parent;
    
    return get;
}



//destroy the iterator list
void tldlist_iter_destroy(TLDIterator *iter) {
    free(iter->nodes);
    free(iter);
}

//return the content of the node.
char *tldnode_tldname(TLDNode *node) {
    return node->content;
}

//return the amount of node being counted
long tldnode_count(TLDNode *node) {
    return node->count;
}



//for AVL
//for balancing the tree
static void balance(TLDNode *node) {
	long b;
    
	if ((node->height = findHeight(node)) > 2) {
        b = findHeight(node->right) - findHeight(node->right);
		if (!abs((int)b) < 2) {
			
            if (b > 0) { // ?-right
				
                if (findHeight(node->left->right) > findHeight(node->left->left)) { // left-right
					
                    LL(node->left);
					node->left->left->height = findHeight(node->left->left);
					node->left->height = findHeight(node->left);
				
                }
				
                RR(node);
				node->right->height = findHeight(node->right);
				node->height = findHeight(node);
			
            } else { // ?-left
				
                if (findHeight(node->right->left) > findHeight(node->right->right)) { // right-left
					RR(node->right);
					node->right->right->height = findHeight(node->right->right);
					node->right->height = findHeight(node->right);
				}
				
                LL(node);
				node->left->height = findHeight(node->left);
				node->height = findHeight(node);
			}
		}
	}
    
}

//find the height of the node
static long findHeight(TLDNode *node) {
    long left, right, retR;
    
	if (node == NULL)
		return 0;
    
    if (node->left == NULL)
        left = 0;
    else
        left = node->left->height;
    
    if (node->right == NULL)
        right = 0;
    else
        right = node->right->height;
    
    if (left>right)
        retR = left + 1;
    else
        retR = right + 1;
    

	return retR;
}

//do rotation
static void RR (TLDNode *node) {
	node->parent->left = node->left;
	node->left->parent = node->parent;
	node->parent = node->left;
	node->left = node->left->right;
	node->left->parent = node;
	node->parent->right = node;
}


static void LL (TLDNode *node) {
	node->parent->right = node->right;
	node->right->parent = node->parent;
	node->parent = node->right;
	node->right = node->right->left;
	node->right->parent = node;
	node->parent->left = node;
}
