/*

*/
#ifndef MEMORY_H
#define MEMORY_H
///////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///////////////////////////////////
#define BYTE_SIZE       8
#define MIN_MEM_SIZE    8000
///////////////////////////////////
typedef unsigned char tMEM;
///////////////////////////////////
#include "FileSystemManager.h"
///////////////////////////////////
tMEM    *mem_init(int mem_space_size);
int     get_location(tMEM *mask_space, int mask_space_size, int file_size);
int     set_location(tMEM *mask_space, int location, int file_size);
int     clr_location(tMEM *mask_space, int location, int file_size);
///////////////////////////////////
#endif /* MEMORY_H */