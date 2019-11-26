/**
 * @file oboo_calendar.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
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
lv_obj_t * oboo_calendar_create(lv_obj_t * parent, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
	/*Background style*/
	static lv_style_t style_calendar_bg;
	lv_style_copy(&style_calendar_bg, &lv_style_plain);
	style_calendar_bg.body.main_color = LV_COLOR_HEX(0xeef3f9);
	style_calendar_bg.body.grad_color = LV_COLOR_HEX(0xf0f3f4);
	style_calendar_bg.body.padding.hor = LV_DPI / 12;			/*Margin on  the left and right of the rows of date numbers*/
	style_calendar_bg.body.padding.inner = LV_DPI / 6;			/*Margin above the date numbers*/
	style_calendar_bg.body.padding.ver = LV_DPI / 10;			/*Margin below the date numbers*/
	style_calendar_bg.body.border.color = LV_COLOR_HEX3(0x888);
	style_calendar_bg.body.border.width = 1;
	style_calendar_bg.body.border.part = LV_BORDER_LEFT | LV_BORDER_RIGHT | LV_BORDER_BOTTOM;
	style_calendar_bg.text.color = LV_COLOR_HEX3(0x666);

	/*Inactive day's style (prev/next month)*/
	static lv_style_t style_calendar_inactive_days;
	lv_style_copy(&style_calendar_inactive_days, &style_calendar_bg);
	style_calendar_inactive_days.text.color = LV_COLOR_HEX3(0xbbb);

	/*Header style*/
	static lv_style_t style_calendar_header;
	lv_style_copy(&style_calendar_header, &lv_style_plain);
	style_calendar_header.body.main_color = LV_COLOR_HEX(0xee8065);
	style_calendar_header.body.grad_color = style_calendar_header.body.main_color;
	style_calendar_header.body.padding.hor = LV_DPI / 10;			/*Margin next to the arrows*/
	style_calendar_header.body.border.color = LV_COLOR_HEX(0x8e3823);
	style_calendar_header.body.radius = 0;
	style_calendar_header.body.border.width = 1;
	style_calendar_header.body.border.part = LV_BORDER_LEFT | LV_BORDER_RIGHT | LV_BORDER_TOP;
	style_calendar_header.text.font = LV_FONT_DEFAULT;
	style_calendar_header.text.color = LV_COLOR_WHITE;

	/*Arrow's pressed style (on the header)*/
	static lv_style_t style_calendar_header_pr;
	lv_style_copy(&style_calendar_header_pr, &style_calendar_header);
	style_calendar_header_pr.text.color = LV_COLOR_HEX(0x8e3823);	/*Dark orange for pressed arrows*/

	/*Day's name style (Sun..Sat)*/
	static lv_style_t style_calendar_day_names;
	lv_style_copy(&style_calendar_day_names, &lv_style_plain);
	style_calendar_day_names.body.padding.ver = LV_DPI / 8;			/*Space above the day names*/
	style_calendar_day_names.body.padding.hor = LV_DPI / 12;		/*Space on the left and right*/
	style_calendar_day_names.text.color = LV_COLOR_HEX(0x62afd8);
	// style_calendar_day_names.text.font = &lv_font_dejavu_10;
	style_calendar_day_names.text.font = &lv_font_dejavu_20;

	/*Highlighted day's style*/
	static lv_style_t style_calendar_highlighted_days;
	lv_style_copy(&style_calendar_highlighted_days, &lv_style_plain);
	style_calendar_highlighted_days.text.color = LV_COLOR_HEX(0xee8065);

	/*"Week box" style*/
	static lv_style_t style_calendar_week_box;
	lv_style_copy(&style_calendar_week_box, &lv_style_pretty);
	style_calendar_week_box.body.main_color = LV_COLOR_HEX(0xb0dcec);
	style_calendar_week_box.body.grad_color = style_calendar_week_box.body.main_color;
	style_calendar_week_box.body.radius = LV_DPI / 30;
	style_calendar_week_box.body.border.width = 2;
	style_calendar_week_box.body.padding.ver = LV_DPI / 30;
	style_calendar_week_box.body.padding.hor = 0;
	style_calendar_week_box.text.color = LV_COLOR_HEX(0x6da0b5);

	/*"Today box" style*/
	static lv_style_t style_calendar_today_box;
	lv_style_copy(&style_calendar_today_box, &lv_style_pretty);
	style_calendar_today_box.body.main_color = LV_COLOR_HEX(0x46baeb);
	style_calendar_today_box.body.grad_color = style_calendar_today_box.body.main_color;
	style_calendar_today_box.body.radius = LV_DPI / 30;
	style_calendar_today_box.body.border.width = 2;
	style_calendar_today_box.body.padding.ver = LV_DPI / 16;
	style_calendar_today_box.body.padding.hor = 0;
	style_calendar_today_box.text.color = LV_COLOR_HEX(0xd4edf3);

	/*Create a calendar*/
	lv_obj_t * calendar = lv_calendar_create(parent, NULL);
	lv_obj_set_size(calendar, 230, 200);
	lv_obj_set_pos(calendar, 5, 5);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_BG, &style_calendar_bg);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_INACTIVE_DAYS, &style_calendar_inactive_days);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_HEADER, &style_calendar_header);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_DAY_NAMES, &style_calendar_day_names);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_HIGHLIGHTED_DAYS, &style_calendar_highlighted_days);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_HEADER_PR, &style_calendar_header_pr);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_WEEK_BOX, &style_calendar_week_box);
	lv_calendar_set_style(calendar, LV_CALENDAR_STYLE_TODAY_BOX, &style_calendar_today_box);
	
	/*Set the day names*/
	static const char * day_names[7] = {"S", "M", "T", "W", "T", "F", "S"};		/*Must be static or global because only it's pointer will be saved*/
    lv_calendar_set_day_names(calendar, day_names);

	/* position and size */
	lv_obj_set_pos(calendar, x, y);
	lv_obj_set_size(calendar, w, h);

	return calendar;
}


/**
 * Set some day 
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 * @param year the current year (e.g. 2018)
 * @param month the current month (e.g. 7 [1..12])
 * @param day the current day (e.g. 23 [1..31])
 */
void oboo_calendar_set_day(lv_obj_t * calendar, uint32_t year, uint8_t month, uint8_t day)
{
	lv_calendar_date_t date;

	date.year = year;
	date.month = month;
	date.day = day;
	lv_calendar_set_today_date(calendar, &date);
	lv_calendar_set_showed_date(calendar, &date);
}

/**
 * Set events for the calendar
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 * @param events an array with the events. Only the pointer is saved so the array can't be a local variable
 * @param event_cnt number of events in the array
 */
void oboo_calendar_set_events(lv_obj_t * calendar, lv_calendar_date_t * events, uint32_t event_cnt)
{
	lv_calendar_set_highlighted_dates(calendar, events, event_cnt);
}

/**
 * Clear the events
 * @param calendar pointer to calendar (return value of `oboo_calendar_create`)
 */
void obbo_calendar_clear_events(lv_obj_t * calendar)
{
	lv_calendar_set_highlighted_dates(calendar, NULL, 0);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
