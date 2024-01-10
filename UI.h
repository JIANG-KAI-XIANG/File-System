/*

*/
#ifndef UI_H
#define UI_H
///////////////////////////////
#include "FileSystemManager.h"
#include "Dump.h"
///////////////////////////////
#define STRING_MAX_LENGTH       100   
#define ARGUMENT_MAX_LENGTH     20
#define ARGUMENTS_MAX_NUM       3    
#define SYSTEM_EXIT             100
#define DEFAULT_DUMP_PATH       "dump"
///////////////////////////////
typedef char tARG[ARGUMENT_MAX_LENGTH];

typedef struct cmd_info
{
    char *cmd_name;
    int (*cmd_func)(tFileSystemManager *fsm, int argc, tARG *argv);
}tCmdInfo;
///////////////////////////////
int options(tFileSystemManager **fsm_ptr);
int get_command(tFileSystemManager *fsm);

// command function with UI ////////////////////////////////
// list directory
int cmd_ls(tFileSystemManager *fsm, int argc, tARG *argv);            
// change directory         
int cmd_cd(tFileSystemManager *fsm, int argc, tARG *argv);   
// remove file
int cmd_rm(tFileSystemManager *fsm, int argc, tARG *argv);   
// make directory
int cmd_mkdir(tFileSystemManager *fsm, int argc, tARG *argv);   
// remove directory
int cmd_rmdir(tFileSystemManager *fsm, int argc, tARG *argv);   
// put file into the space      
int cmd_put(tFileSystemManager *fsm, int argc, tARG *argv);   
// get file into the space                
int cmd_get(tFileSystemManager *fsm, int argc, tARG *argv);   
// show content
int cmd_cat(tFileSystemManager *fsm, int argc, tARG *argv);   
// show status of the space
int cmd_status(tFileSystemManager *fsm, int argc, tARG *argv);
// help
int cmd_help(tFileSystemManager *fsm, int argc, tARG *argv);   
// exit
int cmd_exit(tFileSystemManager *fsm, int argc, tARG *argv);
///////////////////////////////
#endif