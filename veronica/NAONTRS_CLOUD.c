#include "NAONTRS_CLOUD.h"

int NAONTRS_CLOUD_initial(SOCKET sock)
{
	int resTF;
	struct sockaddr_in sin; //����socket
	WORD sockVersion = MAKEWORD(2,2);
	WSADATA wsaData;  
	if(WSAStartup(sockVersion, &wsaData)!=0)  
    {  
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","");
		return FALSE;  
    }  
	sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock==SOCKET_ERROR)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","socket create error");
		return FALSE; 
	} 
	sin.sin_family = AF_INET;
	sin.sin_port=TRANPORT;//��дĬ�϶˿ں�
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	resTF=bind(sock,(LPSOCKADDR)&sin, sizeof(sin));
	if(resTF=SOCKET_ERROR)
	{
		closesocket(sock);       
		WSACleanup();
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","binding error");
		return FALSE;
	}
	//����
	resTF=listen(sock,1);
	if(resTF==SOCKET_ERROR)
	{
		closesocket(sock);       
		WSACleanup();
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","listening error");
		return FALSE;
	}
	return TRUE;
}

int NAONTRS_CLOUD_RECEIVESHARE(AONTSHARE *shareptr,SOCKET sock)
{
	int resTF;
	SOCKET clientsock;
	struct sockaddr_in addrClient;//�û�socket��ַ
	int addrsize;
	CODEPARA *codeptr;
	BYTE *content;
	char *storepath,*FromIP;
	int sizenum;

	codeptr=shareptr->codeptr;
	content=shareptr->content;
	storepath=shareptr->storepath;
	FromIP=shareptr->FromIP;
	addrsize=sizeof(addrClient);
	clientsock=accept(sock,(LPSOCKADDR)&addrClient,&addrsize);
	if(clientsock==SOCKET_ERROR)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","accept error");
		return FALSE;
	}
	//�����ĸ����ֲ���ƴ��
	//����
	resTF=recv(clientsock,(char *)shareptr,sizeof(AONTSHARE),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//�����趨ָ��
	shareptr->codeptr=codeptr;
	shareptr->content=content;
	shareptr->storepath=storepath;
	shareptr->FromIP=FromIP;
	//�ڲ����е�ָ�������趨��������Զ�˵��ڴ��ַ
	//���벿��
	resTF=recv(clientsock,(char *)codeptr,sizeof(CODEPARA),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//����
	sizenum=shareptr->sizenum;
	resTF=recv(clientsock,(char *)content,sizenum,0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//�洢����
	resTF=recv(clientsock,(char *)storepath,PATHlen,0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	recv(clientsock,(char *)FromIP,IPlen,0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	closesocket(clientsock);
	return TRUE;
}
int NAONTRS_CLOUD_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENDSHARE(addressptr,shareptr);
	return resTF;
}

int NAONTRS_CLOUD_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegtr,SOCKET sock)
{
	int i,resTF;
	int sizenum;
	AONTSHARE *shareptr,*curshareptr;

	sharegtr->sharegroupptr;
	sizenum=sharegtr->groupsize;


	curshareptr=shareptr;
	for(i=0;i<sizenum;i++)
	{
		resTF=NAONTRS_CLOUD_RECEIVESHARE(curshareptr,sock);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHAREGROUP","share cannot be received");
			return FALSE;
		}
	}
	return TRUE;
}

int NAONTRS_CLOUD_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr)
{
	int i,resTF;
	AONTSHARE *shareptr,*curshareptr;
	SHAREADDRESS *addressptr,*curaddressptr;
	int addnum,sharenum;

	shareptr=sharegtr->sharegroupptr;
	sharenum=sharegtr->groupsize;

	addressptr=addressgtr->addressptr;
	addnum=addressgtr->sizenum;

	if(sharenum!=addnum)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSHAREGROUP","share number should equal to address number");
		return FALSE;
	}
	curshareptr=shareptr;
	curaddressptr=addressptr;
	for(i=0;i<sharenum;i++)
	{
		resTF=NAONTRS_CLOUD_SENDSHARE(curaddressptr,curshareptr);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSHAREGROUP","share cannot be send");
			return FALSE;
		}
	}
	return TRUE;
}

int NAONTRS_CLOUD_SENDSHAREGROUPPARITY(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr,int seqnum)
{
	int i,j,resTF;
	AONTSHAREGROUP *sendsharegtr;
	AONTSHARE *sendshareptr,*cursendshareptr;
	AONTSHARE *shareptr,*curshareptr;
	CODEPARA *codeptr;
	int pronum,totnum,paritynum,sendnum;
	//CODEPARA *codeptr;

	shareptr=sharegtr->sharegroupptr;
	codeptr=shareptr->codeptr;
	totnum=codeptr->totnum;
	pronum=codeptr->pronum;
	paritynum=totnum-pronum;
	sendnum=paritynum-1;

	sendsharegtr=talloc(AONTSHAREGROUP,1);
	if(sendsharegtr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSHAREGROUPPARITY","No memory");
		return FALSE;
	}
	sendshareptr=talloc(AONTSHARE,sendnum);
	if(shareptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSHAREGROUPPARITY","No memory");
		return FALSE;
	}
	sendsharegtr->sharegroupptr=sendshareptr;
	sendsharegtr->groupsize=sendnum;

	cursendshareptr=sendshareptr;
	curshareptr=shareptr;
	for(i=0;i<paritynum;i++)
	{
		if(i==seqnum)
		{
			curshareptr++;
			continue;
		}
		cursendshareptr=curshareptr;
		curshareptr++;
		cursendshareptr++;
	}
	resTF=NAONTRS_CLOUD_SENDSHAREGROUP(addressgtr,sendsharegtr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSHAREGROUPPARITY","sharegroup cannot be send");
		return FALSE;
	}
	free(sendsharegtr);
	free(sendshareptr);
	return TRUE;
}

// ����intial cloud��ַ
int NAONTRS_CLOUD_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENDADDRESSLIST(addressptr,addrgroupptr);
	return resTF;
}

int NAONTRS_CLOUD_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr)
{
	int i,resTF,sendnum;
	SHAREADDRESS *cursendaddressptr;

	cursendaddressptr=taraddrgroupptr->addressptr;
	sendnum=taraddrgroupptr->sizenum;
	for(i=0;i<sendnum;i++)
	{
		resTF=NAONTRS_CLOUD_SENDADDRESSLIST(cursendaddressptr,addrgroupptr);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","");
			return FALSE;
		}
		cursendaddressptr++;
	}
	
}

int NAONTRS_CLOUD_RECEIVEADDRESSLIST(ADDRESSGROUP *addrgroupptr,SOCKET sock)
{
	int resTF;
	SOCKET clientsock;
	struct sockaddr_in addrClient;//�û�socket��ַ
	int addrsize;
	int i,sizenum;
	SHAREADDRESS *addressptr,*currentptr;
	int stringlen;
	char *storepath,*IPaddress;

	addressptr=addrgroupptr->addressptr;
	addrsize=sizeof(addrClient);
	clientsock=accept(sock,(LPSOCKADDR)&addrClient,&addrsize);
	if(clientsock==SOCKET_ERROR)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","accept error");
		return FALSE;
	}
	//����
	resTF=recv(clientsock,(char *)addrgroupptr,sizeof(SHAREADDRESS),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	addrgroupptr->addressptr=addressptr;
	sizenum=addrgroupptr->sizenum;

	currentptr=addressptr;
	for(i=0;i<sizenum;i++)
	{
		IPaddress=currentptr->IPaddress;
		storepath=currentptr->storepath;

		resTF=recv(clientsock,(char *)currentptr,sizeof(currentptr),0);
		if(resTF<0)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
			return FALSE;
		}
		//�����ַ�����С
		resTF=recv(clientsock,(char *)stringlen,sizeof(int),0);
		if(stringlen==0)
		{
			break;
		}
		//��IP��ַ
		resTF=recv(clientsock,IPaddress,stringlen+1,0);
		if(resTF<0)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
			return FALSE;
		}
		//�����ַ��
		resTF=recv(clientsock,(char *)stringlen,sizeof(int),0);
		if(stringlen==0)
		{
			break;
		}
		resTF=recv(clientsock,storepath,stringlen,0);
		if(resTF<0)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
			return FALSE;
		}
		currentptr++;
	}
	closesocket(clientsock);
	return TRUE;
}

int NAONTRS_CLOUD_SENDSIZENUM(SHAREADDRESS *addressptr,int sizenum)
{
	int resTF;
	SOCKET sock;
	char *IPaddress;
	int IPport;
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

	resTF=connect(sock,(struct sockaddr *)&serAddr,sizeof(serAddr));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_SENDSHARE","connect error");
		return FALSE;
	}
	send(sock,(char *)&sizenum,sizeof(int),0);
	closesocket(sock);  
	WSACleanup();

	return TRUE;
}

int NAONTRS_CLOUD_RECEIVESIZENUM(int *sizenum,SOCKET sock)
{
	int resTF;
	SOCKET clientsock;
	struct sockaddr_in addrClient;//�û�socket��ַ
	int addrsize;
	addrsize=sizeof(addrClient);
	clientsock=accept(sock,(LPSOCKADDR)&addrClient,&addrsize);
	if(clientsock==SOCKET_ERROR)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESIZENUM","accept error");
		return FALSE;
	}
	resTF=recv(sock,(char *)sizenum,sizeof(int),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESIZENUM","receive error");
		return FALSE;
	}
	closesocket(clientsock);
	return TRUE;
}

//��share��С���з���
int NAONTRS_CLOUD_SENDSIZENUMGROUP(ADDRESSGROUP *addrgroupptr,int sizenum)
{
	int i,cloudnum,resTF;
	SHAREADDRESS *curaddressptr;
	cloudnum=addrgroupptr->sizenum;
	curaddressptr=addrgroupptr->addressptr;
	for(i=0;i<cloudnum;i++)
	{
		resTF=NAONTRS_CLOUD_SENDSIZENUM(curaddressptr,sizenum);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_SENDSIZENUMGROUP","share cannot be send");
			return FALSE;
		}
		curaddressptr++;
	}
	return TRUE;
}

int NAONTRS_CLOUD_DIVIDESHARE(AONTSHAREGROUP *sharegroupptr,AONTSHARE *shareptr)//��һ��share�з�k��share
{
	int resTF;
	int i,datanum,pronum,packetsizenum;
	int reminder;
	CODEPARA *codeptr;
	BYTE *contentptr,*curcontentptr;
	int fromtypes,sizenum,seqnum,groupsize;
	char *storepath;
	AONTSHARE *packetptr,*curpacketptr;


	codeptr=shareptr->codeptr;
	contentptr=shareptr->content;
	fromtypes=shareptr->fromtypes;
	seqnum=shareptr->seqnum;
	sizenum=shareptr->sizenum;
	storepath=shareptr->storepath;
	datanum=codeptr->datanum;
	groupsize=sharegroupptr->groupsize;

	packetsizenum=sizenum/groupsize;//��Ҫע������ʱΪ��������Ҫ�����ڴ�
	reminder=sizenum%groupsize;
	//����޷�������Ҫ��1
	if(reminder!=0)
	{
		packetsizenum=packetsizenum+1;
	}
	sharegroupptr->groupsize=groupsize;//�趨
	packetptr=sharegroupptr->sharegroupptr;
	
	seqnum=0;
	curpacketptr=packetptr;
	curcontentptr=contentptr;
	for(i=0;i<groupsize;i++)
	{
		curpacketptr->codeptr=codeptr;
		curpacketptr->content=curcontentptr;
		curpacketptr->fromtypes=FROMINTIALCLOUD;//����һ����frominitialcloud��Ϊֻ��intialcloud���Ƭ
		curpacketptr->seqnum=seqnum;
		curpacketptr->sizenum=sizenum;
		curpacketptr->storepath=storepath;
		curpacketptr++;
		seqnum++;
		curcontentptr=curcontentptr+packetsizenum;
	}
	return TRUE;
}

//�ϲ�share
int NAONTRS_CLOUD_MERGEHSARE(AONTSHARE *shareptr,AONTSHAREGROUP *sharegtpptr,int sizenum)
{
	int i;
	AONTSHARE *packetptr,*curpacketptr;
	BYTE *contentptr,*curcontentptr;
	BYTE *packetcontentptr;
	int groupsize,packetsize;

	groupsize=sharegtpptr->groupsize;
	packetptr=sharegtpptr->sharegroupptr;

	shareptr->codeptr=packetptr->codeptr;
	shareptr->fromtypes=packetptr->fromtypes;
	shareptr->seqnum=packetptr->seqnum;
	shareptr->sizenum=sizenum;
	shareptr->storepath=packetptr->storepath;
	packetsize=packetptr->sizenum;

	contentptr=shareptr->content;
	curcontentptr=contentptr;
	for(i=0;i<groupsize;i++)
	{
		packetcontentptr=curpacketptr->content;
		mcpy(curcontentptr,packetcontentptr,BYTE,packetsize);
		curcontentptr=curcontentptr+packetsize;
	}

	return TRUE;
}

//Ŀ�Ľ��յ��������
int NAONTRS_CLOUD_MERGESHAREPARITY(AONTSHARE *shareptr,AONTSHAREGROUP *recvsharegtpptr,AONTSHAREGROUP *firstsharegtr,int sizenum,ADDRESSGROUP *sendaddressgtr)
{
	int i,j,resTF;
	int seqnum,curseqnum;
	int paritynum,pronum,totnum,sendnum;
	char *tarIPaddress,*FromIPaddress,*storepath;//��Ҫ�Ƚ�IP��ַ��ȷ��λ��
	CODEPARA *codeptr;

	SHAREADDRESS *addressptr,*curaddressptr;
	AONTSHAREGROUP *ressharegtr;
	AONTSHARE *resshareptr,*curresshareptr,*firstshareptr,*curfirstshareptr,*recvshareptr,*currecvshareptr;
	curseqnum=0;

	firstshareptr=firstsharegtr->sharegroupptr;

	codeptr=firstshareptr->codeptr;
	pronum=codeptr->pronum;
	totnum=codeptr->totnum;
	paritynum=totnum-pronum;
	sendnum=paritynum-1;
	//��Ҫ���к�
	seqnum=firstshareptr->seqnum;
	storepath=firstshareptr->storepath;
	curfirstshareptr=firstshareptr+seqnum;

	ressharegtr=talloc(AONTSHAREGROUP,1);
	if(ressharegtr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_MERGESHAREPARITY","No memory");
		return FALSE;
	}
	resshareptr=talloc(AONTSHARE,paritynum);
	if(resshareptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_MERGESHAREPARITY","No memory");
		return FALSE;
	}
	ressharegtr->sharegroupptr=resshareptr;
	ressharegtr->groupsize=paritynum;


	recvshareptr=recvsharegtpptr->sharegroupptr;//�յ�������

	addressptr=sendaddressgtr->addressptr;

	curaddressptr=addressptr;
	currecvshareptr=recvshareptr;
	curresshareptr=resshareptr;
	//��������
	for(i=0;i<paritynum;i++)
	{
		if(i==seqnum)
		{
			curresshareptr=curfirstshareptr;//��������
			curresshareptr++;
			continue;
		}
		//������Ҫ�������б�����
		tarIPaddress=curaddressptr->IPaddress;//��Ҫ��IP��ַ
		for(j=0;j<sendnum;j++)
		{
			FromIPaddress=currecvshareptr->FromIP;
			resTF=strcmp(FromIPaddress,tarIPaddress);
			if(resTF==0)
			{
				curresshareptr=currecvshareptr;//����
				curresshareptr++;
			}
			currecvshareptr++;
		}
		curaddressptr++;
		
	}
	resTF=NAONTRS_CLOUD_MERGEHSARE(shareptr,ressharegtr,sizenum);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_MERGESHAREPARITY","data cannot be merged");
		return FALSE;
	}
	//�ͷſռ�
	//���������ɵĶ�����дһ������
	shareptr->codeptr=codeptr;
	shareptr->FromIP="";//������û��������
	shareptr->fromtypes=FROMPARITYCLOUD;
	shareptr->seqnum=0;
	shareptr->sizenum=sizenum;
	shareptr->storepath=storepath;
	free(ressharegtr);
	free(resshareptr);
	return TRUE;
}

//�����ʼ����Ҫ���еĴ�����
int NAONTRS_CLOUD_SENDCLOUDINITIAL(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,ADDRESSGROUP *initialcloud)
{
	int i,j,resTF;
	int pronum,totnum;
	CODEPARA *codeptr;
	char *initialaddressptr,*tarIPaddress;
	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	sendaddressptr=sendcloud->addressptr;

	pronum=codeptr->pronum;
	totnum=codeptr->totnum;

	cursendaddressptr=sendaddressptr;
	curinialaddressptr=initialcloud->addressptr;
	curaddressptr=addresslistptr->addressptr;

	//����packet���͵�n-d����
	for(i=0;i<totnum;i++)
	{
		tarIPaddress=curaddressptr->IPaddress;
		for(j=0;j<pronum;j++)
		{
			initialaddressptr=curinialaddressptr->IPaddress;
			resTF=strcmp(initialaddressptr,tarIPaddress);
			if(resTF==0)
			{
				curaddressptr++;
				continue;
			}
			curinialaddressptr++;//�Ƚ���һ��
		}
		cursendaddressptr=curaddressptr;//������ַ

		cursendaddressptr++;
		curaddressptr++;
	}
	return TRUE;
}

//���㷢�����е�ַ
int NAONTRS_CLOUD_SENDCLOUDPARITY(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,ADDRESSGROUP *initialcloud)
{
	int i,j,resTF;
	int pronum,totnum;
	CODEPARA *codeptr;
	char *localIPaddress,*initialaddressptr,*tarIPaddress;
	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	sendaddressptr=sendcloud->addressptr;
	curinialaddressptr=initialcloud->addressptr;//������ʼ��ַ��ֵ

	pronum=codeptr->pronum;
	totnum=codeptr->totnum;

	localIPaddress=localptr->IPaddress;
	cursendaddressptr=sendaddressptr;
	curaddressptr=addresslistptr->addressptr;

	//����packet��n-d-1��У����
	for(i=0;i<totnum;i++)
	{
		tarIPaddress=curaddressptr->IPaddress;
		resTF=strcmp(localIPaddress,tarIPaddress);//1
		if(resTF==0)
		{
			curaddressptr++;
			continue;
		}
		for(j=0;j<pronum;j++)//d
		{
			initialaddressptr=curinialaddressptr->IPaddress;
			resTF=strcmp(initialaddressptr,tarIPaddress);
			if(resTF==0)
			{
				curaddressptr++;
				continue;
			}
			curinialaddressptr++;//�Ƚ���һ��
		}
		//������,����Ҫ�����λ��
		cursendaddressptr=curaddressptr;//������ַ

		cursendaddressptr++;
		curaddressptr++;
	}
	return TRUE;
}

int NAONTRS_CLOUD_CODESHARES(AONTSHAREGROUP *paritysharegtr,AONTSHAREGROUP *initialsharegtr)
{
	BITMATRIX *matrix;
	UINT32 *mat;
	int i,j,resTF;
	AONTSHARE *initialshareptr,*curinitialshateptr;
	AONTSHARE *parityshareptr,*curparityshareptr;
	CODEPARA *codeptr;
	int rows,cols,seqnum;
	int datanum,pronum,totnum,codenum,w;//�������
	int initialnum,paritynum,sizenum;
	char *storepath,*FromIP;
	//��������
	parityshareptr=paritysharegtr->sharegroupptr;
	paritynum=paritysharegtr->groupsize;
	initialshareptr=initialsharegtr->sharegroupptr;
	initialnum=initialsharegtr->groupsize;
	sizenum=initialshareptr->sizenum;

	storepath=initialshareptr->storepath;
	FromIP=initialshareptr->FromIP;
	codeptr=initialshareptr->codeptr;
	datanum=codeptr->datanum;
	pronum=codeptr->pronum;
	totnum=codeptr->totnum;
	codenum=codeptr->codenum;//��Ҫ���������n-k
	w=codeptr->w;

	paritynum=totnum-pronum;
	//���ɱ������
	rows=codenum*w;
	cols=datanum*w;
	mat=talloc(UINT32,rows*cols);
	if(mat==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_CODESHARES","No memory");
		return FALSE;
	}
	matrix=talloc(BITMATRIX,1);
	if(matrix==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_CODESHARES","No memory");
		return FALSE;
	}
	//����������
	matrix->w=w;
	matrix->xm=rows;
	matrix->ym=cols;
	matrix->matrix=mat;
	//�ͷž���
	resTF=cauchy_good_general_coding_bitmatrix(matrix);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_CODESHARES","matrix cannot be created");
		return FALSE;
	}
	//
	resTF=matrix_matrix_calculate_packet(matrix,codeptr,paritysharegtr,initialsharegtr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_CODESHARES","packet cannot be calculated");
		return FALSE;
	}
	//�����һ���ȹ�����
	seqnum=0;
	curparityshareptr=parityshareptr;
	for(i=0;i<paritynum;i++)
	{
		curparityshareptr->codeptr=codeptr;
		curparityshareptr->fromtypes=FROMPARITYCLOUD;
		curparityshareptr->seqnum=seqnum;
		curparityshareptr->sizenum=sizenum;
		curparityshareptr->storepath=storepath;
		curparityshareptr->FromIP=FromIP;
		seqnum++;
		curparityshareptr++;
	}
	free(mat);
	free(matrix);
	return TRUE;
}

int NAONTRS_CLOUD_FROMDEVICE(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock)
{
	int i,j,sendnum,resTF;
	BYTE *contentptr;
	AONTSHARE *packetptr,*curshareptr;
	CODEPARA *codeptr;
	int datanum,initialrecvnum,parityrecvnum,pronum,totnum,paritynum;

	AONTSHAREGROUP *sharegtpptr;
	ADDRESSGROUP *addrgroupptr,*sendgrouptr;

	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	char *IPaddress,*storepath;
	int reminder,sizenum,packetsize;
	int fromtype;
	int seqnum;
	//�洢һ������
	resTF=file_FILELAYER_SHARE_WRITER(shareptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be saved");
		return FALSE;
	}
	codeptr=shareptr->codeptr;
	datanum=codeptr->datanum;
	pronum=codeptr->pronum;
	totnum=codeptr->totnum;
	paritynum=totnum-pronum;
	seqnum=shareptr->seqnum;
	//���������û��˵�����
	//��һ�����ճ�ʼ�Ƶ�ַ�б�
	addrgroupptr=talloc(ADDRESSGROUP,1);
	if(addrgroupptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	addrgroupptr->sizenum=pronum;//��ʼ��d��
	//addresslistptr->addressptr;
	addressptr=talloc(SHAREADDRESS,pronum);
	if(addressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	curaddressptr=addressptr;
	for(i=0;i<pronum;i++)
	{
		IPaddress=talloc(char,30);//��30λ3*4=12+4=16
		if(IPaddress==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curaddressptr->IPaddress=IPaddress;
		storepath=talloc(char,60);
		if(storepath==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curaddressptr->storepath=storepath;
	}
	addrgroupptr->addressptr=addressptr;

	resTF=NAONTRS_CLOUD_RECEIVEADDRESSLIST(addrgroupptr,sock);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","address cannot received");
		return FALSE;
	}
	if(seqnum>datanum)//�������k��ֱ����һ��
	{
		return TRUE;//�����
	}
	//�ڶ�����ֳɶ����
	//����ռ�
			
	sizenum=shareptr->sizenum;
	//�������С
	packetsize=sizenum/paritynum;//n-d
	reminder=sizenum%paritynum;
	if(reminder!=0)
	{
		packetsize=packetsize+1;
	}
	sharegtpptr=talloc(AONTSHAREGROUP,1);
	if(sharegtpptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	packetptr=talloc(AONTSHARE,paritynum);//���뷢�Ͱ���ָ��
	if(packetptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	sharegtpptr->sharegroupptr=packetptr;
	sharegtpptr->groupsize=paritynum;

	curshareptr=packetptr;
	for(i=0;i<paritynum;i++)
	{
		curshareptr->codeptr=shareptr->codeptr;
		contentptr=talloc(BYTE,packetsize);
		if(contentptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->content=contentptr;
		curshareptr->storepath=shareptr->storepath;
		curshareptr->FromIP=localptr->IPaddress;
		curshareptr++;
	}
	resTF=NAONTRS_CLOUD_DIVIDESHARE(sharegtpptr,shareptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot divide");
		return FALSE;
	}
	//�������������ǰk������(���������̣���Ϊǰk���������)
	//�͵�ַ�б���0���������з���paritycloud
	//���ɴ����б�
	sendgrouptr=talloc(ADDRESSGROUP,1);
	if(sendgrouptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendaddressptr=talloc(SHAREADDRESS,paritynum);//��ʼ�Ʒ������еļ�����
	if(sendaddressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendgrouptr->addressptr=sendaddressptr;
	sendgrouptr->sizenum=paritynum;//����ֻҪָһ�²�Ҫ��һ�������ַ
	resTF=NAONTRS_CLOUD_SENDCLOUDINITIAL(sendgrouptr,addresslistptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","send clouds cannot be calculated");
	}

	//�ȷ���packet����
	resTF=NAONTRS_CLOUD_SENDSHAREGROUP(sendgrouptr,sharegtpptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
		return FALSE;
	}
	//��������֪����sendgroup�Ƿ��͵�ַ����ô�Ϳ��Կ�ʼ����������
	//���ͳ�ʼ�Ƶ�ַ���к�share��С��������0�ŷ���
	if(seqnum==0)
	{
		//����
		resTF=NAONTRS_CLOUD_SENDADDRESSLISTGROUP(sendgrouptr,addrgroupptr);//����ʼ�Ƶ�ַ���͵�����У����
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
			return FALSE;
		}
		//����share�ķ�С
		resTF=NAONTRS_CLOUD_SENDSIZENUMGROUP(sendgrouptr,sizenum);//��share��С���ͳ�ȥ
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","size cannot be send");
			return FALSE;
		}
	}
	//�����������Դ��free��
	//sendgroup
	free(sendaddressptr);
	free(sendgrouptr);
	//�ͷ�
	packetptr=sharegtpptr->sharegroupptr;
	curshareptr=packetptr;
	for(i=0;i<paritynum;i++)
	{
		contentptr=curshareptr->content;
		free(contentptr);
		curshareptr++;
	}
	free(packetptr);
	free(sharegtpptr);
	//�ͷŵ�ַָ��
	addressptr=addrgroupptr->addressptr;
	curaddressptr=addressptr;
	for(i=0;i<pronum;i++)
	{
		IPaddress=curaddressptr->IPaddress;
		storepath=curaddressptr->storepath;
		free(IPaddress);
		free(storepath);

	}
	free(addressptr);
	free(addrgroupptr);
	return TRUE;
}

int NAONTRS_CLOUD_FROMINITIALCLOUD(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,AONTSHARE *shareptr,SOCKET sock)
{
	int i,j,sendnum,resTF;
	BYTE *contentptr;
	AONTSHARE *packetptr,*curshareptr,*resshareptr;
	CODEPARA *codeptr;
	int datanum,initialrecvnum,parityrecvnum,pronum,totnum,paritynum,bufsize;

	AONTSHAREGROUP *initialsharegtpptr,*sendsharegtr,*recvsharegtr;
	ADDRESSGROUP *addrgroupptr,*sendgrouptr;

	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	char *IPaddress,*storepath,*FromIP;
	int reminder,sizenum,packetsize;
	int fromtype;
	int seqnum;

	codeptr=shareptr->codeptr;
	pronum=codeptr->pronum;
	totnum=codeptr->totnum;
	paritynum=totnum-pronum;
	sendnum=paritynum-1;//���ڳ����Լ������
	//��һ��������ʣ�µ�k-1��packet��һ����ַ�б�
	initialrecvnum=datanum;//��Ҫ���մ�k����������
	parityrecvnum=pronum;

	packetptr=talloc(AONTSHARE,datanum);//Ϊ�˼򻯣���Ҫ��������k���Ƶ���������
	if(packetptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	packetsize=shareptr->sizenum;
	curshareptr=packetptr;
	curshareptr=shareptr;//�ȼ�¼����
	curshareptr++;
	for(i=0;i<(initialrecvnum-1);i++)//��ʣ�µ�k-1���������ݳ�ʼ������ȡ����
	{
		//����ռ�
		codeptr=talloc(CODEPARA,1);
		if(codeptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->codeptr=codeptr;
		contentptr=talloc(BYTE,packetsize);
		if(contentptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->content=contentptr;
		storepath=talloc(char,60);
		if(storepath==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->storepath=storepath;
		FromIP=talloc(char,IPlen);
		if(FromIP==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->FromIP=FromIP;
		resTF=NAONTRS_CLOUD_RECEIVESHARE(curshareptr,sock);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be received");
			return FALSE;
		}
		
		curshareptr++;
	}
	initialsharegtpptr=talloc(AONTSHAREGROUP,1);
	if(initialsharegtpptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	initialsharegtpptr->sharegroupptr=packetptr;
	initialsharegtpptr->groupsize=initialrecvnum;
	//������д���
	//�ڶ��������յ�ַ�б��sizenum
	//����intialcloud��ַ�б�,���ڳ�ʼ����d�������Ҫd��λ��
	addressptr=talloc(SHAREADDRESS,pronum);
	if(addressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	curaddressptr=addressptr;
	for(i=0;i<pronum;i++)
	{
		IPaddress=talloc(char,IPlen);//��30λ3*4=12+4=16
		if(IPaddress==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curaddressptr->IPaddress=IPaddress;
		storepath=talloc(char,PATHlen);
		if(storepath==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curaddressptr->storepath=storepath;
	}
	addrgroupptr->addressptr=addressptr;

	resTF=NAONTRS_CLOUD_RECEIVEADDRESSLIST(addrgroupptr,sock);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","address cannot be received");
		return FALSE;
	}
	resTF=NAONTRS_CLOUD_RECEIVESIZENUM(&sizenum,sock);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","sizenum cannot be received");
		return FALSE;
	}
	//���������������ݽ��б�����б������d��n-1���Ƶ�����
	//��������������n�����еĵ�i��,���ɷ�������	
	sendsharegtr=talloc(AONTSHAREGROUP,1);
	if(sendgrouptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	packetptr=talloc(AONTSHARE,paritynum);
	if(packetptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	curshareptr=packetptr;
	//����ռ�
	for(i=0;i<paritynum;i++)
	{
		contentptr=talloc(BYTE,packetsize);//��Ҫ�е�ַ������
		if(contentptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->content=contentptr;
		curshareptr->sizenum=packetsize;
		curshareptr++;
	}
	sendsharegtr->sharegroupptr=packetptr;
	sendsharegtr->groupsize=paritynum;
	//����
	resTF=NAONTRS_CLOUD_CODESHARES(sendsharegtr,initialsharegtpptr);//ע��������sendgroupΪn-d��
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","");
		return FALSE;
	}
	//���Ĳ������͵�������parity cloud(n-d-1��)
	//���ɷ��͵�ַ�б�
	sendgrouptr=talloc(ADDRESSGROUP,1);
	if(sendgrouptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendaddressptr=talloc(SHAREADDRESS,sendnum);
	if(sendaddressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendgrouptr->addressptr=sendaddressptr;
	sendgrouptr->sizenum=sendnum;
	resTF=NAONTRS_CLOUD_SENDCLOUDPARITY(sendgrouptr,addresslistptr,localptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","address cannot be created");
		return FALSE;
	}
	//��������
	resTF=NAONTRS_CLOUD_SENDSHAREGROUPPARITY(sendgrouptr,sendsharegtr,seqnum);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
		return FALSE;
	}
	//����paritycloud�����ݲ����share
	//��ʼ������տռ�
	recvsharegtr=talloc(AONTSHAREGROUP,1);
	if(recvsharegtr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	packetptr=talloc(AONTSHARE,sendnum);//sendnum=n-d-1
	if(packetptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	recvsharegtr->sharegroupptr=packetptr;
	recvsharegtr->groupsize=sendnum;
	curshareptr=packetptr;
	//��ʼ����ռ�
	for(i=0;i<sendnum;i++)
	{
		codeptr=talloc(CODEPARA,1);
		if(codeptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;  
		}
		contentptr=talloc(BYTE,packetsize);
		if(contentptr==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->content=contentptr;
		storepath=talloc(char,PATHlen);
		if(storepath==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->storepath=storepath;
		FromIP=talloc(char,IPlen);
		if(FromIP==NULL)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
			return FALSE;
		}
		curshareptr->FromIP=FromIP;
		curshareptr++;
	}
	resTF=NAONTRS_CLOUD_RECEIVESHAREGROUP(recvsharegtr,sendnum);//��ȡn-d-1��У���Ƶ����ݣ������Լ��⣩
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be received");
		return FALSE;
	}
	//�ϲ�����
	//����Ŀ��ռ�,ʤ������
	resshareptr=talloc(AONTSHARE,1);
	if(resshareptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	bufsize=paritynum*packetsize;
	contentptr=talloc(BYTE,bufsize);
	if(contentptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	resshareptr->content=contentptr;//�������ֶ����Ա仯ָ��
	resTF=NAONTRS_CLOUD_MERGESHAREPARITY(resshareptr,recvsharegtr,sendsharegtr,seqnum,sendgrouptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be merged");
		return FALSE;
	}
	//�չ�������
	resTF=file_FILELAYER_SHARE_WRITER(resshareptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be writen");
		return FALSE;
	}
	//�ͷ��������Դ
	packetptr=initialsharegtpptr->sharegroupptr;
	curshareptr=packetptr;
	curshareptr++;
	for(i=0;i<datanum;i++)
	{
		codeptr=curshareptr->codeptr;
		contentptr=curshareptr->content;
		storepath=curshareptr->storepath;
		FromIP=curshareptr->FromIP;
		free(codeptr);
		free(contentptr);
		free(storepath);
		free(FromIP);
	}
	free(initialsharegtpptr);
	//intial address d��
	addressptr=addrgroupptr->addressptr;
	curaddressptr=addressptr;
	for(i=0;i<pronum;i++)
	{
		IPaddress=curaddressptr->IPaddress;
		storepath=curaddressptr->storepath;
		free(IPaddress);
		free(storepath);
		curaddressptr++;
	}
	free(addressptr);
	free(addrgroupptr);
	//��������,����n-d��������n-d-1�����͵�ַ
	//����
	packetptr=sendsharegtr->sharegroupptr;
	curshareptr=packetptr;
	for(i=0;i<paritynum;i++)
	{
		contentptr=curshareptr->content;
		free(contentptr);
		curshareptr++;
	}
	//��ַ
	sendaddressptr=sendgrouptr->addressptr;
	free(sendaddressptr);
	free(sendgrouptr);
	//���ջ���
	packetptr=recvsharegtr->sharegroupptr;
	curshareptr=packetptr;
	for(i=0;i<sendnum;i++)
	{
		codeptr=curshareptr->codeptr;
		contentptr=curshareptr->content;
		storepath=curshareptr->storepath;
		FromIP=curshareptr->FromIP;
		free(codeptr);
		free(contentptr);
		free(storepath);
		free(FromIP);
	}
	//�������
	contentptr=resshareptr->content;
	free(contentptr);
	free(resshareptr);
	return TRUE;
}

int NAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr)//�����������ַ�б�ͱ��ص�ַ
{
	//��һ����ʼ��socket
	int i,j,sendnum,resTF;
	BYTE *contentptr;
	AONTSHARE *shareptr,*packetptr,*curshareptr;
	CODEPARA *codeptr;
	int datanum,initialrecvnum,parityrecvnum,pronum,totnum,paritynum;

	AONTSHAREGROUP *sharegtpptr;
	ADDRESSGROUP *addrgroupptr,*sendgrouptr;

	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	SOCKET sock;

	char *IPaddress,*storepath;
	int reminder,sizenum,packetsize;
	int fromtype;
	int seqnum;

	resTF=NAONTRS_CLOUD_initial(sock);
	shareptr=talloc(AONTSHARE,1);
	if(shareptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","server cannot initial");
		return FALSE;
	}
	while(1)
	{
		//����ѭ��״̬
		resTF=NAONTRS_CLOUD_RECEIVESHARE(shareptr,sock);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be received");
			return FALSE;
		}
		fromtype=shareptr->fromtypes;
		if(fromtype==FROMDEVICE)
		{
			resTF=NAONTRS_CLOUD_FROMDEVICE(addresslistptr,localptr,shareptr,sock);
			if(resTF==FALSE)
			{
				show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","device process cannot run");
				return FALSE;
			}
		}
		else
		{
			resTF=NAONTRS_CLOUD_FROMINITIALCLOUD(addresslistptr,localptr,shareptr,sock);
			if(resTF==FALSE)
			{
				show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","initial cloud process cannot run");
				return FALSE;
			}
		}
	}
}