#include <stdio.h>
#include <string.h>
#include "dic_tree.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <wordfile>\n", argv[0]);
        return 1;
    }

    Node *root = build_tree_from_file(argv[1]);
    if (!root) return 1;

    printf("Loaded dictionary from \"%s\".\n", argv[1]);
    printf("Type a prefix to search, \"tree\" to view the full structure, or \"quit\" to exit.\n");

    char line[256];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (!fgets(line, sizeof(line), stdin))
            break;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0')
            continue;
        if (strcmp(line, "quit") == 0 || strcmp(line, "exit") == 0)
            break;
        if (strcmp(line, "tree") == 0) {
            visualize(root, 0);
            continue;
        }

        search_prefix(root, line);
    }

    free_tree(root);
    return 0;
}