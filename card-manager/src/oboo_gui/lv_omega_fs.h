#include "lvgl/lvgl.h"
#include <stdio.h>


#ifndef LV_OMEGA_FS_H
#define LV_OMEGA_FS_H

/**********************
 *      TYPEDEFS
 **********************/
typedef  FILE*  pc_file_t;

#define LV_OMEGA_FS_LETTER    'O'

/**********************
 *  FUNCTION PROTOTYPES
 **********************/
/*Interface functions to standard C file functions (only the ones required for image handling)*/
/*static*/ lv_fs_res_t pcfs_open (void * file_p, const char * fn, lv_fs_mode_t mode);
/*static*/ lv_fs_res_t pcfs_close (void * file_p);
/*static*/ lv_fs_res_t pcfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br);
/*static*/ lv_fs_res_t pcfs_seek (void * file_p, uint32_t pos);
/*static*/ lv_fs_res_t pcfs_tell (void * file_p, uint32_t * pos_p);

#endif  // LV_OMEGA_FS_H