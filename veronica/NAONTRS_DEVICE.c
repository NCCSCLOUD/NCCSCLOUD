#include "NAONTRS_DEVICE.h"

//�������ռ��趨Ϊ���
int NAONTRS_DEVICE_RANDKEY(BYTE *content,
							int strlen)
{
	int resTF;
	resTF=AONTRS_RANDKEY(content,strlen);
	return resTF;
}
//����ֵ
int NAONTRS_DEVICE_HASHKEY(BYTE *key,//���hash���
							BYTE *context,//ԭ��
							int strlen
							)
{
	int resTF;
	resTF=AONTRS_HASHKEY(key,context,strlen);
	return resTF;
}
//���¶�����ܽӿ�
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
//��in���out�浽out
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
	//������Ҫ�ı�һ�±���������������ֻҪ����d��share
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
  
    //����ICMP�����  
     
    //PICMP_HEADER pICMPHeader = (PICMP_HEADER)ICMPPack;
    pICMPHeader->nType = 8;  
    pICMPHeader->nCode = 0;  
    pICMPHeader->nId = (USHORT)GetCurrentProcessId();  
    pICMPHeader->nCheckSum = 0;  
    pICMPHeader->nTimeStamp = 0;  
    memset(&(ICMPPack[ICMP_HEADER_SIZE]),'E',ICMP_DATA_SIZE);  
  
	//��ʼ��WinSock  
	
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
	//������Ϣ
	nRet = recvfrom ( RawSock, szRecvBuf, DEF_BUF_SIZE,0,(SOCKADDR*)&SourSockAddr, &nLen ) ;

	if ( nRet == SOCKET_ERROR )
    {
		show_error("NAONTRS_DEVICE","PING","message cannot be send");
        return FALSE; 
	}
	nTime = GetTickCount() - pICMPHeader->nTimeStamp;

	addressptr->RRTvalue=nTime;

	//��д
	closesocket(RawSock);
	WSACleanup();
	return TRUE;
}

int NAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr)
{
	//�ҳ�RTT��Сd����
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
	//BYTE buffer[DEF_BUF_SIZE];//����������
	SOCKET sock;
	codeptr=shareptr->codeptr;
	contentptr=shareptr->content;
	storepath=shareptr->storepath;
	//sizenum=shareptr->sizenum;
	IPaddress=addressptr->IPaddress;
	IPport=addressptr->port;
	//���ص�ַ
	LocalIP=shareptr->FromIP;
	//�ȷ�������
	
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

	//����
	resTF=connect(sock,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","connect error");
		return FALSE;
	}
	//��������
	send(sock,(char *)shareptr,sizeof(AONTSHARE),0);
	send(sock,(char *)codeptr,sizeof(CODEPARA),0);
	strlen=shareptr->sizenum;
	//send(sock,(char *)sizenum,sizeof(sizenum),0);
	send(sock,(char *)contentptr,strlen,0);
	send(sock,storepath,PATHlen,0);//����60λ��ַ
	send(sock,LocalIP,IPlen,0);//����30λIP
	closesocket(sock);  
	WSACleanup();

	return TRUE;
}
//����share��
int NAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr)
{
	int resTF;
	resTF=NAONTRS_CLOUD_SENDSHAREGROUP(addressgtr,sharegtr);
	return resTF;
}
//���͵�ַ��


//����initial cloud�б�
int NAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr)//����initialclouds��ַ�б�
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

	//����
	resTF=connect(sock,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","connect error");
		return FALSE;
	}
	sizenum=addrgroupptr->sizenum;
	addresslistptr=addrgroupptr->addressptr;
	send(sock,(char *)addrgroupptr,sizeof(ADDRESSGROUP),0);
	//����intial��Ŀ
	currentlistptr=addresslistptr;
	for(i=0;i<sizenum;i++)
	{
		itemaddress=currentlistptr->IPaddress;
		storepath=currentlistptr->storepath;

		send(sock,(char *)currentlistptr,sizeof(currentlistptr),0);
		stringlen=strlen(itemaddress);
		send(sock,(char *)&stringlen,sizeof(int),0);
		if(strlen==0)//�������Ϊ0
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
	char FromIP[IPlen]="192.168.79.20";//����IP������һ�£���Ȼ֮��Ҳû�õ�

	//SHAREADDRESS *addressptr;
	ADDRESSGROUP *addrgroupptr;

	addressptr=addresslistptr->addressptr;
	curaddressptr=addressptr;

	codeptr=addresslistptr->codeptr;
	datanum=codeptr->datanum;
	codeptr->pronum;
	codeptr->totnum;
	codenum=pronum-datanum;//

	cpylen=strlen+SHA256_DIGEST_LENGTH;//����������һ�¿ռ䵥λ���ֽ�
	sharesize=cpylen/datanum;
	codesize=sharesize*codenum;//���㵱ǰ�ı���鳤��


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
		curshareptr->FromIP=FromIP;//�����豸����ζû����
		curshareptr->seqnum=seqnum;
		curshareptr->storepath=curaddressptr->storepath;
		curmemptr=curmemptr+sharesize;
		seqnum++;
		curaddressptr++;
		curshareptr++;
	}
	sharegroupptr->sharegroupptr=shareptr;
	sharegroupptr->groupsize=pronum;//���ݿ�
	
	resTF=NAONTRS_CHRYPTER(cpytext,content,cpylen);//����
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_API","share cannot be ctrypted");
		return FALSE;
	}
	if(codenum>0)
	{
		//��Ҫ���б������
		resTF=NAONTRS_DEVICE_RS(sharegroupptr,cpytext,codeptr,cpylen);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","share group cannot be");
			return FALSE;
		}
	}
	//��ַ��ռ�
	resTF=NAONTRS_DEVICE_SENSOR(addresslistptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","RTT values cannot be sensed");
		return FALSE;
	}
	//��Ҫ����ѡ��
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

	//��������
	resTF=NAONTRS_DEVICE_SENDSHAREGROUP(addrgroupptr,sharegroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","share cannot be send");
		return FALSE;
	}
	//���͵�ַ
	resTF=NAONTRS_DEVICE_SENDADDRESSLISTGROUP(addrgroupptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","address group cannot be send");
		return FALSE;
	}
	//�ͷſռ�
	free(cpytext);
	free(memspace);
	free(sharegroupptr);
	free(shareptr);
	free(addressptr);
	free(addrgroupptr);
	return TRUE;
}