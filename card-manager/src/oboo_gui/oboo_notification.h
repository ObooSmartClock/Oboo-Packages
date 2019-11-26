/**
 * @file oboo_notification.h
 *
 */

#ifndef OBOO_NOTIFICATION_H
#define OBOO_NOTIFICATION_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/
 #define OBOO_NOTIFICATION_DEFAULT_FONT_SIZE    20

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void oboo_notification_init(void);

/**
 * Show a notification with the given parameters. Set `title` to NULL to hide it.
 */
void oboo_notification_show(const char * title, const char * msg, uint8_t font_size);

/**
 * Hide the the notification
 */
void oboo_notification_clear(void);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*OBOO_NOTIFICATION_H*/
