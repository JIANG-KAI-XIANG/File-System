/*

*/
/////////////////////////////////////
#include "FileSystemManager.h"
#include "FileSystemTree.h"
#include "Dump.h"
/////////////////////////////////////
tFileSystemManager *fsm_init(tMEM *mem_space, int mem_space_size)
{
    tFileSystemManager *fsm = (tFileSystemManager *)mem_space;

    fsm->mem_space_size = mem_space_size;
    fsm->mem_space      = mem_space;

    fsm->fsm_space_size = FSM_SPACE_SIZE;
    fsm->fsm_space_loc  = 0;

    fsm->fst_space_size = sizeof(tFileSystemTree);
    fsm->fst_space_loc  = fsm->fsm_space_loc + fsm->fsm_space_size;
    ////////////////////////////////////////////////
    int  remaing_space_size = fsm->mem_space_size - fsm->fsm_space_size - fsm->fst_space_size;
    
    fsm->mask_space_size = remaing_space_size/9;
    fsm->mask_space_loc  = fsm->fst_space_loc + fsm->fst_space_size;
    
    fsm->file_space_size = fsm->mask_space_size << 3;
    fsm->file_space_loc  = fsm->mask_space_loc + fsm->mask_space_size;

    fsm->free_space      = fsm->file_space_size;    

    return fsm;
}


// command funtion //////////////////////////////////////////////////////////
// ls
int list_directory_content(tFileSystemManager *fsm)
{
    tFileSystemTree  *fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    int index, current_directory_index;

    current_directory_index = fst->current_path[fst->current_path_depth];
    index = fst->node_table[current_directory_index].child_index;
    //
    while(index != INVALID_INDEX)
    {
        if(fst->node_table[index].node_type == TYPE_DIRE)
        {
            printf("\033[94m");
        }        
        printf("%s ",fst->node_table[index].file_name);
        printf("\033[0m");
        index = fst->node_table[index].sibling_index_r;
    }
    printf("\n");
    return 0;
}
//cd
int change_directory(tFileSystemManager *fsm, char *path)
{
    tFileSystemTree  *fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    int current_directory_index;
    char path_cpy[FILE_NAME_LENGTH];
    char *directory_name;
    char *next_path = NULL;

    current_directory_index = fst->current_path[fst->current_path_depth];
    strcpy(path_cpy, path);
    directory_name = strtok_s(path_cpy, "/", &next_path);
    //
    if(directory_name == NULL)
    {
        return 0;
    }

    //
    if(strcmp(directory_name, ".") == 0)
    {
        change_directory(fsm, next_path);
        return 0;
    }
        
    //返回上一層
    if(strcmp(directory_name, "..") == 0)
    {
        //沒有上一層
        if(fst->node_table[current_directory_index].parent_index == INVALID_INDEX)
        {
            return -1;
        }
        else
        {
            fst->current_path[fst->current_path_depth--] = INVALID_INDEX;

            if(change_directory(fsm, next_path) < 0)
            {
                return -2;
            }
            return 1;
        }
    }

    //從此path尋找目標directory
    int index = fst->node_table[current_directory_index].child_index;
    while(index > 0)
    {
        int isSameName = !strcmp(fst->node_table[index].file_name, directory_name);
        int isDirectory = (fst->node_table[index].node_type == TYPE_DIRE);
        if(isSameName && isDirectory)
        {
            fst->current_path[++fst->current_path_depth] = index;

            return change_directory(fsm, next_path);
        }
        index = fst->node_table[index].sibling_index_r;
    }
    
    return -4;
}
// rm
int remove_file(tFileSystemManager *fsm, char *file_name)
{
    tFileSystemTree *fst;
    tNode   *target_node;
    tMEM    *mask_space;
    int file_size;

    fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    target_node = find_node(fst, file_name, TYPE_FILE);
    if(target_node == NULL)
    {
        return -1;
    }    

    mask_space = fsm->mem_space + fsm->mask_space_loc;
    if(clr_location(mask_space, (target_node->file_loc)-(fsm->file_space_loc), target_node->file_size) < 0)
    {
        return -3;
    }
    file_size = target_node->file_size;
    if(del_file_node(fst, target_node) < 0)
    {
        return -2;
    }
    fsm->free_space += file_size;  
    return 0;
}
// mkdir
int make_directory(tFileSystemManager *fsm, char* directory_name)
{
    tFileSystemTree  *fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    
    // 確認是否已存在同名 node
    if(find_node(fst, directory_name, TYPE_DIRE) != NULL)
    {
        return -2;
    }
    if(add_child(fst, directory_name, -1, -1, TYPE_DIRE) < 0)
    {
        return -1;
    }
    return 0;
}
// rmdir
int remove_directory(tFileSystemManager *fsm, char *directory_name)
{
    tFileSystemTree *fst;
    tNode *target_node;
    
    fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    target_node = find_node(fst, directory_name, TYPE_DIRE);
    if(target_node == NULL)
    {
        return -1;
    }   
    if(del_dire_node(fst, target_node) < 0) 
    {
        return -2;
    }
    return 0;
}
// put
int put_file(tFileSystemManager *fsm, char* file_name)
{
    tFileSystemTree  *fst;
    FILE *file;
    tMEM *mask_space, *file_space;
    int  file_size, file_loc;      

    fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);

    // 確認是否已存在同名 node
    if(find_node(fst, file_name, TYPE_FILE) != NULL)
    {
        return -4;
    }

    // read file as binary
    file = fopen(file_name, "rb");
    if(!file) 
    {
        return -1;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // check mask
    mask_space      = fsm->mem_space + fsm->mask_space_loc;
    int usable_loc = get_location(mask_space, fsm->mask_space_size, file_size);
    if(usable_loc < 0)
    {
        return -2;
    }
    
    set_location(mask_space, usable_loc, file_size);    
    fsm->free_space -= file_size;
    file_loc = fsm->file_space_loc + usable_loc;       
    
    // read to file space
    file_space = (fsm->mem_space + file_loc);
    fread(file_space, 1, file_size, file);

    //close file
    fclose(file);
    ///////////////////////////////////
    if(add_child(fst, file_name, file_size, file_loc, TYPE_FILE) < 0)
    {
        return -3;
    }
    return 0;
}
//get
int get_file(tFileSystemManager *fsm, char *file_name, char *dump_directory_name)
{
    tFileSystemTree  *fst;
    FILE *file;
    struct stat st;
    int  result;
    char file_path[FILE_NAME_LENGTH];

    sprintf(file_path, "%s%s%s", dump_directory_name, "/", file_name);
    fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);

    tNode *target_node = find_node(fst, file_name, TYPE_FILE);
    if(target_node == NULL)
    {
        return -1;
    }    
    
    // 確認資料夾是否存在
    result = stat(dump_directory_name, &st);    
    // 不存在則創建資料夾
    if(result != 0 || S_ISDIR(st.st_mode) == 0)
    {
        result = mkdir(dump_directory_name);
    }
    // open file and write
    file = fopen(file_path, "wb");
    fwrite(fsm->mem_space+target_node->file_loc, 1, target_node->file_size, file);
    fclose(file);

    // remove file
    remove_file(fsm, file_name);
    return 0;
}
// cat
int show_content(tFileSystemManager *fsm, char *file_name)
{
    tFileSystemTree  *fst;
    tNode *target_node;

    fst = (tFileSystemTree  *)(fsm->mem_space + fsm->fst_space_loc);
    target_node = find_node(fst, file_name, TYPE_FILE);

    if(target_node == NULL)
    {
        return -1;
    }

    for(int i = 0; i < target_node->file_size; i++)
    {
        printf("%c",*(fsm->mem_space + target_node->file_loc+i));
    }
    printf("\n");
    return 0;
}
// status
int show_status(tFileSystemManager *fsm)
{
    tFileSystemTree *fst = (tFileSystemTree *)(fsm->mem_space + fsm->fst_space_loc);

    printf("partition size:\t%d\n", fsm->mem_space_size);
    printf("total nodes:\t%d\n", NODE_NUM);
    printf("used nodes:\t%d\n", NODE_NUM - fst->free_node);
    printf("free space:\t%d\n",fsm->free_space);
    return 0;
}
// help
int help(void)
{
    printf("List of commands\n");
    printf(" 'ls' list directory\n");
    printf(" 'cd' change directory\n");
    printf(" 'rm' remove file\n");
    printf(" 'mkdir' make directory\n");
    printf(" 'rmdir' remove directory\n");
    printf(" 'put' put file into the space \n");
    printf(" 'get' get file into the space \n");
    printf(" 'cat' show content\n");
    printf(" 'status' show status of the space\n");
    printf(" 'help' help\n");
    printf(" 'exit' exit and store img\n");
    return 0;
}

int fs_exit(tFileSystemManager *fsm, char *file_name)
{
    if(save_dump(fsm, file_name) < 0)
    {
        return -1;
    }

    // 釋放記憶體
    free(fsm->mem_space);
    return 0;
}