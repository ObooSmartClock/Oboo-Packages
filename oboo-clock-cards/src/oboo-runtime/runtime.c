#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <time.h>

#include "runtime.h"
#include "config.h"
#include <curl/curl.h>
#include "messageQueue.h"
#include "serialPort.h"
#include "duktape/fileio.h"

#include <unistd.h>
#include <stdio.h>
#include <limits.h>

//popen
#include <string.h>
#include <errno.h>
//#include <libexplain/popen.h>

//alarm nonblocking loop/onetime/stop
#include <pthread.h>
struct ThreadInf aTi[MAX_NUMBER_OF_THREADS];
int curRun = 0; //currently running
int tIndex[MAX_NUMBER_OF_THREADS];


//queue for callbacks
#include <sys/queue.h>

struct callbacks {
    TAILQ_ENTRY(callbacks) cbq;
    char * callbackStr;
};

struct callbacksq q;
TAILQ_HEAD(callbacksq, callbacks);

const char * module_loader = "Duktape.modSearch = function (id) {  \n\
    /* readFile() reads a file from disk, and returns a string or undefined. \n\
     * 'id' is in resolved canonical form so it only contains terms and \n\
     * slashes, and no '.' or '..' terms. \n\
     */ \n\
    var res; \n\
     \n\
    id = ''+id; // make sure id is a string.\n\
    print('loading module:'+ id); \n\
     \n\
    if(!id.endsWith('.js')){ \n\
      id = id+'.js'; \n\
    } \n\
    res = FileIo.readfile('modules/' + id ); \n\
     \n\
    if (typeof res === 'string') { \n\
        return res; \n\
    } \n\
     \n\
    throw new Error('module not found: ' + id); \n\
}";

static void duktape_fatal_handler(void *udata, const char *msg) {
  (void) udata;  /* ignored in this case, silence warning */
    /* Note that 'msg' may be NULL. */
  fprintf(stderr, "*** FATAL ERROR: %s\n", msg ? msg : "no message");
  fprintf(stderr, "Causing intentional segfault...\n");
  fflush(stderr);
  *((volatile unsigned int *) 0) = (unsigned int) 0xdeadbeefUL;
  abort();
}

void initRuntime() {
  int status = 0;
  time_t t;

  /* Init random number generator */
  srand((unsigned) time(&t));

  /* Init Duktape */
  ctx = duk_create_heap(NULL, NULL, NULL, NULL, duktape_fatal_handler);

  /* Init Module Loader */
  duk_module_duktape_init(ctx);

  /* Init System Functions */
  duk_push_c_function(ctx, native_print, 1 /*nargs*/);
  duk_put_global_string(ctx, "print");

  duk_push_c_function(ctx, native_sleep, 1 /*nargs*/);
  duk_put_global_string(ctx, "sleep");

  duk_push_c_function(ctx, native_writeFile, 3 /*nargs*/);
  duk_put_global_string(ctx, "writeFile");

  duk_push_c_function(ctx, native_readFile, 3 /*nargs*/);
  duk_put_global_string(ctx, "readFile");


  fileio_register(ctx);

  duk_eval_string(ctx, module_loader);

  duk_push_c_function(ctx, native_setGpio, 2 /*nargs*/);
  duk_put_global_string(ctx, "setGpio");

  duk_push_c_function(ctx, native_playMp3, 1 /*nargs*/);
  duk_put_global_string(ctx, "playMP3");

  duk_push_c_function(ctx, native_systemCall, 1 /*nargs*/);
  duk_put_global_string(ctx, "system");

  duk_push_c_function(ctx, native_systemPopen, 1 /*nargs*/);
  duk_put_global_string(ctx, "popen");

  duk_push_c_function(ctx, native_launchProcess, 1 /*nargs*/);
  duk_put_global_string(ctx, "launchProcess");

  duk_push_c_function(ctx, native_killProcess, 1 /*nargs*/);
  duk_put_global_string(ctx, "killProcess");

  duk_push_c_function(ctx, native_checkProcess, 1 /*nargs*/);
  duk_put_global_string(ctx, "checkProcess");

  /* Init Card Functions */
  duk_push_c_function(ctx, native_initCard, 2 /*nargs*/);
  duk_put_global_string(ctx, "initCard");

  duk_push_c_function(ctx, native_updateCard, 1 /*nargs*/);
  duk_put_global_string(ctx, "updateCard");

  duk_push_c_function(ctx, native_deleteCard, 1 /*nargs*/);
  duk_put_global_string(ctx, "deleteCard");

  /* Init message queue functions */
  duk_push_c_function(ctx, native_mqConnect, 7 /*nargs*/);
  duk_put_global_string(ctx, "connect");

  duk_push_c_function(ctx, native_subscribe, 3 /*nargs*/);
  duk_put_global_string(ctx, "subscribe");

  duk_push_c_function(ctx, native_publish, 3 /*nargs*/);
  duk_put_global_string(ctx, "publish");

  /* Init Network Functions */
  duk_push_c_function(ctx, native_httpRequest, 1 /*nargs*/);
  /*
    options :
      method
      url
      header
      body
  */
  duk_put_global_string(ctx, "httpRequest");

  /* Init Serial Port Functions */
  duk_push_c_function(ctx, native_openSerialPort, 2 /*nargs*/);
  duk_put_global_string(ctx, "openSerialPort");

  duk_push_c_function(ctx, native_serialWrite, 1 /*nargs*/);
  duk_put_global_string(ctx, "serialWrite");

  duk_push_c_function(ctx, native_serialWriteByteArray, 1 /*nargs*/);
  duk_put_global_string(ctx, "serialWriteByteArray");

  duk_push_c_function(ctx, native_serialRead, 0 /*nargs*/);
  duk_put_global_string(ctx, "serialRead");
  
  duk_push_c_function(ctx, native_serialReadByteArray, 0 /*nargs*/);
  duk_put_global_string(ctx, "serialReadByteArray");


  /* Init Hardware Functions */
  duk_push_c_function(ctx, native_setButton, 2 /*nargs*/);
  duk_put_global_string(ctx, "setButton");

  duk_push_c_function(ctx, native_setNightLight, 1 /*nargs*/);
  duk_put_global_string(ctx, "setNightLight");

  /* Oboo software functions */
  duk_push_c_function(ctx, native_updateSystemTime, 1 /*nargs*/);
  duk_put_global_string(ctx, "updateSystemTime");


  /* initialize the message queue */
  initMessageQueue();

  /* initialize curl */
  curl_global_init(CURL_GLOBAL_SSL);

  for(int i=0; i != MAX_NUMBER_OF_THREADS; i++){
    tIndex[i] = 0; //0 not used, 1 used
  }

  TAILQ_INIT(&q);
}

void destroyRuntime () {
  /* duktape release */
  duk_destroy_heap(ctx);

  /* message queue release */
  destroyMessageQueue();

  /* curl clean up */
  curl_global_cleanup();
}

void printHelp () {
  printf("Oboo Card Runtime.\n\n");

  printf("Usage:\n");
  printf("\t./runtime <javascript file>\n");
  printf("\n");
}

// find path of binary and copy it to the path argument
 void getExeDir (char *path) {
  int numChars;
   char buf[MAX_CHAR_LEN];
  numChars = readlink("/proc/self/exe", buf, MAX_CHAR_LEN);
  buf[numChars] = 0; // null-terminate the string
 
  duk_push_string(ctx, buf);
  duk_put_global_string(ctx, "progName");
  duk_push_string(ctx, "\\/[^\\/]*$");
  duk_put_global_string(ctx, "reg");
  duk_eval_string(ctx,"var re = new RegExp (reg, ''); progName = progName.replace(re, '');");

  duk_get_global_string(ctx, "progName");
  strcpy(path, duk_get_string(ctx, -1));
 }

// find path of current working directory.
void getCWDir (char *path) {
  char buf[PATH_MAX+1];

  if (getcwd(buf, sizeof(buf)) == NULL) {
    perror("getcwd() error");
    return -1;
  }
  sprintf(path, "%s", buf);
  return 1;
}

void getFilePath(char * fullPath, char* filename, char* path){
  // filename path is absolute
  if (filename[0] == '/') {
      sprintf(fullPath, "%s", filename);
  } else {
      sprintf(fullPath, "%s/%s", path, filename);
  }
}

int loadJS (char* filePath) {
  duk_int_t rc;
  char lineBuff[MAX_CHAR_LEN];

  FILE *f = fopen(filePath, FILE_RD_FLAGS);
  if ((long int)f <= 0) {
      printf("ERROR: cannot open file '%s'\n", filePath);
      return -1;
  } else {
      duk_eval_string(ctx, "strBuff = ''");
      while (fgets(lineBuff, sizeof(lineBuff), f) != NULL) {
          duk_push_string(ctx, lineBuff);
          duk_put_global_string(ctx, "lineBuff");
          duk_eval_string(ctx, "strBuff += lineBuff;");
      }
      rc = duk_peval_string(ctx, "eval(strBuff);");
      if (rc != 0) {
        printf("error loading js file '%s': %s\n", filePath, duk_safe_to_string(ctx, -1));
      }

      // close the file
      fclose(f);
  }
  return 0;  /* no return value (= undefined) */
}

int runSetup () {
  duk_int_t rc;
  rc = duk_peval_string(ctx, "setup();");
  if (rc != 0) {
    printf("error running '%s' function: %s\n", "setup()", duk_safe_to_string(ctx, -1));
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int runLoop () {
  duk_int_t rc;

  struct callbacks *p;
  TAILQ_FOREACH(p, &q, cbq) {
    // get the callback
    duk_get_global_string(ctx, p->callbackStr);
    // check to make sure it exists and that it's a function

    if (!duk_is_function(ctx, -1)) {
        printf("No callback registered\n");
        //return;
    }else{
      // remove the callback
      duk_push_null(ctx);
      duk_put_global_string(ctx, p->callbackStr);
      // protected call
      rc = duk_pcall(ctx, 0);
      if (rc != 0) {
          printf("Callback '%s' failed: '%s'\n", p->callbackStr, duk_safe_to_string(ctx, -1));
      }
      duk_pop(ctx);
    }
  }

  while (!TAILQ_EMPTY(&q)) {
    p = TAILQ_FIRST(&q);
    TAILQ_REMOVE(&q, p, cbq);
    free(p->callbackStr);
    free(p);
  }

  rc = duk_peval_string(ctx, "loop();");
  if (rc != 0) {
    printf("error running '%s' function: %s\n", "loop()", duk_safe_to_string(ctx, -1));
  }
  else {
    // check for exit condition
    if (duk_is_number(ctx, -1) && duk_to_int(ctx, -1) == JS_EXIT_CONDITION) {
      return EXIT_FAILURE;
    }
  }
  duk_pop(ctx);
  return EXIT_SUCCESS;
}

void handleMessage(char* topic, char* payload) {
  duk_int_t rc;
  duk_idx_t obj_idx;
  duk_get_global_string(ctx, "onRecvMessage");

  obj_idx = duk_push_object(ctx);
  duk_push_string(ctx, topic);
  duk_put_prop_string(ctx, obj_idx, "topic");
  duk_push_string(ctx, payload);
  duk_put_prop_string(ctx, obj_idx, "payload");

  rc = duk_pcall(ctx, 1);
  if (rc != 0) {
    printf("error running '%s' function: %s\n", "onRecvMessage()", duk_safe_to_string(ctx, -1));
  }
}

void invokeCallback(char* cbIndex) {
  duk_int_t rc;
  char buf[MAX_CHAR_LEN];
  sprintf(buf, "%s_%s", CB_PREFIX, cbIndex);
  printf("sizeof calloc %d\n", 7+sizeof(cbIndex)*sizeof(char));
  char * buff = malloc(7+sizeof(cbIndex)*sizeof(char));
  memset(buff, 0, 7+sizeof(cbIndex)*sizeof(char));
  sprintf(buff, "%s_%s\0", CB_PREFIX, cbIndex);

  struct callbacks* cbFs = malloc(sizeof(struct callbacks));//callbacks(buff);
  memset(cbFs, 0, sizeof(struct callbacks));

  cbFs->callbackStr=buff;

  if(TAILQ_EMPTY(&q)){
    TAILQ_INSERT_HEAD(&q, cbFs, cbq); //callback for save
  }else{
    TAILQ_INSERT_TAIL(&q, cbFs, cbq);
  }
  return;
}

duk_ret_t native_print(duk_context *ctx) {
  char *initString = (char*) duk_to_string(ctx, 0);
  //TODO: sendto MQTT (initString)
  printf("%s\n", initString);
  return 0;  /* no return value (= undefined) */
}

duk_ret_t native_sleep(duk_context *ctx) {
  int sec = duk_to_int32(ctx, 0);
  sleep(sec);
  return 0;  /* no return value (= undefined) */
}


duk_ret_t native_mod_search(duk_context *ctx) {
    /* Nargs was given as 4 and we get the following stack arguments:
     *   index 0: id
     *   index 1: require
     *   index 2: exports
     *   index 3: module
     */

    /* ... */
    printf("on native mod search id");
    return 0;  /* no return value (= undefined) */
}


duk_ret_t native_initCard(duk_context *ctx) {
  if (duk_to_boolean(ctx, 1)) {
      // optional delay
      sleep(5);
  }
  printf("initCard: '%s'\n", duk_to_string(ctx, 0));
  sendMessage(MQ_LOCAL_CLIENT, MQ_CARD_TOPIC, duk_to_string(ctx, 0));
  return 0;  /* no return value (= undefined) */
}

duk_ret_t native_updateCard(duk_context *ctx) {
  printf("updateCard: '%s'\n", duk_to_string(ctx, 0));
  sendMessage(MQ_LOCAL_CLIENT, MQ_CARD_TOPIC, duk_to_string(ctx, 0));
  return 0;  /* no return value (= undefined) */
}

duk_ret_t native_deleteCard(duk_context *ctx) {
  char buf[64];
  int cardId = duk_to_int(ctx, 0);
  printf("deleteCard %d\n", cardId);
  sprintf(buf, "{\"cmd\":\"remove_card\",\"id\":%d}", cardId);
  sendMessage(MQ_LOCAL_CLIENT, MQ_CARD_TOPIC, buf);
  return 0;  /* no return value (= undefined) */
}


// expected arguments:
//  0   host
//  1   port
//  2   clientId  (will be autogenerated if null)
//  3   callback function
//  4   enable TLS (secure) connection (bool)
//  5   will topic (can be null)
//  6   will payload (can be null)
duk_ret_t native_mqConnect(duk_context *ctx) {
  int status;
  char buf[MAX_CHAR_LEN];
  char* clientId;
  char* willTopic = NULL;
  char* willPayload = NULL;
  // handle the clientId
  clientId = duk_to_string(ctx, 2);
  if (strcmp(clientId, JS_NULL) == 0 || clientId == NULL) {
    clientId = generateRandomString(CLIENT_ID_LEN);
  }
  // handle the will topic
  willTopic = duk_to_string(ctx, 5);
  if (strcmp(willTopic, JS_NULL) == 0 || willTopic == NULL) {
    willTopic = NULL;
  }
  // handle the will payload
  willPayload = duk_to_string(ctx, 6);
  if (strcmp(willPayload, JS_NULL) == 0 || willPayload == NULL) {
    willPayload = NULL;
  }

  // register the callback
  if (duk_is_function(ctx, 3)) {
    duk_dup(ctx, 3);
    sprintf(buf, "%s_%s", CB_PREFIX, clientId);
    duk_put_global_string(ctx, buf);
  }

  if (duk_to_boolean(ctx, 4)) {
      printf("establishing secure connection\n");
      status =  connectMessageQueueSecure(MQ_REMOTE_CLIENT, duk_to_string(ctx, 0), duk_to_int(ctx, 1), clientId, willTopic, willPayload);
  }
  else {
    // make the C function call
    // printf("connect to %s:%d as '%s'\n", duk_to_string(ctx, 0), duk_to_int(ctx, 1), clientId);
    status =  connectMessageQueue(MQ_LOCAL_CLIENT, duk_to_string(ctx, 0), duk_to_int(ctx, 1), clientId, willTopic, willPayload);
  }

  duk_push_int(ctx, status);
  return 1; /* 1 = return value at top */
}

duk_ret_t native_subscribe(duk_context *ctx) {
  int clientType = MQ_LOCAL_CLIENT;
  int *idPtr = malloc(sizeof(int));
  char buf[MAX_CHAR_LEN];
  printf("subscribe: '%s'\n", duk_to_string(ctx, 0));

  // check for optional boolean argument
  if (duk_to_boolean(ctx, 2)) {
    clientType = MQ_REMOTE_CLIENT;
  }

  int status = messageQueueSubscribe(clientType, duk_to_string(ctx, 0), idPtr);

  // register the callback - if any
  if (duk_is_function(ctx, 1)) {
    duk_dup(ctx, 1);
    sprintf(buf, "%s_s%d", CB_PREFIX, *idPtr);
    duk_put_global_string(ctx, buf);
  }

  // return value
  duk_push_int(ctx, status);
  free(idPtr);
  return 1; /* 1 = return value at top */
}

duk_ret_t native_publish(duk_context *ctx) {
  int clientType = MQ_LOCAL_CLIENT;
  printf("publishing to topic '%s': '%s'\n", duk_to_string(ctx, 0), duk_to_string(ctx, 1));

  // check for optional boolean argument
  if (duk_to_boolean(ctx, 2)) {
    clientType = MQ_REMOTE_CLIENT;
  }

  int status = sendMessage(clientType, duk_to_string(ctx, 0), duk_to_string(ctx, 1));
  duk_push_int(ctx, status);
  return 1; /* 1 = return value at top */
}

// http requests
size_t recvDataCallback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t 			realSize = size * nmemb;
  strcpy((char*)userdata, ptr);
	return 	realSize;
}

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

int httpRequest (char* url, char* method, char* response) {
  CURL *curl;
  CURLcode res;
  struct MemoryStruct chunk;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // TODO: wrap this with a check to make sure method == GET
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

    /* some servers don't like requests that are made without a user-agent field, so we provide one */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");


    res = curl_easy_perform(curl);

    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s (%d)\n", curl_easy_strerror(res), res);
      curl_easy_cleanup(curl);
      return -1; /* return an error */
    } else {
      curl_easy_cleanup(curl);
      strcpy(response, chunk.memory);
      return 0; /* 1 = return value at top */
    }
  }

  return -1;
}


duk_ret_t native_httpRequest(duk_context *ctx) {
  int status;
  char data[8192];

  /* Get request options */
  duk_push_string(ctx, "url");
  duk_get_prop(ctx, 0);
  char * url = (char*) duk_to_string(ctx, -1);
  printf("url: %s\n", url);

  duk_push_string(ctx, "method");
  duk_get_prop(ctx, 0);
  char * method = (char*) duk_to_string(ctx, -1);
  printf("method: %s\n", method);

  // perform the http request
  status = httpRequest(url, method, data);
  if (status == 0) {
    duk_push_string(ctx, data);
    return 1; /* 1 = return value at top */
  }

  return 0; /* 0 = return 'undefined' */
}

// oboo hardware functions
duk_ret_t native_setButton(duk_context *ctx) {
  // code review: potentially rework this so that the entire payload generated in js and only executed in c
  char buf[MAX_CHAR_LEN];
  printf("native setButton: %d: 0x%06x\n", duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  sprintf(buf, "{\"cmd\":\"button\", \"id\":%d, \"value\":\"0x%06x\"}", duk_to_int(ctx, 0), duk_to_int(ctx, 1));
  int status = sendMessage(MQ_LOCAL_CLIENT, MQ_SET_BUTTON_COLOR_TOPIC, buf);

  return 0;  /* no return value (= undefined) */
}

duk_ret_t native_setNightLight(duk_context *ctx) {
  // code review: potentially rework this so that the entire payload generated in js and only executed in c
  char buf[MAX_CHAR_LEN];
  printf("native setNightLight: 0x%06x\n", duk_to_int(ctx, 0));
  sprintf(buf, "{\"cmd\":\"nightlight\", \"value\":\"0x%06x\"}", duk_to_int(ctx, 0));
  int status = sendMessage(MQ_LOCAL_CLIENT, MQ_SET_NIGHTLIGHT_TOPIC, buf);

  return 0;  /* no return value (= undefined) */
}

// oboo software functions
duk_ret_t native_updateSystemTime(duk_context *ctx) {
  char buf[MAX_CHAR_LEN];
  // TODO: insert a check here to make sure input string from duktape is all numbers
  sprintf(buf, "sh /usr/bin/mcu/setSystemTime.sh %s\n", duk_to_string(ctx, 0));
  system(buf);

  return 0;  /* no return value (= undefined) */
}


// serial port functions
duk_ret_t native_openSerialPort(duk_context *ctx) {
  int baudrate = DEFAULT_BAUDRATE;
  if (duk_is_number(ctx, 1)) {
    baudrate = duk_to_int(ctx, 1);
  }
  int status = initSerialPort(duk_to_string(ctx, 0), baudrate);

  duk_push_int(ctx, status);
  return 1; /* 1 = return value at top */
}

duk_ret_t native_serialWrite(duk_context *ctx) {
  // printf("writing serial message '%s'\n", duk_to_string(ctx, 0));
  int status = writeSerialPort(duk_to_string(ctx, 0));
  // printf("  writeSerialPort returned %d\n", status);

  duk_push_int(ctx, status);
  // printf("pushed int, stack size is %ld\n", (long)duk_get_top_index(ctx));
  return 1; /* 1 = return value at top */
}

duk_ret_t native_serialWriteByteArray(duk_context *ctx) {
  unsigned char *buf;
  duk_size_t sz;
  int status = 0;

  buf = (unsigned char *) duk_get_buffer_data(ctx, 0, &sz);
  if (buf != NULL) {
    // printf("Found buffer of size %d:\n", (int)sz);
    for (int i = 0; i < (int)sz; i++) {
      // printf("%c (%d), ", buf[i], (int)buf[i]);
      writeCharSerialPort(buf[i]);
    }
    // printf("\n");
  }

  duk_push_int(ctx, status);
  // printf("pushed int, stack size is %ld\n", (long)duk_get_top_index(ctx));
  return 1; /* 1 = return value at top */
}

duk_ret_t native_serialRead(duk_context *ctx) {
  char buf[MAX_CHAR_LEN] = "";
  int status = readSerialPort(buf);

  duk_push_string(ctx, buf);
  return 1; /* 1 = return value at top */
}

duk_ret_t native_serialReadByteArray(duk_context *ctx) {
  char buf[MAX_CHAR_LEN] = "";
  int rdlen = readSerialPort(buf);
  
  // create an array to return to JS
  duk_eval_string(ctx, "retArray = [];");

  if (rdlen > 0) {
    for(int i = 0; i < rdlen; i++) {
      duk_push_int(ctx, (uint8_t)buf[i]);
      duk_put_global_string(ctx, "input");
      duk_eval_string(ctx, "retArray.push(input)");
      duk_pop(ctx); // need to remove the integer from the value stack
    }
  }
  
  // return the array (at top of value stack) to JS
  return 1; /* 1 = return value at top */
}

// filesystem operations
duk_ret_t native_writeFile(duk_context *ctx) {
  int status = 0;
  char* filePath  = (char*) duk_to_string(ctx, 0);
  char* content   = (char*) duk_to_string(ctx, 1);

  // write the contents to the file
  FILE *f = fopen(filePath, FILE_WR_FLAGS);
  if ((long int)f <= 0) {
    printf("ERROR: cannot open file '%s'\n", filePath);
    status = -1;
  } else {
    // write contents to file
    status = fputs(content, f);
    if (status < 0) {
      printf("ERROR: writing to file '%s'\n", filePath);
    }
    // close the file
    fclose(f);
  }

  // execute the callback function
  if (duk_is_function(ctx, 2)) {
    duk_int_t rc;
    duk_dup(ctx, 2);
    // push cb err argument to stack
    //  true if error, false if ok
    duk_push_boolean(ctx, (status < 0 ? 1 : 0));

    rc = duk_pcall(ctx, 1);
    if (rc != 0) {
        printf("writeFile callback error: %s\n", duk_safe_to_string(ctx, -1));
    }
  }
  
  // TODO: need a return statement here?
}

duk_ret_t native_readFile(duk_context *ctx) {
  int status = 0;
  char lineBuff[MAX_CHAR_LEN];
  char* filePath  = (char*) duk_to_string(ctx, 0);
  char* encoding   = (char*) duk_to_string(ctx, 1);
  duk_int_t rc;

  // read the file - need the callback though
  if (duk_is_function(ctx, 2)) {
    FILE *f = fopen(filePath, FILE_RD_FLAGS);
    if ((long int)f <= 0) {
      printf("ERROR: cannot open file '%s'\n", filePath);
      status = -1;
    } else {
      // read contents of the file
      duk_eval_string(ctx, "strBuff = String('')");
      while (fgets(lineBuff, sizeof(lineBuff), f) != NULL) {
          duk_push_string(ctx, lineBuff);
          duk_put_global_string(ctx, "lineBuff");
          duk_eval_string(ctx, "strBuff += String(lineBuff);");
          // code review: do we need to call duk_pop here? we're just pushing a bunch of strings to the stack
      }
      // close the file
      fclose(f);
    }

    duk_dup(ctx, 2);
    // push cb err argument to stack
    //  true if error, false if ok
    duk_push_boolean(ctx, (status < 0 ? 1 : 0));
    // push cb data argument to stack
    if (status < 0 ) {
      duk_push_null(ctx);
    } else {
      duk_get_global_string(ctx, "strBuff");
      // duk_safe_to_string(ctx, -1);
    }

    rc = duk_pcall(ctx, 2);
    if (rc != 0) {
      printf("callback error: %s\n", duk_safe_to_string(ctx, -1));
    }

    // code review: potentially call duk_pop here?
  }
  
  // TODO: need a return statement here?
}

/// system operations
// use gpioctl to set GPIO output value
//  arg0 - gpio number
//  arg1 - gpio value (0 or 1)
duk_ret_t native_setGpio(duk_context *ctx) {
  char buf[MAX_CHAR_LEN];
  int gpio = duk_to_int(ctx, 0);
  int value = duk_to_int(ctx, 1);

  sprintf(buf, "gpioctl dirout-%s %d\n", (value == 1 ? "high" : "low"), gpio);
#ifndef MAC_DEBUG
  system(buf);
#else
  printf("%s", buf);
#endif

  return 0;  /* no return value (= undefined) */
}

// use mpg123 to play an MP3 file
//  arg0 - path to mp3 file
duk_ret_t native_playMp3(duk_context *ctx) {
  char buf[MAX_CHAR_LEN];

  sprintf(buf, "mpg123 %s\n", duk_to_string(ctx, 0));
#ifndef MAC_DEBUG
  system(buf);
#else
  printf("%s", buf);
#endif

  return 0;  /* no return value (= undefined) */
}

// make a generic system call
//  arg0 - system call to run
duk_ret_t native_systemCall(duk_context *ctx) {
  char buf[MAX_CHAR_LEN];

  sprintf(buf, "%s\n", duk_to_string(ctx, 0));
#ifndef MAC_DEBUG
  system(buf);
#else
  printf("%s", buf);
#endif

  return 0;  /* no return value (= undefined) */
}

duk_ret_t native_systemPopen(duk_context *ctx) {
  char buf[MAX_CHAR_LEN];

  sprintf(buf, "%s 2$>1\n", duk_to_string(ctx, 0));

  char data[MAX_DATA_SIZE]={0x0};
  char tmp[MAX_CHAR_LEN]={0x0};
  long total=0;
  int retval=0;
  FILE *in=NULL;

  sprintf(buf, "2>&1 %s \n", duk_to_string(ctx, 0));
  printf("%s", buf);
  in=popen(buf, "r");
  if(in==NULL)
  {
    sprintf(data, "%s\n", strerror(errno));
    //perror("Shell execution error");
    //sprintf(tmp, "%s\n", explain_popen(buf, "r"));
    //exit(EXIT_FAILURE);
  }
  while(fgets(tmp,4095,in)!=NULL)
  {
    total+=atol(tmp);
    strcat(data, tmp);
  }
  //printf("inside tmp %s len of tmp %d\n", tmp, strlen(tmp));
  if(!feof(in))
  {
      strcat(data, strerror(errno));
      //perror("Input stream error");
      //exit(EXIT_FAILURE);
  }

  // TODO: bugi: had this sleep in bugi branch, is it required?
  //sleep(10);

  retval=pclose(in);
  if(retval==EOF || retval==127)
  {
      perror("Shell invocation error");
      exit(EXIT_FAILURE);
  }
  printf("pclose return value: %d, perror %s\n ", WEXITSTATUS(retval), strerror(retval));
  //fprintf(stdout,"%-64s total: %d\n",buf,total);
  //duk_push_string(ctx, "vrati li");
  duk_push_string(ctx, data);
  return 1;
}

int getPID(char* processName, int *processID)
{
    char* args[2];
    char str[100];
    int rtn;

    args[0] = "pidof ";

    char * process;
    process = strtok(processName, " ");
    if(process != NULL){
      args[1] = process;
    }else{
      //sprintf(processID, "%d", 0);
      *processID = 0;
      rtn = 0;
      return rtn;
    }
    strcpy (str,args[0]);
    strcat (str,args[1]);

    FILE* cmdresults = popen(str, "r");

    char data[MAX_DATA_SIZE]={0x0};
    char tmp[MAX_CHAR_LEN]={0x0};

    while(fgets(tmp,4095,cmdresults)!=NULL)
    {
      strcat(data, tmp);
    }
    //printf("Data %s", data);

    char * pch;
    pch = strtok (data," ");
    if(pch != NULL){ //first is allways new TODO: analyse
      rtn = strlen(pch);
      *processID = atoi(pch);
    }else { *processID = 0; }

    int retval=pclose(cmdresults);
    if(retval==EOF || retval==127)
    {
        perror("Shell invocation error");
    }
    int a = system(str);
    return rtn;
}


void *run_thread( void *ptr )
{
     struct ThreadInf * thrInfo;
     char tmp[MAX_CHAR_LEN]={0x0};
     thrInfo = (struct ThreadInf *) ptr;
     while( thrInfo->run != 0 ){
      if(thrInfo->in == NULL){
        char start[MAX_CHAR_LEN] = {0x0};
        sprintf(start, "%s", thrInfo->command);
        thrInfo->in = popen(start, "r");
        sleep(1);
        getPID(start, &(thrInfo->pId));
      }else if(thrInfo->run == 1){
        // executes when the process exits
        sleep(1);
        if(fgets(tmp,4095,thrInfo->in)==NULL){
          int retval=pclose(thrInfo->in);
          if(retval==EOF || retval==127)
          {
              perror("Shell invocation error");
          }
          //printf("pclose return value: %d, perror %s\n ", WEXITSTATUS(retval), strerror(retval));
          thrInfo->run = 0;
          thrInfo->in = NULL;
          tIndex[thrInfo->index] = 0;
          thrInfo->index = 0;
          curRun--;
        }
      }
     }
}


duk_ret_t native_launchProcess(duk_context *ctx) {
  char data[MAX_DATA_SIZE]={0x0};
  char buf[MAX_CHAR_LEN] = {0x0};
  sprintf(buf, "%s", duk_to_string(ctx, 0));

  int i;
  if(curRun < MAX_NUMBER_OF_THREADS){
    for(i=0; i!= MAX_NUMBER_OF_THREADS; i++){
      if(tIndex[i] == 0) { tIndex[i] = 1; break; }
    }
    //new index
    strcpy(aTi[i].command, buf);
    aTi[i].in = NULL;
    int iret = pthread_create(&(aTi[i].thread), NULL, run_thread, (void *)&aTi[i]);
    if(iret)
    {
       sprintf(data, "Error - pthread_create() return code: %d\n", iret);
    }
    aTi[i].run = 1;
    aTi[i].index = i;
    curRun++;
    //printf("cnt of treads that are running %d\n", curRun);
  }else{
    printf("number of threads exceeded max number (%d) of separate processes allowed\n", MAX_NUMBER_OF_THREADS );
    duk_push_int(ctx, 0);
    return 1;
  }

  sleep(2); //needed so that we can return pid of command called

  if(i != 0 || (i == 0 && tIndex[i] == 1)) { duk_push_int(ctx, aTi[i].pId); }
  return 1;
}

duk_ret_t native_killProcess(duk_context *ctx) {
  char dat[MAX_DATA_SIZE]={0x0};
  char buff[MAX_CHAR_LEN] = {0x0};
  sprintf(buff, "kill -9 %s", duk_to_string(ctx, 0));
  int j = system(buff);
  if (j == 0) { duk_push_int(ctx, 1); }
  else { duk_push_int(ctx, 0); }
  return 1;
}

duk_ret_t native_checkProcess(duk_context *ctx) {
  char buff[MAX_CHAR_LEN] = {0x0};
  sprintf(buff, "ps -p %s | grep %s | awk '{print $1}'", duk_to_string(ctx, 0), duk_to_string(ctx, 0)); //ps -p 14 | grep 14 | awk '{print $1}'
  printf("cmd %s\n", buff);

  FILE* cmdresults = popen(buff, "r");

  char data[MAX_DATA_SIZE]={0x0};
  char tmp[MAX_CHAR_LEN]={0x0};

  while(fgets(tmp,4095,cmdresults)!=NULL)
  {
  }
  int retval=pclose(cmdresults);
  if(retval==EOF || retval==127)
  {
      perror("Shell invocation error");
  }
  sprintf(data, "%s", duk_to_string(ctx, 0));
  if(strstr(tmp, data)!=NULL){
    duk_push_int(ctx, 1);
  }else duk_push_int(ctx, 0);

  return 1;
}
