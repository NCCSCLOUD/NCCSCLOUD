#ifndef _NAONTRSDEVICE
#define _NAONTRSDEVICE

#include "NAONTRS.h"
#include "NAONTRS_CLOUD.h"

extern int NAONTRS_DEVICE_RANDKEY(BYTE *content,
							int strlen);//将给定空间设定为随机
extern int NAONTRS_DEVICE_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							);//计算值
extern int NAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//重新定义加密方法

extern int NCAONTRS_DEVICE_CHRYPTER(BYTE *cipertext,//密文
							BYTE *context,//原文
							int strlen);//加密接口

extern int NAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//根据编码参数生成一组share
extern int NAONTRS_DEVICE_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//将in异或out存到out
extern int NAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr);//探测所有RTT值

extern USHORT NAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize);//获取校验值

extern int NAONTRS_DEVICE_PING(SHAREADDRESS *addressptr);//填写ping RTT

extern int NAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr);//找出initial clouds

extern int NAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//对于目标云发送一个share

extern int NAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//按地址发送数据

extern int NAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//发送initialclouds地址列表

extern int NAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//将一份地址给多个云

extern int NAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);

#endif