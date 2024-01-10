/*

*/
#include "Dump.h"

int save_dump(tFileSystemManager *fsm, char *file_name)
{
    FILE *file;
    file = fopen(file_name, "wb");

    // write to memory space
    fwrite(fsm->mem_space, 1, fsm->mem_space_size, file);

    fclose(file);
    return 0;
}

int load_dump(tFileSystemManager **fsm_ptr, char *file_name)
{
    FILE *file;
    tMEM *mem_space;

    int  file_size;    

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

    mem_space = (tMEM*)malloc(sizeof(tMEM)*file_size);
    
    // read to memory space
    fread(mem_space, 1, file_size, file);

    *fsm_ptr = (tFileSystemManager *)mem_space;
    (*fsm_ptr)->mem_space = mem_space;

    //close file
    fclose(file);
    return 0;
}

