/**
 * @file ob_card.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "config.h"
#if USE_OB_CARDVIEW != 0

#include "ob_cardview.h"
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/
#if USE_LV_ANIMATION
#  ifndef OB_CARDVIEW_ANIM_TIME
#    define OB_CARDVIEW_ANIM_TIME  300 /*Animation time of focusing to the a list element [ms] (0: no animation)  */
#  endif
#else
#  undef  OB_CARDVIEW_ANIM_TIME
#  define OB_CARDVIEW_ANIM_TIME	0	/*No animations*/
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool ob_cardview_design(lv_obj_t * cardview, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t ob_cardview_signal(lv_obj_t * cardview, lv_signal_t sign, void * param);
static lv_res_t cardpage_signal(lv_obj_t * card_page, lv_signal_t sign, void * param);
static lv_res_t cardpage_scrl_signal(lv_obj_t * card_scrl, lv_signal_t sign, void * param);

static void cardpage_pressed_hadler(lv_obj_t * cardview, lv_obj_t * cardpage);
static void cardpage_pressing_hadler(lv_obj_t * cardview, lv_obj_t * cardpage);
static void cardpage_press_lost_hadler(lv_obj_t * cardview, lv_obj_t * cardpage);
static void cardview_realign(lv_obj_t * cardview);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_design_func_t ancestor_design;
static lv_signal_func_t ancestor_signal;
static lv_signal_func_t page_signal;
static lv_signal_func_t page_scrl_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a Tab view object
 * @param par pointer to an object, it will be the parent of the new card
 * @param copy pointer to a card object, if not NULL then the new object will be copied from it
 * @return pointer to the created card
 */
lv_obj_t * ob_cardview_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of card*/
    lv_obj_t * new_cardview = lv_obj_create(par, copy);
    lv_mem_assert(new_cardview);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_cardview);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_cardview);
    
    /*Allocate the card type specific extended data*/
    ob_cardview_ext_t * ext = lv_obj_allocate_ext_attr(new_cardview, sizeof(ob_cardview_ext_t));
    lv_mem_assert(ext);

    /*Initialize the allocated 'ext' */
    ext->drag_hor = 0;
    ext->draging = 0;
    ext->slide_enable = 1;
    ext->card_cur = 0;
    ext->point_last.x = 0;
    ext->point_last.y = 0;
    ext->content = NULL;
    ext->card_load_action = NULL;
    ext->anim_time = OB_CARDVIEW_ANIM_TIME;
    ext->card_cnt = 0;
    ext->bullet_size_act = 8;
    ext->bullet_size_ina = 5;
    ext->style_bullet_act = &lv_style_pretty_color;
    ext->style_bullet_ina = &lv_style_pretty;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_cardview, ob_cardview_signal);
    lv_obj_set_design_func(new_cardview, ob_cardview_design);

    /*Init the new card*/
    if(copy == NULL) {
        lv_obj_set_size(new_cardview, LV_HOR_RES, LV_VER_RES);

        ext->content = lv_cont_create(new_cardview, NULL);
        lv_cont_set_fit(ext->content, true, false);
        lv_cont_set_layout(ext->content, LV_LAYOUT_ROW_T);
        lv_cont_set_style(ext->content, &lv_style_transp_tight);
        lv_obj_set_height(ext->content, LV_VER_RES);

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            ob_cardview_set_style(new_cardview, OB_CARDVIEW_STYLE_BG, th->cardview.bg);
            ob_cardview_set_style(new_cardview, OB_CARDVIEW_STYLE_BULLET_ACT, th->cardview.bullet.act);
            ob_cardview_set_style(new_cardview, OB_CARDVIEW_STYLE_BULLET_INA, th->cardview.bullet.ina);
        } else {
            ob_cardview_set_style(new_cardview, OB_CARDVIEW_STYLE_BG, &lv_style_plain);
        }
    }
    /*Copy an existing card view*/
    else {

    }
    
    return new_cardview;
}

/*======================
 * Add/remove functions
 *=====================*/


/**
 * Add a card to the end
 */
lv_obj_t * ob_cardview_append_card(lv_obj_t * cardview)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);

	/*Create the container page*/
	lv_obj_t * h_new = lv_page_create(ext->content, NULL);
	lv_obj_set_size(h_new, lv_obj_get_width(cardview), lv_obj_get_height(ext->content));
	//lv_page_set_sb_mode(h, LV_SB_MODE_AUTO);  // onion.io: disabled scrollbars
	lv_page_set_sb_mode(h_new, LV_SB_MODE_OFF);
	lv_page_set_style(h_new, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
	lv_page_set_style(h_new, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

	if(page_signal == NULL) page_signal = lv_obj_get_signal_func(h_new);
	if(page_scrl_signal == NULL) page_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(h_new));
	lv_obj_set_signal_func(h_new, cardpage_signal);
	lv_obj_set_signal_func(lv_page_get_scrl(h_new), cardpage_scrl_signal);

    ext->card_cnt++;
    /*Set the first card as active*/
    if(ext->card_cnt == 1) {
    	ext->card_cur = 0;
    	ob_cardview_set_card_act(cardview, 0, false);
    	cardview_realign(cardview);       /*To set the proper btns height*/
    }
	return h_new;
}


/**
 * Add a new card with the given name
 * @param cardview pointer to Tab view object where to ass the new card
 * @return pointer to the created page object (lv_page). You can create your content here
 */
lv_obj_t * ob_cardview_add_card(lv_obj_t * cardview, int pos)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);

    /*Move all existing objects with `id < pos` to a temporary parent (required to handle `pos`)*/
    lv_obj_t * h = lv_obj_get_child_back(ext->content, NULL);
    int id;
    lv_obj_t * h_next;
    for (id = 0; h != NULL && id < pos; id++) {
    	h_next = lv_obj_get_child_back(ext->content, h);
    	lv_obj_set_parent(h, cardview);
    	h = h_next;
    }


    /*Create the container page*/
    lv_obj_t * h_new = lv_page_create(cardview, NULL);
    lv_obj_set_size(h_new, lv_obj_get_width(cardview), lv_obj_get_height(ext->content));
    //lv_page_set_sb_mode(h, LV_SB_MODE_AUTO);  // onion.io: disabled scrollbars
    lv_page_set_sb_mode(h_new, LV_SB_MODE_OFF);
    lv_page_set_style(h_new, LV_PAGE_STYLE_BG, &lv_style_transp_fit);
    lv_page_set_style(h_new, LV_PAGE_STYLE_SCRL, &lv_style_transp_fit);

    if(page_signal == NULL) page_signal = lv_obj_get_signal_func(h_new);
    if(page_scrl_signal == NULL) page_scrl_signal = lv_obj_get_signal_func(lv_page_get_scrl(h_new));
    lv_obj_set_signal_func(h_new, cardpage_signal);
    lv_obj_set_signal_func(lv_page_get_scrl(h_new), cardpage_scrl_signal);

    /*Move the remaining cards to the temp. parent too.*/
    for (; h != NULL; id++) {
    	h_next = lv_obj_get_child_back(ext->content, h);
       	lv_obj_set_parent(h, cardview);
    	h = h_next;
    }

    /*Move back all cards now in the correct order*/
    h = lv_obj_get_child_back(cardview, NULL);
    while(h != NULL) {
    	h_next = lv_obj_get_child_back(cardview, h);
    	if(h != ext->content) lv_obj_set_parent(h, ext->content);
    	h = h_next;
    }

    ext->card_cnt++;

    /*Set the first card as active*/
    if(ext->card_cnt == 1) {
        ext->card_cur = 0;
        ob_cardview_set_card_act(cardview, 0, false);
        cardview_realign(cardview);       /*To set the proper btns height*/
    }

    return h_new;
}

/**
 * Delete a card from the card view
 */
void ob_cardview_delete_card(lv_obj_t * cardview, lv_obj_t * card)
{
  //printf("ob_cardview_delete_card");
  char * cardId = lv_obj_get_free_ptr(card);
  free(cardId);
	lv_obj_del(card);
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
	ext->card_cnt--;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new card
 * @param cardview pointer to Tab view object
 * @param id index of a card to load
 * @param anim_en true: set with sliding animation; false: set immediately
 */
void ob_cardview_set_card_act(lv_obj_t * cardview, uint16_t id, bool anim_en)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    lv_style_t * style = lv_obj_get_style(ext->content);

    if(id >= ext->card_cnt) id = ext->card_cnt - 1;
    if(ext->card_load_action) ext->card_load_action(cardview, id);

    ext->card_cur = id;

    lv_coord_t cont_x = -(lv_obj_get_width(cardview) * id + style->body.padding.inner * id + style->body.padding.hor);
    if(ext->anim_time == 0 || anim_en == false) {
        lv_obj_set_x(ext->content, cont_x);
    } else {
#if USE_LV_ANIMATION
        lv_anim_t a;
        a.var = ext->content;
        a.start = lv_obj_get_x(ext->content);
        a.end = cont_x;
        a.fp = (lv_anim_fp_t)lv_obj_set_x;
        a.path = lv_anim_path_linear;
        a.end_cb = NULL;
        a.act_time = 0;
        a.time = ext->anim_time;
        a.playback = 0;
        a.playback_pause = 0;
        a.repeat = 0;
        a.repeat_pause = 0;
        lv_anim_create(&a);
#endif
    }
}

/**
 * Set an action to call when a card is loaded (Good to create content only if required)
 * ob_cardview_get_act() still gives the current (old) card (to remove content from here)
 * @param cardview pointer to a cardview object
 * @param action pointer to a function to call when a btn is loaded
 */
void ob_cardview_set_card_load_action(lv_obj_t *cardview, ob_cardview_action_t action)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    ext->card_load_action = action;
}

/**
 * Enable horizontal sliding with touch pad
 * @param cardview pointer to Tab view object
 * @param en true: enable sliding; false: disable sliding
 */
void ob_cardview_set_sliding(lv_obj_t * cardview, bool en)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    ext->slide_enable = en == false ? 0 : 1;
}

/**
 * Set the animation time of card view when a new card is loaded
 * @param cardview pointer to Tab view object
 * @param anim_time_ms time of animation in milliseconds
 */
void ob_cardview_set_anim_time(lv_obj_t * cardview, uint16_t anim_time)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    ext->anim_time = anim_time;
}

/**
 * Set the size of the bullets which indicated the pages
 * @param cardview pointer to Card view object
 * @param size_act size of the active bullet
 * @param size_ina size of the inactive bullet
 */
void ob_cardview_set_bullet_size(lv_obj_t * cardview, lv_coord_t size_act, lv_coord_t size_ina)
{
	ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
	ext->bullet_size_act = size_act;
	ext->bullet_size_ina  = size_ina;
	lv_obj_invalidate(cardview);
}


/**
 * Set the style of a card view
 * @param cardview pointer to a tan view object
 * @param type which style should be set
 * @param style pointer to the new style
 */
void ob_cardview_set_style(lv_obj_t *cardview, ob_cardview_style_t type, lv_style_t *style)
{
    ob_cardview_ext_t *ext = lv_obj_get_ext_attr(cardview);

    switch(type) {
        case OB_CARDVIEW_STYLE_BG:
            lv_obj_set_style(cardview, style);
            break;
        case OB_CARDVIEW_STYLE_BULLET_ACT:
        	ext->style_bullet_act = style;
        	lv_obj_invalidate(cardview);
        	break;
        case OB_CARDVIEW_STYLE_BULLET_INA:
        	ext->style_bullet_ina = style;
        	lv_obj_invalidate(cardview);
        	break;
    }
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the index of the currently active card
 * @param cardview pointer to Tab view object
 * @return the active btn index
 */
uint16_t ob_cardview_get_card_act(lv_obj_t * cardview)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    return ext->card_cur;
}

/**
 * Get the free_num index of the currently active card
 * @param cardview pointer to Card view object
 * @return the active card free_num index
 */
uint16_t ob_cardview_get_card_act_fn(lv_obj_t * cardview)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    uint16_t activeCardId = ext->card_cur;
    uint16_t i = 0;
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);

    while(page != NULL && i != activeCardId) {
        i++;
        page = lv_obj_get_child_back(ext->content, page);
    }
    
    if(i == activeCardId && page != NULL) return lv_obj_get_free_num(page);

    return 0;
}

/**
 * Get the number of cards
 * @param cardview pointer to Tab view object
 * @return btn count
 */
uint16_t ob_cardview_get_card_count(lv_obj_t * cardview)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    return ext->card_cnt;
}

/**
 * Get the page (content area) of a card
 * @param cardview pointer to Tab view object
 * @param id index of the card (>= 0)
 * @return pointer to page (lv_page) object
 */
lv_obj_t * ob_cardview_get_card(lv_obj_t * cardview, uint16_t id)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    uint16_t i = -1;
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);

    // implementation where id is ordering
    // while(page != NULL && i != id) {
    //     i++;
    //     page = lv_obj_get_child_back(ext->content, page);
    // }
    
    // implementation where id is retrieved from the free_num
    while(page != NULL) {
        i = lv_obj_get_free_num(page);
        if (i == id) {
            // found the match
            break;
        }
        // grab the next card
        page = lv_obj_get_child_back(ext->content, page);
    }

    if(i == id) return page;

    return NULL;
}

/**
 * Get the page (content area) of a card by specifying a matching free_ptr string value
 * @param cardview pointer to Tab view object
 * @param searchKey string value of free_ptr of matching card
 * @return pointer to page (lv_page) object
 */
lv_obj_t * ob_cardview_get_card_by_fp_string(lv_obj_t * cardview, char* searchKey)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);
    char* pageFp = NULL;

    while(page != NULL) {
        // grab the free_ptr
        pageFp = lv_obj_get_free_ptr(page);
        if (pageFp != NULL) {
            if (strcmp(pageFp, searchKey) == 0) {
                // found the match
                break;
            }
        }
        // grab the next card
        page = lv_obj_get_child_back(ext->content, page);
    }

    return page;
}

/**
 * Get the card load action
 * @param cardview pointer to a cardview object
 * @param return the current btn load action
 */
ob_cardview_action_t ob_cardview_get_card_load_action(lv_obj_t *cardview)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    return ext->card_load_action;
}

/**
 * Get horizontal sliding is enabled or not
 * @param cardview pointer to Tab view object
 * @return true: enable sliding; false: disable sliding
 */
bool ob_cardview_get_sliding(lv_obj_t * cardview)
{
    ob_cardview_ext_t *ext = lv_obj_get_ext_attr(cardview);
    return ext->slide_enable ? true : false;
}

/**
 * Get the animation time of card view when a new card is loaded
 * @param cardview pointer to Tab view object
 * @return time of animation in milliseconds
 */
uint16_t ob_cardview_get_anim_time(lv_obj_t * cardview)
{
    ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
    return ext->anim_time;
}

/**
 * Get the size of the active bullet
 * @param cardview pointer to Card view object
 * @return size of the bullets (inactive bullets have half size)
 */
lv_coord_t ob_cardview_get_bullet_size_act(lv_obj_t * cardview)
{
	ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
	return ext->bullet_size_act;
}

/**
 * Get the size of the active bullet
 * @param cardview pointer to Card view object
 * @return size of the bullets (inactive bullets have half size)
 */
lv_coord_t ob_cardview_get_bullet_size_ina(lv_obj_t * cardview)
{
	ob_cardview_ext_t  * ext = lv_obj_get_ext_attr(cardview);
	return ext->bullet_size_ina;
}

/**
 * Get a style of a card view
 * @param cardview pointer to a ab view object
 * @param type which style should be get
 * @return style pointer to a style
 */
lv_style_t * ob_cardview_get_style(lv_obj_t *cardview, ob_cardview_style_t type)
{
    ob_cardview_ext_t *ext = lv_obj_get_ext_attr(cardview);

    switch (type) {
        case OB_CARDVIEW_STYLE_BG:           return lv_obj_get_style(cardview);
        case OB_CARDVIEW_STYLE_BULLET_ACT:	 return ext->style_bullet_act;
        case OB_CARDVIEW_STYLE_BULLET_INA:	 return ext->style_bullet_ina;
        default: return NULL;
    }

    /*To avoid warning*/
    return NULL;
}

/**
 * Get highest free_num of all cards in carview
 * @param cardview pointer to a card view object
 * @param type which style should be get
 * @return value of highest free_num
 */
uint16_t ob_cardview_get_max_free_num(lv_obj_t *cardview) {
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    uint16_t i = 0;
    lv_obj_t * page = lv_obj_get_child_back(ext->content, NULL);

    while(page != NULL) {
        if (lv_obj_get_free_num(page) > i) {
            i = lv_obj_get_free_num(page);
        }
        page = lv_obj_get_child_back(ext->content, page);
    }
    
    return i;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool ob_cardview_design(lv_obj_t * cardview, const lv_area_t * mask, lv_design_mode_t mode)
{
	if(mode == LV_DESIGN_COVER_CHK || mode == LV_DESIGN_DRAW_MAIN) {
		return ancestor_design(cardview, mask, mode);
	}
	else if (mode == LV_DESIGN_DRAW_POST) {
		ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
		uint16_t card_cnt = ob_cardview_get_card_count(cardview);
		uint16_t card_act_id = ob_cardview_get_card_act(cardview);
		lv_coord_t w = lv_obj_get_width(cardview);
		lv_obj_t * content = ext->content;
		lv_coord_t h_space = ext->style_bullet_act->body.padding.hor;
		lv_coord_t v_space = ext->style_bullet_act->body.padding.ver;


		lv_coord_t bullet_area_w = h_space * (card_cnt- 1);

		lv_coord_t x_base = w / 2 - bullet_area_w / 2;

		lv_coord_t y_base;
		/*If v_space > 0 align from the top if < 0 align from the bottom*/
		if(v_space > 0) {
			y_base = content->coords.y1 + v_space;
		} else {
			v_space = -v_space;
			y_base = content->coords.y2 - v_space;
		}

		lv_area_t area;

		int i;
		for(i = 0; i < card_cnt; i++) {
			if(i == card_act_id) {
				area.y1 = y_base - ext->bullet_size_act / 2;
				area.y2 = area.y1 + ext->bullet_size_act;
				area.x1 = x_base + i * h_space - ext->bullet_size_act / 2;
				area.x2 = area.x1 + ext->bullet_size_act;
				lv_draw_rect(&area, mask, ext->style_bullet_act);
			} else {
				area.y1 = y_base - ext->bullet_size_ina / 2;
				area.y2 = area.y1 + ext->bullet_size_ina;
				area.x1 = x_base + i * h_space - ext->bullet_size_ina / 2;
				area.x2 = area.x1 + ext->bullet_size_ina;
				lv_draw_rect(&area, mask, ext->style_bullet_ina);
			}
		}
	}
	return true;
}



/**
 * Signal function of the Tab view
 * @param cardview pointer to a Tab view object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t ob_cardview_signal(lv_obj_t * cardview, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cardview, sign, param);
    if(res != LV_RES_OK) return res;

    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    if(sign == LV_SIGNAL_CLEANUP) {
        ext->content= NULL;
    }
    else if(sign == LV_SIGNAL_CORD_CHG) {
        if(ext->content != NULL &&
          (lv_obj_get_width(cardview) != lv_area_get_width(param) ||
           lv_obj_get_height(cardview) != lv_area_get_height(param)))
        {
            cardview_realign(cardview);
        }
    }

    return res;
}


/**
 * Signal function of a card's page
 * @param card pointer to a card page object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t cardpage_signal(lv_obj_t * card_page, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = page_signal(card_page, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * cont = lv_obj_get_parent(card_page);
    lv_obj_t * cardview = lv_obj_get_parent(cont);

    if(ob_cardview_get_sliding(cardview) == false) return res;

    if(sign == LV_SIGNAL_PRESSED) {
        cardpage_pressed_hadler(cardview, card_page);
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        cardpage_pressing_hadler(cardview, card_page);
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        cardpage_press_lost_hadler(cardview, card_page);
    }

    return res;
}
/**
 * Signal function of the card page's scrollable object
 * @param card_scrl pointer to a card page's scrollable object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t cardpage_scrl_signal(lv_obj_t * card_scrl, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = page_scrl_signal(card_scrl, sign, param);
    if(res != LV_RES_OK) return res;

    lv_obj_t * card_page = lv_obj_get_parent(card_scrl);
    lv_obj_t * cont = lv_obj_get_parent(card_page);
    lv_obj_t * cardview = lv_obj_get_parent(cont);

    if(ob_cardview_get_sliding(cardview) == false) return res;

    if(sign == LV_SIGNAL_PRESSED) {
        cardpage_pressed_hadler(cardview, card_page);
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        cardpage_pressing_hadler(cardview, card_page);
    }
    else if(sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        cardpage_press_lost_hadler(cardview, card_page);
    }

    return res;
}

/**
 * Called when a card's page or scrollable object is pressed
 * @param cardview pointer to the btn view object
 * @param cardpage pointer to the page of a btn
 */
static void cardpage_pressed_hadler(lv_obj_t * cardview, lv_obj_t * cardpage)
{
    (void)cardpage;

    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    lv_indev_t * indev = lv_indev_get_act();
    lv_indev_get_point(indev, &ext->point_last);
}

/**
 * Called when a card's page or scrollable object is being pressed
 * @param cardview pointer to the btn view object
 * @param cardpage pointer to the page of a btn
 */
static void cardpage_pressing_hadler(lv_obj_t * cardview, lv_obj_t * cardpage)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_coord_t x_diff = point_act.x - ext->point_last.x;
    lv_coord_t y_diff = point_act.y - ext->point_last.y;

    if(ext->draging == 0) {
        if(x_diff >= LV_INDEV_DRAG_LIMIT || x_diff<= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_hor = 1;
            ext->draging = 1;
            lv_obj_set_drag(lv_page_get_scrl(cardpage), false);
        } else if(y_diff >= LV_INDEV_DRAG_LIMIT || y_diff <= -LV_INDEV_DRAG_LIMIT) {
            ext->drag_hor = 0;
            ext->draging = 1;
        }
    }
    if(ext->drag_hor) {
        lv_obj_set_x(ext->content, lv_obj_get_x(ext->content) + point_act.x - ext->point_last.x);
        ext->point_last.x = point_act.x;
        ext->point_last.y = point_act.y;

   }
}

/**
 * Called when a card's page or scrollable object is released or the press id lost
 * @param cardview pointer to the btn view object
 * @param cardpage pointer to the page of a btn
 */
static void cardpage_press_lost_hadler(lv_obj_t * cardview, lv_obj_t * cardpage)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);
    ext->drag_hor = 0;
    ext->draging = 0;

    lv_obj_set_drag(lv_page_get_scrl(cardpage), true);

    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t point_act;
    lv_indev_get_point(indev, &point_act);
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);
    lv_coord_t x_predict = 0;

    while(vect.x != 0)   {
        x_predict += vect.x;
        vect.x = vect.x * (100 - LV_INDEV_DRAG_THROW) / 100;
    }

    lv_coord_t page_x1 = cardpage->coords.x1 - cardview->coords.x1 + x_predict;
    lv_coord_t page_x2 = page_x1 + lv_obj_get_width(cardpage);
    lv_coord_t treshold = lv_obj_get_width(cardview) / 2;

    uint16_t card_cur = ext->card_cur;
    if(page_x1 > treshold) {
        if(card_cur != 0) card_cur--;
    } else if(page_x2 < treshold) {
        if(card_cur < ext->card_cnt - 1) card_cur++;
    }

    ob_cardview_set_card_act(cardview, card_cur, true);
}

/**
 * Realign and resize the elements of Tab view
 * @param cardview pointer to a Tab view object
 */
static void cardview_realign(lv_obj_t * cardview)
{
    ob_cardview_ext_t * ext = lv_obj_get_ext_attr(cardview);

    if(ext->card_cnt != 0) {

    }

    lv_obj_set_height(ext->content, lv_obj_get_height(cardview));

    lv_obj_t * pages = lv_obj_get_child(ext->content, NULL);
    while(pages != NULL) {
        if(lv_obj_get_signal_func(pages) == cardpage_signal) {  /*Be sure to adjust only the pages (user can other things)*/
            lv_obj_set_size(pages, lv_obj_get_width(cardview), lv_obj_get_height(ext->content));
        }
        pages = lv_obj_get_child(ext->content, pages);
    }

    ob_cardview_set_card_act(cardview, ext->card_cur, false);
}
#endif