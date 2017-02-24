#ifndef _NCAONTRSDEVICE
#define _NCAONTRSDEVICE

#include "NCAONTRS.h"
#include "CAONTRS.h"
#include "NAONTRS_DEVICE.h"

extern int NCAONTRS_DEVICE_HASHKEY(BYTE *key,//���hash���
							BYTE *context,//ԭ��
							int strlen
							);//����ֵ
extern int NCAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//���¶�����ܽӿ�
extern int NCAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//���ݱ����������һ��share
extern int NCAONTRS_DEVICE_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//��in���out�浽out

extern int NCAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr);//̽������RTTֵ

extern USHORT NCAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize);//��ȡУ��ֵ

extern int NCAONTRS_DEVICE_PING(SHAREADDRESS *addressptr);//��дping RTT

extern int NCAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr);//�ҳ�initial clouds

extern int NCAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//����һ��share

extern int NCAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//����ַ��������

extern int NCAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//����initialclouds��ַ�б�

extern int NCAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//��һ�ݵ�ַ�������

extern int NCAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);

#endif