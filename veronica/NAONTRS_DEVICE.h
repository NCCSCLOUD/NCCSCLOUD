#ifndef _NAONTRSDEVICE
#define _NAONTRSDEVICE

#include "NAONTRS.h"
#include "NAONTRS_CLOUD.h"

extern int NAONTRS_DEVICE_RANDKEY(BYTE *content,
							int strlen);//�������ռ��趨Ϊ���
extern int NAONTRS_DEVICE_HASHKEY(BYTE *key,//���hash���
							BYTE *context,//ԭ��
							int strlen
							);//����ֵ
extern int NAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//���¶�����ܷ���

extern int NCAONTRS_DEVICE_CHRYPTER(BYTE *cipertext,//����
							BYTE *context,//ԭ��
							int strlen);//���ܽӿ�

extern int NAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen);//���ݱ����������һ��share
extern int NAONTRS_DEVICE_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//��in���out�浽out
extern int NAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr);//̽������RTTֵ

extern USHORT NAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize);//��ȡУ��ֵ

extern int NAONTRS_DEVICE_PING(SHAREADDRESS *addressptr);//��дping RTT

extern int NAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr);//�ҳ�initial clouds

extern int NAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//����Ŀ���Ʒ���һ��share

extern int NAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//����ַ��������

extern int NAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//����initialclouds��ַ�б�

extern int NAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//��һ�ݵ�ַ�������

extern int NAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr);

#endif