#include <stdlib.h>
#include "oboo.h"
#include "lv_theme_oboo.h"
#include "oboo_notification.h"


#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/display/monitor.h"
#include "lv_omega_fs.h"
#include "lvgl/lvgl.h"


LV_FONT_DECLARE(dejavu_60);
LV_FONT_DECLARE(dejavu_80);

LV_FONT_DECLARE(Futura_15);
LV_FONT_DECLARE(Futura_23);
LV_FONT_DECLARE(Futura_50);
LV_FONT_DECLARE(Futura_82);

/* api functions */
void oboo_graphics_init() {
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    _lv_fb_init();
    /*filesystem init*/
    _lv_omega_fs_init();


    /*Apply Oboo Theme*/
    lv_theme_t *th = lv_theme_oboo_init(200, NULL); //200  is the color in hue formate
    lv_theme_set_current(th);

    /*Create the screen*/
    scr = lv_obj_create(NULL, NULL); 	/*Create a parent object on the current screen*/
	lv_scr_load(scr);       						    /*Load the screen*/

	/* Initialize the Oboo GUI */
	_lv_init_oboo_gui();

	printf("DPI: %d, Anti-aliasing: %d\n", LV_DPI, LV_ANTIALIAS);
}

void oboo_graphics_handle() {
    lv_tick_inc(5);
    lv_task_handler();
}

/* init functions */
void _lv_fb_init () {
#if OB_PC_SIMULATOR
	monitor_init();
#else
	fbdev_init();
#endif
    /*Enable the frame buffer driver with LittlevGL*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
#if OB_PC_SIMULATOR
    disp_drv.disp_flush = monitor_flush;      /*flush the internal graphic buffer to the frame buffer*/
#else
    disp_drv.disp_flush = fbdev_flush;
#endif
    lv_disp_drv_register(&disp_drv);
}

void _lv_omega_fs_init () {
	/* Add a simple drive to open images from PC*/
    lv_fs_drv_t pcfs_drv;                         /*A driver descriptor*/
    memset(&pcfs_drv, 0, sizeof(lv_fs_drv_t));    /*Initialization*/

    pcfs_drv.file_size = sizeof(pc_file_t);       /*Set up fields...*/
    pcfs_drv.letter = LV_OMEGA_FS_LETTER;
    pcfs_drv.open = pcfs_open;
    pcfs_drv.close = pcfs_close;
    pcfs_drv.read = pcfs_read;
    pcfs_drv.seek = pcfs_seek;
    pcfs_drv.tell = pcfs_tell;
    lv_fs_add_drv(&pcfs_drv);
}

void _lv_init_oboo_gui () {
	/* note: ensure this ordering is preserved:
				cardview added first
				and status bar added last
			the lv_oboo_obj_get_card_manager and lv_oboo_obj_get_status_bar functions
			depend on this creation order
	*/

	/* 1 - create tabs */
	lv_obj_t *tv = ob_cardview_create(lv_scr_act(), NULL);
	lv_obj_set_size(tv, LV_HOR_RES, LV_VER_RES - OB_TOP_BAR_RES);
	lv_obj_set_pos(tv, 0, OB_TOP_BAR_RES);
	lv_obj_set_free_num(tv, OB_CARD_VIEW_OBJ_ID);

	/* 2 - create oboo logo splash screen */
	_lv_init_splash_screen();

	/* 3 - create status bar */
	// setup style first
	static lv_style_t sb_style;
	lv_style_copy(&sb_style, &lv_style_plain);
	sb_style.body.main_color = LV_COLOR_BLACK;
	sb_style.body.grad_color = LV_COLOR_BLACK;
	// create the statusbar
	lv_obj_t *sb = ob_statusbar_create(lv_scr_act(), NULL);
	lv_obj_set_style(sb, &sb_style);
	lv_obj_set_size(sb, LV_HOR_RES, OB_TOP_BAR_RES);
	lv_obj_set_pos(sb, 0, 0);
	lv_obj_set_free_num(sb, OB_STATUS_BAR_OBJ_ID);

	// populate status bar
	static lv_style_t sb_text_style;
	lv_style_copy(&sb_text_style, &lv_style_plain);
	sb_text_style.text.font = &lv_font_dejavu_20;
	sb_text_style.text.color = LV_COLOR_WHITE;

	lv_obj_t *statusLeft = lv_label_create(sb, NULL);
	lv_label_set_style(statusLeft, &sb_text_style);

	lv_obj_t *statusRight = lv_label_create(sb, NULL);
	lv_label_set_style(statusRight, &sb_text_style);

	lv_ob_statusbar_update();

	/*Initialize the notification*/
	oboo_notification_init();
}

void _lv_init_splash_screen () {
#if OB_SPLASH_SCREEN_ENABLED == 1
	printf("Setting oboo logo background...");
	LV_IMG_DECLARE(img_oboo_logo);

	lv_obj_t *img_src = lv_img_create(lv_scr_act(), NULL);  /*Create an image object*/
	lv_obj_set_free_num(img_src, OB_SPLASH_SCREEN_OBJ_ID);
	lv_img_set_src(img_src, &img_oboo_logo);              /*Set the created file as image */

	lv_obj_align(img_src, NULL, LV_ALIGN_CENTER, 0, 0);		/*Set the alignment*/
	printf(" done\n");
#endif	// OB_SPLASH_SCREEN_ENABLED
}

/* getter functions */
// DEPRECATED: using lv_obj_get_child_by_fn_index instead
// get an object's child by index that is based on order of addition to parent
lv_obj_t* lv_obj_get_child_by_index(lv_obj_t *parent, int idx) {
	int count = 0;
	lv_obj_t* itr = NULL;
	while (1) {
		itr = lv_obj_get_child_back (parent, itr);
		// printf("count is %d, itr is Null: %s\n", count, itr == NULL ? "yes" : "no");	// debug print statements
		// leave the loop if reached the specified index (or if looped thru all children)
		if (count == idx || itr == NULL) {
			printf("leaving loop\n");
			break;
		}
		count++;	// iterate the count
	}
	return (itr);
}

// get an object's child by a given free_num index
//	generally used to retrieve elements in a card
lv_obj_t* lv_obj_get_child_by_fn_index(lv_obj_t *parent, int idx) {
	int childId = 0;
	lv_obj_t* itr = NULL;
	while (1) {
		itr = lv_obj_get_child_back (parent, itr);
		if (itr != NULL) {
			childId = lv_obj_get_free_num(itr);
			if (childId == idx) {
				// found the child we were looking for
				break;
			}
		} else {
			// leave the loop - we've looped through all the children - will return NULL
			break;
		}
	}
	return (itr);
}

lv_obj_t* lv_obj_get_card_element(int cardIdx, int elementIdx) {
	// printf("looking for card %d element %d\n", cardIdx, elementIdx);
	// get the specified card
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardIdx);
	// return the specified element
	if (card != NULL) {
	    return (lv_obj_get_child_by_fn_index(lv_page_get_scrl(card), elementIdx));
	}
	return NULL;
}

lv_obj_t* lv_oboo_obj_get_card_manager() {
	lv_obj_t *cm = lv_obj_get_child_back (lv_scr_act(), NULL);
	if (cm != NULL && lv_obj_get_free_num(cm) == OB_CARD_VIEW_OBJ_ID) {
		return cm;
	}
	return NULL;
}

lv_obj_t* lv_oboo_obj_get_status_bar() {
	lv_obj_t *sb = lv_obj_get_child (lv_scr_act(), NULL);
	if (sb != NULL && lv_obj_get_free_num(sb) == OB_STATUS_BAR_OBJ_ID) {
		return sb;
	}
	return NULL;
}

int lv_oboo_get_card_id_by_fp(char* uid) {
  int cardIdx = -1;
  lv_obj_t *card = ob_cardview_get_card_by_fp_string(lv_oboo_obj_get_card_manager(), uid);
  if (card != NULL) {
    cardIdx = lv_obj_get_free_num(card);
  }
  return cardIdx;
}

/* add functions */
// add a card to the card manager
//	returns: index of the card
int lv_add_card (int bgColor) {
	return (lv_add_card_idx (bgColor, -1));
}

// add a card to the card manager with a specified index
//	returns: index of the card
int lv_add_card_idx (int bgColor, int pos) {
	int idx;
	lv_obj_t *tab;
	_lv_remove_splash_screen();

	// get the card manager object
	lv_obj_t *cm = lv_oboo_obj_get_card_manager();
	int card_cnt = ob_cardview_get_card_count(cm);
	if(pos > card_cnt) pos = card_cnt;

	// add a card
	if (pos >=0) {
		printf("lv_add_card_idx:: adding card to pos %d\n", pos);
		tab = ob_cardview_add_card(cm, pos);
	}
	else {
		printf("lv_add_card_idx:: appending card to end\n");
		tab = ob_cardview_append_card(cm);
		// update pos to reflect that card was appended to cardview
		//	note: now searching through all cards to find highest free_num
		pos = (int)ob_cardview_get_max_free_num(cm) + 1;
	}

	// set the background color - if greater than 0
	// onion.io: TODO: DEBUG: setting the background doesn't work...
	// kisvegabor@gmail.com: fixed
	// onion.io: TODO: DEBUG: setting the background for one card, sets the same background for all cards
	if (bgColor >= 0) {
		static lv_style_t newStyle;
		lv_style_copy(&newStyle, &lv_style_plain);
		newStyle.body.main_color = LV_COLOR_HEX(bgColor);
		newStyle.body.grad_color = LV_COLOR_HEX(bgColor);
		newStyle.body.padding.hor = 0;
		newStyle.body.padding.ver = 0;
		lv_page_set_style(tab, LV_PAGE_STYLE_BG, &newStyle);
	}

	// first element on a card cannot be properly position on the y-axis for some reason
	// WORKAROUND: add a dummy element that is empty
	lv_obj_t * element = element = lv_label_create(lv_page_get_scrl(tab), NULL);
	lv_label_set_text(element, " ");
	lv_obj_set_free_num(element, 999);

	// get card index
	idx = pos;
	// write the card index to the object
	lv_obj_set_free_num(tab, idx);

	printf("lv_add_card_idx:: returning index %d\n", idx);
	// return the index of the card (based on it's position as a child of card-manager)
	return (idx);
}

int lv_set_card_uid (int cardId, char* uid) {
  lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardId);
  char* mem;
  // write the uid to the element
  if (card != NULL) {
  	mem = malloc(sizeof(char) * (strlen(uid) + 1));
  	strcpy(mem, uid);
    lv_obj_set_free_ptr(card, mem);
    return 0;
  }
  return -1;
}

void lv_delete_card(int cardId)
{
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardId);
  if(card != NULL){
    //printf("not Null");
    
    lv_obj_t *child = lv_obj_get_child_back(card, NULL);
    
	while(child != NULL){
		struct oboo_card_element_data *freePtr = lv_obj_get_free_ptr(child);
		if(freePtr != NULL){
		  free(freePtr);
		}
		child = lv_obj_get_child_back(card, child);
	}
    
	  ob_cardview_delete_card(lv_oboo_obj_get_card_manager(), card);
  }else{
    printf("there was no card with id: %d", cardId);
  }
}

int lv_add_card_element(int cardId, int elementId, char* elementType, int posX, int posY, int size, char* alignment, char* value) {
    lv_obj_t * element = NULL;
    // get the card
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardId);
	
	if (card == NULL) {
		return 1;
	}

	// printf("> C: Adding element to card %d: type '%s' at (%d, %d) of size %d with default value '%s'\n", cardId, elementType, posX, posY, size, value );

	// allocate element data structure
	// struct oboo_card_element_data *elementData = malloc(sizeof *elementData);
	struct oboo_card_element_data *elementData = malloc(sizeof (struct oboo_card_element_data));

	if (elementData != NULL) {
		/* populate the element data structure */
		// element type
		if (strcmp(elementType, "text") == 0) {
			elementData->type = OB_TEXT_ELEMENT;
		} else if (strcmp(elementType, "image") == 0) {
			elementData->type = OB_IMAGE_ELEMENT;
		}
		// alignment
		if (strcmp(alignment, "right") == 0) {
			elementData->alignment = LV_ALIGN_IN_TOP_RIGHT;
		}
		else if (strcmp(alignment, "center") == 0) {
			elementData->alignment = LV_ALIGN_IN_TOP_MID;
		}
		else {
			// if it's 'left' or other
			elementData->alignment = LV_ALIGN_IN_TOP_LEFT;
		}
		// position data
		elementData->posX = posX;
		elementData->posY = posY;

		// create the element based on its type
		if (elementData->type == OB_TEXT_ELEMENT) {
			// create a text element
			element = lv_label_create(lv_page_get_scrl(card), NULL);

			// set the font size
			lv_style_t *style1 = (lv_style_t*)malloc(sizeof(lv_style_t));
	    	lv_style_copy(style1, &lv_style_plain);
	    	style1->text.color = LV_COLOR_WHITE;
	    	switch (size) {
	    		case 10:
	    			style1->text.font = &lv_font_dejavu_10;
	    			break;
	    		case 20:
	    			style1->text.font = &lv_font_dejavu_20;
	    			break;
	    		case 30:
	    			style1->text.font = &lv_font_dejavu_30;
	    			break;
	    		case 40:
	    			style1->text.font = &lv_font_dejavu_40;
	    			break;
	    		case 60:
	    			style1->text.font = &dejavu_60;
	    			break;
	    		case 80:
	    			style1->text.font = &dejavu_80;
	    			break;

	    		case 15:
	    			style1->text.font = &Futura_15;
	    			break;
	    		case 23:
	    			style1->text.font = &Futura_23;
	    			break;
	    		case 50:
	    			style1->text.font = &Futura_50;
	    			break;
	    		case 82:
	    			style1->text.font = &Futura_82;
	    			break;

	    		default:
	    			// style1->text.font = &lv_font_dejavu_40;
	    			style1->text.font = &Futura_50;
	    			break;
	    	}
	    	printf("> setting text element to size '%d'\n", size);
	    	lv_label_set_style(element, style1);

			lv_label_set_recolor(element, true);	// allow recoloring of the text based on input
		} else if (elementData->type == OB_IMAGE_ELEMENT) {
			// create an image element
		    element = lv_img_create(lv_page_get_scrl(card), NULL);
		}

		// set the element ID
		lv_obj_set_free_num(element, elementId);

		// attach the elementData to the element
		lv_obj_set_free_ptr(element, (void*)elementData);

		// populate the element
		lv_update_card_element(cardId, elementId, value);

		// printf("> added element! card now has %d elements\n", (int)lv_obj_count_children(lv_page_get_scrl(card)) );
		
		return 0;
	}
	
	return 1;
}

/* setter functions */
void lv_update_card_element(int cardId, int elementId, char* value) {
	char buf[256];
	struct oboo_card_element_data *elementData = NULL;
	// retrieve the element in question
	lv_obj_t *element = lv_obj_get_card_element(cardId, elementId);
	// grab the element data
	if (element != NULL) {
		elementData = lv_obj_get_free_ptr(element);

		if (elementData != NULL) {
			// update the element value based on its type
			if (elementData->type == OB_TEXT_ELEMENT) {
				// printf("> C: updating text element!\n");
				lv_label_set_text(element, value);
			} else if (elementData->type == OB_IMAGE_ELEMENT) {
				sprintf(buf, "%c:/%s", LV_OMEGA_FS_LETTER, value);
			    // printf("> C: updating image element to '%s'\n", buf);
				lv_img_set_src(element, buf);
			}
	
			// update the position of the element
			if (elementData->alignment == LV_ALIGN_IN_TOP_RIGHT) {
				lv_obj_align(element, NULL, LV_ALIGN_IN_TOP_RIGHT, elementData->posX, elementData->posY-1);
			}
			else if (elementData->alignment == LV_ALIGN_IN_TOP_MID) {
				lv_obj_align(element, NULL, LV_ALIGN_IN_TOP_MID, elementData->posX, elementData->posY-1);
			}
			else {
				lv_obj_set_pos(element, elementData->posX, elementData->posY);
			}
		}
	}
}

int lv_ob_select_card(char* direction) {
	int status;
	int targetIdx	= -1;
	lv_obj_t *cm = lv_oboo_obj_get_card_manager();
	int currentCard = (int)ob_cardview_get_card_act(cm);
	int highestIdx	= (int)ob_cardview_get_card_count(cm) - 1;

	// configure which card is going to be active (if move is valid)
	if (strcmp(direction, "left") == 0) {
		if (currentCard != 0) {
			targetIdx = currentCard - 1;
		} else {
			// loop around - go to last card
			targetIdx = highestIdx;
		}
	}
	else if (strcmp(direction, "right") == 0) {
		if (currentCard != highestIdx) {
			targetIdx = currentCard + 1;
		} else {
			// loop around - go to first card
			targetIdx = 0;
		}
	}
	else if (strcmp(direction, "first") == 0) {
		targetIdx = 0;
	}
	else if (strcmp(direction, "last") == 0) {
		targetIdx = highestIdx;
	}

	// change the active card
	lv_ob_set_active_card(targetIdx);
	// grab the free_num of the active card
	targetIdx = ob_cardview_get_card_act_fn(cm);

	return targetIdx;
}

void lv_ob_set_status(char* statusIdx, int statusValue) {
	// get the statusbar
	lv_obj_t *sb = lv_oboo_obj_get_status_bar();
	if (sb != NULL) {
		ob_statusbar_ext_t  *ext = lv_obj_get_ext_attr(sb);
		if (ext != NULL) {
			if (strcmp(statusIdx, "alarm") == 0) {
				ext->bAlarm = statusValue;
			}
			else if (strcmp(statusIdx, "bluetooth") == 0) {
				ext->bBluetooth = statusValue;
			}
			else if (strcmp(statusIdx, "wifi") == 0) {
				ext->bWifi = statusValue;
			}
			else if (strcmp(statusIdx, "battery") == 0) {
				ext->batteryLevel = statusValue;
			}
		}
	}

	lv_ob_statusbar_update();
}

/* update functions */
void lv_ob_statusbar_update () {
	char buf[64];
	// get the statusbar
	lv_obj_t *sb = lv_oboo_obj_get_status_bar();
	lv_obj_t *sbLabel = NULL;
	if (sb != NULL) {
		ob_statusbar_ext_t  *ext = lv_obj_get_ext_attr(sb);
		if (ext != NULL) {
			// build the left side status
			strcpy(buf, "");
			if (ext->bAlarm > 0) {
				strcat(buf, SYMBOL_BELL);
				// TODO: add alarm text here as well
			}
			// set the left status
			sbLabel = lv_obj_get_child_back(sb, NULL);
			lv_label_set_text(sbLabel, buf);
			lv_obj_align(sbLabel, NULL, LV_ALIGN_IN_LEFT_MID, 2, 0);

			// build the right side status
			strcpy(buf, "");
			if (ext->bBluetooth > 0) {
				strcat(buf, SYMBOL_BLUETOOTH);
			}
			if (ext->bWifi > 0) {
				strcat(buf, " ");
				strcat(buf, SYMBOL_WIFI);
			}
			if (ext->batteryLevel >= 0) {
				strcat(buf, " ");
				switch (ext->batteryLevel) {
					case 0:
						strcat(buf, SYMBOL_BATTERY_EMPTY);
						break;
					case 1:
						strcat(buf, SYMBOL_BATTERY_1);
						break;
					case 2:
						strcat(buf, SYMBOL_BATTERY_2);
						break;
					case 3:
						strcat(buf, SYMBOL_BATTERY_3);
						break;
					default:
						strcat(buf, SYMBOL_BATTERY_FULL);
						break;
				}
			}
			// set the right status
			sbLabel = lv_obj_get_child_back(sb, sbLabel);
			lv_label_set_text(sbLabel, buf);
			lv_obj_align(sbLabel, NULL, LV_ALIGN_IN_RIGHT_MID, -5, 0);
		}
	}
}

int lv_ob_set_active_card(int targetIdx) {
	lv_obj_t *cm;

	// only change the active card if the move is valid
	if (targetIdx >= 0) {
		cm = lv_oboo_obj_get_card_manager();
		printf("> new active card is index %d\n", targetIdx);
		ob_cardview_set_card_act(cm, targetIdx, true);

		return targetIdx;
	}
	return -1;
}

// returns 1 if card exists
int lv_ob_check_card_exists(int cardId) {
	// get the card
	lv_obj_t *card = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), cardId);
	
	if (card != NULL) {
		return 1;
	}
	
	return 0;
}

/* removal functions */
void _lv_remove_splash_screen () {
#if OB_SPLASH_SCREEN_ENABLED == 1
	lv_obj_t* itr = NULL;

	// loop through objects in screen, check for splash screen ID, remove it if found
	while(1) {
		itr = lv_obj_get_child_back (lv_scr_act(), itr);
		if (itr != NULL) {
			if (lv_obj_get_free_num(itr) == OB_SPLASH_SCREEN_OBJ_ID) {
				lv_obj_del(itr);
			}
		} else {
			break;
		}
	}
#endif	// OB_SPLASH_SCREEN_ENABLED
}
