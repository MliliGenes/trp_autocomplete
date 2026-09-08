#ifndef DIC_TREE_H
#define DIC_TREE_H

typedef struct Node {
    char c;
    struct Node *children[128];
    char *full_str; 
} Node;

Node *create_node(char c);
void insert_word(Node *root, const char *word);
Node *build_tree_from_file(const char *filename);
void visualize(Node *node, int depth);
void free_tree(Node *node);

Node *find_prefix_node(Node *root, const char *prefix);
void search_prefix(Node *root, const char *prefix);

#endif