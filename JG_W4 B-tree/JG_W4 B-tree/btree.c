//// Btree study
//// 2021
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//
//#define Node_Order			5
//#define Node_Childs			Node_Order
//#define Node_Keys			Node_Childs-1
//#define Num_Minimum_Keys	Node_Order/2
//
//
//struct BTreeNode {							// 비트리의 노드 구조체
//	bool leaf;
//	int key[Node_Keys + 1];					// 키 데이터
//	int num_key;							// 키 갯수
//	struct BTreeNode* child[Node_Childs + 1]; // 자식의 주소를 저장할 포인터
//	int num_child;							// 자식 개수
//};
//
//struct BTreeNode* root; // 루트 포인터
///*SEARCH******************************************************************************************************/
//int searchNode(struct BTreeNode* node, int val) {			// TO DO:: 이진탐색
//	if (!node) { 											// empty tree!
//		printf("Empty tree!!\n");
//		return 0;
//	}
//	struct BTreeNode* level = node;							// root부터 leaf까지 탐색
//	while (1) {
//		int pos;
//		for (pos = 0; pos < level->num_key; pos++) {
//			if (val == level->key[pos]) {					// 찾으면 리턴 1
//				printf("%d exist!!\n", val);
//				return 1;
//			}
//			else if (val < level->key[pos]) {
//				break;
//			}
//		}
//		if (level->leaf) break;
//		level = level->child[pos];
//	}
//	printf("%d not exist!!\n", val);							// leaf 까지와서도 못찾으면 리턴 0
//	return 0;
//}
//
///*INSERT******************************************************************************************************/
// //Create Node
//struct BTreeNode* createNode(int val) {
//	struct BTreeNode* newNode;
//	newNode = (struct BTreeNode*)malloc(sizeof(struct BTreeNode)); // B트리 구조체만큼 동적할당
//
//	newNode->leaf = false;
//	newNode->key[0] = val;
//	newNode->num_key = 1;
//	newNode->num_child = 0;
//	return newNode;
//}
//
////Split Node
//struct BTreeNode* splitNode(int pos, struct BTreeNode* node, struct BTreeNode* parent) {
//	int median;
//	if (Node_Order % 2 == 0) {								// 짝수 모드
//		median = node->num_key / 2 - 0.5;
//	}
//	else {													// 홀수 모드
//		median = node->num_key / 2;
//	}
//	struct BTreeNode* child;
//
//	child = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
//	child->leaf = node->leaf;
//	child->num_key = 0;
//	child->num_child = 0;
//
//	if (!node->leaf) {										 // leaf가 아닌 경우는 child를 넘겨줘야함
//		int num_iter = node->num_child;
//		for (int i = median + 1; i < num_iter; i++) {
//			child->child[i - median - 1] = node->child[i];
//			child->num_child++;
//			node->num_child--;
//		}
//	}
//	int num_iter = node->num_key;
//	for (int i = median + 1; i < num_iter; i++) {		 // child right에 키 담아주기 기존 노드는 child left가 된다
//		child->key[i - median - 1] = node->key[i];
//		child->num_key++;
//		node->num_key--;
//	}
//
//
//	if (node == root) {										 // 나눌 노드가 루트라면
//		struct BTreeNode* new_parent;						 // 새로운 parent에는 median 키를 넣어준다.
//		new_parent = createNode(node->key[median]);
//		node->num_key--;									 // median 보냈으므로 -1
//		new_parent->child[0] = node;
//		new_parent->child[1] = child;
//		new_parent->num_child = 2;
//		parent = new_parent;
//		return parent;
//	}
//	else {													 // 나눌 노드가 루트가 아니라면
//		for (int i = parent->num_key; i > pos; i--) {
//			parent->key[i] = parent->key[i - 1];
//			parent->child[i + 1] = parent->child[i];		 // TODO:: 잘들어가는지 확인
//		}
//		parent->key[pos] = node->key[median];				 // 부모 노드에 key를 추가해준다.
//		parent->num_key++;
//		node->num_key--;									 // median 보냈으므로 -1
//		parent->child[pos + 1] = child;						 // 부모 노드에 새 child를 추가해준다.
//		parent->num_child += 1;
//	}
//	return node;
//}
//
////Insert Node
//struct BTreeNode* insertNode(int parent_pos, int val, struct BTreeNode* node, struct BTreeNode* parent) {
//	int pos;												// pos는 삽입 될 포지션
//	for (pos = 0; pos < node->num_key; pos++) {
//		if (val == node->key[pos]) {
//			printf("Duplicates are not permitted!!\n");		// 중복된 키는 금지
//			return node;
//		}
//		else if (val < node->key[pos]) {					// val이 들어갈 위치를 찾는다.
//			break;
//		}
//	}
//	if (!node->leaf) {										// leaf가 아닐 경우에는 child로 내려간다.
//		node->child[pos] = insertNode(pos, val, node->child[pos], node);
//		if (node->num_key == Node_Keys + 1) {				// 아래 삽입에 의해 키가 찼다면, 또 split을 해준다
//			node = splitNode(parent_pos, node, parent);
//		}
//	}
//	else {													// leaf에만 삽입한다.
//		for (int i = node->num_key; i > pos; i--) {
//			node->key[i] = node->key[i - 1];
//			node->child[i + 1] = node->child[i];			// TODO:: 잘들어가는지 확인
//		}
//		node->key[pos] = val;
//		node->num_key++;
//		if (node->num_key == Node_Keys + 1) {				// 키가 꽉찼다면? split을 해준다!
//			node = splitNode(parent_pos, node, parent);
//		}
//	}
//
//	return node;											// 노드 반환
//}
//
//void insert(int val) {
//	if (!root) { 											// root 가 없다면 root를 만든다.
//		root = createNode(val);
//		root->leaf = true;
//		return;
//	}
//
//	root = insertNode(0, val, root, root);					// root 가 있다면 노드를 찾아 삽입한다.
//}
///*DELETE*****************************************************************************************************/
//
//int findPredecessor(struct BTreeNode* node) {
//	int predecessor;
//	if (node->leaf) {
//		return node->key[node->num_key - 1];
//	}
//	//if (node->num_key < Num_Minimum_Keys) {
//	//	return node->key[node->num_key - 1];
//	//}
//	return findPredecessor(node->child[node->num_child - 1]);
//}
//
//int findSuccessor(struct BTreeNode* node) {
//	int successor;
//	if (node->leaf) {
//		return node->key[0];
//	}
//	//if (node->num_key > Num_Minimum_Keys) {
//	//	return node->key[0];
//	//}
//	return findSuccessor(node->child[0]);
//}
//
//int inorderPredecessor(struct BTreeNode* node, int pos) {
//	//int predecessor = node->child[pos]->num_key - 1;
//	int predecessor = findPredecessor(node->child[pos]);
//	node->key[pos] = predecessor;
//	return predecessor;
//}
//
//int inorderSuccessor(struct BTreeNode* node, int pos) {
//	//int successor = 0;
//	int successor = findSuccessor(node->child[pos + 1]);
//	node->key[pos] = successor;
//	return successor;
//}
//
//int inorderMerge(struct BTreeNode* node, int pos) {
//	int target = node->child[pos]->num_key;					// 합칠 위치에 들어가야할 idx
//	int send = node->key[pos];
//	node->child[pos]->key[target] = node->key[pos];			// 왼쪽 child에 지워질 값 붙여넣기
//	node->child[pos]->num_key++;
//
//	for (int i = 0; i < node->child[pos + 1]->num_key; i++) {   // 오른쪽 child의 키 왼쪽에 붙여넣기
//		node->child[pos]->key[target + 1 + i] = node->child[pos + 1]->key[i];
//		node->child[pos]->num_key++;
//	}
//	for (int i = 0; i < node->child[pos + 1]->num_child; i++) { // 오른쪽 child의 child 왼쪽에 붙여넣기
//		node->child[pos]->child[target + 1 + i] = node->child[pos + 1]->child[i];
//		node->child[pos]->num_child++;
//	}
//
//	for (int i = pos; i < node->num_key; i++) {				// 내 노드 키값 정리
//		node->key[i] = node->key[i + 1];
//		node->num_key--;
//	}
//	for (int i = pos + 1; i < node->num_child; i++) {			// 내 노드 child값 정리
//		node->child[i] = node->child[i + 1];
//		node->num_child--;
//	}
//	return send;
//}
//
//void deleteInnerTree(struct BTreeNode* node, int pos) { // TO DO:: 자식이 없을경우는 어떻게 되는거지 ? 
//	int result_deletion = 0; // TO DO:: CHECK
//	if (node->child[pos]->num_key >= node->child[pos + 1]->num_key) { // 왼쪽 자식의 키 갯수가 크거나 같을때
//		if (node->child[pos]->num_key > Num_Minimum_Keys) {
//			result_deletion = inorderPredecessor(node, pos);
//			deleteValFromNode(result_deletion, node->child[pos]);
//
//		}
//		else {
//			result_deletion = inorderMerge(node, pos);
//			deleteValFromNode(result_deletion, node->child[pos]);
//		}
//	}
//	else {															  // 오른쪽 자식의 키 갯수가 클 때
//		if (node->child[pos + 1]->num_key > Num_Minimum_Keys) {
//			result_deletion = inorderSuccessor(node, pos);
//			deleteValFromNode(result_deletion, node->child[pos + 1]);
//		}
//		else {
//			result_deletion = inorderMerge(node, pos);
//			deleteValFromNode(result_deletion, node->child[pos]);
//		}
//	}
//}
//
//void borrowFromLeft(struct BTreeNode* node, int pos) {
//	int target = 0;													 // 부족한 위치에 들어가야할 idx
//	for (int i = 0; i < node->child[pos]->num_key; i++) {
//		node->child[pos]->key[i + 1] = node->child[pos]->key[i];
//	}
//	node->child[pos]->key[target] = node->key[pos - 1];				 // 내 꺼를 내준다.
//	node->child[pos]->num_key++;
//
//	int borrow = node->child[pos - 1]->num_key - 1;						 // 가져올 위치의 idx
//	node->key[pos - 1] = node->child[pos - 1]->key[borrow];			 // 내 꺼를 채운다.
//
//	node->child[pos - 1]->num_key--;								 // 가져온 곳의 키갯수를 지운다.
//
//
//	if (node->child[pos - 1]->num_child > 0) {						 // 빌려온 곳의 child가 있다면
//		borrow = node->child[pos - 1]->num_child - 1;			     // 빌려온 곳의 child를 넘겨준다.
//		for (int i = node->child[pos]->num_child; i > 0; i--) {
//			node->child[pos]->child[i] = node->child[pos]->child[i - 1];
//		}
//		node->child[pos]->child[0] = node->child[pos - 1]->child[borrow];
//		node->child[pos]->num_child++;
//
//		node->child[pos - 1]->num_child--;							 // 빌려온 곳의 child 정리
//	}
//}
//
//void borrowFromRight(struct BTreeNode* node, int pos) {
//	int target = node->child[pos]->num_key;							 // 부족한 위치에 들어가야할 idx
//	node->child[pos]->key[target] = node->key[pos];				     // 내 꺼를 내준다.
//	node->child[pos]->num_key++;
//
//	int borrow = 0;												     // 가져올 위치의 idx(맨왼쪽)
//	node->key[pos] = node->child[pos + 1]->key[borrow];				 // 내 꺼를 채운다.
//
//	for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {	 // 빌려온 곳의 키 정리
//		node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
//	}
//	node->child[pos + 1]->num_key--;
//
//	if (node->child[pos + 1]->num_child > 0) {
//		target = node->child[pos]->num_child;							 // 빌려온 곳의 child를 넘겨준다.
//		node->child[pos]->child[target] = node->child[pos + 1]->child[borrow];
//		node->child[pos]->num_child++;
//
//		for (int i = 0; i < node->child[pos + 1]->num_child - 1; i++) {	 // 빌려온 곳의 child 정리
//			node->child[pos + 1]->child[i] = node->child[pos + 1]->child[i + 1];
//		}
//		node->child[pos + 1]->num_child--;
//	}
//}
//
//void mergeNode(struct BTreeNode* node, int pos, int pos_left) {		// 노드 키의 poe_left 값을 내려준다. TODO:: 왜 오른쪽 기준에도 문제없이 돌아가는가?
//	int target = node->child[pos_left]->num_key;					// 왼쪽 자식에 들어갈 idx
//
//	node->child[pos_left]->key[target] = node->key[pos_left];		// 내 노드값을 넣어준다.
//	node->child[pos_left]->num_key++;
//
//	for (int i = 0; i < node->child[pos]->num_key; i++) {			// 오른쪽자식의 키를 왼쪽 자식에 넣어준다.
//		node->child[pos_left]->key[target + 1 + i] = node->child[pos]->key[i];
//		node->child[pos_left]->num_key++;
//	}
//
//	target = node->child[pos_left]->num_child;
//	for (int i = 0; i < node->child[pos]->num_child; i++) {			// 오른쪽자식의 키를 왼쪽 자식에 넣어준다.
//		node->child[pos_left]->child[target + i] = node->child[pos]->child[i];
//		node->child[pos_left]->num_child++;
//	}
//
//	free(node->child[pos]);											// 오른쪽 자식을 free 시킨다.
//
//	for (int i = pos_left; i < node->num_key - 1; i++) {			// 내 노드의 key 정리 
//		node->key[i] = node->key[i + 1];
//	}
//	node->num_key--;
//
//	for (int i = pos; i < node->num_child - 1; i++) {				// 내 노드의 child 정리 
//		node->child[i] = node->child[i + 1];
//	}
//	node->num_child--;
//}
//
//
//void adjustNode(struct BTreeNode* node, int pos) {
//	if (pos == 0) {											// child 가 왼쪽 끝일때, 오른쪽 형제에서만 빌려올 수 있다.
//		if (node->child[pos + 1]->num_key > Num_Minimum_Keys) {
//			borrowFromRight(node, pos);
//		}
//		else {
//			mergeNode(node, pos + 1, pos);
//		}
//		return;
//	}
//	else {
//		if (pos == node->num_key) {										// child 가 오른쪽 끝일때, 왼쪽 형제에서만 빌려올 수 있다.
//			if (node->child[pos - 1]->num_key > Num_Minimum_Keys) {
//				borrowFromLeft(node, pos);
//			}
//			else {
//				mergeNode(node, pos, pos - 1);
//			}
//			return;
//		}
//		else {												// 그 외 상황에서는, 양쪽에서 빌려올 수 있다.
//			if (node->child[pos - 1]->num_key > Num_Minimum_Keys) {
//				borrowFromLeft(node, pos);
//			}
//			else if (node->child[pos + 1]->num_key > Num_Minimum_Keys) {
//				borrowFromRight(node, pos);
//			}
//			else {
//				mergeNode(node, pos, pos - 1);
//			}
//			return;
//		}
//
//	}
//}
//
//int deleteValFromNode(int val, struct BTreeNode* node) {
//	int pos;
//	int flag = false;
//	for (pos = 0; pos < node->num_key; pos++) {				// 이 노드에서 val, 혹은 들어갈 위치를 찾는다.
//		if (val == node->key[pos]) {
//			flag = true;
//			break;
//		}
//		else if (val < node->key[pos]) {
//			break;
//		}
//	}
//	if (flag) {
//		if (node->leaf) {									// case#1 leaf에서 삭제될 때
//			for (int i = pos; i < node->num_key; i++) {
//				node->key[i] = node->key[i + 1];
//			}
//			node->num_key--;
//		}
//		else {
//			deleteInnerTree(node, pos);
//		}
//		return flag;
//	}
//	else {
//		if (node->leaf) {
//			return flag;
//		}
//		else {
//			flag = deleteValFromNode(val, node->child[pos]);
//
//		}
//	}
//	if (node->child[pos]->num_key < Num_Minimum_Keys) {    // 재귀로 나왔을 때 삭제했던 자식이 갯수가 모자를 때 
//		adjustNode(node, pos);
//	}
//
//	return flag;
//}
//
//void delete(struct BTreeNode* node, int val) {
//	if (!node) { 											// empty tree!
//		printf("Empty tree!!\n");
//		return;
//	}
//	int flag = deleteValFromNode(val, node);
//	if (!flag) { 											// 삭제 할 노드가 없을 때
//		printf("%d no exist in the tree!!\n", val);
//		return;
//	}
//	if (node->num_key == 0) {								// case#3 높이의 변화가 있을때
//		node = node->child[0];
//	}
//	root = node;
//
//}
//
///*PRINT******************************************************************************************************/
//void printTree(struct BTreeNode* node, int level) {			 // B트리 그리기
//	if (!node) { 											 // empty tree!
//		printf("Empty tree!!\n");
//		return;
//	}
//	printf("Level %d :   ", level);
//	for (int i = 0; i < level - 1; i++) {
//		printf("            ");
//	}
//	for (int i = 0; i < node->num_key; i++) {
//		printf("%d ", node->key[i]);
//	}
//	printf("\n");
//	level++;
//	for (int i = 0; i < node->num_child; i++) {
//		printTree(node->child[i], level);
//	}
//}
//
/////*MAIN******************************************************************************************************/
//int main(void) {
//
//	insert(10);
//	insert(20);
//	insert(30);
//	insert(40);
//	insert(50);
//	insert(60);
//	insert(70);
//	insert(80);
//	insert(90);
//	insert(100);
//	insert(110);
//	insert(120);
//	insert(130);
//	insert(140);
//	insert(150);
//	insert(160);
//	insert(170);
//	insert(180);
//	insert(190);
//	insert(200);
//	insert(210);
//	insert(220);
//	insert(230);
//	insert(240);
//	insert(250);
//	insert(260);
//
//	insert(9);
//	insert(39);
//	insert(101);
//	insert(102);
//	insert(103);
//	insert(104);
//	insert(161);
//	insert(191);
//	insert(251);
//
//	printTree(root, 1);
//	printf("****************************************************\n");
//	delete(root, 103);
//	delete(root, 70);
//	delete(root, 130);
//	delete(root, 104);
//	delete(root, 60);
//	delete(root, 120);
//	delete(root, 240);
//	delete(root, 160);
//
//
//
//
//	printTree(root, 1);
//
//	//searchNode(root,30);
//
//
//	return 0;
//}