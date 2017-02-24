#ifndef _NCAONTRS
#define _NCAONTRS


#include "global_define.h"
#include "CAONTRS.h"
#include "NAONTRS_DEVICE.h"
#include "file.h"
#include "galois.h"
#include "cauchy.h"
//����hashvalue
//CAONT-RS�м̳еķ���
extern int NCAONTRS_HASHKEY(BYTE *key,//���hash���
							BYTE *context,//ԭ��
							int strlen
							);//����ֵ
extern int NCAONTRS_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen);//���¶�����ܽӿ�
extern int NCAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen);//��in���out�浽out
extern int NCAONTRS_CHRYPTER(BYTE *cipertext,//����
							BYTE *context,//ԭ��
							int strlen);

extern int NCAONTRS_divider();
extern int NCAONTRS_encoder();
extern int NCAONTRS_merger();

extern int NCAONTRS_send();

extern int NCAONTRS_service_API();


#endif