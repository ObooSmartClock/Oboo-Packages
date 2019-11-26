#ifndef _OBOO_HEADER_H_
#define _OBOO_HEADER_H_

#include "config.h"
#include "lvgl/lvgl.h"
#include "ob_statusbar.h"
#include "ob_cardview.h"
#include "oboo_notification.h"
#include "oboo_calendar.h"

#define OB_CARDVIEW_CREATE_LAST  0xFFFF

typedef enum {
    OB_TEXT_ELEMENT = 0,
    OB_IMAGE_ELEMENT,
    OB_CALENDAR_ELEMENT
} ob_element_type;

typedef enum {
    OB_CARD_VIEW_OBJ_ID = 0,
    OB_SPLASH_SCREEN_OBJ_ID,
    OB_STATUS_BAR_OBJ_ID,
    OB_NUM_OBJ_IDS
} ob_main_object_id;

struct oboo_card_element_data {
    ob_element_type type;
    lv_align_t alignment;
    int posX;
    int posY;
} oboo_card_element_data;

// function prototypes
void oboo_graphics_init();
void oboo_graphics_handle();

// add a card to the card manager
//	returns: index of the card
int lv_add_card (int bgColor);
int lv_add_card_idx (int bgColor, int pos);

int lv_set_card_uid (int cardId, char* uid);

void lv_delete_card(int cardId);

int lv_add_card_element(int cardId, int elementId, char* elementType, int posX, int posY, int size, char* alignment, char* value);
void lv_update_card_element(int cardId, int elementId, char* value);

int lv_ob_select_card(char* direction);
void lv_ob_set_status(char* statusIdx, int statusValue);

int lv_ob_set_active_card(int targetIdx);

int lv_ob_check_card_exists(int cardId);



void _lv_fb_init ();
void _lv_omega_fs_init ();
void _lv_init_oboo_gui ();

void _lv_init_splash_screen ();
void _lv_remove_splash_screen ();

void lv_ob_statusbar_update ();


lv_obj_t* lv_obj_get_child_by_index(lv_obj_t *parent, int idx);
lv_obj_t* lv_obj_get_child_by_fn_index(lv_obj_t *parent, int idx);
lv_obj_t* lv_obj_get_card_element(int cardIdx, int elementIdx);

lv_obj_t* lv_oboo_obj_get_card_manager();
lv_obj_t* lv_oboo_obj_get_status_bar();
int lv_oboo_get_card_id_by_fp(char* uid);

// global declaration of lvgl screen
lv_obj_t * scr;

#endif  // _OBOO_HEADER_H_
