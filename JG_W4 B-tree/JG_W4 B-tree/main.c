// B plus tree study
// 2021

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define Node_Order			5
#define MAX_Childs			Node_Order
#define MAX_Keys			MAX_Childs-1
#define MIN_Keys			Node_Order/2 -1

struct Node {							    // ��Ʈ���� ��� ����ü
	bool leaf;
	int key[MAX_Keys + 1];					// Ű ������
	int num_key;							// Ű ����
	struct Node* child[MAX_Childs + 1];     // �ڽ��� �ּҸ� ������ ������
	int num_child;							// �ڽ� ����
	struct Node* next;
	bool end_leaf;							// ������ ������ ���ΰ�
};

struct Node* root; // ��Ʈ ������


/*INSERT******************************************************************************************************/
 //Create Node
struct Node* createNode(int val) {
	struct Node* newNode;
	newNode = (struct Node*)malloc(sizeof(struct Node)); // BƮ�� ����ü��ŭ �����Ҵ�
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
	child->end_leaf = node->end_leaf;						 // ���� ��尡 ������ ���� ����� ��� ���ο� ��尡 ������ ������.
	child->next = node->next;

	if (node->leaf) {										 // ���� ���� ��忡���� �����̶�� 
		node->next = child;								     // �� ���� ���� ��带 ��ũ �����ش�.
		node->end_leaf = false;								 // ������尡 ���ҵȴٸ� ���� ���� ���̻� ������ ������ �� ����.

		for (int i = median; i < node->num_key; i++) {  // child right�� Ű ����ֱ� ���� ���� child left�� �ȴ�
			child->key[i - median] = node->key[i];
		}
		node->num_key = node->num_key - median;
		child->num_key = median;
	}
	else{													 // leaf�� �ƴ� ���� child�� �Ѱ������
		int num_iter = node->num_key;
		for (int i = median + 1; i < num_iter; i++) {  // child right�� Ű ����ֱ� ���� ���� child left�� �ȴ�
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

	if (node == root) {										 // ���� ��尡 ��Ʈ���
		struct Node* new_parent;							 // ���ο� parent���� median Ű�� �־��ش�.
		new_parent = createNode(node->key[median]);
		if (!node->leaf) {									 // ������ �ƴҶ��� ���� ��忡�� median�� �������.
			node->num_key--;								 // median �������Ƿ� -1
		}
		new_parent->child[0] = node;
		new_parent->child[1] = child;
		new_parent->num_child = 2;
		parent = new_parent;
		return parent;
	}
	else {													 // ���� ��尡 ��Ʈ�� �ƴ϶��
		for (int i = parent->num_key; i > pos; i--) {		 // �θ� �� �ڸ��� �����.
			parent->key[i] = parent->key[i - 1];
			parent->child[i + 1] = parent->child[i];		 
		}
		parent->key[pos] = node->key[median];				 // �θ� ��忡 key�� �߰����ش�.
		parent->num_key++;
		if (!node->leaf) {									 // ������ �ƴҶ��� ���� ��忡�� median�� �������.
			node->num_key--;								 // median �������Ƿ� -1
		}
		parent->child[pos + 1] = child;						 // �θ� ��忡 �� child�� �߰����ش�.
		parent->num_child += 1;
	}
	return node;
}

//Insert Node
struct Node* insertNode(int parent_pos, int val, struct Node* node, struct Node* parent) {
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
		if (node->num_key == MAX_Keys + 1) {				// �Ʒ� ���Կ� ���� Ű�� á�ٸ�, �� split�� ���ش�
			node = splitNode(parent_pos, node, parent);
		}
	}
	else {													// leaf���� �����Ѵ�.
		for (int i = node->num_key; i > pos; i--) {
			node->key[i] = node->key[i - 1];
			//node->child[i + 1] = node->child[i];			// TO DO:: �̰� �ʿ��Ѱ�??
		}
		node->key[pos] = val;
		node->num_key++;
		if (node->num_key == MAX_Keys + 1) {				// Ű�� ��á�ٸ�? split�� ���ش�!
			node = splitNode(parent_pos, node, parent);
		}
	}

	return node;											// ��� ��ȯ
}

void insert(int val) {
	if (!root) { 											// root �� ���ٸ� root�� �����.
		root = createNode(val);
		root->leaf = true;
		root->end_leaf = true;
		return;
	}

	root = insertNode(0, val, root, root);					// root �� �ִٸ� ��带 ã�� �����Ѵ�.
}
/*DELETION******************************************************************************************************/


void borrowFromLeft(struct Node* node, int pos) {
	
	if (node->child[pos]->leaf) {									 // �������� borrow�� �Ͼ�ٸ�
		int target = 0;													 // ������ ��ġ�� ������ idx
		int borrow = node->child[pos - 1]->num_key - 1;					 // ������ ��ġ�� idx

		for (int i = 0; i < node->child[pos]->num_key; i++) {            // ä�� ��ġ�� ����д�.
			node->child[pos]->key[i + 1] = node->child[pos]->key[i];
		}
		node->child[pos]->key[target] = node->child[pos-1]->key[borrow];	 // ������ ������ ä���.
		node->child[pos]->num_key++;

		node->child[pos - 1]->num_key--;								 // ������ ���� Ű������ �����.

		int successor = findSuccessor(node->child[pos]);				// successor �� ���� ä���.
		node->key[pos - 1] = successor;									
	}
	else {															 // ���ο��� borrow�� �Ͼ�ٸ�
		int target = 0;													 // ������ ��ġ�� ������ idx
		for (int i = 0; i < node->child[pos]->num_key; i++) {
			node->child[pos]->key[i + 1] = node->child[pos]->key[i];
		}
		node->child[pos]->key[target] = node->key[pos - 1];				 // �� ���� ���ش�.
		node->child[pos]->num_key++;

		int borrow = node->child[pos - 1]->num_key - 1;					 // ������ ��ġ�� idx
		node->key[pos - 1] = node->child[pos - 1]->key[borrow];			 // �� ���� ä���.

		node->child[pos - 1]->num_key--;								 // ������ ���� Ű������ �����.
		
		if (node->child[pos - 1]->num_child > 0) {						 // ������ ���� child�� �ִٸ�
			borrow = node->child[pos - 1]->num_child - 1;			     // ������ ���� child�� �Ѱ��ش�.
			for (int i = node->child[pos]->num_child; i > 0; i--) {
				node->child[pos]->child[i] = node->child[pos]->child[i - 1];
			}
			node->child[pos]->child[0] = node->child[pos - 1]->child[borrow];
			node->child[pos]->num_child++;

			node->child[pos - 1]->num_child--;							 // ������ ���� child ����
		}
	}

}
void borrowFromRight(struct Node* node, int pos) {
	if (node->child[pos]->leaf) {									 // �������� borrow�� �Ͼ�ٸ�
		int target = node->child[pos]->num_key;					 // ������ ��ġ�� ������ idx
		int borrow = 0;											 // ������ ��ġ�� idx

		node->child[pos]->key[target] = node->child[pos+1]->key[borrow];	// ������ ������ ä���.
		node->child[pos]->num_key++;

		for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {	   // ������ ���� Ű ����
			node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
		}
		node->child[pos + 1]->num_key--;

		int successor = findSuccessor(node->child[pos+1]);				// successor �� ���� ä���.
		node->key[pos] = successor;									
	}
	else {															 // ���ο��� borrow�� �Ͼ�ٸ�
		int target = node->child[pos]->num_key;							 // ������ ��ġ�� ������ idx
		node->child[pos]->key[target] = node->key[pos];				     // �� ���� ���ش�.
		node->child[pos]->num_key++;

		int borrow = 0;												     // ������ ��ġ�� idx(�ǿ���)
		node->key[pos] = node->child[pos + 1]->key[borrow];				 // �� ���� ä���.

		for (int i = 0; i < node->child[pos + 1]->num_key - 1; i++) {	 // ������ ���� Ű ����
			node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
		}
		node->child[pos + 1]->num_key--;

		if (node->child[pos + 1]->num_child > 0) {
			target = node->child[pos]->num_child;							 // ������ ���� child�� �Ѱ��ش�.
			node->child[pos]->child[target] = node->child[pos + 1]->child[borrow];
			node->child[pos]->num_child++;

			for (int i = 0; i < node->child[pos + 1]->num_child - 1; i++) {	 // ������ ���� child ����
				node->child[pos + 1]->child[i] = node->child[pos + 1]->child[i + 1];
			}
			node->child[pos + 1]->num_child--;
		}
	}
}

void mergeNode(struct Node* node, int pos, int pos_left) {		// ��� Ű�� poe_left ���� �����ش�. TODO:: �� ������ ���ؿ��� �������� ���ư��°�?
	if (node->child[pos]->leaf) {								     	 // �������� merge�� �Ͼ�ٸ�
		int target = node->child[pos_left]->num_key;

		for (int i = 0; i < node->child[pos]->num_key; i++) {			// �������ڽ��� Ű�� ���� �ڽĿ� �־��ش�.
			node->child[pos_left]->key[target + i] = node->child[pos]->key[i];
			node->child[pos_left]->num_key++;
		}

		node->child[pos_left]->next = node->child[pos]->next;
		node->child[pos_left]->end_leaf = node->child[pos]->end_leaf;

		for (int i = pos_left; i < node->num_key - 1; i++) {			// �� ����� key ���� 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = pos; i < node->num_child - 1; i++) {				// �� ����� child ���� 
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;

	}
	else {																// ��忡�� merge�� �Ͼ�ٸ�
		int target = node->child[pos_left]->num_key;					// ���� �ڽĿ� �� idx

		node->child[pos_left]->key[target] = node->key[pos_left];		// �� ��尪�� �־��ش�.
		node->child[pos_left]->num_key++;

		for (int i = 0; i < node->child[pos]->num_key; i++) {			// �������ڽ��� Ű�� ���� �ڽĿ� �־��ش�.
			node->child[pos_left]->key[target + 1 + i] = node->child[pos]->key[i];
			node->child[pos_left]->num_key++;
		}

		target = node->child[pos_left]->num_child;
		for (int i = 0; i < node->child[pos]->num_child; i++) {			// �������ڽ��� �ڽ��� ���� �ڽĿ� �־��ش�.
			node->child[pos_left]->child[target + i] = node->child[pos]->child[i];
			node->child[pos_left]->num_child++;
		}

		free(node->child[pos]);											// ������ �ڽ��� free ��Ų��.

		for (int i = pos_left; i < node->num_key - 1; i++) {			// �� ����� key ���� 
			node->key[i] = node->key[i + 1];
		}
		node->num_key--;

		for (int i = pos; i < node->num_child - 1; i++) {				// �� ����� child ���� 
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;
	}
}

void adjustNode(struct Node* node, int pos) {
	if (pos == 0) {											// child �� ���� ���϶�, ������ ���������� ������ �� �ִ�.
		if (node->child[pos + 1]->num_key > MIN_Keys) {
			borrowFromRight(node, pos);
		}
		else {
			mergeNode(node, pos + 1, pos);
		}
		return;
	}
	else {
		if (pos == node->num_key) {										// child �� ������ ���϶�, ���� ���������� ������ �� �ִ�.
			if (node->child[pos - 1]->num_key > MIN_Keys) {
				borrowFromLeft(node, pos);
			}
			else {
				mergeNode(node, pos, pos - 1);
			}
			return;
		}
		else {												// �� �� ��Ȳ������, ���ʿ��� ������ �� �ִ�.
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
	//if (node->child[pos]->num_key >= node->child[pos + 1]->num_key) { // ���� �ڽ��� Ű ������ ũ�ų� ������
	//	if (node->child[pos]->num_key > MIN_Keys) {
	//		result_deletion = inorderPredecessor(node, pos);
	//		deleteValFromNode(result_deletion, node->child[pos]);

	//	}
	//	else {
	//		result_deletion = inorderMerge(node, pos);
	//		deleteValFromNode(result_deletion, node->child[pos]);
	//	}
	//}
	//else {															  // ������ �ڽ��� Ű ������ Ŭ ��
	//	if (node->child[pos + 1]->num_key > MIN_Keys) {
	//		result_deletion = inorderSuccessor(node, pos);
	//		deleteValFromNode(result_deletion, node->child[pos + 1]);
	//	}
	//	else {
	//		result_deletion = inorderMerge(node, pos);
	//		deleteValFromNode(result_deletion, node->child[pos]);
	//	}
	//}
}


int deleteValFromNode(int val, struct Node* node) {
	int pos;
	int flag = false;
	for (pos = 0; pos < node->num_key; pos++) {				// �� ��忡�� val, Ȥ�� �� ��ġ�� ã�´�.
		if (val == node->key[pos]) {
			flag = true;
			break;
		}
		if (val < node->key[pos]) {
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
			flag = deleteValFromNode(val, node->child[pos+1]); // ���ο��� �߰����� ���� ���� child�� �Ѿ��.
			if (node->child[pos + 1]->num_key < MIN_Keys) {    // ��ͷ� ������ �� �����ߴ� �ڽ��� ������ ���ڸ� �� 
				adjustNode(node, pos+1);
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
	if (node->child[pos]->num_key < MIN_Keys) {    // ��ͷ� ������ �� �����ߴ� �ڽ��� ������ ���ڸ� �� 
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
	if (!flag) { 											// ���� �� ��尡 ���� ��
		printf("%d no exist in the tree!!\n", val);
		return;
	}

	if (node->num_key == 0) {								// case#3 ������ ��ȭ�� ������
		node = node->child[0];
	}
	root = node;

}


/*PRINT******************************************************************************************************/
void printTree(struct Node* node, int level) {				 // Ʈ�� �׸���
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

void printLeaves(struct Node* node) {			 // leaf �׸���
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

	for (int i=0; i < 500; i++) {
		insert(i);
	}

	printTree(root, 1);
	printLeaves(root);
	printf("----------------------------------------------------------------------------------------------------\n");

	for (int i=300; i < 500; i++) {
		delete(root,i);
	}

	printTree(root, 1);
	printLeaves(root);


}