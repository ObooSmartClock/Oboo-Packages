#ifndef _DEMO_HEADER_H_
#define _DEMO_HEADER_H_

#include "oboo.h"

void oboo_demo_populate_status_bar();

void oboo_demo_create_timer_card(char* timeVal, int pos);
void oboo_demo_create_calendar_card(int pos);

void oboo_demo_add_remove_card(void);

void oboo_demo_test_free_ptr_search(char* key0, char* key1, char* searchKey);


#endif  // _DEMO_HEADER_H_
