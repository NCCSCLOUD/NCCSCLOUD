#include "NAONTRS_CLOUD.h"
#include "NAONTRS_DEVICE.h"
#include "NAONTRS.h"
#include "NAONTRS_test.h"
//这里给一个Ali云的例子，并且测试部分只给出DEVICE部分，CLOUD和DEVICE一样给个地址直接调API即可
//Azure可以把IP换一下，之前我用了Azure.cn

int NAONTRS_test_set_address(SHAREADDRESS *addptr,int n)
{
	//IP地址是阿里云的
	//IP address is Ali cloud, Azure have also been tested, 
	//however VMs of Azure are a little bit expensive to clear the bugs..,7*RMB 699=4890 for 4G memory VM
	//Ali is RMB 230*7=1610...so we use Ali clouds to debug and azure to test the performance
	//由于是本地路径因此全部设定为D盘
	SHAREADDRESS *curaddptr;
	curaddptr=addptr;
	switch(n)
	{
	case 4:
		mcpy(curaddptr->storepath,"D:\\s1.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.103.187",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s2.txt",char,10);
		mcpy(curaddptr->IPaddress,"121.196.233.10",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s3.txt",char,10);
		mcpy(curaddptr->IPaddress,"120.27.242.38",char,14);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s4.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.112",char,15);
		curaddptr++;
		break;
	case 5:
		mcpy(curaddptr->storepath,"D:\\s1.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.103.187",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s2.txt",char,10);
		mcpy(curaddptr->IPaddress,"121.196.233.10",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s3.txt",char,10);
		mcpy(curaddptr->IPaddress,"120.27.242.38",char,14);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s4.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.112",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s5.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.199",char,15);
		curaddptr++;
		break;
	case 6:
		mcpy(curaddptr->storepath,"D:\\s1.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.103.187",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s2.txt",char,10);
		mcpy(curaddptr->IPaddress,"121.196.233.10",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s3.txt",char,10);
		mcpy(curaddptr->IPaddress,"120.27.242.38",char,14);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s4.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.112",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s5.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.199",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s6.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.106.4",char,13);
		curaddptr++;
		break;
	case 7:
		mcpy(curaddptr->storepath,"D:\\s1.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.103.187",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s2.txt",char,10);
		mcpy(curaddptr->IPaddress,"121.196.233.10",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s3.txt",char,10);
		mcpy(curaddptr->IPaddress,"120.27.242.38",char,14);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s4.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.112",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s5.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.149.199",char,15);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s6.txt",char,10);
		mcpy(curaddptr->IPaddress,"114.55.106.4",char,13);
		curaddptr++;
		mcpy(curaddptr->storepath,"D:\\s7.txt",char,10);
		mcpy(curaddptr->IPaddress,"118.178.20.88",char,15);
		curaddptr++;
		break;
	}
	return TRUE;
}


int NAONTRS_test()
{
	int i,k,d,n,r,itemindex,randnum,strlen,resTF;
	char *tracepath;//记录一下轨迹的路径
	FILE *fin,*fout;//用于输入trace，输出结果
	CODEPARA *codeptr;
	BYTE *content;//需要加密的内容
	ADDRESSLIST *addlistptr;
	SHAREADDRESS *addptr,*curaddptr;
	TRACEITEM *itemptr;
	int starttick,endtick;
	double sectime;

	content=talloc(BYTE,chksize_MB(100));//开100MB空间希望不会有比他大的
	if(content==NULL)
	{
		show_error("NAONTRS","NAONTRS_test","No memory");
		return FALSE;
	}
	codeptr=talloc(CODEPARA,1);
	if(codeptr==NULL)
	{
		show_error("NAONTRS","NAONTRS_test","No memory");
		return FALSE;
	}
	addptr=talloc(SHAREADDRESS,7);
	curaddptr=addptr;
	for(i=0;i<7;i++)
	{
		curaddptr->storepath=talloc(char,PATHlen);
		if(curaddptr->storepath==NULL)
		{
			show_error("NAONTRS","NAONTRS_test","No memory");
			return FALSE;
		}
		curaddptr->IPaddress=talloc(char,IPlen);
		if(curaddptr->IPaddress==NULL)
		{
			show_error("NAONTRS","NAONTRS_test","No memory");
			return FALSE;
		}
		curaddptr->port=TRANPORT;
		curaddptr->RRTvalue=0;
		curaddptr++;
	}
	addlistptr=talloc(ADDRESSLIST,1);
	if(addlistptr==NULL)
	{
		show_error("NAONTRS","NAONTRS_test","No memory");
		return FALSE;
	}
	itemptr=talloc(TRACEITEM,1);
	if(itemptr==NULL)
	{
		show_error("NAONTRS","NAONTRS_test","No memory");
		return FALSE;
	}
	addlistptr->addressptr=addptr;
	addlistptr->codeptr=codeptr;
	memset(content,1,chksize_MB(100));
	k=4;
	fout=fopen(".\\NAONT-RSVM1.txt","w");
	if(fout==NULL)
	{
		show_error("NAONTRS","NAONTRS_test","output file cannot open");
		return FALSE;
	}
	for(r=0;r<10;r++)//10轮
	{
		for(i=0;i<7;i++)
		{
			switch(i)
			{
			case 0://轨迹放在本地
				tracepath=".\\usr.txt";
				//linenum=
				//strlen=chksize_KB(64);
				break;
			case 1:
				tracepath=".\\web.txt";
				//strlen=chksize_KB(128);
				break;
			case 2:
				tracepath=".\\proj.txt";
				//strlen=chksize_KB(256);
				break;
			case 3:
				tracepath=".\\ts.txt";
				//strlen=chksize_KB(512);
				break;
			case 4:
				tracepath=".\\rsrch.txt";
				//strlen=chksize_MB(1);
				break;
			case 5:
				tracepath=".\\mds.txt";
				break;
			case 6:
				tracepath=".\\hm.txt";
				break;
			}
			fin=fopen(tracepath,"r");
			if(fin==NULL)
			{
				show_error("NAONTRS","NAONTRS_test","trace cannot open");
				return FALSE;
			}
			k=4;
			d=4;
			n=7;
			for(n=4;n<7;n++)
			{
				//填写所有参数
				codeptr->totnum=n;
				codeptr->pronum=d;
				codeptr->datanum=k;
				codeptr->codenum=0;
				codeptr->w=8;
				NAONTRS_test_set_address(addptr,n);
				//while(!feof(fin))太多了
				randnum=rand()%1000;
				for(itemindex=0;itemindex<randnum;itemindex++)
				{
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("NAONTRS","NAONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
				}
				for(itemindex=0;itemindex<100;itemindex++)
				{
					
					starttick=clock();
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("NAONTRS","NAONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
					strlen=itemptr->length;
					NAONTRS_DEVICE_API(content,strlen,addlistptr);
					endtick=clock();
					sectime=profile_calculate_time_diff(starttick,endtick);
					fprintf(fout,"%f\n",sectime);
				}
				fprintf(fout,"(%d,%d),%d,%d\n",k,k,i,r);
				printf("(%d,%d),%d,%d\n",k,k,i,r);
			}
			k=4;
			d=4;
			n=7;
			for(k=4;k<=7;k++)
			{
				//填写所有参数
				codeptr->totnum=n;
				codeptr->datanum=k;
				codeptr->codenum=0;
				codeptr->w=8;
				NAONTRS_test_set_address(addptr,n);
				randnum=rand()%1000;
				for(itemindex=0;itemindex<randnum;itemindex++)
				{
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("AONTRS","AONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
				}
				//while(!feof(fin))
				for(itemindex=0;itemindex<100;itemindex++)
				{
					starttick=clock();
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("AONTRS","AONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
					strlen=itemptr->length;
					NAONTRS_DEVICE_API(content,strlen,addlistptr);
					endtick=clock();
					sectime=profile_calculate_time_diff(starttick,endtick);
					fprintf(fout,"%f\n",sectime);
				}
				fprintf(fout,"(%d,%d),%d,%d\n",k,k,i,r);
				printf("(%d,%d),%d,%d\n",k,k,i,r);
			}
			k=4;
			d=4;
			n=7;
			for(d=4;d<=7;d++)
			{
				//填写所有参数
				codeptr->totnum=n;
				codeptr->datanum=k;
				codeptr->pronum=d;
				codeptr->codenum=n-k;
				codeptr->w=8;
				NAONTRS_test_set_address(addptr,n);
				randnum=rand()%1000;
				for(itemindex=0;itemindex<randnum;itemindex++)
				{
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("AONTRS","AONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
				}
				//while(!feof(fin))
				for(itemindex=0;itemindex<100;itemindex++)
				{
					starttick=clock();
					resTF=trace_write_IO(fin,itemptr);
					if(resTF==FALSE)
					{
						show_error("AONTRS","AONTRS_test","the traceitem cannot be read");
						return FALSE;
					}
					strlen=itemptr->length;
					NAONTRS_DEVICE_API(content,strlen,addlistptr);
					endtick=clock();
					sectime=profile_calculate_time_diff(starttick,endtick);
					fprintf(fout,"%f\n",sectime);
				}
				fprintf(fout,"(%d,%d),%d,%d\n",n,k,i,r);
				printf("(%d,%d),%d,%d\n",n,k,i,r);
			}
			fclose(fin);
		}
		printf("round %d",r);
	}
	fclose(fout);
	return TRUE;
}