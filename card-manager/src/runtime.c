#include <stdio.h>
#include "duktape/duktape.h"

#include "runtime.h"
#include "oboo.h"

// function definitions:
void duk_print_stack_size(duk_context* ctx);
static duk_ret_t native_print(duk_context *ctx);
static duk_ret_t native_createCard(duk_context *ctx);
static duk_ret_t native_createElement(duk_context *ctx);
static duk_ret_t native_isCardValid(duk_context *ctx);

static duk_ret_t native_updateElement(duk_context *ctx);

static duk_ret_t native_createCalendar(duk_context *ctx);
static duk_ret_t native_updateCalendar(duk_context *ctx);

static duk_ret_t native_selectCard(duk_context *ctx);
static duk_ret_t native_setStatus(duk_context *ctx);

static duk_ret_t native_deleteCardByUid(duk_context *ctx);
static duk_ret_t native_deleteCard(duk_context *ctx);

static duk_ret_t native_setNotification(duk_context *ctx);
static duk_ret_t native_clearNotification(duk_context *ctx);
static duk_ret_t native_setNotificationDuration(duk_context *ctx);


/* helper functions */
void publishCardSelected (int cardId) {
	int status;
	char buf[128];
	// publish to /cardResponse topic
	sprintf(buf, "{\"cardId\":%d, \"action\":\"select\", \"success\":true}", cardId);
	status = sendMessage(MQ_CARD_RESPONSE_TOPIC, buf);
}

/* duktape/C functions */
static duk_ret_t native_print(duk_context *ctx) {
	printf("%s\n", duk_to_string(ctx, 0));
	return 0;  /* no return value (= undefined) */
}

duk_ret_t native_publish(duk_context *ctx) {
  printf("publishing to topic '%s': '%s'\n", duk_to_string(ctx, 0), duk_json_encode(ctx, 1));
  int status = sendMessage(duk_to_string(ctx, 0), duk_json_encode(ctx, 1));
  duk_push_int(ctx, status);
  return 1; /* 1 = return value at top */
}

static duk_ret_t native_createCard(duk_context *ctx) {
	int idx;
	// retreive function arguments
	//	0		background color
	//	1		card unique id
	int bgColor = (int)duk_to_int(ctx, 0);

	printf("> C: Adding card with bgColor 0x%03x\n", bgColor);

	// add card to card manager
	idx = lv_add_card(bgColor);
	if (duk_is_string(ctx, 1)) {
		printf("> C: setting unique id\n");
		lv_set_card_uid(idx, duk_to_string(ctx, 1));
	}
	printf("> C: added card with index %d\n", idx);

	// return the index of the card
	duk_push_int(ctx, idx);
	return 1;	/* 1 = return value at top */
}

static duk_ret_t native_deleteCardByUid(duk_context *ctx) {
	int idx;
	// function arguments
	//	0		card unique id

	// get the card id based on the free ptr uid
	idx = lv_oboo_get_card_id_by_fp(duk_to_string(ctx, 0));
	//printf("DBG: id by fp is %d\n", idx);
	if (idx >= 0) {
		printf("> C: Deleting card with index %d\n", idx);
		lv_delete_card(idx);
	}
	// make sure there is a selected card
	printf("> C: selecting a remaining card\n");
	// TODO: update lv_ob_select_card function to still operate correctly if there are no cards remaining - shouldn't call publishSelectedCard in that case
	idx = lv_ob_select_card("left");
	//printf("idx: %d\n", idx);
    if(idx != -1){
       publishCardSelected(idx);
    }

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_deleteCard(duk_context *ctx) {
	int idx;
	// function arguments
	//	0		card index
	idx = (int)duk_to_int(ctx, 0);

	if (idx >= 0) {
		printf("> C: Deleting card with index %d\n", idx);
		lv_delete_card(idx);
	}
	// make sure there is a selected card
	idx = lv_ob_select_card("left");
	publishCardSelected(idx);

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_updateCalendar(duk_context *ctx) {
//  printf("native update calendar\n");
  int cardId = (int)duk_to_int(ctx, 0);
	int elementId = (int)duk_to_int(ctx, 1);
  int year = duk_to_int(ctx, 2);
	int month = duk_to_int(ctx, 3);
  int day = duk_to_int(ctx, 4);

//    printf("> C: update calendar %d: with date y/m/d %d/%d/%d elementId %d\n", cardId, year, month, day, elementId );

  lv_obj_t *element = lv_obj_get_card_element(cardId, elementId);

  struct oboo_card_element_data *elementData = NULL;
  if(NULL != element){
    elementData = lv_obj_get_free_ptr(element);
    //printf("calendar exist! element Data type is: %d\n", elementData->type);
  }else{
    printf("there is no such calendar with id: %d\n", cardId);
  }

  if (elementData != NULL && elementData->type == OB_CALENDAR_ELEMENT) {
    //TODO: check is date valid
    oboo_calendar_set_day(element, year, month, day);
    //printf("calendar updated\n");
  }else{
    //printf("element data is some crap\n");
  }

}

static duk_ret_t native_createCalendar(duk_context *ctx) {

  //printf("native create calendar\n");

/*
  cardId, \
  msg.elements[i].type, \
  msg.elements[i].position.x || 0, \
  msg.elements[i].position.y || 0,\
  msg.elements[i].size || 0,\
  msg.elements[i].date.year || 1970,\
  msg.elements[i].date.month || 1,\
  msg.elements[i].date.day || 1,\
  msg.elements[i].position.align || 'left',\
  msg.elements[i].id\
*/

	// retreive function arguments
	int cardId = (int)duk_to_int(ctx, 0);
//	char type[64] = duk_to_string(ctx, 1); //string
	int posX = (int)duk_to_int(ctx, 2);
	int posY = (int)duk_to_int(ctx, 3);
	int size = (int)duk_to_int(ctx, 4);
	int year = duk_to_int(ctx, 5);
	int month = duk_to_int(ctx, 6);
  int day = duk_to_int(ctx, 7);
//  char align[64] = duk_to_string(ctx, 8);
	int elementId = (int)duk_to_int(ctx, 9);

//  printf("> C: Adding calendar %d: type '%s' at (%d, %d) of size %d with date y/m/d %d/%d/%d elementId %d\n", cardId, duk_to_string(ctx, 1), posX, posY, size, year, month, day, elementId );

	//// directly using lv functions - need to implement calendar element handling
	// get the card
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardId);

	// add the calendar
//	lv_obj_t *calendar = oboo_calendar_create(card, 5, 0, 230, 240);
	lv_obj_t *calendar = NULL;
	if(card != NULL){
		//printf("card exists!\n");
		calendar = oboo_calendar_create(card, posX, posY, OB_CAL_DEFAULT_WIDTH, OB_CAL_DEFAULT_HEIGHT);
	
		if (calendar != NULL) {
			// setup the calendar
			oboo_calendar_set_day(calendar, year, month, day);
			//try to add elem so it can be found for update
			struct oboo_card_element_data *elementData = malloc(sizeof (struct oboo_card_element_data));
			if(elementData != NULL){
				elementData->type = OB_CALENDAR_ELEMENT;
				elementData->alignment = LV_ALIGN_CENTER; //TODO: blind setup (without device)
				elementData->posX = posX;
				elementData->posY = posY;
				lv_obj_set_free_ptr(calendar, (void*)elementData);
				printf("calendar data added\n");
			}else{
				printf("malloc failed!\n");
			}
		}
	}

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_isCardValid(duk_context *ctx) {
	int idx;
	int bValid = 0;
	
	// function arguments
	//	0		card index
	idx = (int)duk_to_int(ctx, 0);

	if (idx >= 0) {
		bValid = lv_ob_check_card_exists(idx);
		printf("> C: card with index %d is valid: %d\n", idx, bValid);
	}
	
	duk_push_boolean(ctx, bValid);	// return bValid
    return 1;
}

static duk_ret_t native_createElement(duk_context *ctx) {

	// retreive function arguments
	int cardId = (int)duk_to_int(ctx, 0);
	// char type[64] = duk_to_string(ctx, 1); //string
	int posX = (int)duk_to_int(ctx, 2);
	int posY = (int)duk_to_int(ctx, 3);
	int size = (int)duk_to_int(ctx, 4);
	// char align[64] = duk_to_string(ctx, 5);
	// char value[64] = duk_to_string(ctx, 6);
	int elementId = (int)duk_to_int(ctx, 7);

	lv_add_card_element(cardId, elementId, duk_to_string(ctx, 1), posX, posY, size, duk_to_string(ctx, 5), duk_to_string(ctx, 6));

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_updateElement(duk_context *ctx) {
	// retreive function arguments
	int cardId = (int)duk_to_int(ctx, 0);
	int elementId = (int)duk_to_int(ctx, 1);

	printf("> C: Updating card %d, element %d\n", cardId, elementId);
	lv_update_card_element(cardId, elementId, duk_to_string(ctx, 2));

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_selectCard(duk_context *ctx) {
	int status;
	int cardId = lv_ob_select_card(duk_to_string(ctx, 0));

	printf("select card: %d", cardId);
	// publish to /cardResponse topic
	if(cardId!=-1)
		publishCardSelected(cardId);

	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_setStatus(duk_context *ctx) {
	lv_ob_set_status(duk_to_string(ctx, 0), (int)duk_to_int(ctx, 1) );

	return 0;  /* no return value (= undefined) */
}

// arguments:
//	0	- notification message - string
//	1	- notification font size - int (optional)
static duk_ret_t native_setNotification(duk_context *ctx) {
	int fontSize = OBOO_NOTIFICATION_DEFAULT_FONT_SIZE;
	
	if (duk_is_number(ctx, 1) && duk_to_int(ctx, 1) > 0) {
		fontSize = duk_to_int(ctx, 1);
	}
	
	printf("> C: Setting notification to '%s' with '%d' font size\n", duk_to_string(ctx, 0), fontSize);
	oboo_notification_show(NULL, duk_to_string(ctx, 0), fontSize);
	
	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_clearNotification(duk_context *ctx) {
	oboo_notification_clear();
	
	return 0;  /* no return value (= undefined) */
}

static duk_ret_t native_setNotificationDuration(duk_context *ctx) {
	int duration = duk_to_int(ctx, 0);	// notification duration in seconds
	
	// duration of 0 means permanent notification 
	if (duration > 0) {
		// printf("setting notification duration to %d\n", duration);
		// TODO: need to implement notification duration
	}
	
	return 0;  /* no return value (= undefined) */
}

// debug function
void duk_print_stack_size(duk_context* ctx) {
	duk_idx_t idx_top;

	idx_top = duk_get_top_index(ctx);
	if (idx_top == DUK_INVALID_INDEX) {
	    printf("DBG: duktape stack is empty\n");
	} else {
	    printf("DBG: duktape stack: index of top element: %ld, type is ", (long) idx_top);
	    switch (duk_get_type(ctx, idx_top)) {
	    	case DUK_TYPE_NULL:
	    		printf("null\n");
	    		break;
	    	case DUK_TYPE_BOOLEAN:
	    		printf("boolean\n");
	    		break;
	    	case DUK_TYPE_NUMBER:
	    		printf("number\n");
	    		break;
	    	case DUK_TYPE_STRING:
	    		printf("string\n");
	    		break;
	    	case DUK_TYPE_OBJECT:
	    		printf("object\n");
	    		break;
	    	case DUK_TYPE_BUFFER:
	    		printf("buffer\n");
	    		break;
	    	case DUK_TYPE_POINTER:
	    		printf("pointer\n");
	    		break;
	    	case DUK_TYPE_LIGHTFUNC:
	    		printf("lightfunc\n");
	    		break;
	    	default:
	    		printf("OTHER\n");
	    		break;
	    }
	}
}

// api functions
/*duktape init*/
void initRuntime() {
	ctx = duk_create_heap_default();
	if (!ctx) {
		printf("ERROR: unable to create duk_context\n");
		exit(1);
	}

	/* duktape - register duktape/C functions */
	duk_push_c_function(ctx, native_print, 1 /*nargs*/);
	duk_put_global_string(ctx, "print");
	duk_push_c_function(ctx, native_createCard, 2 /*nargs*/);
	duk_put_global_string(ctx, "createCard");
	duk_push_c_function(ctx, native_createElement, 8 /*nargs*/);
	duk_put_global_string(ctx, "createElement");
  duk_push_c_function(ctx, native_createCalendar, 10 /*nargs*/);
	duk_put_global_string(ctx, "createCalendar");
	duk_push_c_function(ctx, native_updateElement, 3 /*nargs*/);
	duk_put_global_string(ctx, "updateElement");
  duk_push_c_function(ctx, native_updateCalendar, 5 /*nargs*/);
	duk_put_global_string(ctx, "updateCalendar");
	duk_push_c_function(ctx, native_selectCard, 1 /*nargs*/);
	duk_put_global_string(ctx, "selectCard");
	duk_push_c_function(ctx, native_setStatus, 2 /*nargs*/);
	duk_put_global_string(ctx, "setStatus");
	duk_push_c_function(ctx, native_publish, 2 /*nargs*/);
	duk_put_global_string(ctx, "publish");
	duk_push_c_function(ctx, native_deleteCardByUid, 1 /*nargs*/);
	duk_put_global_string(ctx, "deleteCardByUid");
	duk_push_c_function(ctx, native_deleteCard, 1 /*nargs*/);
	duk_put_global_string(ctx, "deleteCard");
	duk_push_c_function(ctx, native_setNotification, 2 /*nargs*/);
	duk_put_global_string(ctx, "setNotification");
	duk_push_c_function(ctx, native_clearNotification, 0 /*nargs*/);
	duk_put_global_string(ctx, "clearNotification");
	duk_push_c_function(ctx, native_setNotificationDuration, 1 /*nargs*/);
	duk_put_global_string(ctx, "setNotificationDuration");
	duk_push_c_function(ctx, native_isCardValid, 1 /*nargs*/);
	duk_put_global_string(ctx, "isCardValid");
	


	/*duktape - register js functions*/
	// function to parse new_card commands
	duk_eval_string(ctx, "function newCard(msg) { \
	    print ('new_card command received');\
	    if (msg.bg_color != null && msg.elements && Array.isArray(msg.elements) && msg.responseTopic) {\
	        print('lets make a new card');\
	        var cardId = createCard(msg.bg_color, msg.responseTopic);\
	        print('elements array has length of ' + msg.elements.length);\
	        for (var i = 0; i < msg.elements.length; i++) {\
              if(msg.elements[i].type !== null && msg.elements[i].type === 'calendar'){\
                createCalendar(  cardId, \
                                  msg.elements[i].type, \
                                  msg.elements[i].position.x || 0, \
                                  msg.elements[i].position.y || 0,\
                                  msg.elements[i].size || 0,\
                                  msg.elements[i].date.year || 1970,\
                                  msg.elements[i].date.month || 1,\
                                  msg.elements[i].date.day || 1,\
                                  msg.elements[i].position.align || 'left',\
                                  msg.elements[i].id\
                              );\
              }else if(msg.elements[i].id !== null && msg.elements[i].type !== null && msg.elements[i].value !== null && msg.elements[i].type !== null && msg.elements[i].type !== 'calendar'){\
                createElement(  cardId, \
                				msg.elements[i].type, \
								msg.elements[i].position.x || 0, \
								msg.elements[i].position.y || 0,\
								msg.elements[i].size || 0,\
								msg.elements[i].position.align || 'left',\
								msg.elements[i].value,\
								msg.elements[i].id\
							);\
              }\
	        }\
	        publish('/cardResponse', {\
	        	cardId: cardId,\
	        	attention: msg.responseTopic,\
	        	action: 'create',\
	        	success: true\
	        });\
	        selectCard('last');\
	    }\
	}");
	duk_push_global_object(ctx);	// make this function available globally
	duk_get_prop_string(ctx, -1, "newCard" ); // pushes function from loaded script to stack

	// function to parse update_card commands
	duk_eval_string(ctx, "function updateCard(msg) { \
	    print ('update_card command received');\
	    if (msg.id !== null && msg.elements && Array.isArray(msg.elements)) {\
	    	if (isCardValid(msg.id)) {\
		        print('card ' + msg.id + ': elements array has length of ' + msg.elements.length);\
		        for (var i = 0; i < msg.elements.length; i++) {\
		            print('element ' + i + ' being updated to ' + msg.elements[i].value);\
		            if (msg.elements[i].id !== null && msg.elements[i].value !== null && msg.elements[i].type !== 'calendar') {\
		                updateElement(  msg.id,\
		                                msg.elements[i].id,\
		                                msg.elements[i].value\
		                            );\
		            }else{\
	                  print('calendar update');\
	                  updateCalendar( msg.id,\
		                                msg.elements[i].id,\
		                                msg.elements[i].date.year || 1970,\
	                                  msg.elements[i].date.month || 1,\
	                                  msg.elements[i].date.day || 1\
		                            );\
	              }\
		        }\
	    	} else { \
        		print('Invalid card with id ', msg.id); \
        		publish('/cardResponse', {\
		        	cardId: msg.id,\
		        	action: 'update',\
		        	success: false,\
		        	reason: 'invalid cardId'\
		        });\
        	}\
	    }else{\
        	print('there is no Id of object in json');\
    	}\
	}");
	duk_push_global_object(ctx);	// make this function available globally
	duk_get_prop_string(ctx, -1, "updateCard" ); // pushes function from loaded script to stack
	
	// function to parse remove_card commands
	duk_eval_string(ctx, "function removeCard(msg) { \
		print ('remove_card command received');\
		if ('id' in msg) {\
			print ('deleting card with id ' + msg.id);\
			deleteCard(msg.id);\
		}\
		else if ('cardName' in msg) {\
			print ('deleting card with string uid ' + msg.cardName);\
			deleteCardByUid(msg.cardName);\
		}\
	}");
	duk_push_global_object(ctx);	// make this function available globally
	duk_get_prop_string(ctx, -1, "removeCard" ); // pushes function from loaded script to stack

	// function to parse all incoming messages
	duk_eval_string(ctx, "function parseMsg(topic, msg) {\
	    print ('parseMsg running');\
	    switch (topic) {\
	        case '/card':\
	            switch (msg.cmd) {\
	                case 'new_card':\
	                    newCard(msg);\
	                    break;\
	                case 'update_card':\
	                    updateCard(msg);\
	                    break;\
	                case 'remove_card':\
	                    removeCard(msg);\
	                    break;\
	                case 'select_card':\
	                    print ('select_card command received');\
	                    if (msg.action != null) {\
	                        selectCard(msg.action);\
	                    }\
	                    break;\
	                default:\
	                    print ('no cmd specified!');\
	                    break;\
	            }\
	            break;\
	        case '/status':\
	            switch (msg.cmd) {\
	                case 'update':\
	                    print ('update command received');\
	                    if (msg.elements && Array.isArray(msg.elements)) {\
	                        for (var i = 0; i < msg.elements.length; i++) {\
	                            if (msg.elements[i].type != null && msg.elements[i].value != null) {\
	                                print('element ' + i + ': ' + msg.elements[i].type + ' ' + msg.elements[i].value);\
	                                if (typeof msg.elements[i].value === 'number') {\
	                                    print('value is a number!');\
	                                    setStatus(msg.elements[i].type, msg.elements[i].value);\
	                                }\
	                            }\
	                        }\
	                    }\
	                    break;\
	                default:\
	                    print ('no cmd specified!');\
	                    break;\
	            }\
	            break;\
	        case '/notification':\
	        	print('Notification command received: ' + msg.cmd);\
	        	switch (msg.cmd) {\
	        		case 'set':\
	        			print('  set notification to ' + msg.text + ' w/ size ' + msg.size);\
	        			setNotification(msg.text, msg.size);\
	        			if ('duration' in msg) {\
	        				setNotificationDuration(msg.duration);\
	        			}\
	        			break;\
	        		case 'clear':\
	        			clearNotification();\
	        			break;\
	        		default:\
		        		print ('notification: no cmd specified!');\
	                    break;\
	        	}\
	        	break;\
	        default:\
	            print ('topic not recognized specified!');\
	            break;\
	    }\
	}");
	duk_push_global_object(ctx);	// make this function available globally
	duk_get_prop_string(ctx, -1, "parseMsg" ); // pushes function from loaded script to stack
}

void destroyRuntime() {
    duk_destroy_heap(ctx);
}

void handleMessage(char* topic, char* payload) {
    duk_dup_top(ctx);	// duplicate the parseMsg function so that the original doesn't get consumed

	// push the topic name
	duk_push_string(ctx, topic);

	// push the json string
	printf("pushing string '%s' to ctx\n", payload);
	duk_push_string(ctx, payload);
	duk_json_decode(ctx, -1);
	// todo: catch error for bad json

	duk_pcall(ctx, 2);
	duk_pop(ctx);
}
