#include "duktape/duktape.h"
#include "config.h"
#include "runtime.h"

int main(int argc, char *argv[]) {
    int status = 0;
    char binPath[MAX_CHAR_LEN];
    char filename[MAX_CHAR_LEN];


    if (argc < 2) {
        printHelp();
        return 0;
    } else {
        strcpy(filename, argv[1]);
    }


    /* Init Runtime */
    initRuntime();

    getExeDir(binPath);

    /* file operations */
    // loadJS("./js/lib.js", binPath);
    loadJS("./js/card-lib.js", binPath);
    loadJS("./js/yahooWeather.js", binPath); // TODO: this is a hack, fix this by loading modules from js
    status = loadJS(filename, binPath);

    if (status == EXIT_SUCCESS) {
      runSetup();

      while (status == EXIT_SUCCESS) {
          status = runLoop();
          usleep(100000);
      }
    }


    /* destroy runtime */
    destroyRuntime();
    return status;
}
