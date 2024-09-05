#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));          // rbtree를 위한 메모리할당, malloc도 가능
  if (p==NULL){                                             // 메모리 할당 실패 시 return
    return NULL;
  }
  p->nil = (node_t *)calloc(1, sizeof(node_t));             // 마찬가지로 malloc으로 해도 문제 없으나 nil의 필드에 garbage있는 것보다 0으로 되어있는게 디버깅하기 편했음
  if (p->nil == NULL){                                      // 메모리 할당 실패 시 앞서 할당한 rbtree 해제하고 return
    free(p);
    return NULL;
  }
  p->root = p->nil;                                         // 빈 트리에서 root는 nil
  p->nil->color = RBTREE_BLACK;															// sentinel은 BLACK
  return p;
}

void delete_rbtree(rbtree *t) {
  node_t *x = t->root;																			// root를 시작으로 후위순회하며 노드들을 삭제
  while (x != t->nil){
    while (!(x->left ==t-> nil && x->right == t->nil)){
      if (x->left != t->nil){																// right를 시작으로 후위 순회해도 문제 없음, 그냥 그렇다구요
        x = x->left;
      } else {
        x = x->right;
      }
    }
    if (x->parent == t->nil) {															// 현재 노드가 root인 경우
      x = t->nil;
      free(t->root);
    } else if (x == x->parent->left){												// 왼쪽 자식인 경우
      x = x->parent;																				// 부모 입장에서
      free(x->left);																				// 자식 보내고
      x->left = t->nil;																			// 빈자리는 sentinel로 채움
    } else {																								// 오른쪽 자식
      x = x->parent;
      free(x->right);
      x->right = t->nil;
    }
  }
  free(t->nil);																							// sentinel 삭제
  free(t);																									// 트리 삭제
}

void rotate_left(rbtree *t, node_t *x) {
  node_t *y= x->right;
  x->right = y->left;																				// y자식의 왼쪽 자식 -> x의 오른쪽 자식
  if (y->left != t->nil){																		// sentinel이 아닌 경우 부모 변경
    y->left->parent = x;
  }
  y->parent = x->parent;																		// y가 x의 부모 승계
  if (x->parent == t->nil) {    														// x가 root인경우
    t->root = y;
  } else if (x == x->parent->left) {												// y가 x의 부모로 부터 승계
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;																							// x가 y의 왼쪽 자식으로 입양
  x->parent = y;
}

void rotate_right(rbtree *t, node_t *x) {										// left<->right 다 그렇게 하잖아?
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
  while (z->parent->color == RBTREE_RED) {									// 부모가 BLACK이면 자식이 레드여도 노상관
    if (z->parent == z->parent->parent->left){							// 부모가 왼쪽 자식이면 삼촌은 오른쪽 자식
      node_t *u = z->parent->parent->right;									// 삼촌 등장
      if (u->color == RBTREE_RED) {													// 삼촌이 RED면 부모, 삼촌 BLACK으로 바꾸고 조부모 RED로 바꾸고
        z->parent->color = RBTREE_BLACK;
        u->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;															// 조부모 입장에서 다시 고칠게 있는지 while 반복
      } else {																							// 삼촌이 BLACK인 경우
        if (z == z->parent->right){													// Z가 오른쪽 자식이면 조부모 회전시 Z가 조부모 자식으로 휩쓸리지 않도록 부모 레벨에서 회전
          z = z->parent;
          rotate_left(t,z);
        }
        z->parent->color = RBTREE_BLACK;										// 경로별 BLACK의 수를 맞추기 위해서
        z->parent->parent->color =RBTREE_RED;								// 조부모와 부모의 색을 바꾸고 조부모 회전
        rotate_right(t,z->parent->parent);
      }
    } else {																								// left<->right 굿
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
  node_t *z= (node_t *)malloc(sizeof(node_t));							// 추가할 노드 메모리 할당, 추가되는 노드 구초제 필드의 메모리는 모두 본 함수에서 모두 채워지므로 굳이 calloc으로 초기화 필요없음.
  if (z==NULL){
    return NULL;
  }
  z->color = RBTREE_RED;																		// 추가되는 노드는 RED로 시작
  z->key = key;																							// 키값 받아주고
  z->left = t->nil;																					// 추가될 노드는 리프노드가 될 예정이므로
  z->right = t->nil;																				// 양쪽 자식 nil
  
  node_t *x = t->root;																			// x를 시작으로 nil까지 내려가는데 nil이 되면 부모가 누군지 모르는 상황이기에
  node_t *y = t->nil;																				// 부모도 함께 y로 tracking

  while (x != t->nil) {																			// 리프노드 도달까지 계속 반복
    y = x;																									// x가 nil일 수도 있으니 y 선업데이트
    if (key < x->key) {																			// 작으면 왼쪽
      x = x->left;
    } else {																								// 같거나 크면 오른쪽
      x = x->right;																					// 같은 경우에 오른쪽으로 가지만 fix과정에서 rotation되어 왼쪽에 있을지도?
    }
  }
  z->parent = y;																						// tracking해온 y노드를 부모로 삼고
  if (y == t->nil){																					// y가 만약에 nil이라면 위의 while문을 탄적이 없고 root가 nil이었다는 말이니
    t->root = z;																						// 새로운 노드 z를 트리의 root로 삼음
  } else if (key < y->key){																	// 위쪽과 동일한 조건으로 부모의 자식 입양
    y->left = z;
  } else {
    y->right = z;
  }
  insert_fix(t,z);																					// 노드 규칙이 안맞을 수 있으니 고치는 함수 호출
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {			// 특정 key값 search
  node_t *x = t->root;																			// root를 시작으로
  while (x!= t->nil){
    if (key == x->key){																			// key값 찾으면 해당 node 리턴
      return x;
    } else if (key < x->key){																// 아닌 경우 key값 비교하면 자식으로 내려감
      x=x->left;
    } else {
      x=x->right;
    }
  }
  return NULL;																							// nil에 도착한 경우 해당 key값 가진 node 발견 못했으므로 NULL 리턴
}

node_t *rbtree_min(const rbtree *t) {												// 트리의 최소key값을 가진 노드를 찾으므로
  node_t *x = t->root;
  if (x == t->nil){																					// 빈 트리일 경우 NULL 반환
    return NULL;
  }
  while (x->left != t->nil){																// 왼쪽 자식 nil인 경우 까지 내려가고 해당 노드 반환
    x = x->left;
  }
  return x;
}

node_t *rbtree_sub_min(const rbtree *t, node_t *x) {				// 특정 sub 트리에서의 최소 key값 찾기로 rbtree_erase에서 양쪽 자식이 있는 경우 successor를 찾을 때만 씀
  if (x == t->nil){																					// 특정 node에서 시작하므로 특정 node를 인자로 받고 나머지는 위와 동일
    return NULL;
  }
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {												// 오른쪽으로만 가세요
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

void transplant(rbtree *t, node_t *u, node_t *v){						// 부모 강탈
  if (u->parent == t->nil){																	// 강탈당하는 node의 부모가 nil이다? root였네
    t->root = v;
  } else if (u == u->parent->left) {												// 조부모로부터 자식으로 인정 받음
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;																		// 네부모 내부모
}

void erase_fix(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {				// 고쳐야하는 애가 root이거나 RED다? 밑에서 그냥 BLACK으로 바꾸면 됨.
    node_t *s;																							// 혈육 출현
    if (x == x->parent->left) {															// 고쳐야할 애가 왼쪽 자식이면
      s = x->parent->right;																	// 혈육은 오른쪽 자식
      if (s->color == RBTREE_RED) {													// 본 조건문 안의 코드는 RED인 sibling을 BLACK으로 바꾸는 내용
        s->color = RBTREE_BLACK;														// sibling이 RED면 그 자식은 BLACK인데 rotate에서 그 BLACK인 자식이 x의 sibling이 된다.
        x->parent->color = RBTREE_RED;
        rotate_left(t,x->parent);
        s = x->parent->right;
      }
      if (s->left->color == RBTREE_BLACK && s->right->color == RBTREE_BLACK) {	// sibling이 양쪽 자식이 BLACK인 경우 
        s->color = RBTREE_RED;																									// sibling을 RED로 바꾼다. 그럼 x와 s의 경로 둘다 BLACK이 하나씩 부족한 상황인데
        x = x->parent;																													// 이는 x의 부모가 BLACK이 하나 부족한 상황과 동일하다.
      } else {																																	// sibling의 자식에 RED가 있는 경우
        if (s->right->color == RBTREE_BLACK) {																	// 아래쪽에서 x의 부모를 회전하면서 s의 오른쪽 라인에 BLACK이 하나 부족해지는데
          s->color = RBTREE_RED;																								// 후에 BLACK을 추가하기 위해 s의 오른쪽 자식을 red로 바꾸는 작업
          s->left->color = RBTREE_BLACK;
          rotate_right(t,s);
          s = x->parent->right;
        }
        s->color = x->parent->color;																						// s가 x의 부모를 승계
        x->parent->color = RBTREE_BLACK;																				// x라인에 BLACK을 추가
        s->right->color = RBTREE_BLACK;																					// 부족해질 s의 오른쪽 라인을 위해 s의 오른쪽 자식 BLACK으로 변경
        rotate_left(t,x->parent);
        x = t->root;																														// 종료하기위해 x를 root로 변경
      }
    } else {																																		// left<->right 굿
      s = x->parent->left;
      if (s->color == RBTREE_RED) {
        s->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        rotate_right(t,x->parent);
        s = x->parent->left;
      }
      if (s->right->color == RBTREE_BLACK && s->left->color == RBTREE_BLACK) {
        s->color = RBTREE_RED;
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

int rbtree_erase(rbtree *t, node_t *z) {					// z는 사라질 친구
  node_t *y = z;																	// z의 양쪽 자식이 모두 nil이 아닌 경우 z의 오른쪽 sub트리에서 z의 계승자 y를 찾는데 y는 왼쪽 자식이 nil이다.
  node_t *x;																			// 하여 문제를 line-279의 else문에서 y를 삭제하는 경우로 생각할 수 있다.
  color_t to_be_deleted = y->color;								// 사라질 color
  if (z->left == t->nil) {
    x = y->right;
    transplant(t,z,z->right);
  } else if (z->right == t->nil) {
    x = y->left;
    transplant(t,z,z->left);
  } else {
    y = rbtree_sub_min(t,z->right);								// z의 계승자 y를 찾는다. y를 삭제하는 경우로 생각할 수 있다.
    x = y->right;																	// y를 계승할 x는 항상 오른쪽 자식이다.
    to_be_deleted = y->color;											// 사라질 color
    if (y != z->right) {													// y가 z의 오른쪽 자식이 아닌 경우
      transplant(t,y,x);													// x가 y의 부모 자식이 됨
      y->right = z->right;												// y가 z의 오른쪽 자식 뺏음
      z->right->parent = y;
    } else {																			// 추후 erase_fix에서 node x를 살피는데 x가 nil인 경우를 위해서 nil의 부모를 임시로 지정해줌
      x->parent = y;
    }
    transplant(t,z,y);														// y가 z의 부모 자식이 됨
    y->left = z->left;														// y가 z의 필드를 물려받음
    z->left->parent = y;
    y->color = z->color;
  }
  if (to_be_deleted == RBTREE_BLACK) {						// 사라진 node가 RED인 경우 rule 위반이 아니므로 BLACK인 경우만 erase_fix 호출
    erase_fix(t,x);
  }
  free(z);																				// 사라진 모든 것들에게 좋아요
  return 0;
}

int inorder_travels(const rbtree *t, key_t *arr, const size_t n, int *arr_size, node_t *x) {		// key값의 오름차순으로 배열을 만들기 위해 left를 시작으로 중위 순회
  if (x == t->nil) {																								// node가 nil인 경우 할게 없음
    return 0;																												// 재귀함수 return값이 0인 경우 계속 순회
  }
  if (inorder_travels(t,arr,n,arr_size,x->left)) {									// return값이 1인 경우 순회 종료
    return 1;																												// 근데 테스트 케이스에서 트리의 node 개수만큼 n을 줘서
  }																																	// 항상 전부 순회해도 통과되는 것 같음...
  if (*arr_size<n){																									// arr에 채운게 n보다 작은 경우
    *(arr+*arr_size) = x->key;																			// key값 arr에 추가
    (*arr_size)++;																									// arr_size 증가
  } else {																													// n만큼 채웠으면 1 return
    return 1;
  }
  if (inorder_travels(t,arr,n,arr_size,x->right)) {
    return 1;
  }
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  int *arr_size = (int *)malloc(sizeof(int));												// arr개수 담을 메모리 할당, 함수 인자로 할 수도 있을텐데
  if (arr_size == NULL){																						// 할당 실패시 -1 return, 테스트 코드에서 딱히 안쓰는듯
    return -1;
  }
  *arr_size = 0;
  inorder_travels(t,arr,n,arr_size,t->root);
  free(arr_size);																										// 잘 쓰고 갑니다
  return 0;
}
