/**
 * @file ob_card.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "config.h"
#if USE_OB_STATUSBAR != 0

#include "ob_statusbar.h"

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * ob_statusbar_create(lv_obj_t * par, lv_obj_t * copy) {
    /*Create the ancestor of statubar*/
    lv_obj_t * new_statusbar = lv_obj_create(par, copy);
    lv_mem_assert(new_statusbar);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_statusbar);
    
    /*Allocate the statusbar type specific extended data*/
    ob_statusbar_ext_t * ext = lv_obj_allocate_ext_attr(new_statusbar, sizeof(ob_statusbar_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->bAlarm         = -1;
    ext->bBluetooth     = -1;
    ext->bWifi          = -1;
    ext->batteryLevel   = -1;
    
    return new_statusbar;
}


#endif
