#include "FileSystemManager.h"
#include "FileSystemTree.h"

int fst_init(tFileSystemTree *fst)
{
    // Initailize current path
    memset(fst->current_path, INVALID_INDEX, sizeof(int)*MAX_PATH_DEPTH);

    // 初始化node_table，並產生 root 目錄
    node_table_init(fst);
    
    //current_path 放入索引值 0，也就是 root
    fst->current_path[fst->current_path_depth] = 0;
    return 0;
}

int node_table_init(tFileSystemTree  *fst)
{
    memset(fst->node_table, -1, sizeof(tNode)*NODE_NUM);
    strncpy(fst->node_table[0].file_name, "root", FILE_NAME_LENGTH);
    fst->node_table[0].node_type = TYPE_DIRE;
    fst->free_node = NODE_NUM - 1;
    return 0;
}

tNode *find_node(tFileSystemTree  *fst, char *name, int node_type)
{
    tNode *node_table = fst->node_table;
    int current_directory_index;

    current_directory_index = fst->current_path[fst->current_path_depth];
    int child_index = node_table[current_directory_index].child_index;

    while(child_index != INVALID_INDEX)
    {
        if(strcmp(node_table[child_index].file_name, name) == 0 && node_table[child_index].node_type == node_type)
        {
            return node_table+child_index;
        }
        child_index = node_table[child_index].sibling_index_r;
    }
    return NULL;
}

int add_child(tFileSystemTree  *fst, char *file_name, int file_size, int file_loc, int node_type)
{
    int index = 0, current_directory_index;

    current_directory_index = fst->current_path[fst->current_path_depth];

    while(1)
    {
        // No space
        if(index >= NODE_NUM)
        {
            return -1;
        }
        // 找到未使用的Node
        if(fst->node_table[index].node_type == TYPE_NONE)
        {
            break;
        }
        index++;
    }

    strncpy(fst->node_table[index].file_name, file_name, FILE_NAME_LENGTH);
    fst->node_table[index].file_size = file_size;
    fst->node_table[index].file_loc = file_loc;
    fst->node_table[index].node_type = node_type;    
    fst->node_table[index].parent_index  = current_directory_index;
    
    // check current node 有沒有 child node
    if(fst->node_table[current_directory_index].child_index == INVALID_INDEX)
    {
        fst->node_table[current_directory_index].child_index = index;
        fst->free_node--;
        return index;
    }

    // 找到最右邊的 node
    int sibling_index = fst->node_table[current_directory_index].child_index;
    while(1)
    {
        if(fst->node_table[sibling_index].sibling_index_r == INVALID_INDEX)
        {
            fst->node_table[sibling_index].sibling_index_r = index;
            fst->node_table[index].sibling_index_l   = sibling_index;
            fst->free_node--;
            return index;
        }
        sibling_index = fst->node_table[sibling_index].sibling_index_r;
    }
}

int del_file_node(tFileSystemTree *fst, tNode *target_node)
{
    tNode           *node_table;
    tMEM            *mask_space;    

    node_table = fst->node_table;
    if(target_node->node_type != TYPE_FILE)
    {
        return -1;        
    }
    //////////////////////////////
    if(target_node->sibling_index_l != INVALID_INDEX)
    {
        node_table[target_node->sibling_index_l].sibling_index_r = target_node->sibling_index_r;
    }
    if(target_node->sibling_index_r != INVALID_INDEX)
    {
        node_table[target_node->sibling_index_r].sibling_index_l = target_node->sibling_index_l;
    }
    //檢查此node是否為parent node的child node
    tNode *parent_node = node_table+target_node->parent_index;
    if(node_table+(parent_node->child_index) == target_node)
    {
        parent_node->child_index = target_node->sibling_index_r;
    }
    memset(target_node, -1, sizeof(tNode));
    /////////////////////////////
    fst->free_node++;
    return 0;
}

int del_dire_node(tFileSystemTree *fst, tNode *target_node)
{
    tNode *node_table = fst->node_table;

    if(target_node->node_type != TYPE_DIRE)
    {
        return -1;
    }

    int child_index = target_node->child_index; 
    while(child_index != INVALID_INDEX)
    {
        if(node_table[child_index].node_type == TYPE_DIRE)
        {
            del_dire_node(fst, node_table+child_index);
        }
        else if(node_table[child_index].node_type == TYPE_FILE)
        {
            del_file_node(fst, node_table+child_index);
        }
        child_index = node_table[child_index].sibling_index_r;
    }
    //////////////////////////////
    if(target_node->sibling_index_l != INVALID_INDEX)
    {
        node_table[target_node->sibling_index_l].sibling_index_r = target_node->sibling_index_r;
    }
    if(target_node->sibling_index_r != INVALID_INDEX)
    {
        node_table[target_node->sibling_index_r].sibling_index_l = target_node->sibling_index_l;
    }
    // 檢查此node是否為parent node的child node
    tNode *parent_node = node_table+target_node->parent_index;
    if(node_table+(parent_node->child_index) == target_node)
    {
        parent_node->child_index = target_node->sibling_index_r;
    }
    memset(target_node, -1, sizeof(tNode));
    /////////////////////////////
    fst->free_node++;

    return 0;
}

int print_current_path(tFileSystemTree  *fst)
{
    printf("/");
    for(int i = 1; i <= fst->current_path_depth; i++)
    {
        printf("%s/", fst->node_table[fst->current_path[i]].file_name);
    }    
    return 0;
}