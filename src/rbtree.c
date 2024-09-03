#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = (node_t *)calloc(1, sizeof(node_t));
  p->root = p->nil;
  p->nil->color = RBTREE_BLACK;
  // TODO: initialize struct if needed
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  node_t *x = t->root;
  while (x != t->nil){
    while (!(x->left ==t-> nil && x->right == t->nil)){
      if (x->left != t->nil){
        x = x->left;
      } else {
        x = x->right;
      }
    }
    if (x->parent == t->nil) {
      x = t->nil;
      free(t->root);
    } else if (x == x->parent->left){
      x = x->parent;
      free(x->left);
    } else {
      x = x->parent;
      free(x->right);
    }
  }
  free(t->nil);
  free(t);
}

void rotate_left(rbtree *t, node_t *x) {
  node_t *y= x->right;
  x->right = y->left;
  if (y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil) {    // x가 root인경우
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void rotate_right(rbtree *t, node_t *x) {
  node_t *y= x->left;
  x->left = y->right;
  if (y->right != t->nil){
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil) {    // x가 root인경우
    t->root = y;
  } else if (x == x->parent->right) {
    x->parent->right = y;
  } else {
    x->parent->left = y;
  }
  y->right = x;
  x->parent = y;
}

void insert_fix(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    if (z->parent == z->parent->parent->left){
      node_t *u = z->parent->parent->right;
      if (u->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right){
          z = z->parent;
          rotate_left(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color =RBTREE_RED;
        rotate_right(t,z->parent->parent);
      }
    } else {
      node_t *u = z->parent->parent->left;
      if (u->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left){
          z = z->parent;
          rotate_right(t,z);
        }
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color =RBTREE_RED;
        rotate_left(t,z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z= (node_t *)malloc(sizeof(node_t));
  z->color = RBTREE_RED;
  z->key = key;
  z->left = t->nil;
  z->right = t->nil;
  
  node_t *x = t->root;
  node_t *y = t->nil;

  while (x != t->nil) {
    y = x;
    if (key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }
  z->parent = y;
  if (y == t->nil){
    t->root = z;
  } else if (key < y->key){
    y->left = z;
  } else {
    y->right = z;
  }
  insert_fix(t,z);
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *x = t->root;

  while (x!= t->nil){
    if (key == x->key){
      return x;
    } else if (key< x->key){
      x=x->left;
    } else {
      x=x->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  if (x == t->nil){
    return NULL;
  }
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *rbtree_sub_min(const rbtree *t, node_t *x) {
  // TODO: implement find
  if (x == t->nil){
    return NULL;
  }
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  node_t *x = t->root;
  if (x == t->nil){
    return NULL;
  }
  while (x->right != t->nil){
    x = x->right;
  }
  return x;
}

void transplant(rbtree *t, node_t *u, node_t *v){
  if (u->parent == t->nil){
    t->root = v;
  } else if (u == u->parent->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

void erase_fix(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    node_t *s;
    if (x == x->parent->left) {
      s = x->parent->right;
      if (s->color == RBTREE_RED) {
        s->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_left(t,x->parent);
        s = x->parent->right;
      }
      if (s->left->color == RBTREE_BLACK && s->right->color == RBTREE_BLACK) {
        s->color = RBTREE_RED;
        x->parent->color = RBTREE_BLACK;
        x = x->parent;
      } else {
        if (s->right->color == RBTREE_BLACK) {
          s->color = RBTREE_RED;
          s->left->color = RBTREE_BLACK;
          rotate_right(t,s);
          s = x->parent->right;
        }
        s->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        s->right->color = RBTREE_BLACK;
        rotate_left(t,x->parent);
        x = t->root;
      }
    } else {
      s = x->parent->left;
      if (s->color == RBTREE_RED) {
        s->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_right(t,x->parent);
        s = x->parent->left;
      }
      if (s->right->color == RBTREE_BLACK && s->left->color == RBTREE_BLACK) {
        s->color = RBTREE_RED;
        x->parent->color = RBTREE_BLACK;
        x = x->parent;
      } else {
        if (s->left->color == RBTREE_BLACK) {
          s->color = RBTREE_RED;
          s->right->color = RBTREE_BLACK;
          rotate_left(t,s);
          s = x->parent->left;
        }
        s->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        s->left->color = RBTREE_BLACK;
        rotate_right(t,x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t *y = z;
  node_t *x;
  color_t to_be_deleted = y->color;
  if (z->left == t->nil) {
    x = y->right;
    transplant(t,z,z->right);
  } else if (z->right == t->nil) {
    x = y->left;
    transplant(t,z,z->left);
  } else {
    y = rbtree_sub_min(t,z->right);
    x = y->right;
    to_be_deleted = y->color;
    if (y != z->right) {
      transplant(t,y,x);
      y->right = z->right;
      z->right->parent = y;
    } else {
      x->parent = y;
    }
    transplant(t,z,y);
    y->left = z->left;
    z->left->parent = y;
    y->color = z->color;
  }
  if (to_be_deleted == RBTREE_BLACK) {
    erase_fix(t,x);
  }
  free(z);
  return 0;
}

int inorder_travels(const rbtree *t, key_t *arr, const size_t n, int *arr_size, node_t *x) {
  if (x == t->nil) {
    return 0;
  }
  if (inorder_travels(t,arr,n,arr_size,x->left)) {
    return 1;
  }
  if (*arr_size<n){
    *(arr+*arr_size) = x->key;
  } else {
    return 1;
  }
  if (inorder_travels(t,arr,n,arr_size,x->left)) {
    return 1;
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  int *arr_size = 0;
  inorder_travels(t,arr,n,arr_size,t->root);
  return 0;
}
