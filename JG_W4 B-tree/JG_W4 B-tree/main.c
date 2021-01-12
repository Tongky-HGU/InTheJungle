// B plus tree study
// 2021

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


#define Node_Order			3
#define MAX_Childs			Node_Order
#define MAX_Keys			MAX_Childs-1
#define MIN_Keys			(int)(ceil(Node_Order/2.0))-1

struct Node {							    // 비트리의 노드 구조체
	bool leaf;
	int key[MAX_Keys + 1];					// 키 데이터
	int num_key;							// 키 갯수
	struct Node* child[MAX_Childs + 1];     // 자식의 주소를 저장할 포인터
	int num_child;							// 자식 개수
	struct Node* next;
	bool end_leaf;							// 리프의 오른쪽 끝인가
};

struct Node* root; // 루트 포인터

/*SEARCH******************************************************************************************************/
int searchNode(struct Node* node, int val) {			// TO DO:: 이진탐색
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return 0;
	}
	struct Node* level = node;							// root부터 leaf까지 탐색
	while (1) {
		int pos;
		for (pos = 0; pos < level->num_key; pos++) {
			if (val == level->key[pos]) {					// 찾으면 리턴 1
				printf("%d exist!!\n", val);
				return 1;
			}
			else if (val < level->key[pos]) {
				break;
			}
		}
		if (level->leaf) break;
		level = level->child[pos];
	}
	printf("%d not exist!!\n", val);							// leaf 까지와서도 못찾으면 리턴 0
	return 0;
}


/*INSERT******************************************************************************************************/
 //Create Node
struct Node* createNode(int val) {
	struct Node* newNode;
	newNode = (struct Node*)malloc(sizeof(struct Node)); // B트리 구조체만큼 동적할당
	newNode->leaf = false;
	newNode->key[0] = val;
	newNode->num_key = 1;
	newNode->num_child = 0;
	newNode->end_leaf = false;
	return newNode;
}

//Split Node
struct Node* splitNode(int pos, struct Node* node, struct Node* parent) {
	int median = node->num_key / 2;
	struct Node* child;

	child = (struct Node*)malloc(sizeof(struct Node));
	child->leaf = node->leaf;
	child->num_key = 0;
	child->num_child = 0;
	child->end_leaf = node->end_leaf;						 // 나눌 노드가 마지막 리프 노드일 경우 새로운 노드가 마지막 리프다.
	child->next = node->next;

	if (node->leaf) {										 // 만약 리프 노드에서의 분할이라면 
		node->next = child;								     // 새 노드와 기존 노드를 링크 시켜준다.
		node->end_leaf = false;								 // 리프노드가 분할된다면 기존 노드는 더이상 마지막 리프일 수 없다.

		int num_iter = node->num_key;
		for (int i = median; i < num_iter; i++) {			 // child right에 키 담아주기 기존 노드는 child left가 된다
			child->key[i - median] = node->key[i];
			node->num_key--;
			child->num_key++;
		}
	}
	else{													 // leaf가 아닌 경우는 child를 넘겨줘야함
		int num_iter = node->num_key;
		for (int i = median + 1; i < num_iter; i++) {  // child right에 키 담아주기 기존 노드는 child left가 된다
			child->key[i - median - 1] = node->key[i];
			child->num_key++;
			node->num_key--;
		}
		num_iter = node->num_child;
		for (int i = median + 1; i < num_iter; i++) {
			child->child[i - median - 1] = node->child[i];
			child->num_child++;
			node->num_child--;
		}
	}

	if (node == root) {										 // 나눌 노드가 루트라면
		struct Node* new_parent;							 // 새로운 parent에는 median 키를 넣어준다.
		new_parent = createNode(node->key[median]);
		if (!node->leaf) {									 // 리프가 아닐때만 기존 노드에서 median이 사라진다.
			node->num_key--;								 // median 보냈으므로 -1
		}
		new_parent->child[0] = node;
		new_parent->child[1] = child;
		new_parent->num_child = 2;
		parent = new_parent;
		return parent;
	}
	else {													 // 나눌 노드가 루트가 아니라면
		for (int i = parent->num_key; i > pos; i--) {		 // 부모에 들어갈 자리를 만든다.
			parent->key[i] = parent->key[i - 1];
			parent->child[i + 1] = parent->child[i];		 
		}
		parent->key[pos] = node->key[median];				 // 부모 노드에 key를 추가해준다.
		parent->num_key++;
		if (!node->leaf) {									 // 리프가 아닐때만 기존 노드에서 median이 사라진다.
			node->num_key--;								 // median 보냈으므로 -1
		}
		parent->child[pos + 1] = child;						 // 부모 노드에 새 child를 추가해준다.
		parent->num_child += 1;
	}
	return node;
}

//Insert Node
struct Node* insertNode(int parent_pos, int val, struct Node* node, struct Node* parent) {
	int pos;												// pos는 삽입 될 포지션
	for (pos = 0; pos < node->num_key; pos++) {
		if (val == node->key[pos]) {
			printf("Duplicates are not permitted!!\n");		// 중복된 키는 금지
			return node;
		}
		else if (val < node->key[pos]) {					// val이 들어갈 위치를 찾는다.
			break;
		}
	}
	if (!node->leaf) {										// leaf가 아닐 경우에는 child로 내려간다.
		node->child[pos] = insertNode(pos, val, node->child[pos], node);
		if (node->num_key == MAX_Keys + 1) {				// 아래 삽입에 의해 키가 찼다면, 또 split을 해준다
			node = splitNode(parent_pos, node, parent);
		}
	}
	else {													// leaf에만 삽입한다.
		for (int i = node->num_key; i > pos; i--) {
			node->key[i] = node->key[i - 1];
			//node->child[i + 1] = node->child[i];			// TO DO:: 이게 필요한가??
		}
		node->key[pos] = val;
		node->num_key++;
		if (node->num_key == MAX_Keys + 1) {				// 키가 꽉찼다면? split을 해준다!
			node = splitNode(parent_pos, node, parent);
		}
	}

	return node;											// 노드 반환
}

void insert(int val) {
	if (!root) { 											// root 가 없다면 root를 만든다.
		root = createNode(val);
		root->leaf = true;
		root->end_leaf = true;
		return;
	}

	root = insertNode(0, val, root, root);					// root 가 있다면 노드를 찾아 삽입한다.
}
/*DELETION******************************************************************************************************/


void borrowFromLeft(struct Node* node, int pos) {
	
	if (node->child[pos]->leaf) {									 // 리프에서 borrow가 일어난다면
		int target = 0;													 // 부족한 위치에 들어가야할 idx
		int borrow = node->child[pos - 1]->num_key - 1;					 // 가져올 위치의 idx

		for (int i = 0; i < node->child[pos]->num_key; i++) {            // 채울 위치를 비워둔다.
			node->child[pos]->key[i + 1] = node->child[pos]->key[i];
		}
		node->child[pos]->key[target] = node->child[pos-1]->key[borrow];	 // 빌려올 형제로 채운다.
		node->child[pos]->num_key++;

		node->child[pos - 1]->num_key--;								 // 가져온 곳의 키갯수를 지운다.

		int successor = findSuccessor(node->child[pos]);				// successor 로 나를 채운다.
		node->key[pos - 1] = successor;									
	}
	else {															 // 내부에서 borrow가 일어난다면
		int target = 0;													 // 부족한 위치에 들어가야할 idx
		for (int i = 0; i < node->child[pos]->num_key; i++) {
			node->child[pos]->key[i + 1] = node->child[pos]->key[i];
		}
		node->child[pos]->key[target] = node->key[pos - 1];				 // 내 꺼를 내준다.
		node->child[pos]->num_key++;

		int borrow = node->child[pos - 1]->num_key - 1;					 // 가져올 위치의 idx
		node->key[pos - 1] = node->child[pos - 1]->key[borrow];			 // 내 꺼를 채운다.

		node->child[pos - 1]->num_key--;								 // 가져온 곳의 키갯수를 지운다.
		
		if (node->child[pos - 1]->num_child > 0) {						 // 빌려온 곳의 child가 있다면
			borrow = node->child[pos - 1]->num_child - 1;			     // 빌려온 곳의 child를 넘겨준다.
			for (int i = node->child[pos]->num_child; i > 0; i--) {
				node->child[pos]->child[i] = node->child[pos]->child[i - 1];
			}
			node->child[pos]->child[0] = node->child[pos - 1]->child[borrow];
			node->child[pos]->num_child++;

			node->child[pos - 1]->num_child--;							 // 빌려온 곳의 child 정리
		}
	}

}
void borrowFromRight(struct Node* node, int pos) {
	if (node->child[pos]->leaf) {									 // 리프에서 borrow가 일어난다면
		int target = node->child[pos]->num_key;					 // 부족한 위치에 들어가야할 idx
		int borrow = 0;											 // 가져올 위치의 idx

		node->child[pos]->key[target] = node->child[pos+1]->key[borrow];	// 빌려올 형제로 채운다.
		node->child[pos]->num_key++;

		for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {	   // 빌려온 곳의 키 정리
			node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
		}
		node->child[pos + 1]->num_key--;

		int successor = findSuccessor(node->child[pos+1]);				// successor 로 나를 채운다.
		node->key[pos] = successor;									
	}
	else {															 // 내부에서 borrow가 일어난다면
		int target = node->child[pos]->num_key;							 // 부족한 위치에 들어가야할 idx
		node->child[pos]->key[target] = node->key[pos];				     // 내 꺼를 내준다.
		node->child[pos]->num_key++;

		int borrow = 0;												     // 가져올 위치의 idx(맨왼쪽)
		node->key[pos] = node->child[pos + 1]->key[borrow];				 // 내 꺼를 채운다.

		for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {	 // 빌려온 곳의 키 정리
			node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
		}
		node->child[pos + 1]->num_key--;

		if (node->child[pos + 1]->num_child > 0) {
			target = node->child[pos]->num_child;							 // 빌려온 곳의 child를 넘겨준다.
			node->child[pos]->child[target] = node->child[pos + 1]->child[borrow];
			node->child[pos]->num_child++;

			for (int i = 0; i < node->child[pos + 1]->num_child - 1; i++) {	 // 빌려온 곳의 child 정리
				node->child[pos + 1]->child[i] = node->child[pos + 1]->child[i + 1];
			}
			node->child[pos + 1]->num_child--;
		}
	}
}

void mergeNode(struct Node* node, int pos, int pos_left) {		// 노드 키의 poe_left 값을 내려준다. TODO:: 왜 오른쪽 기준에도 문제없이 돌아가는가?
	if (node->child[pos]->leaf) {								     	 // 리프에서 merge가 일어난다면
		int target = node->child[pos_left]->num_key;

		for (int i = 0; i < node->child[pos]->num_key; i++) {			// 오른쪽자식의 키를 왼쪽 자식에 넣어준다.
			node->child[pos_left]->key[target + i] = node->child[pos]->key[i];
			node->child[pos_left]->num_key++;
		}

		node->child[pos_left]->next = node->child[pos]->next;
		node->child[pos_left]->end_leaf = node->child[pos]->end_leaf;

		for (int i = pos_left; i < node->num_key - 1; i++) {			// 내 노드의 key 정리 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = pos; i < node->num_child - 1; i++) {				// 내 노드의 child 정리 
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;

	}
	else {																// 노드에서 merge가 일어난다면
		int target = node->child[pos_left]->num_key;					// 왼쪽 자식에 들어갈 idx

		node->child[pos_left]->key[target] = node->key[pos_left];		// 내 노드값을 넣어준다.
		node->child[pos_left]->num_key++;

		for (int i = 0; i < node->child[pos]->num_key; i++) {			// 오른쪽자식의 키를 왼쪽 자식에 넣어준다.
			node->child[pos_left]->key[target + 1 + i] = node->child[pos]->key[i];
			node->child[pos_left]->num_key++;
		}

		target = node->child[pos_left]->num_child;
		for (int i = 0; i < node->child[pos]->num_child; i++) {			// 오른쪽자식의 자식을 왼쪽 자식에 넣어준다.
			node->child[pos_left]->child[target + i] = node->child[pos]->child[i];
			node->child[pos_left]->num_child++;
		}

		free(node->child[pos]);											// 오른쪽 자식을 free 시킨다.

		for (int i = pos_left; i < node->num_key - 1; i++) {			// 내 노드의 key 정리 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = pos; i < node->num_child - 1; i++) {				// 내 노드의 child 정리 
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;
	}
}

void adjustNode(struct Node* node, int pos) {
	if (pos == 0) {											// child 가 왼쪽 끝일때, 오른쪽 형제에서만 빌려올 수 있다.
		if (node->child[pos + 1]->num_key > MIN_Keys) {
			borrowFromRight(node, pos);
		}
		else {
			mergeNode(node, pos + 1, pos);
		}
		return;
	}
	else {
		if (pos == node->num_key) {										// child 가 오른쪽 끝일때, 왼쪽 형제에서만 빌려올 수 있다.
			if (node->child[pos - 1]->num_key > MIN_Keys) {
				borrowFromLeft(node, pos);
			}
			else {
				mergeNode(node, pos, pos - 1);
			}
			return;
		}
		else {												// 그 외 상황에서는, 양쪽에서 빌려올 수 있다.
			if (node->child[pos - 1]->num_key > MIN_Keys) {
				borrowFromLeft(node, pos);
			}
			else if (node->child[pos + 1]->num_key > MIN_Keys) {
				borrowFromRight(node, pos);
			}
			else {
				mergeNode(node, pos, pos - 1);
			}
			return;
		}

	}
}

int findSuccessor(struct Node* node) {
	int successor;
	if (node->leaf) {
		return node->key[0];
	}
	return findSuccessor(node->child[0]);
}

void deleteInnerTree(struct Node* node, int pos) { 
	int sucessor = findSuccessor(node->child[pos + 1]);
	node->key[pos] = sucessor;
}


int deleteValFromNode(int val, struct Node* node) {
	int pos;
	int flag = false;
	for (pos = 0; pos < node->num_key; pos++) {				// 이 노드에서 val, 혹은 들어갈 위치를 찾는다.
		if (val == node->key[pos]) {
			flag = true;
			break;
		}
		if (val < node->key[pos]) {
			break;
		}
	}
	if (flag) {
		if (node->leaf) {									// case#1 leaf에서 삭제될 때
			for (int i = pos; i < node->num_key; i++) {
				node->key[i] = node->key[i + 1];
			}
			node->num_key--;
		}
		else {
			flag = deleteValFromNode(val, node->child[pos+1]); // 내부에서 발견했을 때는 다음 child로 넘어간다.
			if (node->child[pos + 1]->num_key < MIN_Keys) {    // 재귀로 나왔을 때 삭제했던 자식이 갯수가 모자를 때 
				adjustNode(node, pos+1);
				deleteValFromNode(val, node);
			}
			else {
				deleteInnerTree(node, pos);
			}
		}

		return flag;
	}
	else {
		if (node->leaf) { 
			return flag;
		}
		else {
			flag = deleteValFromNode(val, node->child[pos]);

		}
	}
	if (node->child[pos]->num_key < MIN_Keys) {    // 재귀로 나왔을 때 삭제했던 자식이 갯수가 모자를 때 
		adjustNode(node, pos);
	}

	return flag;
}

void delete(struct Node* node, int val) {
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return;
	}
	int flag = deleteValFromNode(val, node);
	if (!flag) { 											// 삭제 할 노드가 없을 때
		printf("%d no exist in the tree!!\n", val);
		return;
	}

	if (node->num_key == 0) {								// case#3 높이의 변화가 있을때
		node = node->child[0];
	}
	root = node;

}


/*PRINT******************************************************************************************************/
void printTree(struct Node* node, int level) {				 // 트리 그리기
	if (!node) { 											 // empty tree!
		printf("Empty tree!!\n");
		return;
	}
	printf("Level %d :   ", level);
	for (int i = 0; i < level - 1; i++) {
		printf("            ");
	}
	for (int i = 0; i < node->num_key; i++) {
		printf("%d ", node->key[i]);
	}
	printf("\n");
	level++;
	for (int i = 0; i < node->num_child; i++) {
		printTree(node->child[i], level);
	}
}

void printLeaves(struct Node* node) {			 // leaf 그리기
	if (!node) { 											 // empty tree!
		printf("Empty tree!!\n");
		return;
	}
	if (node->leaf) {
		for (int i = 0; i < node->num_key; i++) {
			printf("%d ", node->key[i]);
		}
		printf("| ");
		if (!node->end_leaf) {
			printLeaves(node->next);
		}
		else {
			printf("\n");
		}
	}
	else {
		printLeaves(node->child[0]);
	}
}

int main(void) {
	//insert(1);
	//insert(4);
	//insert(7);
	//insert(10);
	//insert(17);
	//insert(21);
	//insert(31);
	//insert(25);
	//insert(19);
	//insert(20);
	//insert(28);
	//insert(42);

	//delete(root, 21);
	//delete(root, 31);
	//delete(root, 20);
	//delete(root, 10);
	//delete(root, 7);
	//delete(root, 25);
	//delete(root, 42);
	//delete(root, 4);

	for (int i = 10; i < 100; i += 10) {
		insert(i);
	}





	printTree(root, 1);
	printLeaves(root);
	printf("----------------------------------------------------------------------------------------------------\n");
	delete(root, 50);

	printTree(root, 1);
	printLeaves(root);

	searchNode(root, 20);
	searchNode(root, 120);

}