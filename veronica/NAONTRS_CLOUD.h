
#ifndef _NAONTRSCLOUD
#define _NAONTRSCLOUD

#include "NAONTRS.h"
#include "NAONTRS_DEVICE.h"

//10.159.228.148

//��ʼ���ƶ���Ϣ

extern int NAONTRS_cloud_server(ADDRESSLIST *addresslistptr);

extern int NAONTRS_initial_cloud(ADDRESSLIST *addresslistptr,char *hostIP);

extern int NAONTRS_encoding_cloud(ADDRESSLIST *addresslistptr,char *hostIP);

extern int NAONTRS_CLOUD_get_Host_IP(char *IP);//��ȡ����IP

extern int NAONTRS_CLOUD_initial(SOCKET sock);//��ʼ��

extern int NAONTRS_CLOUD_RECEIVESHARE(AONTSHARE *shareptr,SOCKET sock);//����һ��share

extern int NAONTRS_CLOUD_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//����

extern int NAONTRS_CLOUD_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegtr,SOCKET sock);//������������

extern int NAONTRS_CLOUD_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//����ַ��������

extern int NAONTRS_CLOUD_SENDSHAREGROUPPARITY(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr,int seqnum);//��Ҫ�ҵ���Ҫ���͵����ݲ�����

extern int NAONTRS_CLOUD_RECEIVEADDRESSLIST(ADDRESSGROUP *addrgroupptr,SOCKET sock);//���յ�ַ�б�

extern int NAONTRS_CLOUD_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//���͵�ַ�б�

extern int NAONTRS_CLOUD_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//��һ�ݵ�ַ�������

extern int NAONTRS_CLOUD_SENDSIZENUM(SHAREADDRESS *addressptr,int sizenum);//����share��С

extern int NAONTRS_CLOUD_RECEIVESIZENUM(int *sizenum,SOCKET sock);

extern int NAONTRS_CLOUD_SENDSIZENUMGROUP(ADDRESSGROUP *addrgroupptr,int sizenum);//��share��С���з���

extern int NAONTRS_CLOUD_DIVIDESHARE(AONTSHAREGROUP *sharegroupptr,AONTSHARE *shareptr);//��һ��share�з�k��share

extern int NAONTRS_CLOUD_MERGEHSARE(AONTSHARE *shareptr,AONTSHAREGROUP *sharegtpptr,int sizenum);//�ϲ�share

extern int NAONTRS_CLOUD_MERGESHAREPARITY(AONTSHARE *shareptr,AONTSHAREGROUP *recvsharegtpptr,AONTSHAREGROUP *firstsharegtr,int sizenum,ADDRESSGROUP *sendaddressgtr);//�����ƺϲ��ӿ�,��Ҫ������

extern int NAONTRS_CLOUD_SENDCLOUDINITIAL(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,ADDRESSGROUP *initialcloud);//�����ʼ����Ҫ���еĴ�����

extern int NAONTRS_CLOUD_SENDCLOUDPARITY(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,ADDRESSGROUP *initialcloud);//�����������Ҫ���еĴ�����

extern int NAONTRS_CLOUD_CODESHARES(AONTSHAREGROUP *paritysharegtr,AONTSHAREGROUP *initialsharegtr);

extern int NAONTRS_CLOUD_FROMDEVICE(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock);//���豸�յ����ݴ���

extern int NAONTRS_CLOUD_FROMINITIALCLOUD(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock);//�ӳ�ʼ���յ����ݴ���

//addresslist�Ƿ�������ַ�б�,
extern int NAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr);

#endif