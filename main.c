#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/////////////////////////////////////
#include "Memory.h"
#include "FileSystemTree.h"
#include "FileSystemManager.h"
#include "Dump.h"
#include "UI.h"
/////////////////////////////////////

int main(int argc, char* argv[])
{
    tFileSystemManager  *fsm = NULL;
    int ret = -1;

    while (ret < 0)
    {            
        ret = options(&fsm);
    }
    while (1)
    {
        ret = get_command(fsm);
        if(ret == SYSTEM_EXIT)
        {
            break;
        }
    }    

    return 0;
}
