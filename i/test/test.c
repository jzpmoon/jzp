#include "uerror.h"
#include "ibmp.h"

int main (int argc,char** args)
{
  ibmp* bmp;
  int i,j;
  
  bmp = ibmp24_new(100,100);
  if (!bmp) {
    uabort("bmp init error!");
  }
  ibmp_color_fill(bmp,irgb(255,255,255));

  for (i = 0;i < 100;i++) {
    for (j = 0;j < 100;j++) {
      if (i == j) {
	ibmp_pixel_set(bmp,ipoint2d(i,j),irgb(255,0,0));
	ibmp_pixel_set(bmp,ipoint2d(99 - i,j),irgb(255,0,0));
      }
    }
  }
  
  if (ibmp_save(bmp,"test.bmp")) {
    uabort("save bmp error!");
  }
  return 0;
}
