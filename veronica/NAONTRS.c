#include "NAONTRS.h"
int NAONTRS_RANDKEY(BYTE *content,int strlen)
{
	int resTF;
	resTF=AONTRS_RANDKEY(content,strlen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_RANDKEY","random key cannot calculate random key");
		return FALSE;
	}
	return TRUE;
}

int NAONTRS_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen)
{
	int resTF;
	resTF=AONTRS_HASHKEY(key,context,strlen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_RANDKEY","hashkey cannot calculate hash value");
		return FALSE;
	}
	return TRUE;
}
int NAONTRS_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen)
{
	int resTF;
	resTF=AONTRS_CHRYPTEXT(out,in,strlen,key,keylen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_CHRYPTEXT","CHRYPTTEXT cannot be calculated");
		return FALSE;
	}
	return TRUE;
}
int NAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen)
{
	int resTF;
	resTF=NAONTRS_XORKEY(out,in,strlen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_XORKEY","XORKEY cannot be calculated");
		return FALSE;
	}
	return FALSE;
}
int NAONTRS_CHRYPTER(BYTE *cipertext,//密文
							BYTE *context,//原文
							int strlen)
{
	int resTF;
	resTF=AONTRS_CHRYPTER(cipertext,context,strlen);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_CHRYPTER","CHRYPTER cannot be calculated");
		return FALSE;
	}
	return FALSE;
}
int NAONTRS_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen)
{
	int resTF;
	codeptr->totnum=codeptr->pronum;//在用户端需要编码d个
	codeptr->codenum=codeptr->pronum-codeptr->datanum;
	resTF=AONTRS_RS(sharegroupptr,cipertext,codeptr,strlen);
	{
		show_error("NAONTRS","NAONTRS_RS","NAONTRS cannot");
		return FALSE;
	}
	return TRUE;
}





int NAONTRS_divider(AONTSHARE *shareptr,PACKETGROUP *packetgrpptr,int packetnumber)
{
	int i,packetsize;
	int sharesize;
	CODEPARA *codeptr;
	PACKET *packetptr,*curpacketptr;
	BYTE *shareconptr,*curshareptr;
	BYTE *packetconptr,*curpacketconptr;

	codeptr=shareptr->codeptr;

	sharesize=shareptr->sizenum;
	shareconptr=shareptr->content;
	curshareptr=shareconptr;

	packetsize=sharesize/packetnumber;

	packetptr=packetgrpptr->packetgrpptr;
	packetgrpptr->grpnum=packetnumber;

	curpacketptr=packetptr;
	for(i=0;i<packetnumber;i++)
	{
		curpacketptr->codeptr=codeptr;
		packetconptr=curpacketptr->content;
		//将数据copy过来
		mcpy(packetconptr,curshareptr,BYTE,packetsize);
		curpacketptr->packetsize=packetsize;
		curshareptr=curshareptr+packetsize;
		curpacketptr++;
	}
	return TRUE;
}
int NAONTRS_merger(AONTSHARE *shareptr,PACKETGROUP *packetgrpptr)
{
	int i,packetnum,packetsize,sharesize;
	BYTE *shareconptr,*curshareconptr;
	BYTE *packetconptr,*curpacketconptr;

	PACKET *packetptr,*curpacketptr;

	packetptr=packetgrpptr->packetgrpptr;
	packetsize=packetptr->packetsize;
	packetnum=packetgrpptr->grpnum;

	sharesize=packetsize*packetnum;

	curpacketptr=packetptr;
	curshareconptr=shareconptr;
	for(i=0;i<packetnum;i++)
	{
		packetconptr=curpacketptr->content;
		mcpy(curshareconptr,packetconptr,BYTE,packetsize);
		curpacketptr++;
		curshareconptr=curshareconptr+packetsize;
	}
	return TRUE;
}
int NAONTRS_encoder(PACKETGROUP *tarpacketgepptr,PACKETGROUP *scrpacketgrpptr,CODEPARA *codeptr)
{
	//为了方面实现我们仅仅使用前k个进行编码
	//虽然这样传输数据量有点大，可以可以有效避免随机线性编码
	//先生成编码矩阵(主要在cauchy中实现)
	int resTF;
	resTF=cauchy_API_Packetgroup(tarpacketgepptr,scrpacketgrpptr,codeptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_encoder","packet cannot be calculated");
		return FALSE;
	}
	return TRUE;
}


int NAONTRS_STORESHARE(AONTSHARE *shareptr)
{
	int resTF;
	resTF=file_FILELAYER_SHARE_WRITER(shareptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_STORESHARE","share cannot be stored");
		return FALSE;
	}
	return TRUE;

}

int NAONTRS_initial()
{
	//主要是初始化server
	WORD wVersionRequests;
	WSADATA wsadata;
	SOCKADDR_IN sin;
	int resTF;

	wVersionRequests=MAKEWORD(2,2);

	resTF=WSAStartup(wVersionRequests,&wsadata);
	if(resTF!=QTRUE)
	{//不是不创建成功
		show_error("NAONTRS","NAONTRS_initial","socket cannot startup");
		return FALSE;
	}

	NAONTsocksrv=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(NAONTsocksrv==INVALID_SOCKET)
	{
		show_error("NAONTRS","NAONTRS_initial","socket cannot been open");
		return FALSE;
	}
	//开地址
	
	sin.sin_family=AF_INET;
	sin.sin_port=htons(6000);
	sin.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	resTF=bind(NAONTsocksrv,(SOCKADDR*)&sin,sizeof(sin));
	if(resTF==SOCKET_ERROR)
	{
		show_error("NAONTRS","NAONTRS_initial","socket error");
		return FALSE;
	}
	resTF=listen(NAONTsocksrv,10);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_initial","socket cannot listen");
		return FALSE;
	}
	return TRUE;
}
int NAONTRS_SOCKETADDRESS(SOCKET sockptr,SHAREADDRESS *addressptr)//根据地址创建一个socket
{
	int resTF;
	WSADATA wsdata;
	WORD sockversions=MAKEWORD(2,2);
	char *IPaddress;
	int portnumer;
	SOCKADDR_IN seraddr;



	resTF=WSAStartup(sockversions,&wsdata);
	if(resTF!=QTRUE)
	{
		show_error("NAONTRS","NAONTRS_SOCKETADDRESS","socket cannot startup");
		return FALSE;
	}
	IPaddress=addressptr->IPaddress;
	portnumer=addressptr->port;

	seraddr.sin_family=AF_INET;
	seraddr.sin_port=htons(portnumer);
	seraddr.sin_addr.S_un.S_addr=inet_addr(IPaddress);

	sockptr=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockptr==INVALID_SOCKET)
	{
		//closesocket(sockptr);此时socket还不存在
		WSACleanup();
		show_error("NAONTRS","NAONTRS_SOCKETADDRESS","spcket cannot create");
		return FALSE;
	}
	resTF=connect(sockptr,(struct suckaddr *)&seraddr,sizeof(seraddr));
	if(resTF==SOCKET_ERROR)
	{

		closesocket(sockptr); 
		WSACleanup();
		show_error("NAONTRS","NAONTRS_SOCKETADDRESS","socket cannot connect");
		return FALSE;
	}
	return TRUE;
}
int NAONTRS_SENDSHARE(AONTSHARE *shareptr,SHAREADDRESS *addressptr)//发送一个SHARE到指定IP
{
	
	SOCKET sock;
	int resTF;

	

	AONTSHARE *tmpbuffer;//

	
	resTF=NAONTRS_SOCKETADDRESS(sock,addressptr);

	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_SENDSHARE","socket cannot create");
		return FALSE;
	}
	
	tmpbuffer=talloc(AONTSHARE,1);
	if(tmpbuffer==NULL)
	{
		show_error("NAONTRS","NAONTRS_SENDSHARE","No memory");
		return FALSE;
	}

	mcpy(shareptr,tmpbuffer,AONTSHARE,1);

	resTF=send(sock,(char *)tmpbuffer,sizeof(AONTSHARE),0);
	if(resTF==SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		show_error("NAONTRS","NAONTRS_SENDSHARE","send error");
		return FALSE;
	}
	free(tmpbuffer);
	closesocket(sock);
	 WSACleanup();
	return TRUE;
}
int NAONTRS_RECEIVESHARE(AONTSHARE *shareptr)
{
	//从服务器端获取一个share
	SOCKET sockcon;
	SOCKADDR_IN remoteaddr;
	AONTSHARE *sharebuffer;
	int addrlen,retnum;

	addrlen=sizeof(SOCKADDR_IN);
	sockcon=accept(NAONTsocksrv,(SOCKADDR *)&remoteaddr,&addrlen);
	if(sockcon==INVALID_SOCKET)
	{
		show_error("NAONTRS","NAONTRS_RECEIVESHARE","accept error");
		return FALSE;
	}
	sharebuffer=talloc(AONTSHARE,1);
	retnum=recv(sockcon,(char *)sharebuffer,sizeof(AONTSHARE),0);
	if(retnum<0)
	{
		show_error("NAONTRS","NAONTRS_RECEIVESHARE","message cannot recieve");
		closesocket(sockcon);
		return FALSE;
	}
	mcpy(shareptr,sharebuffer,AONTSHARE,1);
	free(sharebuffer);
	closesocket(sockcon);
	return TRUE;
}
int NAONTRS_SENDPACKET(PACKET *packetptr,SHAREADDRESS *addressptr)
{
	SOCKET sock;
	int resTF;

	PACKET *tmpbuffer;

	resTF=NAONTRS_SOCKETADDRESS(sock,addressptr);
	if(resTF==FALSE)
	{
		show_error("NAONTRS","NAONTRS_SENDSHARE","socket cannot create");
		return FALSE;
	}

	tmpbuffer=talloc(PACKET,1);
	if(tmpbuffer==NULL)
	{
		show_error("NAONTRS","NAONTRS_SENDPACKET","No memory");
		return FALSE;
	}

	mcpy(tmpbuffer,packetptr,PACKET,1);
	resTF=send(sock,(char *)tmpbuffer,sizeof(AONTSHARE),0);
	if(resTF==SOCKET_ERROR)
	{
		closesocket(sock);
		WSACleanup();
		show_error("NAONTRS","NAONTRS_SENDSHARE","packet cannot send");
		return FALSE;
	}
	
	free(tmpbuffer);
	closesocket(sock);
		WSACleanup();
	return TRUE;
}
int NAONTRS_RECEIVEPACKET(PACKET *packetptr)
{
	SOCKET sockcon;
	SOCKADDR_IN remoteaddr;
	PACKET *packetbuffer;
	int addrlen,retnum;


	//从服务器端获取一个packet
	addrlen=sizeof(remoteaddr);
	sockcon=accept(NAONTsocksrv,(SOCKADDR *)&remoteaddr,&addrlen);
	if(sockcon==INVALID_SOCKET)
	{
		show_error("NAONTRS","NAONTRS_RECEIVESHARE","accept error");
		return FALSE;
	}
	packetbuffer=talloc(PACKET,1);
	retnum=recv(sockcon,(char *)packetbuffer,sizeof(PACKET),0);
	if(retnum<0)
	{
		show_error("NAONTRS","NAONTRS_RECEIVESHARE","message cannot recieve");
		closesocket(sockcon);
		return FALSE;
	}
	mcpy(packetptr,packetbuffer,AONTSHARE,1);
	free(packetbuffer);
	closesocket(sockcon);
	return TRUE;
}

int NAONTRS_SENDSHAREGROUP(AONTSHAREGROUP *sharegroupptr,ADDRESSLIST *addresslistptr)
{
	int i,resTF,groupsize;
	AONTSHARE *shareptr,*curshareptr;
	SHAREADDRESS *addressptr,*curaddressptr;

	groupsize=sharegroupptr->groupsize;
	shareptr=sharegroupptr->sharegroupptr;
	addressptr=addresslistptr->addressptr;

	curshareptr=shareptr;
	curaddressptr=addressptr;
	for(i=0;i<groupsize;i++)
	{
		resTF=NAONTRS_SENDSHARE(curshareptr,curaddressptr);
		if(resTF==FALSE)
		{
			show_error("NAONTRS","NAONTRS_SENDSHAREGROUP","share cannot send");
			return FALSE;
		}
		curshareptr++;
		curaddressptr++;
	}
	return TRUE;
}
int NAONTRS_RECEIVESHAREGROUP(AONTSHAREGROUP *sharegroupptr)
{
	int i,resTF,groupsize;
	AONTSHARE *shareptr,*curshareptr;
	//SHAREADDRESS *addressptr,*curaddressptr;

	groupsize=sharegroupptr->groupsize;
	shareptr=sharegroupptr->sharegroupptr;
	//addressptr=addresslistptr->addressptr;

	curshareptr=shareptr;
	//curaddressptr=addressptr;
	for(i=0;i<groupsize;i++)
	{
		
		
		resTF=NAONTRS_RECEIVESHARE(curshareptr);
		if(resTF==FALSE)
		{
			show_error("NAONTRS","NAONTRS_RECEIVESHAREGROUP","share cannot receive");
			return FALSE;
		}
		//curaddressptr++;
		curshareptr++;
	}
	return TRUE;
}

int NAONTRS_SENDPACKETGROUP(PACKETGROUP *packetgrpptr,ADDRESSLIST *addresslistptr)
{
	int i,resTF,groupsize;
	SHAREADDRESS *addressptr,*curaddressptr;
	PACKET *packetptr,*curpacketptr;

	groupsize=packetgrpptr->grpnum;
	packetptr=packetgrpptr->packetgrpptr;
	addressptr=addresslistptr->addressptr;
	//groupsize=groupsize;


	curpacketptr=packetptr;
	curaddressptr=addresslistptr->addressptr;
	for(i=0;i<groupsize;i++)
	{
		resTF=NAONTRS_SENDPACKET(curpacketptr,curaddressptr);
		if(resTF==FALSE)
		{
			show_error("","","");
			return FALSE;
		}
		curpacketptr++;
		curaddressptr++;
	}

	return TRUE;
}
int NAONTRS_RECEIVEPACKETGROUP(PACKETGROUP *packetgrpptr)
{
	int i,resTF,groupsize;
	//SHAREADDRESS *addressptr,*curaddressptr;
	PACKET *packetptr,*curpacketptr;
	//packetsize

	packetptr=packetgrpptr->packetgrpptr;
	//addressptr=addresslistptr->addressptr;
	groupsize=packetgrpptr->grpnum;

	curpacketptr=packetptr;
	//curaddressptr=addressptr;

	for(i=0;i<groupsize;i++)
	{
		resTF=NAONTRS_RECEIVEPACKET(curpacketptr);
		if(resTF==FALSE)
		{
			show_error("NAONTRS","NAONTRS_RECEIVEPACKETGROUP","packet cannot receive");
			return FALSE;
		}
		curpacketptr++;
		//curaddressptr++;
	}

	return TRUE;
}