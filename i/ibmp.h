#ifndef _IBMP_H_
#define _IBMP_H_

#include "umacro.h"
#include "udef.h"

typedef struct _ibmp_file_header{
  /*file type "BM"*/
  uui8 bf_type1,bf_type2;
  /*file size*/
  uui8 bf_size1,bf_size2,bf_size3,bf_size4;
  /*reserved must be set 0*/
  uui8 bf_reserved11,bf_reserved12;
  /*reserved must be set 0*/
  uui8 bf_reserved21,bf_reserved22;
  /*bit map data offset size*/
  uui8 bf_off_bits1,bf_off_bits2,bf_off_bits3,bf_off_bits4;
} ibmp_file_header;

typedef struct _ibmp_infor_header{
  /*information header size fixed 40*/
  uui8 bi_size1,bi_size2,bi_size3,bi_size4;
  /*signed int image width in pixels*/
  uui8 bi_width1,bi_width2,bi_width3,bi_width4;
  /*signed int image height in pixels*/
  uui8 bi_height1,bi_height2,bi_height3,bi_height4;
  /*number of color planes (must be 1)*/
  uui8 bi_planes1,bi_planes2;
  /*
   *number of bits per pixel
   *1,4,8(contain color palette),16,24(RGB),32(RGBA)
  */
  uui8 bi_bit_count1,bi_bit_count2;
  /*compression methods used,not compression 0*/
  uui8 bi_compression1,bi_compression2,bi_compression3,bi_compression4;
  /*size of bitmap in bytes*/
  uui8 bi_size_images1,bi_size_images2,bi_size_images3,bi_size_images4;
  /*signed int horizontal resolution in pixels per meter*/
  uui8 bi_x_pels_per_meter1,bi_x_pels_per_meter2,bi_x_pels_per_meter3,
    bi_x_pels_per_meter4;
  /*signed int vertical resolution in pixels per meter*/
  uui8 bi_y_pels_per_meter1,bi_y_pels_per_meter2,
    bi_y_pels_per_meter3,bi_y_pels_per_meter4;
  /*number of color in the image*/
  uui8 bi_clr_used1,bi_clr_used2,bi_clr_used3,bi_clr_used4;
  /*number of important color*/
  uui8 bi_clr_important1,bi_clr_important2,
    bi_clr_important3,bi_clr_important4;
} ibmp_infor_header;

#define iassign(type,lval,rval) ((*(type*)&lval) = rval)

typedef struct _icolor{
  uui8 blue;
  uui8 green;
  uui8 red;
  uui8 alpha;
} icolor;

typedef struct _icoord{
  usi32 x,y,z;
} icoord;

typedef struct _ibmp{
  ibmp_file_header bfh;
  ibmp_infor_header bih;
  /*row size must be align 4*/
  uui8* bd;
  usize_t bdsize;
  usi32 height;
  usi32 width;
  uui32 stride;
  uui16 bpp;
} ibmp;

#define ipoint2d(x,y) (icoord){x,y,0}

#define irgb(r,g,b) (icolor){b,g,r,0}

ibmp* ibmp24_new(usi32 width,usi32 height);
int ibmp_resize(ibmp* bmp,usi32 width,usi32 height);
void ibmp_dest(ibmp* bmp);
void ibmp_color_fill(ibmp* bmp,icolor clr);
void ibmp_pixel_set(ibmp* bmp,icoord coord,icolor clr);
int ibmp_save(ibmp* bmp,char* file_path);

#endif
