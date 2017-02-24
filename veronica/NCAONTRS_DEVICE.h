#ifndef _NCAONTRSDEVICE
#define _NCAONTRSDEVICE

#include "NCAONTRS.h"
#include "CAONTRS.h"
#include "NAONTRS_DEVICE.h"

extern int NCAONTRS_DEVICE_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							);//计算值
extern int NCAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//重新定义加密接口
extern int NCAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//根据编码参数生成一组share
extern int NCAONTRS_DEVICE_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//将in异或out存到out

extern int NCAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr);//探测所有RTT值

extern USHORT NCAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize);//获取校验值

extern int NCAONTRS_DEVICE_PING(SHAREADDRESS *addressptr);//填写ping RTT

extern int NCAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr);//找出initial clouds

extern int NCAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//发送一个share

extern int NCAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//按地址发送数据

extern int NCAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//发送initialclouds地址列表

extern int NCAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//将一份地址给多个云

extern int NCAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);

#endif