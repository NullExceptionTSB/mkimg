#include <inttypes.h>
#include <stdlib.h>
#include <math.h>

#include <chs.h>
#include <image.h>

chs chs_lba_to_chs_size(size_t sz)  {
    chs r;
    size_t s = sz/512;
    r.spt = (s>=63)?63:s;
    r.head = ((s/r.spt)>=16)?16:((s/r.spt)?(s/r.spt):1);
    r.cylinder = ceil(((float)s)/(r.spt*r.head));
    return r;
}

chs chs_lba_to_chs_addr(size_t lba, void* imge) {
    image* img = (image*)imge;
    chs r;
    size_t cyl_den = (img->image_size_chs.head * img->image_size_chs.spt);

    r.cylinder = lba/cyl_den;
    r.head = (lba%cyl_den)/img->image_size_chs.spt;
    r.sector = ((lba%cyl_den)%img->image_size_chs.spt)+1;
    return r;
}

size_t chs_chs_to_lba_size(chs size) {
    return (size.cylinder*size.head*size.spt*512);
}

size_t chs_chs_to_lba_addr(chs addr, void* imge) { 
    image* img = (image*)imge;

    return (addr.cylinder * img->image_size_chs.head + addr.head) *
            img->image_size_chs.spt +
            (addr.sector - 1);
}