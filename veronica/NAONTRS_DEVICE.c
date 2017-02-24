#include "NAONTRS_DEVICE.h"

//将给定空间设定为随机
int NAONTRS_DEVICE_RANDKEY(BYTE *content,
							int strlen)
{
	int resTF;
	resTF=AONTRS_RANDKEY(content,strlen);
	return resTF;
}
//计算值
int NAONTRS_DEVICE_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							)
{
	int resTF;
	resTF=AONTRS_HASHKEY(key,context,strlen);
	return resTF;
}
//重新定义加密接口
int NAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen)
{
	int resTF;
	resTF=AONTRS_CHRYPTEXT(out,in,strlen,key,keylen);
	return resTF;
}
//将in异或out存到out
int NAONTRS_DEVICE_XORKEY(BYTE *out,
						BYTE *in,
						int strlen)
{
	int resTF;
	resTF=AONTRS_XORKEY(out,in,strlen);
	return resTF;
}
int NAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen)
{
	int resTF,totnum,pronum;
	//这里需要改变一下编码器，本地我们只要变异d个share
	pronum=codeptr->pronum;
	totnum=codeptr->totnum;
	codeptr->totnum=pronum;
	resTF=cauchy_API_Context(sharegroupptr,codeptr,cipertext,strlen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_RS","share cannot be coded");
		return FALSE;
	}
	codeptr->totnum=totnum;
	return FALSE;
}
int NAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr)
{
	int resTF;
	SHAREADDRESS *addressptr,*curaddressptr;
	CODEPARA *codeptr;
	int i,totnum,d,k;
	int RTTvalue;
	
	addressptr=addresslistptr->addressptr;
	totnum=codeptr->totnum;
	curaddressptr=addressptr;
	for(i=0;i<totnum;i++)
	{
		NAONTRS_DEVICE_PING(curaddressptr);
		curaddressptr++;
	}
	return TRUE;
}

USHORT NAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize)
{
	DWORD dwCheckSum = 0;  
    USHORT* lpWord = (USHORT*)lpBuf;  
  
    while( dwSize > 1)  
    {  
        dwCheckSum += *lpWord++;  
        dwSize -= 2;  
    }  

    if(1 == dwSize)  
		dwCheckSum += *((USHORT*)lpWord);  
  
    dwCheckSum = ( dwCheckSum >> 16) + ( dwCheckSum & 0xffff);  
	return (USHORT)(~dwCheckSum);  

}

int NAONTRS_DEVICE_PING(SHAREADDRESS *addressptr)
{
	//System32("ping");
	//system("ping 127.0.0.1");
	//WSDATA
	char* lpDestIp;
	WORD wVersionRequested = MAKEWORD(2,2);  
    WSADATA wsaData;  
	SOCKADDR_IN DestAddr;  
	char ICMPPack[ICMP_PACK_SIZE] = {0}; 
	PICMP_HEADER pICMPHeader = (PICMP_HEADER)ICMPPack; 
	int nTime = 1000;
	int nLen,nRet; 
	SOCKET RawSock; 
	char szRecvBuf [ DEF_BUF_SIZE] ;
    SOCKADDR_IN    SourSockAddr;

	lpDestIp=addressptr->IPaddress;
    DestAddr.sin_family = AF_INET;  
    DestAddr.sin_addr.S_un.S_addr = inet_addr(lpDestIp);  
    DestAddr.sin_port = htons(0);  
  
    //创建ICMP请求包  
     
    //PICMP_HEADER pICMPHeader = (PICMP_HEADER)ICMPPack;
    pICMPHeader->nType = 8;  
    pICMPHeader->nCode = 0;  
    pICMPHeader->nId = (USHORT)GetCurrentProcessId();  
    pICMPHeader->nCheckSum = 0;  
    pICMPHeader->nTimeStamp = 0;  
    memset(&(ICMPPack[ICMP_HEADER_SIZE]),'E',ICMP_DATA_SIZE);  
  
	//初始化WinSock  
	
    if(WSAStartup(wVersionRequested,&wsaData) != 0)  
    {  
		show_error("NAONTRS_DEVICE","PING","socket cannot be created");
        return FALSE;  
    }  
	RawSock = socket(AF_INET,SOCK_RAW,IPPROTO_ICMP);
	nRet=setsockopt(RawSock, SOL_SOCKET, SO_RCVTIMEO,(char*)(&nTime),sizeof(nTime));
	if(nRet==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","PING","socket cannot be setted");
        return FALSE; 
	}
	pICMPHeader->nCheckSum = 0;
    pICMPHeader->nTimeStamp = GetTickCount();
    pICMPHeader->nSequence = 0;

    pICMPHeader->nCheckSum = NAONTRS_DEVICE_GetCheckSum ( (LPBYTE)ICMPPack, ICMP_PACK_SIZE ) ;

    nRet = sendto( RawSock, ICMPPack, ICMP_PACK_SIZE, 0, (SOCKADDR*)&DestAddr, sizeof(DestAddr));
	if(nRet==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","PING","message cannot be send");
        return FALSE; 
	}
	nLen = sizeof(SourSockAddr) ;
	//接收消息
	nRet = recvfrom ( RawSock, szRecvBuf, DEF_BUF_SIZE,0,(SOCKADDR*)&SourSockAddr, &nLen ) ;

	if ( nRet == SOCKET_ERROR )
    {
		show_error("NAONTRS_DEVICE","PING","message cannot be send");
        return FALSE; 
	}
	nTime = GetTickCount() - pICMPHeader->nTimeStamp;

	addressptr->RRTvalue=nTime;

	//填写
	closesocket(RawSock);
	WSACleanup();
	return TRUE;
}

int NAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr)
{
	//找出RTT最小d个云
	int i,j,totnum,pronum;
	int minRTTvalue,currentRTTvalue,mindex;
	SHAREADDRESS *initialcloudptr,*addressptr;
	SHAREADDRESS *currentinitialcloudptr,*currentaddressptr;
	CODEPARA *codeptr;
	codeptr=addresslistptr->codeptr;
	//codeptr;

	totnum=codeptr->totnum;
	pronum=codeptr->pronum;
	minRTTvalue=MAXVALUE;

	initialcloud->sizenum=pronum;

	initialcloudptr=initialcloud->addressptr;
	addressptr=addresslistptr->addressptr;


	currentinitialcloudptr=initialcloudptr;
	for(i=0;i<pronum;i++)
	{
		currentaddressptr=addressptr;
		for(j=0;j<totnum;j++)
		{
			currentRTTvalue=currentaddressptr->RRTvalue;
			if(currentRTTvalue<minRTTvalue)
			{
				minRTTvalue=currentRTTvalue;
				mindex=j;
			}
		}

		currentaddressptr=addressptr+j;
		currentaddressptr->RRTvalue=MAXVALUE;
		currentinitialcloudptr=currentaddressptr;
		currentinitialcloudptr++;
	}
	return TRUE;
}

int NAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr)
{
	int resTF;
	int strlen;
	CODEPARA *codeptr;
	BYTE *contentptr;
	char *storepath;
	char *IPaddress;
	char *LocalIP;
	int contentsize;
	WSADATA WSAData;
	struct sockaddr_in serAddr;  
	int IPport;
	//BYTE buffer[DEF_BUF_SIZE];//创建缓冲区
	SOCKET sock;
	codeptr=shareptr->codeptr;
	contentptr=shareptr->content;
	storepath=shareptr->storepath;
	//sizenum=shareptr->sizenum;
	IPaddress=addressptr->IPaddress;
	IPport=addressptr->port;
	//本地地址
	LocalIP=shareptr->FromIP;
	//先发送内容
	
	if(WSAStartup(MAKEWORD(2,0),&WSAData)==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","socket error");
		return FALSE;
	}
	sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
	if(sock==INVALID_SOCKET)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","socket create error");
		return FALSE;
	}
	serAddr.sin_family = AF_INET;  
	serAddr.sin_port = htons(IPport);  
	serAddr.sin_addr.S_un.S_addr = inet_addr(IPaddress);  

	//链接
	resTF=connect(sock,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","connect error");
		return FALSE;
	}
	//传输内容
	send(sock,(char *)shareptr,sizeof(AONTSHARE),0);
	send(sock,(char *)codeptr,sizeof(CODEPARA),0);
	strlen=shareptr->sizenum;
	//send(sock,(char *)sizenum,sizeof(sizenum),0);
	send(sock,(char *)contentptr,strlen,0);
	send(sock,storepath,PATHlen,0);//发送60位地址
	send(sock,LocalIP,IPlen,0);//发送30位IP
	closesocket(sock);  
	WSACleanup();

	return TRUE;
}
//发送share组
int NAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr)
{
	int resTF;
	resTF=NAONTRS_CLOUD_SENDSHAREGROUP(addressgtr,sharegtr);
	return resTF;
}
//发送地址组


//传输initial cloud列表
int NAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr)//发送initialclouds地址列表
{
	int i,resTF;
	char *IPaddress,*itemaddress,*storepath;
	int stringlen;
	int IPport;
	SOCKET sock;
	int sizenum;
	SHAREADDRESS *addresslistptr,*currentlistptr;
	struct sockaddr_in serAddr;
	WSADATA WSAData;
	if(WSAStartup(MAKEWORD(2,0),&WSAData)==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","socket error");
		return FALSE;
	}
	sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
	if(sock==INVALID_SOCKET)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","socket create error");
		return FALSE;
	}
	IPaddress=addressptr->IPaddress;
	IPport=addressptr->port;

	serAddr.sin_family = AF_INET;  
	serAddr.sin_port = htons(IPport);  
	serAddr.sin_addr.S_un.S_addr = inet_addr(IPaddress);

	//链接
	resTF=connect(sock,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","connect error");
		return FALSE;
	}
	sizenum=addrgroupptr->sizenum;
	addresslistptr=addrgroupptr->addressptr;
	send(sock,(char *)addrgroupptr,sizeof(ADDRESSGROUP),0);
	//传输intial条目
	currentlistptr=addresslistptr;
	for(i=0;i<sizenum;i++)
	{
		itemaddress=currentlistptr->IPaddress;
		storepath=currentlistptr->storepath;

		send(sock,(char *)currentlistptr,sizeof(currentlistptr),0);
		stringlen=strlen(itemaddress);
		send(sock,(char *)&stringlen,sizeof(int),0);
		if(strlen==0)//如果长度为0
		{
			break;
		}
		send(sock,(char *)stringlen,stringlen+1,0);
		stringlen=strlen(itemaddress);
		send(sock,(char *)&stringlen,sizeof(int),0);
		if(stringlen==0)
		{
			break;
		}
		send(sock,(char *)itemaddress,stringlen+1,0);
		currentlistptr++;
	}
	closesocket(sock);  
	WSACleanup();
	return TRUE;
}

int NAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr)
{
	int resTF;
	resTF=NAONTRS_CLOUD_SENDADDRESSLISTGROUP(taraddrgroupptr,addrgroupptr);
	return resTF;
}


int NAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr)
{
	int i,resTF,cpylen,sharesize,codesize;
	BYTE *cpytext,*memspace,*curmemptr;
	AONTSHAREGROUP *sharegroupptr;
	AONTSHARE *shareptr,*curshareptr;
	CODEPARA *codeptr;
	SHAREADDRESS *addressptr,*curaddressptr;
	int totnum,codenum,datanum,pronum,seqnum;
	char FromIP[IPlen]="192.168.79.20";//本地IP可以填一下，虽然之后也没用到

	//SHAREADDRESS *addressptr;
	ADDRESSGROUP *addrgroupptr;

	addressptr=addresslistptr->addressptr;
	curaddressptr=addressptr;

	codeptr=addresslistptr->codeptr;
	datanum=codeptr->datanum;
	codeptr->pronum;
	codeptr->totnum;
	codenum=pronum-datanum;//

	cpylen=strlen+SHA256_DIGEST_LENGTH;//我们来计算一下空间单位是字节
	sharesize=cpylen/datanum;
	codesize=sharesize*codenum;//计算当前的编码块长度


	cpytext=talloc(BYTE,cpylen);
	if(cpytext==NULL)
	{
		show_error("AONTRS","AONTRS_API","No memory");
		return FALSE;
	}

	memspace=talloc(BYTE,cpylen+codesize);
	if(memspace==NULL)
	{
		show_error("AONTRS","AONTRS_API","No memory");
		return FALSE;
	}
	curmemptr=memspace;

	sharegroupptr=talloc(AONTSHAREGROUP,1);
	if(sharegroupptr==NULL)
	{
		show_error("AONTRS","AONTRS_API","No memory");
		return FALSE;
	}

	shareptr=talloc(AONTSHARE,pronum);
	if(shareptr==NULL)
	{
		show_error("AONTRS","AONTRS_API","No memory");
		return FALSE;
	}
	curshareptr=shareptr;
	seqnum=0;
	for(i=0;i<pronum;i++)
	{
		curshareptr->content=curmemptr;
		curshareptr->sizenum=sharesize;
		curshareptr->codeptr=codeptr;
		curshareptr->FromIP=FromIP;//来自设备这意味没意义
		curshareptr->seqnum=seqnum;
		curshareptr->storepath=curaddressptr->storepath;
		curmemptr=curmemptr+sharesize;
		seqnum++;
		curaddressptr++;
		curshareptr++;
	}
	sharegroupptr->sharegroupptr=shareptr;
	sharegroupptr->groupsize=pronum;//数据块
	
	resTF=NAONTRS_CHRYPTER(cpytext,content,cpylen);//加密
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_API","share cannot be ctrypted");
		return FALSE;
	}
	if(codenum>0)
	{
		//需要进行编码操作
		resTF=NAONTRS_DEVICE_RS(sharegroupptr,cpytext,codeptr,cpylen);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","share group cannot be");
			return FALSE;
		}
	}
	//地址组空间
	resTF=NAONTRS_DEVICE_SENSOR(addresslistptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","RTT values cannot be sensed");
		return FALSE;
	}
	//需要进行选择
	addrgroupptr=talloc(ADDRESSGROUP,1);
	if(addrgroupptr==NULL)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","No memory");
		return FALSE;
	}
	addressptr=talloc(SHAREADDRESS,pronum);
	if(addressptr==NULL)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","No memory");
		return FALSE;
	}
	addrgroupptr->addressptr=addressptr;
	addrgroupptr->sizenum=pronum;

	resTF=NAONTRS_DEVICE_SELECTCLOUD(addrgroupptr,addresslistptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","cloud cannote be selected");
		return FALSE;
	}

	//发送数据
	resTF=NAONTRS_DEVICE_SENDSHAREGROUP(addrgroupptr,sharegroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","share cannot be send");
		return FALSE;
	}
	//发送地址
	resTF=NAONTRS_DEVICE_SENDADDRESSLISTGROUP(addrgroupptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","address group cannot be send");
		return FALSE;
	}
	//释放空间
	free(cpytext);
	free(memspace);
	free(sharegroupptr);
	free(shareptr);
	free(addressptr);
	free(addrgroupptr);
	return TRUE;
}