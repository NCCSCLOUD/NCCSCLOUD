#ifndef _NAONTRS
#define _NAONTRS
//计算hashvalue
#include "global_define.h"
#include "AONTRS.h"
#include "file.h"
#include "galois.h"
#include "cauchy.h"
SOCKET NAONTsocksrv;//定义一个服务器handle

//AONT-RS中继承的方法
extern int NAONTRS_RANDKEY(BYTE *content,
							int strlen);//将给定空间设定为随机;
extern int NAONTRS_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							);//计算值
extern int NAONTRS_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//重新定义加密接口
extern int NAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//将in异或out存到out
extern int NAONTRS_CHRYPTER(BYTE *cipertext,//密文
							BYTE *context,//原文
							int strlen);
extern int NAONTRS_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//生成一组share


//Network coding
extern int NAONTRS_divider(AONTSHARE *shareptr,PACKETGROUP *packetgrpptr,int packetnumber);
extern int NAONTRS_merger(AONTSHARE *shareptr,PACKETGROUP packetgrpptr);
extern int NAONTRS_encoder(PACKETGROUP *tarpacketgepptr,PACKETGROUP *scrpacketgrpptr,CODEPARA *codeptr);

extern int NAONTRS_initial();//初始化socket函数

extern int NAONTRS_SENDSHARE(AONTSHARE *shareptr,SHAREADDRESS *addressptr);//发送一个SHARE到指定IP
extern int NAONTRS_RECEIVESHARE(AONTSHARE *shareptr);

extern int NAONTRS_SENDPACKET(PACKET *packetptr,SHAREADDRESS *addressptr);
extern int NAONTRS_RECEIVEPACKET(PACKET *packetptr);

extern int NAONTRS_SENDSHAREGROUP(AONTSHAREGROUP *sharegroupptr,ADDRESSLIST *addresslistptr);
extern int NAONTRS_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegroupptr);

extern int NAONTRS_SENDPACKETGROUP(PACKETGROUP *packetgrpptr,ADDRESSLIST *addresslistptr);
extern int NAONTRS_RECEIVEPACKETGROUP(PACKETGROUP *packetgrpptr); 

extern int NAONTRS_STORESHARE(AONTSHARE *shareptr);//存储在本地磁盘上

extern int NAONTRS_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);//NAONTRS API
#endif