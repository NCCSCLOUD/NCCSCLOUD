#ifndef _NAONTRS
#define _NAONTRS
//����hashvalue
#include "global_define.h"
#include "AONTRS.h"
#include "file.h"
#include "galois.h"
#include "cauchy.h"
SOCKET NAONTsocksrv;//����һ��������handle

//AONT-RS�м̳еķ���
extern int NAONTRS_RANDKEY(BYTE *content,
							int strlen);//�������ռ��趨Ϊ���;
extern int NAONTRS_HASHKEY(BYTE *key,//���hash���
							BYTE *context,//ԭ��
							int strlen
							);//����ֵ
extern int NAONTRS_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//���¶�����ܽӿ�
extern int NAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//��in���out�浽out
extern int NAONTRS_CHRYPTER(BYTE *cipertext,//����
							BYTE *context,//ԭ��
							int strlen);
extern int NAONTRS_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//����һ��share


//Network coding
extern int NAONTRS_divider(AONTSHARE *shareptr,PACKETGROUP *packetgrpptr,int packetnumber);
extern int NAONTRS_merger(AONTSHARE *shareptr,PACKETGROUP packetgrpptr);
extern int NAONTRS_encoder(PACKETGROUP *tarpacketgepptr,PACKETGROUP *scrpacketgrpptr,CODEPARA *codeptr);

extern int NAONTRS_initial();//��ʼ��socket����

extern int NAONTRS_SENDSHARE(AONTSHARE *shareptr,SHAREADDRESS *addressptr);//����һ��SHARE��ָ��IP
extern int NAONTRS_RECEIVESHARE(AONTSHARE *shareptr);

extern int NAONTRS_SENDPACKET(PACKET *packetptr,SHAREADDRESS *addressptr);
extern int NAONTRS_RECEIVEPACKET(PACKET *packetptr);

extern int NAONTRS_SENDSHAREGROUP(AONTSHAREGROUP *sharegroupptr,ADDRESSLIST *addresslistptr);
extern int NAONTRS_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegroupptr);

extern int NAONTRS_SENDPACKETGROUP(PACKETGROUP *packetgrpptr,ADDRESSLIST *addresslistptr);
extern int NAONTRS_RECEIVEPACKETGROUP(PACKETGROUP *packetgrpptr); 

extern int NAONTRS_STORESHARE(AONTSHARE *shareptr);//�洢�ڱ��ش�����

extern int NAONTRS_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);//NAONTRS API
#endif