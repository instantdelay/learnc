#include <stdio.h>
#include <stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
//#define DEBUG

typedef enum { false, true } bool;

typedef struct node node;
struct node {
	char v;
	short code;
	node* next;
	node* child;
	node* parent;
};

typedef struct bits bits;
struct bits {
	unsigned char* data;
	unsigned char* d;
	int r;
};

node* trie_newNode(char v, short code) {
	node* n = NULL;
	n = calloc(1, sizeof(node));
	if (n == NULL) {
		#ifdef DEBUG
		printf("Malloc failed");
		#endif
		return n;
	}
	n->v = v;
	n->code = code;
	return n;
}

void trie_free(node* n) {
	if (n->child != NULL)
		trie_free(n->child);
	if (n->next != NULL)
		trie_free(n->next);
	free(n);
}

node* trie_new() {
	return trie_newNode('\0', 0);
}

void trie_debugPrint(node* start) {
	node* n = start;
	char buf[100] = {0};
	char* cur = buf - 1;
	bool up = false;
	
	do {
		if (n->child != NULL && !up) {
			n = n->child;
			cur++;
			
			*cur = n->v;
			printf("\"%s\"\t%d\n", buf, n->code);
		}
		else if (n->next != NULL) {
			n = n->next;
			up = false;
			
			*cur = n->v;
			printf("\"%s\"\t%d\n", buf, n->code);
		}
		else {
			n = n->parent;
			*cur = '\0';
			cur--;
			up = true;
		}
	} while (n != start);
}

node* trie_find(node* root, char* str) {
	node* n = root->child;
	char* c = str;
	node* next = NULL;
	while (1) {
		if (n->v == *c) {
			// matched node
			c++;
			
			if (*c == '\0') {
				// and it was the last char
				return n;
			}
			
			// keep going
			next = n->child;
		}
		else {
			next = n->next;
		}
		
		if (next == NULL) {
			return n->parent;
		}
		n = next;
	}
}

/*!
* Find the longest prefix of the given string in this tree.
*/
node* trie_getPrefix(node* root, char** strPtr) {
	node* n = root->child;
	node* next = NULL;
	
	if (n == NULL)
		return root;
	
	while (1) {
		if (n->v == **strPtr) {
			// matched node
			(*strPtr)++;
			
			if (**strPtr == '\0') {
				// end so no more to find
				return n;
			}
			
			if (n->child == NULL)
				return n;
				
			n = n->child;
		}
		else {
			if (n->next == NULL)
				return n->parent;
			
			n = n->next;
		}
	}
}

node* trie_addChild(node* n, char v, short code) {
	if (n == NULL) {
		printf("NULL node!");
		return n;
	}
	
	node* nn = trie_newNode(v, code);
	
	if (n->child == NULL) {
		n->child = nn;
	}
	else {
		node* ins = n->child;
		while (ins->next != NULL) ins = ins->next;
		
		ins->next = nn;
	}
	
	nn->parent = n;
	
	return nn;
}

node* trie_insert(node* n, char* str) {
	node* f = trie_getPrefix(n, &str);
	
	while (*str != '\0') {
		f = trie_addChild(f, *str, 0);
		str++;
	}
	
	return f;
}

lzw_prep(node* t, short* lc) {
	char* allSymbs = "#abcdefghijklmnopqrstuvwxyz ";
	char* c = allSymbs;
	while (*c != '\0') {
		trie_addChild(t, *c, (*lc)++);
		c++;
	}
}

main() {
	char arr[10] = {0};
	bits b;
	b.data = arr;
	b.d = b.data;
	b.r = 0;
	
	// bit_write(&b, 0b1010100);
	// bit_write(&b, 0b1111111);
	bit_write(&b, 0b0000001);
	bit_write(&b, 0b0000001);
	printBits(sizeof(char) * 4, b.data);
	
	return;
	char* input = "this is some cool text to compress";
	short lc = 0;
	
	node* trie = trie_new();
	lzw_prep(trie, &lc);
	
	char* i = input;
	
	node* f;
	while (1) {
		f = trie_getPrefix(trie, &i);
		printf("%d ", f->code);
		
		if (*i == '\0')
			break;
			
		trie_addChild(f, *i, lc++);
	}
	
	printf("\n\n");
	trie_debugPrint(trie);
	trie_free(trie);
	
	// testInsert();
	// testTrie();
}

testInsert() {
	node* root = trie_newNode('\0', 0);
	trie_insert(root, "apple");
	trie_insert(root, "apiary");
	trie_insert(root, "angle");
	trie_insert(root, "angie");
	
	char* toFind = "ancillary";
	node* f = trie_getPrefix(root, &toFind);
	printf("found: %c\n", f->v);
	
	trie_free(root);
}

testTrie() {
	node* root = trie_newNode('\0', 0);
	node* a = trie_newNode('a', 0);
	node* b = trie_newNode('b', 0);
	
	root->child = a;
	a->child = b;
	
	a->parent = root;
	b->parent = a;
	
	char* str = "abcdefghijk";
	printf(str);
	printf("\n");
	char* strStart = str;
	
	node* found = trie_getPrefix(root, &str);
	printf(str);
	printf("\n");
	
	if (found == NULL) {
		printf("failed");
	}
	else {
		int len = (str - strStart);
		printf("Found %d length prefix ending with: %c", len, found->v);
	}
	
	free(a);
	free(root);
}