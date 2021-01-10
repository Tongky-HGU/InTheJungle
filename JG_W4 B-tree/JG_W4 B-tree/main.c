// Btree study
// 2021

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Node_Order			3
#define Node_Childs			Node_Order
#define Node_Keys			Node_Childs-1
#define Num_Minimum_Keys	Node_Order/2


struct BTreeNode {							// ��Ʈ���� ��� ����ü
	bool leaf;
	int key[Node_Keys + 1];					// Ű ������
	int num_key;							// Ű ����
	struct BTreeNode* child[Node_Childs + 1]; // �ڽ��� �ּҸ� ������ ������
	int num_child;							// �ڽ� ����
};

struct BTreeNode* root; // ��Ʈ ������
/*SEARCH******************************************************************************************************/
int searchNode(struct BTreeNode* node, int val) {			// TO DO:: ����Ž��
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return 0;
	}
	struct BTreeNode* level = node;							// root���� leaf���� Ž��
	while (1) {
		int pos;
		for (pos = 0; pos < level->num_key; pos++) {		
			if (val == level->key[pos]) {					// ã���� ���� 1
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
	printf("%d not exist!!\n", val);							// leaf �����ͼ��� ��ã���� ���� 0
	return 0;
}

/*INSERT******************************************************************************************************/
 //Create Node
struct BTreeNode* createNode(int val) {
	struct BTreeNode* newNode;
	newNode = (struct BTreeNode*)malloc(sizeof(struct BTreeNode)); // BƮ�� ����ü��ŭ �����Ҵ�
	
	newNode->leaf = false;
	newNode->key[0] = val;
	newNode->num_key = 1;
	newNode->num_child = 0;
	return newNode;
}

 //Split Node
struct BTreeNode* splitNode(int pos, struct BTreeNode* node, struct BTreeNode* parent) {
	int median;
	if (Node_Order % 2 == 0) {								// ¦�� ���
		median = node->num_key / 2 - 0.5;							 
	}
	else {													// Ȧ�� ���
		median = node->num_key / 2;
	}
	struct BTreeNode* child;

	child = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
	child->leaf = node->leaf;
	child->num_key = 0;
	child->num_child = 0;

	if (!node->leaf) {										 // leaf�� �ƴ� ���� child�� �Ѱ������
		for (int i = median + 1; i <= node->num_key; i++) {
			child->child[i - median - 1] = node->child[i];
			child->num_child++;
			node->num_child--;
		}
	}
	for (int i = median + 1; i <= node->num_key; i++) {		 // child right�� Ű ����ֱ� ���� ���� child left�� �ȴ�
		child->key[i - median - 1] = node->key[i];
		child->num_key++;
		node->num_key--;
	}


	if (node==root) {										 // ���� ��尡 ��Ʈ���
		struct BTreeNode* new_parent;						 // ���ο� parent���� median Ű�� �־��ش�.
		new_parent = createNode(node->key[median]);
		node->num_key--;									 // median �������Ƿ� -1
		new_parent->child[0] = node;
		new_parent->child[1] = child;
		new_parent->num_child = 2;
		parent = new_parent;
		return parent;
	}
	else {													 // ���� ��尡 ��Ʈ�� �ƴ϶��
		for (int i = parent->num_key; i > pos; i--) {
			parent->key[i] = parent->key[i - 1];
			parent->child[i + 1] = parent->child[i];		 // TODO:: �ߵ����� Ȯ��
		}
		parent->key[pos] = node->key[median];				 // �θ� ��忡 key�� �߰����ش�.
		parent->num_key++;
		node->num_key--;									 // median �������Ƿ� -1
		parent->child[pos+1] = child;						 // ���� ��忡 child left�� �߰����ش�.
		parent->num_child += 1;
	}
	return node;
}

//Insert Node
struct BTreeNode* insertNode(int parent_pos, int val, struct BTreeNode* node, struct BTreeNode* parent) {
	int pos;												// pos�� ���� �� ������
	for (pos = 0; pos < node->num_key; pos++) {
		if (val == node->key[pos]) {
			printf("Duplicates are not permitted!!\n");		// �ߺ��� Ű�� ����
			return node;
		}
		else if (val < node->key[pos]) {					// val�� �� ��ġ�� ã�´�.
			break;
		}
	}
	if (!node->leaf) {										// leaf�� �ƴ� ��쿡�� child�� ��������.
		node->child[pos] = insertNode(pos, val, node->child[pos], node);
		if (node->num_key == Node_Keys + 1) {				// �Ʒ� ���Կ� ���� Ű�� á�ٸ�, �� split�� ���ش�
			node = splitNode(parent_pos, node, parent);
		}
	}
	else {													// leaf���� �����Ѵ�.
		for (int i = node->num_key; i > pos; i--) {
			node->key[i] = node->key[i - 1];
			node->child[i + 1] = node->child[i];			// TODO:: �ߵ����� Ȯ��
		}
		node->key[pos] = val;
		node->num_key++;
		if (node->num_key == Node_Keys + 1) {				// Ű�� ��á�ٸ�? split�� ���ش�!
			node = splitNode(parent_pos, node, parent);
		}
	}

	return node;											// ��� ��ȯ
}

void insert(int val) {
	if (!root) { 											// root �� ���ٸ� root�� �����.
		root = createNode(val);
		root->leaf = true;
		return;
	}

	root = insertNode(0, val, root, root);					// root �� �ִٸ� ��带 ã�� �����Ѵ�.
}
/*DELETE*****************************************************************************************************/
//void delete(x,k)
//void delete_internal_node(x,k,i)
//void delete_predecessor(x)
//void delete_successor(x)
//void delete_merge(x)
//void delete_sibling(c,i,j)
//**********************************
//void deletion(int k);
//void removeFromLeaf(int idx);
//void removeFromNonLeaf(int idx);
//int getPredecessor(int idx);
//int getSuccessor(int idx);
//void fill(int idx);
//void borrowFromPrev(int idx);
//void borrowFromNext(int idx);
//void merge(int idx);
//**********************************
//void removeVal(struct BTreeNode* myNode, int pos)
//void rightShift(struct BTreeNode* myNode, int pos)
//void leftShift(struct BTreeNode* myNode, int pos)
//void mergeNodes(struct BTreeNode* myNode, int pos)
//void adjustNode(struct BTreeNode* myNode, int pos)
//int delValFromNode(int item, struct BTreeNode* myNode)
//void delete (int item, struct BTreeNode* myNode)

int inorderPredecessor(struct BTreeNode* node, int pos) {
	int predecessor = node->child[pos]->num_key - 1;
	node->key[pos] = node->child[pos]->key[predecessor];
	//node->child[pos]->num_key--;
	//node->child[pos]->key[predecessor] = 0; // �ʱ�ȭ
	return node->child[pos]->key[predecessor];
}

int inorderSuccessor(struct BTreeNode* node, int pos) {
	int successor = 0;
	node->key[pos] = node->child[pos+1]->key[successor];
	//for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {
	//	node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
	//}
	//node->child[pos+1]->num_key--;
	//node->child[pos+1]->key[node->child[pos + 1]->num_key] = 0; // �ʱ�ȭ
	return node->child[pos + 1]->key[successor];
}

int inorderMerge(struct BTreeNode* node, int pos) {
	int target = node->child[pos]->num_key;					// ��ĥ ��ġ�� ������ idx
	int send = node->key[pos];
	node->child[pos]->key[target] = node->key[pos];			// ���� child�� ������ �� �ٿ��ֱ�
	node->child[pos]->num_key++;

	for (int i = 0; i < node->child[pos + 1]->num_key; i++) {   // ������ child�� Ű ���ʿ� �ٿ��ֱ�
		node->child[pos]->key[target + 1 + i] = node->child[pos+1]->key[i];
		node->child[pos]->num_key++;
	}
	for (int i = 0; i < node->child[pos + 1]->num_child; i++) { // ������ child�� child ���ʿ� �ٿ��ֱ�
		node->child[pos]->child[target + 1 + i] = node->child[pos + 1]->child[i];
		node->child[pos]->num_child++;
	}

	for (int i = pos; i < node->num_key - 1; i++) {			// �� ��� Ű�� ����
		node->key[i] = node->key[i + 1];
		node->num_key--;
	}
	for (int i = pos + 1; i < node->num_child - 1; i++) {			// �� ��� child�� ����
		node->child[i] = node->child[i + 1];
		node->num_child--;
	}
	return send;
}

void deleteInnerTree(struct BTreeNode* node, int pos) { // TO DO:: �ڽ��� �������� ��� �Ǵ°��� ? 
	int result_deletion = 0; // TO DO:: CHECK
	if (node->child[pos]->num_key >= node->child[pos + 1]->num_key) { // ���� �ڽ��� Ű ������ ũ�ų� ������
		if (node->child[pos]->num_key > Num_Minimum_Keys) {
			result_deletion = inorderPredecessor(node, pos);
			deleteValFromNode(result_deletion, node->child[pos]);

		}
		else {
			result_deletion = inorderMerge(node, pos);
			deleteValFromNode(result_deletion, node->child[pos]);
		}
	}
	else {															  // ������ �ڽ��� Ű ������ Ŭ ��
		if (node->child[pos+1]->num_key > Num_Minimum_Keys) {
			result_deletion = inorderSuccessor(node, pos);
			deleteValFromNode(result_deletion, node->child[pos+1]);
		}
		else {
			result_deletion = inorderMerge(node, pos);
			deleteValFromNode(result_deletion, node->child[pos]);
		}
	}
}

void borrowFromLeft(struct BTreeNode* node, int pos) {
	int target = node->child[pos]->num_key;					// ������ ��ġ�� ������ idx
	node->child[pos]->key[target] = node->key[pos-1];			// �� ���� ���ش�.
	node->child[pos]->num_key++;						

	int borrow = node->child[pos-1]->num_key-1;				// ������ ��ġ�� idx
	node->key[pos-1] = node->child[pos - 1]->key[borrow];		// �� ���� ä���.

	node->child[pos - 1]->num_key--;						// ������ ���� Ű������ �����.
}

void borrowFromRight(struct BTreeNode* node, int pos) {
	int target = node->child[pos]->num_key;					// ������ ��ġ�� ������ idx
	node->child[pos]->key[target] = node->key[pos];			// �� ���� ���ش�.
	node->child[pos]->num_key++;

	int borrow = 0;											// ������ ��ġ�� idx(�ǿ���)
	node->key[pos] = node->child[pos + 1]->key[borrow];		// �� ���� ä���.

	for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {
		node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
	}
	node->child[pos + 1]->num_key--;						// ������ ���� Ű������ �����.
}

void mergeNode(struct BTreeNode* node, int pos, int deleted_pos) {			// pos =���� �ٲ� node�� child idx
	if (pos < deleted_pos) { //���ʰŸ� �պ��� ��
		int target = node->child[pos]->num_key;					// ���� �ڽĿ� �� idx
		node->child[pos]->key[target] = node->key[pos];			// �� ��尪�� �־��ش�.
		node->child[pos]->num_key++;

		for (int i = pos; i < node->num_key - 1; i++) {			// �� ����� key ���� 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = deleted_pos; i < node->num_child - 1; i++) {
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;
	}
	else { //�����ʰŸ� �պ��� ��
		int target = 0;					// ������ �ڽĿ� �� idx
		for (int i = 0; i < node->num_key; i++) {			// �� child key �̸� �б�
			node->child[pos]->key[i + 1] = node->child[pos]->key[i];
		}
		node->child[pos]->key[target] = node->key[deleted_pos];			// �� ��尪�� �־��ش�.
		node->child[pos]->num_key++;

		for (int i = deleted_pos; i < node->num_key - 1; i++) {			// �� ����� key ���� 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = deleted_pos; i < node->num_child - 1; i++) {
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;
	}
}


void adjustNode(struct BTreeNode* node, int pos) {
	if (pos == 0) {											// child �� ���� ���϶�, ������ ���������� ������ �� �ִ�.
		if (node->child[pos + 1]->num_key > Num_Minimum_Keys) {
			borrowFromRight(node, pos);
		}
		else {
			mergeNode(node, pos+1,pos);
		}
		return;
	}
	else {
		if (pos == node->num_key) {										// child �� ������ ���϶�, ���� ���������� ������ �� �ִ�.
			if (node->child[pos-1]->num_key > Num_Minimum_Keys) {
				borrowFromLeft(node, pos);
			}
			else {
				mergeNode(node, pos-1,pos);
			}
			return;
		}
		else {												// �� �� ��Ȳ������, ���ʿ��� ������ �� �ִ�.
			if (node->child[pos - 1]->num_key > Num_Minimum_Keys) {
				borrowFromLeft(node, pos);
			}
			else if (node->child[pos + 1]->num_key > Num_Minimum_Keys) {
				borrowFromRight(node, pos);
			}
			else {
				mergeNode(node, pos-1,pos);
			}
			return;
		}

	}

}

int deleteValFromNode(int val, struct BTreeNode* node) {
	int pos;
	int flag = false;
	for (pos = 0; pos < node->num_key; pos++) {				// �� ��忡�� val, Ȥ�� �� ��ġ�� ã�´�.
		if (val == node->key[pos]) {
			flag = true;
			break;
		}
		else if (val < node->key[pos]) {					
			break;
		}
	}
	if (flag) {
		if (node->leaf) {									// case#1 leaf���� ������ ��
			for (int i = pos; i < node->num_key; i++) {
				node->key[i] = node->key[i + 1];
			}
			node->num_key--;
		}
		else {
			deleteInnerTree(node, pos);
		}
		return flag;
	}
	else {
		flag = deleteValFromNode(val, node->child[pos]);
	}
	if (node->child[pos]) {
		if (node->child[pos]->num_key < Num_Minimum_Keys) {
			adjustNode(node, pos);
		}
	}

	return flag;
}

void delete(struct BTreeNode* node, int val) {
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return;
	}
	deleteValFromNode(val, node);
}

/*PRINT******************************************************************************************************/
void printTree(struct BTreeNode* node, int level) {			 // BƮ�� �׸���
	if (!node) { 											// empty tree!
		printf("Empty tree!!\n");
		return;
	}
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

/*MAIN******************************************************************************************************/
int main(void) {

	insert(5);
	insert(10);
	insert(15);
	insert(20);
	insert(30);
	insert(35);
	insert(70);


	printTree(root, 1);
	printf("*************************\n");
	delete(root, 10);
	printTree(root, 1);

	//searchNode(root,30);


	return 0;
}