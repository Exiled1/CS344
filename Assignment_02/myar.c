#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

bool append_to_arch = false;        // -q
bool extract_files = false;         // -x
bool ls_brief_contents = false;     // -t
bool ls_verbose_contents = false;    // -v
bool delete_files = false;           // -d
bool quick_append = false;          // -A
bool time_file_add = false;         // -w (extra credit)

/**
 * @brief useful system commands for this will be:
 *  - int mkdir(const char *pathname, mode_t mode);
 *  - int rmdir(const char *pathname);
 *  - DIR *opendir(const char *name);
 *      - Needs:#include <sys/types.h>
                #include <dirent.h>
 *  - int closedir(DIR *dirp);
 *  - struct dirent *readdir(DIR *dirp); 
 */
int main(int argc, char const *argv[])
{
    
    return 0;
}
