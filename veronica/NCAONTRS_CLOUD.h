
#ifndef _NCAONTRSCLOUD
#define _NCAONTRSCLOUD

#include "NAONTRS_CLOUD.h"

//由于NAONTRS和NCAONTRS只有加密部分不同，DEVICE已经完成了加密操作，因此这里只要继承NAONTRS
int NCAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr);


#endif