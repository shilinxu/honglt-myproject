/*************************************************
 * libr263: fast H.263 encoder library
 *
 * Copyright (C) 1996, Roalt Aalmoes, Twente University
 * SPA multimedia group
 *
 * Based on Telenor TMN 1.6 encoder (Copyright (C) 1995, Telenor R&D)
 * created by Karl Lillevold 
 *
 * Author encoder: Roalt Aalmoes, 
 * 
 * Date: 31-07-96
 **************************************************/

#if !defined _LIBR263_H
#define _LIBR263_H

#include "Global.h"
#include "def.h"
#include "stream.h"
#include "huffman.h"
#include "coder.h"





/* Order of usage of lib263:
   1. Set CParams.format to some value and call InitH23Encoder(cparams).
   2. WriteByteFunction = OwnWriteFunction  (1 and 2 in arbitrary order)
   3. Set cparams and do CompressFrame(cparams) with INTRA encoding
   4. Set cparams and do CompressFrame(cparams) with either INTRA 
      or INTER encoding
   5. redo 4. or to stop do 6
   6. Finally close with ExitH263Encoder()
 */


/* Prototypes */
int CompressFrame(CParam *params, Bits *bits);
int InitH263Encoder(CParam *params);
void ExitH263Encoder(CParam *params);
void SkipH263Frames(int frames_to_skip);

/* Procedure to detect motion, expects param->EncodeThisBlock is set to
   array. 
   Advised values for threshold: mb_threholds = 2; pixel_threshold = 2 */
int FindMotion(CParam *params, int mb_threshold, int pixel_threshold);


static void init_motion_detection(void);
//__inline__ static int Check8x8(unsigned int *orig, 
//			       unsigned int *recon, int pos);

static int Check8x8(unsigned int *orig,unsigned int *recon, int pos);

static int HasMoved(int call_time,  void *real, void *recon, int x, int y);




#endif // !defined _LIBR263_H
