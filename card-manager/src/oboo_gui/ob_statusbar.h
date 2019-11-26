/**
 * @file ob_statusbar.h
 * 
 */

#ifndef OB_STATUSBAR_H
#define OB_STATUSBAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "config.h"
#if USE_OB_STATUSBAR != 0

#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/


/*Data of card*/
typedef struct
{
   /*Ext. of ancestor*/
    /*New data for this type */
    int bAlarm;
    int bBluetooth;
    int bWifi;
    int batteryLevel;
}ob_statusbar_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/


/**
 * Create a Card view object
 * @param par pointer to an object, it will be the parent of the new card
 * @param copy pointer to a card object, if not NULL then the new object will be copied from it
 * @return pointer to the created card
 */
lv_obj_t * ob_statusbar_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/
 
 /**
 * Set a status variable
 * @param cardview pointer to Card view object
 * @param id index of a card to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
// void ob_statusbar_set_card_act(lv_obj_t * cardview, uint16_t id, bool anim_en);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the status varuable
 * @param cardview pointer to Card view object
 * @return the active card index
 */
// uint16_t ob_statusbar_get_card_act(lv_obj_t * cardview);


#endif  /*USE_OB_STATUSBAR*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*OB_STATUSBAR_H*/
