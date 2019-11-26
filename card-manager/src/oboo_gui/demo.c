#include "demo.h"
#include "oboo.h"
#include <stdio.h>

void oboo_demo_populate_status_bar() {
    lv_ob_set_status("wifi", 1);
    lv_ob_set_status("bluetooth", 1);
    lv_ob_set_status("battery", 2);
    lv_ob_set_status("alarm", 1);
}

void oboo_demo_create_timer_card(char* timeVal, int pos) {
    int cardIdx;

    // add the card
    // cardIdx = lv_add_card_idx (0x123456, pos);	//kisvegabor@gmai.com: changed to test (the color was 0)
    cardIdx = lv_add_card_idx (-1, pos);

    // add the card elements
    // cards can be "text" or "image"
    lv_add_card_element(cardIdx, 0, "image" , 2, 46, 0, "left", "img/img_timer_bg.bin");
    lv_add_card_element(cardIdx, 1, "image" , 21, 10, 0, "left", "img/img_timer_buttons.bin");
    lv_add_card_element(cardIdx, 2, "text"  , 98, 103, 20, "center", "14:00");
    lv_add_card_element(cardIdx, 3, "text"  , 31, 131, 80, "center", timeVal);
}

void oboo_demo_create_calendar_card(int pos) {
    int cardIdx;

    // add the card
    cardIdx = lv_add_card_idx (-1, pos);	//kisvegabor@gmai.com: changed to test (the color was 0)

    //// directly using lv functions - need to implement calendar element handling
    // get the card
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardIdx);

	// add the calendar
	lv_obj_t *calendar = oboo_calendar_create(card, 5, 0, 230, 240);

	// setup the calendar
	//oboo_calendar_set_today(calendar, 2018, 6, 12);
  oboo_calendar_set_day(calendar, 2018, 6, 12);


	/*static*/ lv_calendar_date_t events[] = {{2018, 6, 9},
								    {2018, 6, 14}};
	oboo_calendar_set_events(calendar, events, 2);
}



void oboo_demo_add_remove_card(void) {
    int cardIdx;

    // add the card
    cardIdx = lv_add_card_idx (0x123456, 1);	//kisvegabor@gmai.com: changed to test (was 0)

    // add the card elements
    // cards can be "text" or "image"
    lv_add_card_element(cardIdx, 0, "image" , 2, 46, 0, "left", "img/img_timer_bg.bin");
    lv_add_card_element(cardIdx, 1, "image" , 21, 10, 0, "left", "img/img_timer_buttons.bin");
    lv_add_card_element(cardIdx, 2, "text"  , 98, 103, 20, "center", "14:00");
    lv_add_card_element(cardIdx, 3, "text"  , 31, 131, 80, "center", "2:00");

    lv_delete_card(cardIdx);
}

void oboo_demo_test_free_ptr_search(char* key0, char* key1, char* searchKey) {
    int idx;
    int cardIdx;
    int ret;
    lv_obj_t *card;
    
    // create a card
    printf("\n  -> creating a card\n");
    cardIdx = lv_add_card_idx (-1, -1);
    lv_add_card_element(cardIdx, 3, "text"  , 31, 131, 80, "center", "CARD0");
    // set card uid
    if (key0 != NULL) {
        ret = lv_set_card_uid (cardIdx, key0);
    }
    printf("  -> card '%d' added\n", cardIdx);
    
    // create an additional card
    printf("\n  -> creating a card\n");
    cardIdx = lv_add_card_idx (-1, -1);
    lv_add_card_element(cardIdx, 3, "text"  , 31, 131, 80, "center", "CARD1");
    if (key1 != NULL) {
        ret = lv_set_card_uid (cardIdx, key1);
    }
    printf("  -> card '%d' added\n", cardIdx);
    
    // search for card 0
    cardIdx = 0;
    card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardIdx);
    if (card != NULL) {
        printf("\nfound card '%d'\n", cardIdx);
    } else {
        printf("\ncould not find card '%d'\n", cardIdx);
    }
    
    cardIdx = 1;
    card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardIdx);
    if (card != NULL) {
        printf("\nfound card '%d'\n", cardIdx);
    } else {
        printf("\ncould not find card '%d'\n", cardIdx);
    }
    
    
    // search for the card by free_ptr value
    printf("\nsearching for card with fp '%s'\n", searchKey);
    idx = lv_oboo_get_card_id_by_fp(searchKey);
    printf("search returned '%d'\n", idx);
    if (idx >= 0) {
        printf("SUCCESS! found card!\n");
    }
    else {
        printf("CARD NOT FOUND\n");
    }
}
