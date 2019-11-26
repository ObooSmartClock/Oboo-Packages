#ifndef OBOO_RUNTIME_H
#define OBOO_RUNTIME_H


#include <stdio.h>
#include "duktape/duktape.h"

#define FILE_RD_FLAGS   "rb"
#define FILE_WR_FLAGS   "w"

#define MAX_CHAR_LEN    256
#define MAX_DATA_SIZE   8192

#define CB_PREFIX       "_cb"
#define JS_NULL         "null"

struct MemoryStruct {
  char *memory;
  size_t size;
};

struct ThreadInf {
    pthread_t thread;
    char command[MAX_CHAR_LEN];
    char run;
    int pId;
    int index;
    FILE *in;
};

duk_context *ctx;

void printHelp ();

void initRuntime();
void destroyRuntime();
int loadJS (char* filename, char* path);

void getExeDir (char *path);

void handleMessage(char* topic, char* payload);

void invokeCallback(char* cbIndex);

void runSetup();
int runLoop();

duk_ret_t native_print(duk_context*);
duk_ret_t native_sleep(duk_context*);

duk_ret_t native_mqConnect(duk_context *ctx);
duk_ret_t native_subscribe(duk_context *ctx);
duk_ret_t native_publish(duk_context *ctx);

duk_ret_t native_initCard(duk_context *ctx);
duk_ret_t native_updateCard(duk_context *ctx);
duk_ret_t native_deleteCard(duk_context *ctx);

duk_ret_t native_httpRequest(duk_context *ctx);

duk_ret_t native_setButton(duk_context *ctx);
duk_ret_t native_setNightLight(duk_context *ctx);

duk_ret_t native_updateSystemTime(duk_context *ctx);

duk_ret_t native_openSerialPort(duk_context *ctx);
duk_ret_t native_serialWrite(duk_context *ctx);
duk_ret_t native_serialWriteByteArray(duk_context *ctx);
duk_ret_t native_serialRead(duk_context *ctx);
duk_ret_t native_serialReadByteArray(duk_context *ctx);

duk_ret_t native_writeFile(duk_context *ctx);
duk_ret_t native_readFile(duk_context *ctx);

duk_ret_t native_setGpio(duk_context *ctx);
duk_ret_t native_playMp3(duk_context *ctx);
duk_ret_t native_systemCall(duk_context *ctx);
duk_ret_t native_systemPopen(duk_context *ctx);
duk_ret_t native_launchProcess(duk_context *ctx);
duk_ret_t native_killProcess(duk_context *ctx);
duk_ret_t native_checkProcess(duk_context *ctx);


#endif // OBOO_RUNTIME_H
