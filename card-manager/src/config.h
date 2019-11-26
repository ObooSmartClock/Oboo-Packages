#ifndef OBOO_CARD_MANAGER_CONFIG_H
#define OBOO_CARD_MANAGER_CONFIG_H


#define OB_TOP_BAR_RES              24
#define OB_SPLASH_SCREEN_ENABLED    1

#define OB_PC_SIMULATOR             0
#define USE_OB_THEME_OBOO           1       /*Oboo Smart Clock Theme*/

/*Card view (dependencies: lv_page, lv_btnm)*/
#define USE_OB_CARDVIEW      1
#if USE_OB_CARDVIEW != 0
#define OB_CARDVIEW_ANIM_TIME    150     /*Time of slide animation [ms] (0: no animation)*/
#endif

/*Calendar element default size */
#define OB_CAL_DEFAULT_WIDTH    230
#define OB_CAL_DEFAULT_HEIGHT   240

/*Status bar*/
#define USE_OB_STATUSBAR     1

/*MQ settings*/
#define MQ_HOST                     "localhost"
#define MQ_HOST_PORT                1883

#define MQ_CARD_TOPIC		    "/card"
#define MQ_STATUS_TOPIC	        "/status"
#define MQ_CARD_RESPONSE_TOPIC	"/cardResponse"
#define MQ_NOTIFICATION_TOPIC	"/notification"

/*General*/
#define MAX_CHAR_LEN        256


#endif  // OBOO_CARD_MANAGER_CONFIG_H
