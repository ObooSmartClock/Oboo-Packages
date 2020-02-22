#if !defined(DUKTAPE_FILEIO_INCLUDED)
#define DUKTAPE_FILEIO_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "duktape.h"



/* Push file as a buffer. */
void fileio_push_file_buffer(duk_context *ctx, const char *filename);

/* Push file as a string. */
int fileio_push_file_string(duk_context *ctx, const char *filename);

int fileio_readfile(duk_context *ctx);

int fileio_readfile_string(duk_context *ctx);

void fileio_register(duk_context *ctx);

void setDefaultDirectory(char *folderPath);
void setGlobalDirectory(char *folderPath);

#endif