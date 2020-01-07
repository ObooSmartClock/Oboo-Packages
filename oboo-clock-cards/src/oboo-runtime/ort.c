#include "duktape/duktape.h"
#include "duktape/fileio.h"
#include "config.h"
#include "runtime.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>


int main(int argc, char *argv[]) {
    char cwd[PATH_MAX+1];
    char globalDirPath[PATH_MAX+1];
    char tempDir[PATH_MAX+1];
    char libPath[MAX_CHAR_LEN]; // removing in next commit.

    int status = 0;
    char filename[MAX_CHAR_LEN];
    char *dirPath;
    char filePath[PATH_MAX+1];



    if (argc < 2) {
        printHelp();
        return 0;
    } else {
        strcpy(filename, argv[1]);
    }


    /* Init Runtime */
    initRuntime();

    getCWDir(cwd);

    getExeDir(tempDir);
    sprintf(globalDirPath,"%s",dirname(tempDir));
    setGlobalDirectory(globalDirPath);

    /* file operations */
    getFilePath(filePath, filename, cwd);
    dirPath = strdup(filePath);
    sprintf(dirPath,"%s",dirname(dirPath));
    setDefaultDirectory(dirPath);
    status = loadJS(filePath);

    if (status == EXIT_SUCCESS) {
      status = runSetup();

      while (status == EXIT_SUCCESS) {
          status = runLoop();
          usleep(100000);
      }
    }


    /* destroy runtime */
    destroyRuntime();
    return status;
}
