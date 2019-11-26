#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "lv_omega_fs.h"

/**
 * Open a file from the PC
 * @param file_p pointer to a FILE* variable
 * @param fn absolute path to the file
//  * @param fn name of the file.
 * @param mode element of 'fs_mode_t' enum or its 'OR' connection (e.g. FS_MODE_WR | FS_MODE_RD)
 * @return LV_FS_RES_OK: no error, the file is opened
 *         any error from lv_fs_res_t enum
 */
/*static*/ lv_fs_res_t pcfs_open (void * file_p, const char * fn, lv_fs_mode_t mode)
{
    errno = 0;

    const char * flags = "";

    if(mode == LV_FS_MODE_WR) flags = "wb";
    else if(mode == LV_FS_MODE_RD) flags = "rb";
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = "a+";

    char buf[256];
    // onion.io: set the path to the file based on whether the file is an absolute or relative path
    if (fn[0] == '/') {
        sprintf(buf, "%s", fn);
    }
    else {
        /*Make the path relative to the current directory (the projects root folder)*/
        sprintf(buf, "./%s", fn);
    }

    pc_file_t f = fopen(buf, flags);
    // pc_file_t f = fopen(fn, flags);
    if((long int)f <= 0) {
        // onion.io: tmp debug
        printf("pcfs_open: cannot open file '%s'\n", buf);
        return LV_FS_RES_UNKNOWN;
    }
    else {
        fseek(f, 0, SEEK_SET);

        /* 'file_p' is pointer to a file descriptor and
         * we need to store our file descriptor here*/
        pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
        *fp = f;
    }

    return LV_FS_RES_OK;
}


/**
 * Close an opened file
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
/*static*/ lv_fs_res_t pcfs_close (void * file_p)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    fclose(*fp);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param file_p pointer to a FILE variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
/*static*/ lv_fs_res_t pcfs_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    *br = fread(buf, 1, btr, *fp);
    return LV_FS_RES_OK;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param file_p pointer to a FILE* variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
/*static*/ lv_fs_res_t pcfs_seek (void * file_p, uint32_t pos)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    fseek(*fp, pos, SEEK_SET);
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FILE* variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv__fs_res_t enum
 */
/*static*/ lv_fs_res_t pcfs_tell (void * file_p, uint32_t * pos_p)
{
    pc_file_t *fp = file_p;         /*Just avoid the confusing casings*/
    *pos_p = ftell(*fp);
    return LV_FS_RES_OK;
}