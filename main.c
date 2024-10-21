#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int *info;
    int release;
    struct Node *next;
} Node;

typedef struct KeySpace {
    int busy;
    char *key;
    Node *info;
} KeySpace;

typedef struct Table {
    KeySpace *ks;
    int msize;
} Table;

int get_int(int *number) {
    int n, f = -10;
    char symbol;
    do {
        f = 0;
        n = scanf("%u", number);
        if (n < 0) {
            return 0;
        } else if (n == 0) {
            printf("Error! Repeat input, please:\n");
            scanf("%*[^\n]");
        } else if (*number < 0) {
            printf("Error! Repeat input, please:\n");
            scanf("%*[^\n]");
        } else if (n == 1) {
            f = scanf("%c", &symbol);
            while (symbol == ' ' || symbol == '\t') {
                f = scanf("%c", &symbol);
            }
            if (symbol != '\n') {
                scanf("%*[^\n]");
                printf("Error! Repeat input, please:\n");
            }
        }
    } while (n == 0 || *number < 0 || symbol != '\n');
    return 1;
}

char *create_str() {
    int len_str = 0;
    char *stdin_str = malloc(len_str + 2);
    char symbol = ' ';
    while (symbol != '\n') {
        stdin_str = realloc(stdin_str, len_str + 2);
        symbol = getchar();
        if (symbol == EOF) {
            printf("End of working.\n");
            free(stdin_str);
            return NULL;
        } else {
            stdin_str[len_str] = symbol;
            len_str++;
        }
    }
    stdin_str[len_str - 1] = '\0';
    return stdin_str;
}

int hash1(char *key) {
    int h = 2139062;
    for (int i = 0; i < strlen(key); ++i) {
        h += key[i];
    }
    return abs(h);
}

int hash2(char *key) {
    int h = 2139062;
    for (int i = 0; i < strlen(key); ++i) {
        h += key[i];
    }
    if (h % 2 == 0) {
        return abs(h + 1);
    } else {
        return abs(h);
    }
}

int hash(int h1, int h2, int coefficient, Table table) {
    return abs(abs((h1 + coefficient * h2)) % table.msize);
}

int count_release(Table table, char *key) {
    int release = 1;
    for (int j = 0; j < table.msize; ++j) {
        int index = hash(hash1(key), hash2(key), j, table);
        if (strcmp(key, table.ks[index].key) == 0) {
            Node *tmp = table.ks[index].info;
            while (tmp) {
                tmp = tmp->next;
                release++;
            }
            //table.ks[index].info = tmp;
            break;
        }
    }
    return release;
}

int add(Table *table, char *key, int info) {
    int flag = 0;
    for (int i = 0; i < table->msize; ++i) {
        int index = hash(hash1(key), hash2(key), i, *table);
        if (table->ks[index].busy == 1 && strcmp(key, table->ks[index].key) == 0) { ;
            Node *tmp = table->ks[index].info;
            while (tmp->next) {
                tmp = tmp->next;
            }
            int rel = count_release(*table, key);
            Node *new_node = malloc(sizeof(Node));
            new_node->info = malloc(sizeof(int));
            new_node->release = rel;
            new_node->info[0] = info;
            new_node->next = NULL;
            tmp->next = new_node;
            flag = 1;
            break;
        } else if (table->ks[index].busy == 1) {
            continue;
        } else {
            (*table).ks[index].busy = 1;
            free(table->ks[index].key);
            (*table).ks[index].key = strdup(key);
            int rel = count_release(*table, key);
            (*table).ks[index].info = malloc(sizeof(Node));
            (*table).ks[index].info->info = malloc(sizeof(int));
            table->ks[index].info->release = rel;
            table->ks[index].info->info[0] = info;
            table->ks[index].info->next = NULL;
            flag = 1;
            break;
        }
    }
    //free(key);
    if (flag == 0) {
        return 1;
    }
    return 0;
}

int d_add(Table *table) {
    printf("Enter the key:\n");
    char *key = create_str();
    if (!key) {
        return 2;
    }
    printf("Enter the information:\n");
    int info;
    if (get_int(&info) != 1) {
        printf("Error! Restart program, please.\n");
        free(key);
        return 2;
    }
    if (add(table, key, info) == 1) {
        printf("Your element wasn't added because the table is full. Try to do something else.\n");
        return 1;
    }
    free(key);
    return 0;
}

int remove_element(Table *table, int release, char *delete_key, int flag) {
    if (flag == 1) {
        for (int i = 0; i < table->msize; ++i) {
            int index = hash(hash1(delete_key), hash2(delete_key), i, *table);
            if (table->ks[index].busy == 1) {
                if ((strcmp(table->ks[index].key, delete_key) == 0)) {
                    table->ks[index].busy = 0;
                    free(table->ks[index].key);
                    table->ks[index].key = NULL;
                    Node *delete_node = NULL;
                    Node *ptr = table->ks[index].info;
                    while (ptr) {
                        delete_node = ptr;
                        ptr = ptr->next;
                        free(delete_node->info);
                        free(delete_node);
                    }
                    table->ks[index].info = NULL;
                    delete_key = NULL;
                    break;
                } else {
                    continue;
                }
            }
        }
    } else {
        for (int i = 0; i < table->msize; ++i) {
            int index = hash(hash1(delete_key), hash2(delete_key), i, *table);
            if (table->ks[index].busy == 1) {
                if (release == 1) {
                    Node *to_delete = table->ks[index].info;
                    table->ks[index].info = table->ks[index].info->next;
                    free(to_delete->info);
                    free(to_delete);
                    Node *ptr = table->ks[index].info;
                    while (ptr) {
                        ptr->release--;
                        ptr = ptr->next;
                    }
                    break;
                } else {
                    Node *ptr = table->ks[index].info;
                    Node *delete_node = table->ks[index].info->next;
                    int j = 2;
                    while (j != release) {
                        ptr = delete_node;
                        delete_node = delete_node->next;
                        j++;
                    }
                    if (delete_node->next) {
                        Node *ptr2 = delete_node->next;
                        ptr->next = ptr2;
                        while (ptr2) {
                            ptr2->release--;
                            ptr2 = ptr2->next;
                        }
                    } else {
                        ptr->next = NULL;
                    }
                    free(delete_node->info);
                    free(delete_node);
                    break;
                }
            }
        }
    }
    //if (delete_key) {
    //    free(delete_key);
    //}
    delete_key = NULL;
    return 0;
}

int d_remove_element(Table *table) {
    printf("Enter the key would you like to delete:\n");
    int flag2 = 0;
    char *delete_key;
    do {
        delete_key = create_str();
        if (!delete_key) {
            return 2;
        }
        int k = 0;
        while (k < table->msize) {
            if (table->ks[k].busy == 1) {
                if (strcmp(table->ks[k].key, delete_key) == 0) {
                    flag2 = 1;
                    break;
                }
            }
            k++;
        }
        if (flag2 != 1) {
            free(delete_key);
            printf("Table doesn't contain this key. Try again:\n");
        }
    } while (flag2 != 1);
    printf("You would like to delete all releases of this key or only one version?\n");
    printf("Enter 1, if all releases.\n");
    printf("Enter 2, if only one version.\n");
    int flag = 0;
    while (flag != 1 && flag != 2) {
        if (get_int(&flag) != 1) {
            printf("Error! Restart program, please.\n");
            free(delete_key);
            return 2;
        }
        if (flag != 2 && flag != 1) {
            printf("You've entered incorrect data. Try again:\n");
        }
    }
    int release = 0;
    if (flag == 2) {
        printf("Enter the release:\n");
        int max_release = count_release(*table, delete_key) - 1;
        do {
            if (get_int(&release) != 1) {
                printf("Error! Restart program, please.\n");
                free(delete_key);
                return 2;
            }
            if (release > max_release || release == 0) {
                printf("You've entered incorrect data. Try again:\n");
            }
        } while (release > max_release || release == 0);
    }
    remove_element(table, release, delete_key, flag);
    if (delete_key) {
        free(delete_key);
    }
    return 0;
}

void free_table(Table *table) {
    int i = 0;
    while (i < table->msize) {
        if (table->ks[i].busy == 1) {
            remove_element(table, 1, table->ks[i].key, 1);
        }
        if (table->ks[i].key) {
            free(table->ks[i].key);
        }
        table->ks[i].key = NULL;
        i++;
    }
    if (table->ks != NULL) {
        free(table->ks);
    }
}

int output(Table *table) {
    int i = 0;
    printf("==========================================\n");
    while (i < table->msize) {
        if (table->ks[i].busy == 1) {
            Node *ptr = table->ks[i].info;
            while (table->ks[i].info) {
                printf("Key number %d: %s\n", i + 1, table->ks[i].key);
                printf("Release: %d\n", table->ks[i].info->release);
                printf("Busy: %d\n", table->ks[i].busy);
                if (table->ks[i].info->info) {
                    printf("Information: %d\n", *table->ks[i].info->info);
                } else {
                    printf("Information: null\n");
                }
                table->ks[i].info = table->ks[i].info->next;
                printf("======================\n");
            }
            table->ks[i].info = ptr;
            printf("==========================================\n");
        }
        i++;
    }
    return 0;
}

int find(Table *table, char *find_key, Table output_table, int release, int flag, int len_table) {
    if (flag == 1) {
        for (int i = 0; i < table->msize; ++i) {
            int max_release = count_release(*table, find_key) - 1;
            int index = hash(hash1(find_key), hash2(find_key), i, *table);
            if (table->ks[index].busy == 1) {
                if (strcmp(find_key, table->ks[index].key) == 0) {
                    Node *ptr = table->ks[index].info;
                    while (table->ks[index].info) {
                        add(&output_table, find_key, table->ks[index].info->info[0]);
                        table->ks[index].info = table->ks[index].info->next;
                        len_table++;
                    }
                    table->ks[index].info = ptr;
                    if (len_table == max_release) {
                        break;
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < table->msize; ++i) {
            int index = hash(hash1(find_key), hash2(find_key), i, *table);
            if (strcmp(find_key, table->ks[index].key) == 0 && table->ks[index].busy == 1) {
                Node *ptr = table->ks[index].info;
                while (table->ks[index].info) {
                    if (table->ks[index].info->release == release) {
                        add(&output_table, find_key, table->ks[index].info->info[0]);
                        break;
                    }
                    table->ks[index].info = table->ks[index].info->next;
                }
                table->ks[index].info = ptr;
            }
        }
    }
    //output_table.msize = len_table;
    output(&output_table);
    free_table(&output_table);
    free(find_key);
    return 0;
}


int d_find(Table *table) {
    Table output_table;
    output_table.msize = table->msize;
    output_table.ks = (KeySpace *) malloc(sizeof(KeySpace) * output_table.msize);
    for (int i = 0; i < output_table.msize; ++i) {
        output_table.ks[i].key = malloc(1);
        output_table.ks[i].key[0] = '\0';
        output_table.ks[i].busy = 0;
        output_table.ks[i].info = NULL;
    }
    int len_table = 0;
    printf("Enter the key would you like to find:\n");
    int flag2 = 0;
    char *find_key;
    do {
        find_key = create_str();
        if (!find_key) {
            free(find_key);
            free_table(&output_table);
            return 2;
        }
        int k = 0;
        while (k < table->msize) {
            if (table->ks[k].busy == 1) {
                if (strcmp(table->ks[k].key, find_key) == 0) {
                    flag2 = 1;
                    break;
                }
            }
            k++;
        }
        if (flag2 != 1) {
            free(find_key);
            printf("Table doesn't contain this key. Try again:\n");
        }
    } while (flag2 != 1);
    printf("You would like to delete all releases of this key or only one version?\n");
    printf("Enter 1, if all releases.\n");
    printf("Enter 2, if only one version.\n");
    int flag = 0;
    while (flag != 1 && flag != 2) {
        if (get_int(&flag) != 1) {
            printf("Error! Restart program, please.\n");
            free(find_key);
            return 2;
        }
        if (flag != 2 && flag != 1) {
            printf("You've entered incorrect data. Try again:\n");
        }
    }
    int release = 0;
    if (flag == 2) {
        printf("Enter the release:\n");
        int max_release = count_release(*table, find_key) - 1;
        do {
            if (get_int(&release) != 1) {
                printf("Error! Restart program, please.\n");
                free(find_key);
                return 2;
            }
            if (release > max_release || release == 0) {
                printf("You've entered incorrect data. Try again:\n");
            }
        } while (release > max_release || release == 0);
    }
    find(table, find_key, output_table, release, flag, len_table);
    return 0;
}

int dialog(const char *msgs[], int len_msg) {
    int rc;
    for (int i = 0; i < len_msg; ++i) {
        puts(msgs[i]);
    }
    do {
        if (get_int(&rc) != 1) {
            printf("Error! Restart program, please.\n");
            return -1;
        }
    } while (rc < 0 || rc > len_msg - 1);
    printf("%d\n", rc);
    return rc;
}

int read_file(Table *table, FILE *start, char *st) {
    char buffer[256];
    if (start) {
        int i = 0;
        char *str;
        while ((fgets(buffer, 256, start)) != NULL) {
            if (i % 2 == 0) {
                str = strdup(buffer);
                str[strlen(str)-1] = '\0';
            } else {
                int info = atoi(buffer);
                add(table, str, info);
                free(str);
            }
            i++;
        }
        fclose(start);
    }
    free(st);
    return 0;
}

int d_read_file(Table *table){
    FILE *start;
    printf("Enter file name:\n");
    char *st = create_str(), st1[100], st2[100];
    start = fopen(st, "r");
    if (start == NULL) {
        printf("No such file exists.\n");
        free(st);
        return 1;
    }
    read_file(table, start, st);
    return 0;
}

int main() {
    const char *msg[] = {"0. Quit",
                         "1. Add element in the table",
                         "2. Remove element in the table",
                         "3. Show element by a key in a the table",
                         "4. Show whole table",
                         "5. Get table from the file"};
    const int len_msg = sizeof(msg) / sizeof(msg[0]);

    Table table;
    int count;
    printf("Enter the maximum count of elements in keyspace:\n");
    if (get_int(&count) != 1) {
        printf("Error! Restart program, please.\n");
        return 0;
    }
    table.msize = 1;
    while (table.msize < count) {
        table.msize = table.msize * 2;
    }
    printf("Your maximum count of elements will be convented to the next power of 2.\n");
    printf("Your new size of the table: %d\n", table.msize);
    table.ks = (KeySpace *) malloc(sizeof(KeySpace) * table.msize);
    for (int i = 0; i < table.msize; ++i) {
        table.ks[i].key = malloc(1);
        table.ks[i].key[0] = '\0';
        table.ks[i].busy = 0;
        table.ks[i].info = NULL;
    }
    int (*fptr[])(Table *) = {NULL, d_add, d_remove_element, d_find, output, d_read_file};
    int rc = 142412;
    while (rc != 0) {
        if ((rc = dialog(msg, len_msg)) == -1) {
            break;
        }
        if (rc == 0) {
            break;
        }
        if (fptr[rc](&table) == 2) {
            break;
        }
    }
    printf("That's all. Bye!\n");
    free_table(&table);
    return 0;
}
