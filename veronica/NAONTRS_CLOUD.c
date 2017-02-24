#include "NAONTRS_CLOUD.h"

int NAONTRS_CLOUD_initial(SOCKET sock)
{
	int resTF;
	struct sockaddr_in sin; //本地socket
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
	sin.sin_port=TRANPORT;//填写默认端口号
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	resTF=bind(sock,(LPSOCKADDR)&sin, sizeof(sin));
	if(resTF=SOCKET_ERROR)
	{
		closesocket(sock);       
		WSACleanup();
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","binding error");
		return FALSE;
	}
	//监听
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
	struct sockaddr_in addrClient;//用户socket地址
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
	//接收四个部分并且拼接
	//主题
	resTF=recv(clientsock,(char *)shareptr,sizeof(AONTSHARE),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//重新设定指针
	shareptr->codeptr=codeptr;
	shareptr->content=content;
	shareptr->storepath=storepath;
	shareptr->FromIP=FromIP;
	//内部所有的指针重新设定，由于是远端的内存地址
	//编码部分
	resTF=recv(clientsock,(char *)codeptr,sizeof(CODEPARA),0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//内容
	sizenum=shareptr->sizenum;
	resTF=recv(clientsock,(char *)content,sizenum,0);
	if(resTF<0)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
		return FALSE;
	}
	//存储长度
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

// 发送intial cloud地址
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
	struct sockaddr_in addrClient;//用户socket地址
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
	//接收
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
		//传输字符串大小
		resTF=recv(clientsock,(char *)stringlen,sizeof(int),0);
		if(stringlen==0)
		{
			break;
		}
		//收IP地址
		resTF=recv(clientsock,IPaddress,stringlen+1,0);
		if(resTF<0)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_RECEIVESHARE","receive error");
			return FALSE;
		}
		//传输地址项
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
	struct sockaddr_in addrClient;//用户socket地址
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

//将share大小进行发送
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

int NAONTRS_CLOUD_DIVIDESHARE(AONTSHAREGROUP *sharegroupptr,AONTSHARE *shareptr)//将一个share切分k个share
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

	packetsizenum=sizenum/groupsize;//需要注意申请时为了整除需要清零内存
	reminder=sizenum%groupsize;
	//如果无法整除需要加1
	if(reminder!=0)
	{
		packetsizenum=packetsizenum+1;
	}
	sharegroupptr->groupsize=groupsize;//设定
	packetptr=sharegroupptr->sharegroupptr;
	
	seqnum=0;
	curpacketptr=packetptr;
	curcontentptr=contentptr;
	for(i=0;i<groupsize;i++)
	{
		curpacketptr->codeptr=codeptr;
		curpacketptr->content=curcontentptr;
		curpacketptr->fromtypes=FROMINTIALCLOUD;//这里一定是frominitialcloud因为只有intialcloud会分片
		curpacketptr->seqnum=seqnum;
		curpacketptr->sizenum=sizenum;
		curpacketptr->storepath=storepath;
		curpacketptr++;
		seqnum++;
		curcontentptr=curcontentptr+packetsizenum;
	}
	return TRUE;
}

//合并share
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

//目的将收到的乱序的
int NAONTRS_CLOUD_MERGESHAREPARITY(AONTSHARE *shareptr,AONTSHAREGROUP *recvsharegtpptr,AONTSHAREGROUP *firstsharegtr,int sizenum,ADDRESSGROUP *sendaddressgtr)
{
	int i,j,resTF;
	int seqnum,curseqnum;
	int paritynum,pronum,totnum,sendnum;
	char *tarIPaddress,*FromIPaddress,*storepath;//需要比较IP地址来确定位置
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
	//需要序列号
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


	recvshareptr=recvsharegtpptr->sharegroupptr;//收到的数据

	addressptr=sendaddressgtr->addressptr;

	curaddressptr=addressptr;
	currecvshareptr=recvshareptr;
	curresshareptr=resshareptr;
	//进行排序
	for(i=0;i<paritynum;i++)
	{
		if(i==seqnum)
		{
			curresshareptr=curfirstshareptr;//本地数据
			curresshareptr++;
			continue;
		}
		//不是需要带接收列表中找
		tarIPaddress=curaddressptr->IPaddress;//需要的IP地址
		for(j=0;j<sendnum;j++)
		{
			FromIPaddress=currecvshareptr->FromIP;
			resTF=strcmp(FromIPaddress,tarIPaddress);
			if(resTF==0)
			{
				curresshareptr=currecvshareptr;//记下
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
	//释放空间
	//对于新生成的东西填写一下数据
	shareptr->codeptr=codeptr;
	shareptr->FromIP="";//不用填没有意义了
	shareptr->fromtypes=FROMPARITYCLOUD;
	shareptr->seqnum=0;
	shareptr->sizenum=sizenum;
	shareptr->storepath=storepath;
	free(ressharegtr);
	free(resshareptr);
	return TRUE;
}

//计算初始云需要进行的传输云
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

	//计算packet发送到n-d个云
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
			curinialaddressptr++;//比较下一个
		}
		cursendaddressptr=curaddressptr;//给个地址

		cursendaddressptr++;
		curaddressptr++;
	}
	return TRUE;
}

//计算发送序列地址
int NAONTRS_CLOUD_SENDCLOUDPARITY(ADDRESSGROUP *sendcloud,ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr,ADDRESSGROUP *initialcloud)
{
	int i,j,resTF;
	int pronum,totnum;
	CODEPARA *codeptr;
	char *localIPaddress,*initialaddressptr,*tarIPaddress;
	SHAREADDRESS *addressptr,*curaddressptr,*curinialaddressptr,*sendaddressptr,*cursendaddressptr;

	sendaddressptr=sendcloud->addressptr;
	curinialaddressptr=initialcloud->addressptr;//将将初始地址赋值

	pronum=codeptr->pronum;
	totnum=codeptr->totnum;

	localIPaddress=localptr->IPaddress;
	cursendaddressptr=sendaddressptr;
	curaddressptr=addresslistptr->addressptr;

	//发送packet到n-d-1个校验云
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
			curinialaddressptr++;//比较下一个
		}
		//都不是,就是要传输的位置
		cursendaddressptr=curaddressptr;//给个地址

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
	int datanum,pronum,totnum,codenum,w;//编码参数
	int initialnum,paritynum,sizenum;
	char *storepath,*FromIP;
	//编码数据
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
	codenum=codeptr->codenum;//需要编码的数据n-k
	w=codeptr->w;

	paritynum=totnum-pronum;
	//生成编码矩阵
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
	//进入编码过程
	matrix->w=w;
	matrix->xm=rows;
	matrix->ym=cols;
	matrix->matrix=mat;
	//释放矩阵
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
	//最后填一下先关数据
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
	//存储一下数据
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
	//接收来自用户端的数据
	//第一步接收初始云地址列表
	addrgroupptr=talloc(ADDRESSGROUP,1);
	if(addrgroupptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	addrgroupptr->sizenum=pronum;//初始云d个
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
		IPaddress=talloc(char,30);//给30位3*4=12+4=16
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
	if(seqnum>datanum)//如果大于k个直接下一步
	{
		return TRUE;//如果是
	}
	//第二步拆分成多个块
	//申请空间
			
	sizenum=shareptr->sizenum;
	//计算包大小
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
	packetptr=talloc(AONTSHARE,paritynum);//申请发送包的指针
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
	//第三步，如果是前k个将块(方便编码过程，因为前k个是明码块)
	//和地址列表（第0个发）进行发送paritycloud
	//生成传输列表
	sendgrouptr=talloc(ADDRESSGROUP,1);
	if(sendgrouptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendaddressptr=talloc(SHAREADDRESS,paritynum);//初始云发送所有的检验云
	if(sendaddressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memroy");
		return FALSE;
	}
	sendgrouptr->addressptr=sendaddressptr;
	sendgrouptr->sizenum=paritynum;//由于只要指一下不要进一步申请地址
	resTF=NAONTRS_CLOUD_SENDCLOUDINITIAL(sendgrouptr,addresslistptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","send clouds cannot be calculated");
	}

	//先发送packet内容
	resTF=NAONTRS_CLOUD_SENDSHAREGROUP(sendgrouptr,sharegtpptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
		return FALSE;
	}
	//我们现在知道了sendgroup是发送地址组那么就可以开始发送数据了
	//发送初始云地址序列和share大小，仅仅在0号发送
	if(seqnum==0)
	{
		//发送
		resTF=NAONTRS_CLOUD_SENDADDRESSLISTGROUP(sendgrouptr,addrgroupptr);//将初始云地址发送到所有校验云
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
			return FALSE;
		}
		//发送share的发小
		resTF=NAONTRS_CLOUD_SENDSIZENUMGROUP(sendgrouptr,sizenum);//将share大小发送出去
		if(resTF==FALSE)
		{
			show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","size cannot be send");
			return FALSE;
		}
	}
	//最后把申请的资源都free了
	//sendgroup
	free(sendaddressptr);
	free(sendgrouptr);
	//释放
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
	//释放地址指针
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
	sendnum=paritynum-1;//等于除了自己以外的
	//第一步，收完剩下的k-1个packet和一个地址列表
	initialrecvnum=datanum;//需要接收从k个来的数据
	parityrecvnum=pronum;

	packetptr=talloc(AONTSHARE,datanum);//为了简化，需要接受来自k个云的明码数据
	if(packetptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	packetsize=shareptr->sizenum;
	curshareptr=packetptr;
	curshareptr=shareptr;//先记录下来
	curshareptr++;
	for(i=0;i<(initialrecvnum-1);i++)//从剩下的k-1个发送数据初始云中收取数据
	{
		//申请空间
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
	//数据填写完毕
	//第二步，接收地址列表和sizenum
	//申请intialcloud地址列表,由于初始云是d个因此需要d个位置
	addressptr=talloc(SHAREADDRESS,pronum);
	if(addressptr==NULL)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","No memory");
		return FALSE;
	}
	curaddressptr=addressptr;
	for(i=0;i<pronum;i++)
	{
		IPaddress=talloc(char,IPlen);//给30位3*4=12+4=16
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
	//第三步，对于数据进行编码进行编码产生d到n-1和云的数据
	//现在我们收齐了n个云中的第i份,生成发送数据	
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
	//申请空间
	for(i=0;i<paritynum;i++)
	{
		contentptr=talloc(BYTE,packetsize);//需要有地址存内容
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
	//编码
	resTF=NAONTRS_CLOUD_CODESHARES(sendsharegtr,initialsharegtpptr);//注意编码出的sendgroup为n-d个
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","");
		return FALSE;
	}
	//第四步，发送到其他的parity cloud(n-d-1个)
	//生成发送地址列表
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
	//发送数据
	resTF=NAONTRS_CLOUD_SENDSHAREGROUPPARITY(sendgrouptr,sendsharegtr,seqnum);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be send");
		return FALSE;
	}
	//接收paritycloud的数据并组成share
	//开始申请接收空间
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
	//开始缓存空间
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
	resTF=NAONTRS_CLOUD_RECEIVESHAREGROUP(recvsharegtr,sendnum);//收取n-d-1个校验云的数据（即除自己外）
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be received");
		return FALSE;
	}
	//合并数据
	//申请目标空间,胜利在望
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
	resshareptr->content=contentptr;//其他部分都可以变化指针
	resTF=NAONTRS_CLOUD_MERGESHAREPARITY(resshareptr,recvsharegtr,sendsharegtr,seqnum,sendgrouptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be merged");
		return FALSE;
	}
	//收工存下来
	resTF=file_FILELAYER_SHARE_WRITER(resshareptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS_CLOUD","NAONTRS_CLOUD_API","share cannot be writen");
		return FALSE;
	}
	//释放申请的资源
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
	//intial address d个
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
	//发送内容,包括n-d个编码块和n-d-1个发送地址
	//内容
	packetptr=sendsharegtr->sharegroupptr;
	curshareptr=packetptr;
	for(i=0;i<paritynum;i++)
	{
		contentptr=curshareptr->content;
		free(contentptr);
		curshareptr++;
	}
	//地址
	sendaddressptr=sendgrouptr->addressptr;
	free(sendaddressptr);
	free(sendgrouptr);
	//接收缓冲
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
	//结果缓冲
	contentptr=resshareptr->content;
	free(contentptr);
	free(resshareptr);
	return TRUE;
}

int NAONTRS_CLOUD_API(ADDRESSLIST *addresslistptr,SHAREADDRESS *localptr)//输入服务器地址列表和本地地址
{
	//第一步初始化socket
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
		//进入循环状态
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