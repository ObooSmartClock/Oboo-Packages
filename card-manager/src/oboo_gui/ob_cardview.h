/**
 * @file ob_cardview.h
 * 
 */

#ifndef OB_CARDVIEW_H
#define OB_CARDVIEW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "config.h"
#if USE_OB_CARDVIEW != 0


/*Testing of dependencies*/
#if USE_LV_BTNM == 0
#error "ob_cardview: lv_btnm is required. Enable it in lv_conf.h (USE_LV_BTNM  1) "
#endif

#if USE_LV_PAGE == 0
#error "ob_cardview: lv_page is required. Enable it in lv_conf.h (USE_LV_PAGE  1) "
#endif

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/* parametes: pointer to a cardview object, card_id*/
typedef void (*ob_cardview_action_t)(lv_obj_t *, uint16_t);

/*Data of card*/
typedef struct
{
   /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * content;   /*A rectangle to show the current card*/
    lv_point_t point_last;
    uint16_t card_cur;
    uint16_t card_cnt;
    uint16_t anim_time;
    lv_coord_t bullet_size_act;
    lv_coord_t bullet_size_ina;
    uint8_t slide_enable :1;    /*1: enable horizontal sliding by touch pad*/
    uint8_t draging :1;
    uint8_t drag_hor :1;
    ob_cardview_action_t card_load_action;
    lv_style_t * style_bullet_act;
    lv_style_t * style_bullet_ina;
}ob_cardview_ext_t;

typedef enum {
    OB_CARDVIEW_STYLE_BG,
    OB_CARDVIEW_STYLE_BULLET_ACT,
    OB_CARDVIEW_STYLE_BULLET_INA,
}ob_cardview_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a Card view object
 * @param par pointer to an object, it will be the parent of the new card
 * @param copy pointer to a card object, if not NULL then the new object will be copied from it
 * @return pointer to the created card
 */
lv_obj_t * ob_cardview_create(lv_obj_t * par, lv_obj_t * copy);

/*======================
 * Add/remove functions
 *=====================*/

/**
 * Add a card to the end
 */
lv_obj_t * ob_cardview_append_card(lv_obj_t * cardview);

/**
 * Add a new card with the given name
 * @param cardview pointer to Card view object where to ass the new card
 * @return pointer to the created page object (lv_page). You can create your content here
 */
lv_obj_t * ob_cardview_add_card(lv_obj_t * cardview, int pos);


void ob_cardview_delete_card(lv_obj_t * cardview, lv_obj_t * card);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new card
 * @param cardview pointer to Card view object
 * @param id index of a card to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void ob_cardview_set_card_act(lv_obj_t * cardview, uint16_t id, bool anim_en);

/**
 * Set an action to call when a card is loaded (Good to create content only if required)
 * ob_cardview_get_act() still gives the current (old) card (to remove content from here)
 * @param cardview pointer to a cardview object
 * @param action pointer to a function to call when a card is loaded
 */
void ob_cardview_set_card_load_action(lv_obj_t *cardview, ob_cardview_action_t action);

/**
 * Enable horizontal sliding with touch pad
 * @param cardview pointer to Card view object
 * @param en true: enable sliding; false: disable sliding
 */
void ob_cardview_set_sliding(lv_obj_t * cardview, bool en);

/**
 * Set the animation time of card view when a new card is loaded
 * @param cardview pointer to Card view object
 * @param anim_time time of animation in milliseconds
 */
void ob_cardview_set_anim_time(lv_obj_t * cardview, uint16_t anim_time);

/**
 * Set the size of the bullets which indicated the pages
 * @param cardview pointer to Card view object
 * @param size_act size of the active bullet
 * @param size_ina size of the inactive bullet
 */
void ob_cardview_set_bullet_size(lv_obj_t * cardview, lv_coord_t size_act, lv_coord_t size_ina);

/**
 * Set the style of a card view
 * @param cardview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void ob_cardview_set_style(lv_obj_t *cardview, ob_cardview_style_t type, lv_style_t *style);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active card
 * @param cardview pointer to Card view object
 * @return the active card index
 */
uint16_t ob_cardview_get_card_act(lv_obj_t * cardview);

/**
 * Get the free_num index of the currently active card
 * @param cardview pointer to Card view object
 * @return the active card free_num index
 */
uint16_t ob_cardview_get_card_act_fn(lv_obj_t * cardview);


/**
 * Get the number of cards
 * @param cardview pointer to Card view object
 * @return card count
 */
uint16_t ob_cardview_get_card_count(lv_obj_t * cardview);
/**
 * Get the page (content area) of a card
 * @param cardview pointer to Card view object
 * @param id index of the card (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * ob_cardview_get_card(lv_obj_t * cardview, uint16_t id);

/**
 * Get the card load action
 * @param cardview pointer to a cardview object
 * @param return the current card load action
 */
ob_cardview_action_t ob_cardview_get_card_load_action(lv_obj_t *cardview);
/**
 * Get the page (content area) of a card by specifying a matching free_ptr string value
 * @param cardview pointer to Tab view object
 * @param searchKey string value of free_ptr of matching card
 * @return pointer to page (lv_page) object
 */
lv_obj_t * ob_cardview_get_card_by_fp_string(lv_obj_t * cardview, char* searchKey);

/**
 * Get horizontal sliding is enabled or not
 * @param cardview pointer to Card view object
 * @return true: enable sliding; false: disable sliding
 */
bool ob_cardview_get_sliding(lv_obj_t * cardview);

/**
 * Get the animation time of card view when a new card is loaded
 * @param cardview pointer to Card view object
 * @return time of animation in milliseconds
 */
uint16_t ob_cardview_get_anim_time(lv_obj_t * cardview);



/**
 * Get the size of the active bullet
 * @param cardview pointer to Card view object
 * @return size of the bullets (inactive bullets have half size)
 */
lv_coord_t ob_cardview_get_bullet_size_act(lv_obj_t * cardview);

/**
 * Get the size of the active bullet
 * @param cardview pointer to Card view object
 * @return size of the bullets (inactive bullets have half size)
 */
lv_coord_t ob_cardview_get_bullet_size_ina(lv_obj_t * cardview);

/**
 * Get a style of a card view
 * @param cardview pointer to a card view object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * ob_cardview_get_style(lv_obj_t *cardview, ob_cardview_style_t type);

/**
 * Get highest free_num of all cards in carview
 * @param cardview pointer to a card view object
 * @param type which style should be get
 * @return value of highest free_num
 */
uint16_t ob_cardview_get_max_free_num(lv_obj_t *cardview);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_OB_CARDVIEW*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*OB_CARDVIEW_H*/
