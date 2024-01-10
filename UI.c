/*

*/
#include "UI.h"

tCmdInfo cmdTable[] = {
    {"ls"       , cmd_ls},
    {"cd"       , cmd_cd},
    {"rm"       , cmd_rm},
    {"mkdir"    , cmd_mkdir},
    {"rmdir"    , cmd_rmdir},
    {"put"      , cmd_put},
    {"get"      , cmd_get},
    {"cat"      , cmd_cat},
    {"status"   , cmd_status},
    {"help"     , cmd_help},
    {"exit"     , cmd_exit},    
    {NULL       , NULL}
};

int options(tFileSystemManager **fsm_ptr)
{
    tFileSystemTree  *fst;
    char str_in[STRING_MAX_LENGTH], *endptr;
    long choice;
    
    printf("options:\n");
    printf("  1. loads from file\n");
    printf("  2. create new partition in memory\n => ");

    if(fgets(str_in, sizeof(str_in) , stdin) == NULL)
    {
        printf("Input error\n\n");
        return -1;
    }
    
    choice = strtol(str_in, &endptr, 10);
    if(*endptr != '\n' && *endptr != '\0')
    {
        printf("Input error\n\n");
        return -2;
    }

    if(choice == 1)
    {
        int ret = -1;
        char dump_file_name[FILE_NAME_LENGTH];
        while(ret)
        {
            printf("Enter file name => ");
            scanf("%s", dump_file_name);
            ret = load_dump(fsm_ptr, dump_file_name);
            if(ret == 0)
            {
                break;
            }
            printf("No such file\n\n");
        }        
        return 0;
    }
    else if(choice == 2)
    {
        tMEM *mem;
        int mem_space_size = -1;
        int res;

        while(mem_space_size < MIN_MEM_SIZE)
        {
            printf("Input size of a new partition (min = %d) => ", MIN_MEM_SIZE);
            //scanf("%d", &mem_space_size);
            if(fgets(str_in, sizeof(str_in) , stdin) == NULL)
            {
                printf("fgets error\n\n");
                continue;
            }
            mem_space_size = strtol(str_in, &endptr, 10);
            if(*endptr != '\n' && *endptr != '\0')
            {
                printf("Input error\n\n");
                continue;
            }
            if(mem_space_size >= MIN_MEM_SIZE)
            {
                break;
            }
        }
        mem = mem_init(mem_space_size);
        *fsm_ptr = fsm_init(mem, mem_space_size);
        fst = (tFileSystemTree*) ( (*fsm_ptr)->mem_space + (*fsm_ptr)->fst_space_loc );
        res = fst_init(fst);
        return 1;
    }
    else
    {
        printf("No such choice\n\n");
        return -3;
    }
}

int get_command(tFileSystemManager *fsm)
{
    tFileSystemTree *fst;
    tCmdInfo        *cmd_table_ptr;
    tARG            cmd_name, argv[ARGUMENTS_MAX_NUM];
    char            cmd[STRING_MAX_LENGTH], *token, *next_str = NULL;
    int             argc, ret;

    // current_path/ $ ...
    fst = (tFileSystemTree *)(fsm->mem_space + fsm->fst_space_loc);
    print_current_path(fst);
    printf(" $ ");   
    
    // init argv
    memset(argv, 0, sizeof(argv));
    
    // clear stdin
    fflush(stdin);

    // get input
    fgets(cmd, sizeof(cmd), stdin);
    
    // only enter
    if(cmd[0] == '\n')
    {
        return -1;
    }
    // Remove trailing newline character
    size_t len = strlen(cmd);
    if (len > 0 && cmd[len - 1] == '\n') {
        cmd[len - 1] = '\0';
    }

    // get command name
    token = strtok_s(cmd, " ", &next_str);
    strcpy(cmd_name, token);

    // get arguments
    argc = 0;
    while (next_str != NULL)
    {
        token = strtok_s(NULL, " ", &next_str);
        if(token == NULL)
        {
            break;
        }
        strcpy(argv[argc++], token);
    }

    // 確認 cmd 是否存在
    cmd_table_ptr = cmdTable;
    while(1)
    {
        if(cmd_table_ptr->cmd_name == NULL)
        {
            break;
        }
        if(strcmp(cmd_name, cmd_table_ptr->cmd_name) == 0)
        {
            ret = cmd_table_ptr->cmd_func(fsm, argc, argv);
            return ret;
        }
        cmd_table_ptr++;
    }        
    printf("No such command\n");

    return -1;
}


// list directory
int cmd_ls(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc>0)
    {
        printf("Too much arguments\n");
        return -1;
    }    
    return list_directory_content(fsm);
}    
// change directory         
int cmd_cd(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }
    ret = change_directory(fsm, argv[0]);
    if(ret < 0)
    {
        printf("No such path\n");
    }
    return ret;
}  
// remove file
int cmd_rm(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc < 1)
    {
        printf("Need a file name\n");
        return -1;
    }
    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }
    
    ret = remove_file(fsm, argv[0]);
    if(ret < 0)
    {
        printf("No such file\n");
    }
    return ret;
}    
// make directory
int cmd_mkdir(tFileSystemManager *fsm, int argc, tARG *argv)
{    
    int ret;
    
    if(argc < 1)
    {
        printf("Need a name of the directory\n");
        return -1;
    }
    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }
    
    ret = make_directory(fsm, argv[0]);
    if(ret == -2)
    {
        printf("\'%s\' is exist\n", argv[0]);
    }
    return ret;
}    
// remove directory
int cmd_rmdir(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc < 1)
    {
        printf("Need a directory name\n");
        return -1;
    }
    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }

    ret = remove_directory(fsm, argv[0]);
    if(ret == -1)
    {
        printf("No such directory\n");
    }
    return ret;    
}    
// put file into the space      
int cmd_put(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc < 1)
    {
        printf("Need a file name\n");
        return -1;
    }
    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }

    ret = put_file(fsm, argv[0]);
    if(ret == -1)
    {
        printf("failed to open \'%s\'\n", argv[0]);
    }
    else if(ret == -2)
    {
        printf("No enough space\n");
    }
    else if(ret == -4)
    {
        printf("\'%s\' is exist\n", argv[0]);
    }
    return ret;
}    
// get file into the space                
int cmd_get(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc < 1)
    {
        printf("Need a file name\n");
        return -1;
    }
    if(argc > 2)
    {
        printf("Too much arguments\n");
        return -1;
    }

    ret = get_file(fsm, argv[0], argc>1 ? argv[1]:DEFAULT_DUMP_PATH);
    if(ret == -1)
    {
        printf("No such file\n");
    }
    return ret;
}     
// show content
int cmd_cat(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;

    if(argc < 1)
    {
        printf("Need a file name\n");
        return -1;
    }
    if(argc > 1)
    {
        printf("Too much arguments\n");
        return -1;
    }
    ret = show_content(fsm, argv[0]);
    if(ret == -1)
    {
        printf("No such file\n");
    }
    return ret;
}   
// show status of the space
int cmd_status(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;   

    if(argc > 0)
    {
        printf("Too much arguments\n");
        return -1;
    }
    ret = show_status(fsm);
    return ret;
}   
// help
int cmd_help(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;
    
    if(argc > 0)
    {
        printf("Too much arguments\n");
        return -1;
    }
    ret = help();
    return ret;
}  
// exit
int cmd_exit(tFileSystemManager *fsm, int argc, tARG *argv)
{
    int ret;
    char dump_file_name[FILE_NAME_LENGTH];
    printf("Enter file name => ");
    scanf("%s", dump_file_name);
    if(strlen(dump_file_name) == 0)
    {
        strcpy(dump_file_name, "my_fs.dump");
    }
    ret = fs_exit(fsm, dump_file_name);
    if(ret == 0)
    {
        return SYSTEM_EXIT;
    }
    return ret;
}  