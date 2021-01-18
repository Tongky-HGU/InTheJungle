#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define order		4
#define Max_child	order
#define Max_key		order-1
#define Min_key		(int)(ceil(order/2) -1)

struct node {
	bool leaf;
	int key[Max_key + 1];
	int	num_key;
	struct node* child[Max_child + 1];
	int	num_child;
	int data[Max_key + 1];
};

struct node* root;

//insertion**********************************************************************************************************

struct node* creatNode(int val_given, int data_given) {
	struct node* newNode;
	newNode = (struct node*)malloc(sizeof(struct node));
	newNode->leaf = false;
	newNode->key[0] = val_given;
	newNode->data[0] = data_given;
	newNode->num_key = 1;
	newNode->num_child = 0;
	return newNode;
}

struct node* splitNode(struct node* parent_node, struct node* node,int parent_pos) {
	int median = (node->num_key / 2) -0.5;

	struct node* newNode;
	newNode = (struct node*)malloc(sizeof(struct node));
	newNode->leaf = node->leaf;
	newNode->num_key = 0;
	newNode->num_child = 0;


	if (node->leaf) {
		for (int i = median + 1; i <= Max_key; i++) {
			newNode->key[i - (median + 1)] = node->key[i];
			newNode->data[i - (median + 1)] = node->data[i];
			newNode->num_key++;
			node->num_key--;
		}
	}
	else {
		for (int i = median + 1; i <= Max_key; i++) {
			newNode->key[i - (median + 1)] = node->key[i];
			newNode->num_key++;
			node->num_key--;
		}
		node->num_key--;

		for (int i = median + 1; i <= Max_child; i++) {
			newNode->child[i - (median + 1)] = node->child[i];
			newNode->num_child++;
			node->num_child--;
		}
	}

	if (node == root) {
		struct node* newParentNode;
		newParentNode = creatNode(node->key[median], node->data[median]);
		newParentNode->child[0] = node;
		newParentNode->child[1] = newNode;
		newParentNode->num_child = 2;
		return newParentNode;
	}
	else {
		for (int i = parent_node->num_key; i > parent_pos; i--) {
			parent_node->key[i] = parent_node->key[i-1];
		}
		parent_node->key[parent_pos] = node->key[median];
		parent_node->num_key++;

		for (int i = parent_node->num_child; i > parent_pos; i--) {
			parent_node->child[i] = parent_node->child[i-1];
		}
		parent_node->child[parent_pos+1] = newNode;
		parent_node->num_child++;

		return parent_node;
	}
}

struct node* insertValueInNode(struct node* parent_node, struct node* node, int parent_pos, int key, int data) {
	int pos;
	for (pos = 0; pos < node->num_key; pos++) {
		if (key < node->key[pos]) {
			break;
		}
	}
	if (node->leaf) {
		for(int i = node->num_key; i > pos; i--) {
			node->key[i] = node->key[i-1];
			node->data[i] = node->data[i-1];
		}
		node->key[pos] = key;
		node->data[pos] = data;
		node->num_key++;
		if (node->num_key > Max_key) {
			parent_node = splitNode(parent_node, node, parent_pos);
		}
	}
	else {
		node = insertValueInNode(node,node->child[pos], pos, key, data);
		if (node->num_key > Max_key) {
			parent_node = splitNode(parent_node, node, parent_pos);
		}
	}
	return parent_node;
}

void insert(int key, int data) {
	if (!root) {
		root = creatNode(key, data);
		root->leaf = true;
		return;
	}
	root = insertValueInNode(root,root,0, key, data);
}

//deletion*******************************************************************************************************

int findInorderPredecessor(struct node* node) {
	int predecessor;
	if (node->leaf) {
		return node->key[node->num_key - 1];
	}
	else {
		predecessor = findInorderPredecessor(node->child[node->num_child-1]);
		return predecessor;
	}
}

void deletetValueInInnerNode(struct node* node, int pos) {
	node->key[pos] = findInorderPredecessor(node->child[pos]);
}

void merge(struct node* node, int pos, int pos_left) {
	if (node->child[pos]->leaf) {
		int target = node->child[pos_left]->num_key;
		
		int iter = node->child[pos]->num_key;
		for (int i = 0; i < iter; i++) {
			node->child[pos_left]->key[target+i] = node->child[pos]->key[i];
			node->child[pos_left]->data[target + i] = node->child[pos]->data[i];
			node->child[pos_left]->num_key++;
			node->child[pos]->num_key--;
		}

		for (int i = pos; i < node->num_key; i++) {
			node->key[i] = node->key[i + 1];
			node->data[i] = node->data[i + 1];
		}
		node->num_key--;

		for (int i = pos; i < node->num_child; i++) {
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;
	}
	else {
		int target = node->child[pos_left]->num_key;
		node->child[pos_left]->key[target] = node->key[pos-1];
		node->child[pos_left]->data[target] = node->data[pos-1];
		node->child[pos_left]->num_key++;

		for (int i = pos; i < node->num_key; i++) {
			node->key[i] = node->key[i + 1];
			node->data[i] = node->data[i + 1];
		}
		node->num_key--;

		target = node->child[pos_left]->num_key;
		int iter = node->child[pos]->num_key;
		for (int i = 0; i < iter; i++) {
			node->child[pos_left]->key[target + i] = node->child[pos]->key[i];
			node->child[pos_left]->data[target + i] = node->child[pos]->data[i];
			node->child[pos_left]->num_key++;
			node->child[pos]->num_key--;
		}

		target = node->child[pos_left]->num_child;
		iter = node->child[pos]->num_child;
		for (int i = 0; i < iter; i++) {
			node->child[pos_left]->child[target+i] = node->child[pos]->child[i];
			node->child[pos_left]->num_child++;
			node->child[pos]->num_child--;
		}

		for (int i = pos; i < node->num_child; i++) {
			node->child[i] = node->child[i + 1];
		}
		node->num_child--;

	}

}


void borrowFromLeft(struct node* node, int pos) {
	if (node->child[pos]->leaf) {
		int borrow = node->child[pos-1]->num_key-1;
		int target = 0;

		for (int i = node->child[pos]->num_key; i > target; i--) {
			node->child[pos]->key[i] = node->child[pos]->key[i - 1];
			node->child[pos]->data[i] = node->child[pos]->data[i - 1];
		}
		node->child[pos]->key[target] = node->child[pos -1]->key[borrow];
		node->child[pos]->data[target] = node->child[pos -1]->data[borrow];
		node->child[pos]->num_key++;

		node->child[pos - 1]->num_key--;

		node->key[pos-1] = findInorderPredecessor(node->child[pos-1]);
	}
	else {
		int borrow = node->child[pos - 1]->num_key - 1;
		int target = 0;
		for (int i = node->child[pos]->num_key; i > target; i--) {
			node->child[pos]->key[i] = node->child[pos]->key[i - 1];
			node->child[pos]->data[i] = node->child[pos]->data[i - 1];
		}
		node->child[pos]->key[target] = node->key[pos - 1];
		node->child[pos]->data[target] = node->data[pos - 1];
		node->child[pos]->num_key++;


		node->key[pos - 1] = node->child[pos - 1]->key[borrow];

		node->child[pos - 1]->num_key--;


		for (int i = node->child[pos]->num_child; i > target; i--) {
			node->child[pos]->child[i] = node->child[pos]->child[i - 1];
		}

		borrow = node->child[pos - 1]->num_child - 1;
		node->child[pos]->child[target] = node->child[pos - 1]->child[borrow];
		node->child[pos-1]->num_child--;
		node->child[pos]->num_child++;

	}
}

void borrowFromRight(struct node* node, int pos) {
	if (node->child[pos]->leaf) {
		int borrow = 0;
		int target = node->child[pos]->num_key;
		node->child[pos]->key[target] = node->child[pos + 1]->key[borrow];
		node->child[pos]->data[target] = node->child[pos + 1]->data[borrow];

		node->child[pos]->num_key++;
		
		for (int i = 0; i < node->child[pos + 1]->num_key; i++) {
			node->child[pos + 1]->key[i] = node->child[pos + 1]->key[i + 1];
			node->child[pos + 1]->data[i] = node->child[pos + 1]->data[i + 1];
		}
		node->child[pos+1]->num_key--;

		node->key[pos] = findInorderPredecessor(node->child[pos]);
	}
	else {
		int borrow = 0;
		int target = node->child[pos]->num_key;
		node->child[pos]->key[target] = node->key[pos];
		node->child[pos]->data[target] = node->data[pos];
		node->child[pos]->num_key++;

		node->key[pos] = node->child[pos +1]->key[borrow]; 

		for (int i = 0; i < node->child[pos]->num_key; i++) {
			node->child[pos+1]->key[i] = node->child[pos+1]->key[i + 1];
			node->child[pos+1]->data[i] = node->child[pos+1]->data[i + 1];
		}
		node->child[pos +1]->num_key--;

		target = node->child[pos]->num_child;
		node->child[pos]->child[target] = node->child[pos + 1]->child[borrow];
		node->child[pos]->num_child++;

		for (int i = 0; i < node->child[pos + 1]->num_child - 1; i++) {	
			node->child[pos + 1]->child[i] = node->child[pos + 1]->child[i + 1];
		}
		node->child[pos + 1]->num_child--;


	}
}

void balanceNode(struct node* node, int pos) {
	if (pos == node->num_key) {
		if (node->child[pos-1]->num_key > Min_key) {
			borrowFromLeft(node, pos);
		}
		else {
			merge(node, pos, pos - 1);
		}
	}
	else if (pos == 0) {
		if (node->child[pos+1]->num_key > Min_key) {
			borrowFromRight(node, pos);
		}
		else {
			merge(node, pos + 1, pos);
		}
	}
	else {
		if (node->child[pos+1]->num_key > Min_key) {
			borrowFromRight(node, pos);
		}
		else if (node->child[pos-1]->num_key > Min_key) {
			borrowFromLeft(node, pos);
		}
		else {
			merge(node, pos, pos - 1);
		}

	}
}

void deletetValueInNode(struct node* node, int key) {
	int pos;
	bool flag = false;
	for (pos = 0; pos < node->num_key; pos++) {
		if (key == node->key[pos]) {
			flag = true;
			break;
		}
		else if (key < node->key[pos]) {
			break;
		}
	}
	if (flag) {
		if (node->leaf) {
			for (int i = pos; i <node->num_key; i++) {
				node->key[i] = node->key[i + 1];
				node->data[i] = node->data[i + 1];
			}
			node->num_key--;
		}
		else {
			deletetValueInNode(node->child[pos], key);
			if (node->child[pos]->num_key < Min_key) {
				balanceNode(node, pos);
				deletetValueInNode(node, key);
			}
			else {
				deletetValueInInnerNode(node, pos);
			}
		}
	}
	else {
		if (node->leaf) {
			return;
		}
		deletetValueInNode(node->child[pos], key);
 		if (node->child[pos]->num_key < Min_key) {
			balanceNode(node, pos);
		}
	}

}

void delete(int key) {
	deletetValueInNode(root, key);

	if (root->num_key == 0) {
		root = root->child[0];
	}
}

//print**********************************************************************************************************
void printNode(struct node* node, int level) {
	for (int i = 1; i < level; i++) {
		printf("          ");
	}

	if (node->leaf) {
		for (int pos = 0; pos < node->num_key; pos++) {
			printf("%d->%d, ", node->key[pos], node->data[pos]);
		}
		printf("\n");
		
	}
	else {
		for (int pos = 0; pos < node->num_key; pos++) {
			printf("%d, ", node->key[pos]);
		}
		printf("\n");
		for (int pos = 0; pos < node->num_child; pos++) {
			printNode(node->child[pos], level + 1);
		}
	}

	return;
}

void Traverse(struct node* root) {
	printNode(root, 1);
}

//main**********************************************************************************************************

//int main(void) {
//
//	insert(1, 1);
//	insert(4, 4);
//	insert(7, 7);
//	insert(10, 10);
//	insert(17, 17);
//	insert(21, 21);
//	insert(31, 31);
//	insert(25, 25);
//	insert(19, 19);
//	insert(20, 20);
//	insert(8, 8);
//	insert(9, 9);
//
//	delete(19);
//	delete(25);
//	delete(17);
//	delete(8);
//	delete(7);
//	delete(20);
//	delete(31);
//	delete(4);
//
//
//
//
//	Traverse(root);
//	printf("-----------------------------------------------\n");
//
//	delete(9);
//
//	Traverse(root);
//}

#define TEST { 10,1,3,63,82,6,31,8,2,16,11,77,96,14,92,21,47,23,24,78,26,97,15,4,30,69,37,38,76,90,17,87,53,44,45,46,9,41,54,43,22,84,58,39,65,28,42,59,99,70,71,72,29,74,73,68,13,60,79,80,81,85,83,64,94,86,66,88,93,40,91,62,25,20,36,95,19,52,55,100 }

// 확인 함수
void print_for_exam(struct Node* cur);

// 테스트
int main()
{
	int arr[80] = TEST;

	// TEST 1 CASE
	insert(4, 4 * 1000);
	insert(1, 1 * 1000);
	insert(3, 3 * 1000);
	insert(2, 2 * 1000);

	delete(4);
	delete(2);
	delete(3);

	printf("---- TEST1 ----\n");
	print_for_exam(root);

	// TEST 2 CASE
	for (int i = 2; i <= 100; i++) {
		insert(i, i * 1000);
	}
	for (int i = 50; i <= 70; i++) {
		delete(i);
	}


	printf("\n\n\n\n\n\n");
	printf("---- TEST2 ----\n");
	print_for_exam(root);

	// TEST3 CASE
	for (int i = 50; i <= 70; i++) {
		insert(i, i * 1000);
	}

	for (int i = 0; i < 80; i++) {
		delete(arr[i]);
	}

	printf("\n\n\n\n\n\n");
	printf("---- TEST3 ----\n");
	print_for_exam(root);

	printf("프로그램이 정상적으로 종료 되었음.");
	return 0;
}

// 출력(DFS)
void print_for_exam(struct node* cur) {
	if (cur->leaf) {
		for (int i = 0; i < cur->num_key; i++) {
			printf("[%5d, %5d]\n", cur->key[i], cur->data[i]);
		}
	}
	else {
		for (int i = 0; i < cur->num_key; i++) {
			print_for_exam(cur->child[i]);
			printf("[%5d]\n", cur->key[i]);
		}
		print_for_exam(cur->child[cur->num_key]);
	}
}
