//// Btree study
//// 2021
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Node_Order		4
#define Node_Childs		Node_Order
#define Node_Keys		Node_Childs-1

struct BTreeNode {							// 비트리의 노드 구조체
	bool leaf;
	int key[Node_Keys + 1];					// 키 데이터
	int num_key;							// 키 갯수
	struct BTreeNode* child[Node_Childs + 1]; // 자식의 주소를 저장할 포인터
	int num_child;							// 자식 개수
};

struct BTreeNode* root; // 루트 포인터

// create Node
struct BTreeNode* createNode(int val) {
	struct BTreeNode* newNode;
	newNode = (struct BTreeNode*)malloc(sizeof(struct BTreeNode)); // B트리 구조체만큼 동적할당
	newNode->leaf = false;
	newNode->key[0] = val;
	newNode->num_key = 1;
	newNode->num_child = 0;
	return newNode;
}

// SPLIT Node
struct BTreeNode* splitNode(int pos, struct BTreeNode* node, struct BTreeNode* parent) {
	int median;
	if (Node_Order % 2 == 0) {								// 짝수 모드
		median = node->num_key / 2 - 0.5;							 
	}
	else {													// 짝수 모드
		median = node->num_key / 2;
	}
	struct BTreeNode* child;

	child = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
	child->leaf = node->leaf;
	child->num_key = 0;
	child->num_child = 0;

	if (!node->leaf) {										 // leaf가 아닌 경우는 child를 넘겨줘야함
		for (int i = median + 1; i <= node->num_key; i++) {
			child->child[i - median - 1] = node->child[i];
			child->num_child++;
			node->num_child--;
		}
	}
	for (int i = median + 1; i <= node->num_key; i++) {		 // child right에 키 담아주기 기존 노드는 child left가 된다
		child->key[i - median - 1] = node->key[i];
		child->num_key++;
		node->num_key--;
	}


	if (node==root) {										 // 나눌 노드가 루트라면
		struct BTreeNode* new_parent;						 // 새로운 parent에는 median 키를 넣어준다.
		new_parent = createNode(node->key[median]);
		node->num_key--;									 // median 보냈으므로 -1
		new_parent->child[0] = node;
		new_parent->child[1] = child;
		new_parent->num_child = 2;
		parent = new_parent;
		return parent;
	}
	else {													 // 나눌 노드가 루트가 아니라면
		for (int i = node->num_key; i > pos; i--) {
			parent->key[i] = parent->key[i - 1];
			parent->child[i + 1] = parent->child[i];		 // TODO:: 잘들어가는지 확인
		}
		parent->key[pos] = node->key[median];				 // 부모 노드에 key를 추가해준다.
		parent->num_key++;
		node->num_key--;									 // median 보냈으므로 -1
		parent->child[pos+1] = child;						 // 원래 노드에 child left를 추가해준다.
		//parent->child[parent->num_child + 1] = child;			 // 원래 노드에 child right를 추가해준다.
		parent->num_child += 1;
	}
	return node;
}


struct BTreeNode* insertNode(int parent_pos, int val, struct BTreeNode* node, struct BTreeNode* parent) {
	int pos;												// pos는 삽입 될 포지션
	for (pos = 0; pos < node->num_key; pos++) {
		if (val == node->key[pos]) {
			printf("Duplicates are not permitted\n");		// 중복된 키는 금지
			return;
		}
		else if (val < node->key[pos]) {					// val이 들어갈 위치를 찾는다.
			break;
		}
	}
	if (!node->leaf) {										// leaf가 아닐 경우에는 child로 내려간다.
		node->child[pos] = insertNode(pos, val, node->child[pos], node);
		if (node->num_key == Node_Keys + 1) {				// 아래 삽입에 의해 키가 찼다면, 또 split을 해준다
			node = splitNode(parent_pos, node, parent);
		}
	}
	else {													// leaf에만 삽입한다.
		for (int i = node->num_key; i > pos; i--) {
			node->key[i] = node->key[i - 1];
			node->child[i + 1] = node->child[i];			// TODO:: 잘들어가는지 확인
		}
		node->key[pos] = val;
		node->num_key++;
		if (node->num_key == Node_Keys + 1) {				// 키가 꽉찼다면? split을 해준다!
			node = splitNode(parent_pos, node, parent);
		}
	}

	return node;											// 노드 반환
}

void insert(int val) {
	if (!root) { 											// root 가 없다면 root를 만든다.
		root = createNode(val);
		root->leaf = true;
		return;
	}
	//struct BTreeNode* temp;								// 부모 포인터 (temp)
	//temp = root;
	root = insertNode(0, val, root, root);					// root 가 있다면 노드를 찾아 삽입한다.
}

void printTree(struct BTreeNode* node, int level) {			 // 트리 그리기
	printf("Level %d :", level);
	for (int i = 0; i < node->num_key; i++) {
		printf("%d ", node->key[i]);
	}
	printf("\n");
	level++;
	for (int i = 0; i < node->num_child; i++) {
		printTree(node->child[i], level);
	}
}


int main(void) {

	//insert(10);
	//insert(8);
	//insert(9);
	//insert(11);
	//insert(15);
	//insert(20);
	//insert(17);
	//insert(18);
	//insert(23);


	printTree(root, 1);

	return 0;
}