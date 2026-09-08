#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dic_tree.h"

#define COLOR_PREFIX "\033[1;36m"  // bold cyan - what you typed
#define COLOR_MATCH  "\033[0;33m"  // yellow - the completed part
#define COLOR_RESET  "\033[0m"

Node *create_node(char c) {
    Node *node = malloc(sizeof(Node));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->c = c;
    node->full_str = NULL;
    for (int i = 0; i < 128; i++)
        node->children[i] = NULL;
    return node;
}

void insert_word(Node *root, const char *word) {
    Node *current = root;
    for (int i = 0; word[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)word[i];
        if (ch >= 128) {
            fprintf(stderr, "Skipping non-ASCII char in \"%s\"\n", word);
            return;
        }
        if (!current->children[ch]) {
            current->children[ch] = create_node(word[i]);
        }
        current = current->children[ch];
    }
    if (current != root) {
        free(current->full_str); // in case of duplicate insert
        current->full_str = strdup(word);
    }
}

Node *build_tree_from_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("fopen");
        return NULL;
    }

    Node *root = create_node('\0');
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = '\0'; // strip newline
        if (line[0] == '\0') continue;      // skip empty lines
        insert_word(root, line);
    }

    fclose(f);
    return root;
}

void visualize(Node *node, int depth) {
    if (!node) return;
    for (int i = 0; i < 128; i++) {
        Node *child = node->children[i];
        if (child) {
            for (int d = 0; d < depth; d++) printf("  ");
            printf("%c", child->c);
            if (child->full_str) printf("  <- \"%s\"", child->full_str);
            printf("\n");
            visualize(child, depth + 1);
        }
    }
}

Node *find_prefix_node(Node *root, const char *prefix) {
    Node *current = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        unsigned char ch = (unsigned char)prefix[i];
        if (ch >= 128 || !current->children[ch])
            return NULL; // prefix not present in the tree
        current = current->children[ch];
    }
    return current;
}

static void collect_words(Node *node, int *count, size_t prefix_len) {
    if (!node) return;
    if (node->full_str) {
        printf("  " COLOR_PREFIX "%.*s" COLOR_RESET
               COLOR_MATCH "%s" COLOR_RESET "\n",
               (int)prefix_len, node->full_str,
               node->full_str + prefix_len);
        (*count)++;
    }
    for (int i = 0; i < 128; i++)
        collect_words(node->children[i], count, prefix_len);
}

void search_prefix(Node *root, const char *prefix) {
    Node *node = find_prefix_node(root, prefix);
    if (!node) {
        printf("No matches for \"%s\"\n", prefix);
        return;
    }
    int count = 0;
    printf("Matches for \"%s\":\n", prefix);
    collect_words(node, &count, strlen(prefix));
    if (count == 0)
        printf("  (no complete words under this prefix)\n");
}

void free_tree(Node *node) {
    if (!node) return;
    for (int i = 0; i < 128; i++)
        free_tree(node->children[i]);
    free(node->full_str);
    free(node);
}