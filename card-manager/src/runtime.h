#ifndef OBOO_RUNTIME_H
#define OBOO_RUNTIME_H

#include "config.h"

#include "duktape/duktape.h"

duk_context *ctx;

void initRuntime();
void destroyRuntime();

void handleMessage(char* topic, char* payload);


#endif  // OBOO_RUNTIME_H