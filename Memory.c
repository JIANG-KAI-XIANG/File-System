/*

*/
#include "Memory.h"

tMEM *mem_init(int mem_space_size)
{
    tMEM *mem_space = (tMEM *)malloc(sizeof(tMEM)*mem_space_size);
    memset(mem_space, 0, sizeof(tMEM)*mem_space_size);
    return mem_space;
}

int get_location(tMEM *mask_space, int mask_space_size, int file_size)
{
    int usable_location     = -1;
    int current_loaction    = 0;
    int count = 0;
    tMEM check;

    for(int i=0 ; i<mask_space_size ; i++)
    {
        check = 0x80;
        for(tMEM j=0 ; j<BYTE_SIZE ; j++)
        {
            if((mask_space[i] & check) == 0)
            {
                if(usable_location == -1)
                {
                    usable_location = current_loaction;
                }
                count++;
                if(count >= file_size)
                {
                    return usable_location;
                }
            }
            else
            {
                usable_location = -1;
                count = 0;
            }
            check = check >> 1;
            current_loaction++;
        }
    }
    return -1;
}

int set_location(tMEM *mask_space, int location, int file_size)
{
    tMEM    set;
    int     row;
    
    row  = location/BYTE_SIZE;
    set = 0x80 >> (location % BYTE_SIZE);  

    for(int i=0 ; i<file_size ; i++)
    {
        mask_space[row] = mask_space[row]|set;
        if(set == 0x01)
        {
            row++;
            set = 0x80;
        }
        else
        {
            set = set >> 1;
        }
    }
    return 0;
}

int clr_location(tMEM *mask_space, int location, int file_size)
{
    tMEM    set;
    int     row;
    
    row  = location/BYTE_SIZE;
    set = 0x80 >> (location % BYTE_SIZE);  
    
    for(int i=0 ; i<file_size ; i++)
    {
        mask_space[row] = mask_space[row]&(~set);
        if(set == 0x01)
        {
            row++;
            set = 0x80;
        }
        else
        {
            set = set >> 1;
        }
    }
    return 0; 
}