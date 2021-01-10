//// Btree study
//// 2021
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Node_Order		4
#define Node_Childs		Node_Order
#define Node_Keys		Node_Childs-1

struct BTreeNode {							// ��Ʈ���� ��� ����ü
	bool leaf;
	int key[Node_Keys + 1];					// Ű ������
	int num_key;							// Ű ����
	struct BTreeNode* child[Node_Childs + 1]; // �ڽ��� �ּҸ� ������ ������
	int num_child;							// �ڽ� ����
};

struct BTreeNode* root; // ��Ʈ ������

// create Node
struct BTreeNode* createNode(int val) {
	struct BTreeNode* newNode;
	newNode = (struct BTreeNode*)malloc(sizeof(struct BTreeNode)); // BƮ�� ����ü��ŭ �����Ҵ�
	newNode->leaf = false;
	newNode->key[0] = val;
	newNode->num_key = 1;
	newNode->num_child = 0;
	return newNode;
}

// SPLIT Node
struct BTreeNode* splitNode(int pos, struct BTreeNode* node, struct BTreeNode* parent) {
	int median;
	if (Node_Order % 2 == 0) {								// ¦�� ���
		median = node->num_key / 2 - 0.5;							 
	}
	else {													// ¦�� ���
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
		for (int i = node->num_key; i > pos; i--) {
			parent->key[i] = parent->key[i - 1];
			parent->child[i + 1] = parent->child[i];		 // TODO:: �ߵ����� Ȯ��
		}
		parent->key[pos] = node->key[median];				 // �θ� ��忡 key�� �߰����ش�.
		parent->num_key++;
		node->num_key--;									 // median �������Ƿ� -1
		parent->child[pos+1] = child;						 // ���� ��忡 child left�� �߰����ش�.
		//parent->child[parent->num_child + 1] = child;			 // ���� ��忡 child right�� �߰����ش�.
		parent->num_child += 1;
	}
	return node;
}


struct BTreeNode* insertNode(int parent_pos, int val, struct BTreeNode* node, struct BTreeNode* parent) {
	int pos;												// pos�� ���� �� ������
	for (pos = 0; pos < node->num_key; pos++) {
		if (val == node->key[pos]) {
			printf("Duplicates are not permitted\n");		// �ߺ��� Ű�� ����
			return;
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
	//struct BTreeNode* temp;								// �θ� ������ (temp)
	//temp = root;
	root = insertNode(0, val, root, root);					// root �� �ִٸ� ��带 ã�� �����Ѵ�.
}

void printTree(struct BTreeNode* node, int level) {			 // Ʈ�� �׸���
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