
/**
 * @file oboo_calendar.h
 *
 */

#ifndef OBBO_CALENDAR_H
#define OBBO_CALENDAR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initializes the calendar styles and create a calendar
 * @param parent create the calendar on this parent
 * @aparam x position in x
 * @aparam y position in y
 * @aparam w width
 * @aparam h height
 * @return the created calendar object (lv_calendar)
 */
lv_obj_t * oboo_calendar_create(lv_obj_t * parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h);



/**
 * Set the some day
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 * @param year the current year (e.g. 2018)
 * @param month the current month (e.g. 7 [1..12])
 * @param day the current day (e.g. 23 [1..31])
 */
void oboo_calendar_set_day(lv_obj_t * calendar, uint32_t year, uint8_t month, uint8_t day);

/**
 * Set events for the calendar
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 * @param events an array with the events. Only the pointer is saved so the array can't be a local variable
 * @param event_cnt number of events in the array
 */
void oboo_calendar_set_events(lv_obj_t * calendar, lv_calendar_date_t * events, uint32_t event_cnt);
/**
 * Clear the events
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 */
void obbo_calendar_clear_events(lv_obj_t * calendar);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*OBBO_CALENDAR_H*/










