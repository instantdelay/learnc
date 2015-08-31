#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "bits.h"

typedef enum { false, true } bool;

typedef struct node node;
struct node {
	char v;
	short code;
	node* next;
	node* child;
	node* parent;
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

void lzw_prep(node* t, short* lc) {
	char* allSymbs = "#abcdefghijklmnopqrstuvwxyz ";
	char* c = allSymbs;
	while (*c != '\0') {
		trie_addChild(t, *c, (*lc)++);
		c++;
	}
}

void lzw_compress(char* input, bits* b) {
	node* trie = trie_new();
	short lc = 1;
	lzw_prep(trie, &lc);
	
	char* i = input;
	
	node* f;
	while (1) {
		f = trie_getPrefix(trie, &i);
		bits_write(b, f->code, 7);
		
		#ifdef DEBUG
		printf("%d ", f->code);
		#endif
		
		if (*i == '\0')
			break;
			
		trie_addChild(f, *i, lc++);
	}
	
	#ifdef DEBUG
	printf("\n\n");
	trie_debugPrint(trie);
	#endif
	
	trie_free(trie);
}

void lzw_decompress(bits* b) {
	char* allSymbs = "#abcdefghijklmnopqrstuvwxyz ";
	char ent[100][10] = {0};
	char* c = allSymbs;
	int i = 1;
	while (*c != '\0') {
		ent[i][0] = *c;
		ent[i++][1] = '\0';
		c++;
	}
	
	char* w;
	char* k;
	
	int v = bits_read(b, 7);
	k = ent[v];
	printf("%s", k);
	w = k;
	
	while(1) {
		v = bits_read(b, 7);
		if (v == 0)
			break;
		
		k = ent[v];
		
		printf("%s", k);
		
		char* cpy = w;
		while (*cpy != '\0') {
			ent[i][cpy - w] = *cpy;
			cpy++;
		}
		ent[i][cpy - w] = k[0];
		ent[i][cpy - w + 1] = '\0';
		// printf("adding entry %d : %s\n", i, ent[i]);
		i++;
		
		w = k;
	}
}

int main() {
	char* input = "this is some cool text to compress";
	printf("Compressing %d byte string: \"%s\"\n", strlen(input), input);
	
	bits* b = bits_new(50);
	lzw_compress(input, b);
	
	printf("Into %d bytes", (b->pos - b->data));
	
	#ifdef DEBUG
	printf("\n\n");
	printBin(b->data, 30);
	#endif
	
	bits_reset(b);
	
	printf("\n\n");
	lzw_decompress(b);
	
	bits_free(b);
	
	// testInsert();
	// testTrie();
}

void testInsert() {
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

void testTrie() {
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