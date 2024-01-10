/*

*/
#ifndef DUMP_H
#define DUMP_H
///////////////////////////////////////////
#include "Memory.h"
#include "FileSystemManager.h"
#include "FileSystemTree.h"
///////////////////////////////////////////
int save_dump(tFileSystemManager *fsm, char *file_name);
int load_dump(tFileSystemManager **fsm_ptr, char *file_name);
///////////////////////////////////////////
#endif /* DUMP_H */