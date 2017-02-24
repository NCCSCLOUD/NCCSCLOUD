#include "NCAONTRS_DEVICE.h"

int NCAONTRS_DEVICE_HASHKEY(BYTE *key,//输出hash结果
							BYTE *context,//原文
							int strlen
							)
{
	int resTF;
	resTF=CAONTRS_HASHKEY(key,context,strlen);
	return resTF;
}
int NCAONTRS_DEVICE_CHRYPTEXT(BYTE *out,
							BYTE *in,
							int strlen,
							BYTE *key,
							int keylen)
{
	int resTF;
	resTF=CAONTRS_CHRYPTEXT(out,in,strlen,key,keylen);
	return resTF;
}

int NCAONTRS_DEVICE_CHRYPTER(BYTE *cipertext,//密文
							BYTE *context,//原文
							int strlen)
{
	int resTF;
	resTF=CAONTRS_CHRYPTER(cipertext,context,strlen);
	return resTF;
}

int NCAONTRS_DEVICE_RS(AONTSHAREGROUP *sharegroupptr,
					BYTE *cipertext,
					CODEPARA *codeptr,
					int strlen)
{
	int resTF;
	resTF=NAONTRS_DEVICE_RS(sharegroupptr,cipertext,codeptr,strlen);
	return resTF;
}

int NCAONTRS_XORKEY(BYTE *out,
						BYTE *in,
						int strlen)
{
	int resTF;
	resTF=CAONTRS_XORKEY(out,in,strlen);
	return resTF;
}

int NCAONTRS_DEVICE_SENSOR(ADDRESSLIST *addresslistptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENSOR(addresslistptr);
	return resTF;
}

USHORT NCAONTRS_DEVICE_GetCheckSum(LPBYTE lpBuf, DWORD dwSize)
{
	int resTF;
	resTF=NAONTRS_DEVICE_GetCheckSum(lpBuf,dwSize);
	return resTF;
}

int NCAONTRS_DEVICE_PING(SHAREADDRESS *addressptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_PING(addressptr);
	return resTF;
}

int NCAONTRS_DEVICE_SELECTCLOUD(ADDRESSGROUP *initialcloud,ADDRESSLIST *addresslistptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SELECTCLOUD(initialcloud,addresslistptr);
	return resTF;
}

int NCAONTRS_DEVICE_SENDSHARE(SHAREADDRESS *addressptr,AONTSHARE *shareptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENDSHARE(addressptr,shareptr);
	return resTF;
}

int NCAONTRS_DEVICE_SENDSHAREGROUP(ADDRESSGROUP *addressgtr,AONTSHAREGROUP *sharegtr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENDSHAREGROUP(addressgtr,sharegtr);
	return resTF;
}

int NCAONTRS_DEVICE_SENDADDRESSLIST(SHAREADDRESS *addressptr,ADDRESSGROUP *addrgroupptr)
{
	int resTF;
	resTF=NCAONTRS_DEVICE_SENDADDRESSLIST(addressptr,addrgroupptr);
	return resTF;
}

int NCAONTRS_DEVICE_SENDADDRESSLISTGROUP(ADDRESSGROUP *taraddrgroupptr,ADDRESSGROUP *addrgroupptr)
{
	int resTF;
	resTF=NAONTRS_DEVICE_SENDADDRESSLISTGROUP(taraddrgroupptr,addrgroupptr);
	return resTF;
}

//基本逻辑和AONTRS一致，但是中间的加密算法改变，因此需要改一下程序
int NCAONTRS_DEVICE_API(BYTE *content,int strlen,ADDRESSLIST *addresslistptr)
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

	resTF=NCAONTRS_DEVICE_CHRYPTER(cpytext,content,cpylen);//加密
	if(resTF==FALSE)
	{
		show_error("NCAONTRS","NCAONTRS_API","share cannot be ctrypted");
		return FALSE;
	}
	if(codenum>0)
	{
		//需要进行编码操作
		resTF=NCAONTRS_DEVICE_RS(sharegroupptr,cpytext,codeptr,cpylen);
		if(resTF==FALSE)
		{
			show_error("NAONTRS_DEVICE","NAONTRS_DEVICE_API","share group cannot be");
			return FALSE;
		}
	}

	//地址组空间
	resTF=NCAONTRS_DEVICE_SENSOR(addresslistptr);
	if(resTF==FALSE)
	{
		show_error("NCAONTRS_DEVICE","NCAONTRS_DEVICE_API","RTT values cannot be sensed");
		return FALSE;
	}


	//需要进行选择
	addrgroupptr=talloc(ADDRESSGROUP,1);
	if(addrgroupptr==NULL)
	{
		show_error("NCAONTRS_DEVICE","NCAONTRS_DEVICE_API","No memory");
		return FALSE;
	}
	addressptr=talloc(SHAREADDRESS,pronum);
	if(addressptr==NULL)
	{
		show_error("NCAONTRS_DEVICE","NCAONTRS_DEVICE_API","No memory");
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
	resTF=NCAONTRS_DEVICE_SENDSHAREGROUP(addrgroupptr,sharegroupptr);
	if(resTF==FALSE)
	{
		show_error("NCAONTRS_DEVICE","NCAONTRS_DEVICE_API","share cannot be send");
		return FALSE;
	}
	//发送地址
	resTF=NCAONTRS_DEVICE_SENDADDRESSLISTGROUP(addrgroupptr,addrgroupptr);
	if(resTF==FALSE)
	{
		show_error("NCAONTRS_DEVICE","NCAONTRS_DEVICE_API","address group cannot be send");
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