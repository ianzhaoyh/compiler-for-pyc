/****************************************************/
/* File: libs.h                                     */
/* Global types and vars for Pyc compiler       */
/*                    */
/*                                                  */
/****************************************************/

#ifndef _LIBS_H_
#define _LIBS_H_

/*  Putting  the library head files here.
 *  Let this file be included in each file where they are needed */

typedef enum
{
    FALSE,
    TRUE
} Bool;
#include <stdio.h>
#include <stdlib.h>   /* for malloc and strcpy */
#include <ctype.h>
#include <string.h>


#endif
