/**
 * @file oboo_notification.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#define TITLE_FONT_SIZE		20
#define TEXT_WIDTH		140

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_font_t * get_font_from_height(uint8_t h);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * notif_cont = NULL;	/*Then main container for the notification*/
static lv_obj_t * notif_title = NULL;
static lv_obj_t * notif_text = NULL;

static lv_style_t style_bg;
static lv_style_t style_title;
static lv_style_t style_text;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void oboo_notification_init(void)
{

	lv_style_copy(&style_bg, &lv_style_plain);
	style_bg.body.main_color = LV_COLOR_HEX(0x2082be);
	style_bg.body.grad_color = LV_COLOR_HEX(0x2082be);
	style_bg.body.radius = 7;
	style_bg.body.padding.hor = 10;
	style_bg.body.padding.ver = 5;


	lv_style_copy(&style_title, &lv_style_plain);
	style_title.text.color = LV_COLOR_WHITE;
	style_title.text.font = get_font_from_height(TITLE_FONT_SIZE);


	lv_style_copy(&style_text, &style_title);

	notif_cont = lv_cont_create(lv_layer_top(), NULL);
	lv_cont_set_fit(notif_cont, true, true);
	lv_cont_set_layout(notif_cont, LV_LAYOUT_CENTER);
	lv_cont_set_style(notif_cont, &style_bg);
	lv_obj_set_hidden(notif_cont, true);

	notif_title = lv_label_create(notif_cont, NULL);
	lv_label_set_style(notif_title, &style_title);
	lv_label_set_long_mode(notif_title, LV_LABEL_LONG_BREAK);
	lv_obj_set_width(notif_title, TEXT_WIDTH);

	notif_text = lv_label_create(notif_cont, notif_title);
	lv_label_set_style(notif_text, &style_text);
	lv_label_set_align(notif_text, LV_LABEL_ALIGN_CENTER);

	lv_obj_align(notif_cont, NULL, LV_ALIGN_CENTER, 0, 0);

}

/**
 * Show a notification with the given parameters. Set `title` to NULL to hide it.
 */
void oboo_notification_show(const char * title, const char * msg, uint8_t font_size)
{
	if(title != NULL) {
		lv_label_set_text(notif_title, title);
		lv_obj_set_hidden(notif_title, false);
	} else {
		lv_obj_set_hidden(notif_title, true);
	}

	style_text.text.font = get_font_from_height(font_size);
	lv_label_set_text(notif_text, msg);

	lv_obj_set_hidden(notif_cont, false);
}

/**
 * Hide the the notification
 */
void oboo_notification_clear(void)
{
	lv_obj_set_hidden(notif_cont, true);
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
/**
 * Get a pointer to a built-in font when it's height is given
 */
static lv_font_t * get_font_from_height(uint8_t h)
{
	switch(h) {
#if USE_LV_FONT_DEJAVU_10
		case 10: return &lv_font_dejavu_10;
#endif
#if USE_LV_FONT_DEJAVU_10
		case 20: return &lv_font_dejavu_20;
#endif
#if USE_LV_FONT_DEJAVU_10
		case 30: return &lv_font_dejavu_30;
#endif
#if USE_LV_FONT_DEJAVU_10
		case 40: return &lv_font_dejavu_40;
#endif
		default: return NULL;
	}
}
