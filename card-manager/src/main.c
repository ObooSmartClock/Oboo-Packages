#include <stdio.h>
#include <unistd.h>
#include "lvgl/lvgl.h"

// #include "lv_examples/lv_apps/demo/demo.h"

#include "config.h"

#include "runtime.h"
#include "messageQueue.h"
#include "oboo.h"



int main(int argc, char *argv[]) {
	int status;
	int counter = 0;
	int activeTab = 0;
	int bDemoActive = 0;
	int numActiveCards = 0;
	
	int tmp;
	lv_obj_t* pCard;
	
	bool clean_session = true;
	struct mosquitto *mosq = NULL;
	
	
	/* parse arguments */
	if (argc > 1) {
		if (strcmp(argv[1], "demo") == 0) {
			bDemoActive = 1;
		}
	}
	
	printf("Program version: %s %s\n", __DATE__, __TIME__);
	
	/*duktape init*/
	initRuntime();
	
	/* graphics init */
	oboo_graphics_init();
	
	/* mqtt init */
	status = initMessageQueue(MQ_HOST, MQ_HOST_PORT);
	
	if (status != 0) {
		fprintf(stderr, "ERROR: could not properly initialize!\n");
		return -1;
	}
	
	
	/* run the main program */
	
	// if (bDemoActive) {
	// 	numActiveCards = 6;
	// 	demo_setup(lv_oboo_obj_get_card_manager());
	// } else if (0) {
	// 	printf("parsing JSON!\n");
		
	// 	/*lvgl - create an initial tab with an image */
	// 	printf("adding card #1\n");
	// 	tmp = lv_add_card(0);
	// 	printf("added initial card with index '%d'\n", tmp);
		
	// 	pCard = ob_cardview_get_card(lv_oboo_obj_get_card_manager(), tmp);
	// 	printf("found card with index %d\n", tmp);
		
	// 	if (pCard != NULL) {
	// 		printf("card is not null!\n");
	// 		LV_IMG_DECLARE(img_tiger);
	// 		lv_img_create_file("img_tiger", img_tiger);       /*Create a file in the RAM FS*/
			
	// 		lv_obj_t *img_src = lv_img_create(pCard, NULL);  /*Crate an image object*/
	// 		lv_img_set_file(img_src, "U:/img_tiger");              /*Set the created file as image */
	// 		lv_obj_set_pos(img_src, 0, 0);      /*Set the positions*/
			
			
			
	// 		lv_obj_t * btn = lv_btn_create(pCard, NULL); 
	// 		lv_obj_set_pos(btn, 50, 10);
	// 		lv_obj_t * label = lv_label_create(btn, NULL);
	// 		lv_label_set_text(label, "base card!");
	
	// 		numActiveCards++;
	// 		printf("base card configured!\n");
	// 	}
		
		
	// 	/*duktape - process JSON input */
		
	// 	printf("pushing string '%s' to ctx\n", argv[1]);
	// 	duk_push_string(ctx, argv[1]);
	// 	duk_json_decode(ctx, -1);
	// 	// todo: catch error for bad json
	// 	duk_pcall(ctx, 1);
	// 	duk_pop_2(ctx);
	// 	numActiveCards++;
	// 	printf("dynamic card configured!\n");
		
	// 	// enable demoActive to switch between the cards
	// 	bDemoActive = 1;
	// }
	
	


    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
    	if (bDemoActive && ++counter > 300) {
    		activeTab = (++activeTab) % numActiveCards;
    		ob_cardview_set_card_act(lv_oboo_obj_get_card_manager(), activeTab, true);
    		printf("switching to tab %d\n", activeTab);
    		counter = 0;
    	}
    	
        oboo_graphics_handle();
        usleep(5000);
    }
    
    
    /* duktape release */
    destroyRuntime();
    
    /* mqtt release */
    destroyMessageQueue();
	

    return 0;
}
