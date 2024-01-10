/*

*/
#ifndef FILE_SYSTEM_MANAGER_H
#define FILE_SYSTEM_MANAGER_H
///////////////////////////////////
#include <sys/stat.h>
///////////////////////////////////
#include "Memory.h"
///////////////////////////////////
#define FSM_SPACE_SIZE     48
///////////////////////////////////
typedef struct FileSystemManager
{
   int      fsm_space_size;
   int      fsm_space_loc;
   //////////////////////////////
   int      fst_space_size;
   int      fst_space_loc;
   //////////////////////////////
   int      mask_space_size;
   int      mask_space_loc;
   //////////////////////////////
   int      file_space_size;
   int      file_space_loc;
   //////////////////////////////
   int      free_space;
   //////////////////////////////
   int      mem_space_size;
   tMEM     *mem_space;
   //////////////////////////////
   //size = 4*10 + 8 = 48
}tFileSystemManager;
///////////////////////////////////
tFileSystemManager   *fsm_init(tMEM *mem_space, int mem_space_size);
// command funtion //////////////////////////////////////////////////////////////////////
// list directory
int                  list_directory_content(tFileSystemManager *fsm);            
// change directory         
int                  change_directory(tFileSystemManager *fsm, char *path);
// remove file
int                  remove_file(tFileSystemManager *fsm, char *file_name);     
// make directory
int                  make_directory(tFileSystemManager *fsm, char *directory_name); 
// remove directory
int                  remove_directory(tFileSystemManager *fsm, char *directory_name); 
// put file into the space      
int                  put_file(tFileSystemManager *fsm, char *file_name);  
// get file into the space                
int                  get_file(tFileSystemManager *fsm, char *file_name, char *dump_directory_name);
// show content
int                  show_content(tFileSystemManager *fsm, char *file_name);
// show status of the space
int                  show_status(tFileSystemManager *fsm);
// help
int                  help(void);
// exit
int                  fs_exit(tFileSystemManager *fsm, char *file_name);
///////////////////////////////////
#endif /* FILE_SYSTEM_MANAGER_H */