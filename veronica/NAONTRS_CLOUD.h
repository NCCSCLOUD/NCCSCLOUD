
#ifndef _NAONTRSCLOUD
#define _NAONTRSCLOUD

#include "NAONTRS.h"
#include "NAONTRS_DEVICE.h"

//10.159.228.148

//初始化云端信息

extern int NAONTRS_cloud_server(ADDRESSLIST *addresslistptr);

extern int NAONTRS_initial_cloud(ADDRESSLIST *addresslistptr,char *hostIP);

extern int NAONTRS_encoding_cloud(ADDRESSLIST *addresslistptr,char *hostIP);

extern int NAONTRS_CLOUD_get_Host_IP(char *IP);//获取主机IP

extern int NAONTRS_CLOUD_initial(SOCKET sock);//初始化

extern int NAONTRS_CLOUD_RECEIVESHARE(AONTSHARE *shareptr,SOCKET sock);//接收一个share

extern int NAONTRS_CLOUD_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr);//数据

extern int NAONTRS_CLOUD_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegtr,SOCKET sock);//接收所有数据

extern int NAONTRS_CLOUD_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr);//按地址发送数据

extern int NAONTRS_CLOUD_SENDSHAREGROUPPARITY(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr,int seqnum);//需要找到需要发送的数据并发送

extern int NAONTRS_CLOUD_RECEIVEADDRESSLIST(ADDRESSGROUP *addrgroupptr,SOCKET sock);//接收地址列表

extern int NAONTRS_CLOUD_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr);//发送地址列表

extern int NAONTRS_CLOUD_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr);//将一份地址给多个云

extern int NAONTRS_CLOUD_SENDSIZENUM(SHAREADDRESS *addressptr,int sizenum);//传输share大小

extern int NAONTRS_CLOUD_RECEIVESIZENUM(int *sizenum,SOCKET sock);

extern int NAONTRS_CLOUD_SENDSIZENUMGROUP(ADDRESSGROUP *addrgroupptr,int sizenum);//将share大小进行发送

extern int NAONTRS_CLOUD_DIVIDESHARE(AONTSHAREGROUP *sharegroupptr,AONTSHARE *shareptr);//将一个share切分k个share

extern int NAONTRS_CLOUD_MERGEHSARE(AONTSHARE *shareptr,AONTSHAREGROUP *sharegtpptr,int sizenum);//合并share

extern int NAONTRS_CLOUD_MERGESHAREPARITY(AONTSHARE *shareptr,AONTSHAREGROUP *recvsharegtpptr,AONTSHAREGROUP *firstsharegtr,int sizenum,ADDRESSGROUP *sendaddressgtr);//检验云合并接口,需要将数据

extern int NAONTRS_CLOUD_SENDCLOUDINITIAL(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,ADDRESSGROUP *initialcloud);//计算初始云需要进行的传输云

extern int NAONTRS_CLOUD_SENDCLOUDPARITY(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,ADDRESSGROUP *initialcloud);//计算检验云需要进行的传输云

extern int NAONTRS_CLOUD_CODESHARES(AONTSHAREGROUP *paritysharegtr,AONTSHAREGROUP *initialsharegtr);

extern int NAONTRS_CLOUD_FROMDEVICE(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock);//从设备收到数据处理

extern int NAONTRS_CLOUD_FROMINITIALCLOUD(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock);//从初始云收到数据处理

//addresslist是服务器地址列表,
extern int NAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr);

#endif