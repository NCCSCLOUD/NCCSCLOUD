#ifndef _NCAONTRS
#define _NCAONTRS


#include "global_define.h"
#include "CAONTRS.h"
#include "NAONTRS_DEVICE.h"
#include "file.h"
#include "galois.h"
#include "cauchy.h"
//计算hashvalue
//CAONT-RS中继承的方法
extern int NCAONTRS_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							);//计算值
extern int NCAONTRS_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//重新定义加密接口
extern int NCAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//将in异或out存到out
extern int NCAONTRS_CHRYPTER(BYTE *cipertext,//密文
							BYTE *context,//原文
							int strlen);

extern int NCAONTRS_divider();
extern int NCAONTRS_encoder();
extern int NCAONTRS_merger();

extern int NCAONTRS_send();

extern int NCAONTRS_service_API();


#endif