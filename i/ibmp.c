#include "ualloc.h"
#include "uerror.h"
#include "ibmp.h"

ibmp* ibmp24_new(usi32 width,usi32 height)
{
  ibmp* bmp;

  bmp = ualloc(sizeof(ibmp));
  if (!bmp) {
    return NULL;
  }
  bmp->bfh = (ibmp_file_header){
    0x42,0x4D,           /*bf_type*/
    0x00,0x00,0x00,0x00, /*bf_size*/
    0x00,0x00,           /*bf_reserved1*/
    0x00,0x00,           /*bf_reserved2*/
    0x36,0x00,0x00,0x00  /*bf_off_bits*/
  };
  bmp->bih = (ibmp_infor_header){
    0x28,0x00,0x00,0x00, /*bi_size*/
    0x00,0x00,0x00,0x00, /*bi_width*/
    0x00,0x00,0x00,0x00, /*bi_height*/
    0x01,0x00,           /*bi_planes*/
    0x18,0x00,           /*bi_bit_count*/
    0x00,0x00,0x00,0x00, /*bi_compression*/
    0x00,0x00,0x00,0x00, /*bi_size_images*/
    0x00,0x00,0x00,0x00, /*bi_x_pels_per_meter*/
    0x00,0x00,0x00,0x00, /*bi_y_pels_per_meter*/
    0x00,0x00,0x00,0x00, /*bi_clr_used*/
    0x00,0x00,0x00,0x00  /*bi_clr_important*/
  };
  bmp->bd = NULL;
  bmp->bpp = 3;

  if (ibmp_resize(bmp, width, height)) {
    ufree(bmp);
    return NULL;
  }
  return bmp;
}

int ibmp_resize(ibmp* bmp,usi32 width,usi32 height)
{
  uui32 stride;
  usi32 bdsize;
  uui16 bpp;
  int mod;
  uui8* bd;
  usi32 w_buf;
  usi32 h_buf;
  uui32 bf_size;

  bpp = bmp->bpp;
  if (width > 0) {
    mod = (width * bpp) % 4;
    if (mod) {
      stride = width * bpp + 4 - mod;
    } else {
      stride = width * bpp;
    }
  } else {
    mod = (-width * bpp) % 4;
    if (mod) {
      stride = -width * bpp + 4 - mod;
    } else {
      stride = -width * bpp;
    }
  }

  if (height > 0) {
    bdsize = stride * height;
  } else {
    bdsize = stride * -height;
  }

  bd = ualloc(bdsize);
  if (!bd) {
    return -1;
  }
  if (bmp->bd) {
    ufree(bmp->bd);
  }
  bmp->bd = bd;
  bmp->bdsize = bdsize;
  bmp->height = height;
  bmp->width = width;
  bmp->stride = stride;
  
  w_buf = width;
  h_buf = height;
  bf_size = bdsize + 54;
  
#ifdef BIG_ENDIAN
  uarrev((uui8*)&w_buf,sizeof(usi32));
  uarrev((uui8*)&h_buf,sizeof(usi32));
  uarrev((uui8*)&bf_buf,sizeof(uui32));
#endif
  
  iassign(usi32,bmp->bih.bi_width1,w_buf);
  iassign(usi32,bmp->bih.bi_height1,h_buf);
  iassign(uui32,bmp->bfh.bf_size1,bf_size);
  
  return 0;
}

void ibmp_dest(ibmp* bmp)
{
  ufree(bmp->bd);
  ufree(bmp);
}

void ibmp_color_fill(ibmp* bmp,icolor clr)
{
  usize_t i,j;
  uui32 height;
  uui32 width;
  uui32 stride;
  uui16 bpp;
  uui8* bd;

  bpp = bmp->bpp;
  if (bmp->height >= 0) {
    height = bmp->height;
  } else {
    height = -bmp->height;
  }
  if (bmp->width >= 0) {
    width = bmp->width * bpp;
  } else {
    width = -bmp->width * bpp;
  }
  stride = bmp->stride;
  bd = bmp->bd;
  i = 0;
  while (i < height) {
    j = 0;
    while (j < stride) {
      usize_t idx = i * stride + j;
      if (j < width) {
	bd[idx] = clr.blue;
	bd[idx + 1] = clr.green;
	bd[idx + 2] = clr.red;
	if (bpp == 4) {
	  bd[idx + 3] = clr.alpha;
	}
      }
      j += bpp;
    }
    i++;
  }
}

void ibmp_pixel_set(ibmp* bmp,icoord coord,icolor clr)
{
  usize_t idx;
  uui8* bd;

  idx = bmp->stride * coord.y + bmp->bpp * coord.x;
  bd = bmp->bd;
  bd[idx] = clr.blue;
  bd[idx + 1] = clr.green;
  bd[idx + 2] = clr.red;
  if (bmp->bpp == 4) {
    bd[idx + 3] = clr.alpha;
  }
}

int ibmp_save(ibmp* bmp,char* file_path)
{
  FILE* file = fopen(file_path,"w");
  if (!file) {
    uabort("open file error!");
    return -1;
  }

  if (fwrite(&bmp->bfh, sizeof(ibmp_file_header), 1, file) < 1)
    return -2;
  if (fwrite(&bmp->bih, sizeof(ibmp_infor_header), 1, file) < 1)
    return -2;
  if (fwrite(bmp->bd, bmp->bdsize, 1, file) < 1)
    return -2;

  return 0;
}
