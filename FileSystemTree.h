/*
*/
#ifndef FILE_SYSTEM_TREE_H
#define FILE_SYSTEM_TREE_H
/////////////////////////////////////////////
#include <stdlib.h>
#include <string.h>
/////////////////////////////////////////////
#define NODE_NUM            100

#define FILE_NAME_LENGTH    20
#define MAX_PATH_DEPTH      20

#define TYPE_NONE           -1
#define TYPE_FILE           1
#define TYPE_DIRE           2

#define INVALID_INDEX       -1
/////////////////////////////////////////////
typedef struct node
{
    char    file_name[FILE_NAME_LENGTH];
    int     file_size;
    int     file_loc;
    /////////////
    int     node_type;
    /////////////
    int     parent_index;
    int     sibling_index_r; 
    int     sibling_index_l;   
    int     child_index;
    //size = 20+4+4+4 + 4+4+4+4 = 48
}tNode;

typedef struct FileSystemTree
{
    int     current_path[MAX_PATH_DEPTH];
    int     current_path_depth;

    int     free_node;
    
    tNode   node_table[NODE_NUM];
    //size = 4*20 + 4+4 + 48*100 = 80+8+4800 = 4888
}tFileSystemTree;
/////////////////////////////////////////////

int     fst_init(tFileSystemTree  *fst);
int     node_table_init(tFileSystemTree  *fst);
tNode   *find_node(tFileSystemTree  *fst, char *name, int node_type);
int     add_child(tFileSystemTree  *fst, char *file_name, int file_size, int file_loc, int node_type);
int     del_file_node(tFileSystemTree *fst, tNode *target_node);
int     del_dire_node(tFileSystemTree *fst, tNode *target_node);
int     print_current_path(tFileSystemTree  *fst);
#endif /* FILE_SYSTEM_TREE_H */