#include "NCAONTRS_CLOUD.h"

//����NAONTRS��NCAONTRSֻ�м��ܲ��ֲ�ͬ��DEVICE�Ѿ�����˼��ܲ������������ֻҪ�̳�NAONTRS
int NCAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr)
{
	int resTF;
	resTF=NAONTRS_CLOUD_API(addresslistptr,localptr);
	return resTF;
}