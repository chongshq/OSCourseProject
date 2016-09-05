
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
/*=====================================================================*
							进程相关函数
*=====================================================================*/
int AddProcess(int TaskID);//添加一个进程到表
t_bool KillProcess(int Pid);//结束pid号进程
void  KernelError();//错误
void KillPro();
void Help();
void ProcessManage();
void Zza();
void GraphicMode();
void Newprocess(int id);
void ProMain();
void sys_shutdown(PROCESS * p_proc);
void strtolower(char * src);
int strcmp(char * src, char * dst);
void DisPlay();
/*======================================================================*
                            yinix_main
 *======================================================================*/
PUBLIC int yinix_main()
{
	_VGAMode = 0x03;
	int i;
	disp_str("=====\"JiongJiongYouShen\" begins=====\n");
	for( i = 0 ; i < NR_TASKS_PROCS ; i++)
	{
		if ( i < NR_TASKS_PROCS )
		{
			if (AddProcess(task_proc_table[i]) < 0)//添加两个最主要的任务
			{
				KernelError();//只是显示错误信息
				while (1);
			} 
		}
	}
	p_proc_ready = proc_table;//传递进程表
	
	k_reenter = 0;
	ticks = 0;
	init_clock();

	restart();


	while(1){}
}


/*======================================================================*
                               进程相关函数
 *======================================================================*/
 int AddProcess(int TaskID)
{
	int			i;
	t_8			privilege;
	t_8			rpl;
	int			eflags;
	for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i ) 	//这个for循环是对每一个进程做检查
	{
		if (proc_table[i].priority == 0)	//如果进程的优先级是0（即这个table的位置可以用）则退出
		{					//进程表的定义在proc.h
			break;
		}
	}

	if ( i == MAX_TASKS_PROCS ) return -1;		//当达到了最大的进程数
	if ( task_whole_list[TaskID].task_or_no == TRUE )//如果在任务表里面编号为taskID是系统任务的话
	{
		privilege = PRIVILEGE_TASK;
		rpl = RPL_TASK;
		eflags = 0x1202;
	}
	else						//如果在任务表里面编号为taskID的是用户任务的话
	{
		privilege = PRIVILEGE_USER;		
		rpl = RPL_USER;
		eflags = 0x202;
	}
// 	printf("TaskID:%d, ProcID:%d", TaskID, i);
	//对照task_whole_list中的样例例程，添加进程表
	strcpy(proc_table[i].p_name, task_whole_list[TaskID].name);	// 拷贝name of the process
	proc_table[i].pid	= i;			// 进程序号
	//全部模拟书上代码p235，都是用于初始化进程表
	proc_table[i].ldt_sel	= SELECTOR_LDT_FIRST + 8 * i;//修改ldt选择子
	memcpy(&proc_table[i].ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
	proc_table[i].ldts[0].attr1 = DA_C | privilege << 5;	// change the DPL
	memcpy(&proc_table[i].ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
	proc_table[i].ldts[1].attr1 = DA_DRW | privilege << 5;	// change the DPL
	//以下用于调整各个寄存器
	proc_table[i].regs.cs	= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	proc_table[i].regs.ds	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	proc_table[i].regs.es	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	proc_table[i].regs.fs	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	proc_table[i].regs.ss	= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
	proc_table[i].regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
	proc_table[i].regs.eip	= (t_32)task_whole_list[TaskID].initial_eip;
	proc_table[i].regs.esp	= (t_32)task_stack + STACK_SIZE_TOTAL - STACK_SIZE_COMMON * i;
	proc_table[i].regs.eflags	= eflags;
	proc_table[i].nr_tty = task_whole_list[TaskID].defaultTTY;
	proc_table[i].ticks = proc_table[i].priority = task_whole_list[TaskID].priority;
	proc_table[i].task_or_no = task_whole_list[TaskID].task_or_no;
	return i;//返回占用进程表中的第几个
}


/*======================================================================*/
void  KernelError()
{
	disp_str("Kernel error, boot failed!\n");//仅仅显示错误消息
}


/*======================================================================*/
t_bool KillProcess(int Pid)		  //取消进程
{
	if (proc_table[Pid].priority == 0)//原来就是空
	{
		return FALSE;
	}
	proc_table[Pid].priority = 0;
	proc_table[Pid].p_name[0] = 0;
	return TRUE;
}


/*======================================================================*
								进程体函数
*======================================================================*/
void KillPro()
{
	clear();	//清屏
	printf("\n");
	printf("                        ==================================\n");
	printf("                             Task Manager (vesion1.0)\n");
	printf("                        ==================================\n");
	do
	{
		printf("[Task Manager(addnew/kill)]#");
		char szCmd[256];
		char szCmd_id[32];
		int  id;
		readacommand(szCmd);	//读取一条指令
		strtolower(szCmd);	//全部转为小写方便比较
		if (!strcmp(szCmd, "kill"))
		{
			printf("[Enter the pro-ID here]#");
			readacommand(szCmd_id);
			if (_atoi(szCmd_id,&id))
			{
				if (proc_table[id].task_or_no)	//如果是系统进程
				{
					printf("You can't kill system processes ! \n");
				}
				else
				{
					if(KillProcess(id)==TRUE)
						printf("kill success!\n");
					else
						printf("kill failed!\n");
				}
			}
			else
					printf("error...\n",id);
			
		}
		else if (!strcmp(szCmd, "addnew"))
		{
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (proc_table[i].priority == 0)
				{
					break;
				}
			}
			if ( i == MAX_TASKS_PROCS )printf("process list is full....\n");
			else
			{
				AddProcess(0);//0是因为从最低的位置开始改
				proc_table[i].priority=5;	
				printf("A new process is running on CONSOLE-2 now!\n");
			}
		}
		else if (!strcmp(szCmd, "clear"))
		{
			clear();
			printf("\n");
			printf("                        ==================================\n");
			printf("                             Task Manager (vesion1.0)\n");
			printf("                        ==================================\n");
		}
		else
		{
			printf("command not correct ...\n");
		}
		milli_delay(200);
	}
	while (1);
}


/*======================================================================*/
void Zza()
{
			clear();
			printf("\n");
			printf("                        ==================================\n");
			printf("                             Number puzzle (vesion1.0)\n");
			printf("                        ==================================\n");
	do
	{
		char temp;
		int temp1;
		int a[4], b[4], flaga[4], flagb[4], i, j,ii, x, y, num; 
		//a数组记录四个随机数,b数组记录用户输入的四个数,flaga表示四个随机数中哪些已被猜出,flagb表示用户四个输入哪些对了 
		char error; 
		//printf("现在开始猜数游戏,请按提示依次输入四个0~9的整数.您共有10次机会.\n"); 
		printf("Let's start, please input 4 integers 0~9 in turn, you have 10 times.\n"); 
		//srand((unsigned)time(NULL));//利用系统当前时间秒数做随机种子 
		for (i = 0; i < 4; i++) 
		{

			a[i]=(int)(get_ticks() * 3.1415926 / (i + 1)) % 10;

		} 

		for( num = 0; num < 10; num ++) 
	{ 
			x = 0; y = 0;//结果统计清零 
		for (i = 0; i < 4; i++) 
		{ 
			flaga[i] = 0;//表示该随机数尚未被猜出 
			flagb[i] = 0;//表示该用户输入不对 
			b[i] = -1; 
		while( b[i] == -1 ) 
			{ 
				//printf("请输入您猜测的第%d个数:", i+1);

				printf("please input the %d number:", i+1);
				readacommand(temp);

				_atoi(temp,&temp1);
				b[i] = temp1;
			////////////////////////////////////////////////////

				if (b[i] == -1) 
				{ 
					printf("the number is not an positive integer, please input again\n"); 
					readacommand(error);
				} 
			} 
	if (a[i]==b[i]) 
	{ 
		x++;//数字对且位置对的个数x 
		flaga[i] = 1;//当前随机数被猜出 
		flagb[i] = 1;//当前用户的输入是正确的 
	} 
} 
for (i = 0; i < 4; i++) 
{
	if (flaga[i] == 1) continue;//如果当前随机数已经被猜出,就比较下一个随机数 
	for (j = 0; j < 4; j++) 
	{ 
		if ( (flagb[j] == 0) && (a[i] == b[j]) ) 
		{ 
		y++;//数字对但位置不对的个数y加1 
		flagb[j] = 1;//表示当前输入是对的. 
		break; 
		} 
	} 
} 
if (x == 4) 
{ 
//printf("恭喜你,猜对了!"); 
printf("Congratulations! you are right!\n"); 
milli_delay(2000);
clear();
printf("\n");
printf("                        ==================================\n");
printf("                             Number puzzle (vesion1.0)\n");
printf("                        ==================================\n");
break; 
} 
else 
{ 
//printf("抱歉,您完全猜对的有%d个数,数字对而位置不对的有%d个数\n", x, y);
printf("Sorry,there are %d exact right numbers,and %d wrong location but right numbers\n", x, y); 

if (num == 9) 
	{
//printf("对不起,您失败了."); 
printf("Sorry,you failed\n"); 
milli_delay(2000);
clear();
printf("\n");
printf("                        ==================================\n");
printf("                             Number puzzle (vesion1.0)\n");
printf("                        ==================================\n");
	}
else 
//printf("您还有%d次机会.\n", 9-num);
printf("You still have %d times\n", 9-num);
printf("Tips: the answer is: ");
for(ii=0;ii<4;ii++)
	{
	printf(" <%d> ",a[ii]);
	}
printf("\n");
milli_delay(5000);
clear();
printf("\n");
printf("                        ==================================\n");
printf("                             Number puzzle (vesion1.0)\n");
printf("                        ==================================\n");
} 
} 
	}
	while (TRUE);
}


/*======================================================================*/
void Newprocess(int id)
{
	printf("\n");
	printf("A new process is running !");
	while(1);
}


/*======================================================================*/
void ProMain()
{
	/*===================================================================*
								
	*====================================================================*/
	DisPlay();
	char szCmd[256];
	int i;
	t_bool fIsRunning = TRUE;
	clear();	//清屏
	//显示欢迎信息
	printf("                        ==================================\n");
	printf("                                    virtual Linux 1++                     \n");
	printf("                                 JiongJiongYouShen\n");
	printf("                             kernel 1.0.0 on a i386\n\n");
	printf("                                    welcome !\n");
	printf("                        ==================================\n");
	do{
		printf("[root@localhost root]#");
		readacommand(szCmd);//读取一条指令
		strtolower(szCmd);
		if (!strcmp(szCmd, "shutdown"))//关闭系统
		{
			break;
		}
		else if (!strcmp(szCmd, "prosinfo"))//查看系统内运行情况
		{
			ProcessManage();
		}
		else if (!strcmp(szCmd, "help"))
		{
			Help();
		}
		else if (!strcmp(szCmd, "runzza"))//运行一个用于演示的数字游戏
		{
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (!strcmp(proc_table[i].p_name, task_whole_list[3].name))//因为固定地把										         //runzza放置在task_whole_list【3】里
				{
					printf("Zza is already running on CONSOLE-1 ! \n");
					break;
				}
			}
			if (i == MAX_TASKS_PROCS)
			{
				printf("Zza run success(on CONSOLE-1) \n", AddProcess(3));
			}			
		}
		else if(!strcmp(szCmd, "taskmanager"))//启用进程管理软件
		{
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (!strcmp(proc_table[i].p_name, task_whole_list[4].name))//因为固定地把										                                 //runzza放置在task_whole_list【4】里
				{
					printf("taskmanager is already running on CONSOLE-3 ! \n");
					break;
				}
			}
			if (i == MAX_TASKS_PROCS)
			{
				printf("taskmanager run success (on CONSOLE-3) \n", AddProcess(4));
			}	
		}
		else if (!strcmp(szCmd, "clear"))
		{
			clear();
			printf("                        ==================================\n");
			printf("                              virtual Linux 1             \n");
			printf("                             kernel 1.0.0 on a i386\n\n");
			printf("                                    welcome !\n");
			printf("                        ==================================\n");
		}
		else if (!strcmp(szCmd, "saojiong"))
		{
// 			dispmode(0x12, 0);
// 			break;
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (!strcmp(proc_table[i].p_name, task_whole_list[5].name))
				{
					printf("saojiong is already running on CONSOLE-2 ! \n");
					break;
				}
			}
			if (i == MAX_TASKS_PROCS)
			{
				int np = AddProcess(5);
				printf("saojiong Process is %d\n", np);
				printf("saojiong run success(on CONSOLE-2) \n");
			}
		}
		else
		{
			printf("command not found ...\n");
		}
		milli_delay(200);
	}while(TRUE);
	clear();
	printf("\n\n\n\n\n\n");
	printf("                        ==================================\n");
	printf("                              virtual Linux 1             \n");
	printf("                             kernel 1.0.0 on a i386\n\n");
	printf("                            your OS has been shutdown...\n");
	printf("                        ==================================\n");
	for ( i = 0 ; i < 20 ;  ++i )
	{
		milli_delay(100);
	}
	shutdown();
}

/*======================================================================*
								命令
*======================================================================*/
void Help()
{
	printf("=============================================================================\n");
	printf("Command list	:\n");
	printf("1. prosinfo      : A process manage,show you all process-info here\n");
	printf("2. clear         : Clear the screen\n");
	printf("3. shutdown      : Shut down the OS\n");
	printf("4. help          : Show this help message\n");
	printf("5. taskmanager   : Run a task manager,you can add or kill a process here\n");
	printf("6. runzza        : Run a small game of this OS\n");
	printf("7. Saojiong     :Run into Saojiong Game\n");
	printf("==============================================================================\n");		
}

/*======================================================================*
								显示模式调整
=======================================================================*/
void GraphicMode()
{
	while(1)
	{
		printf("\n");
		printf("Ready to New GraphicMode!\n");
		printf("Press ENTER to continue...\n");
		char cmd[64];
		readacommand(cmd);
		t_8* bm = (t_8*)0x7E00;
		int i = 0;
		int offset = 48;
 		dispmode(0x12, 0);
	}
}

/*======================================================================*/
void ProcessManage()
{
	int i;
	printf("=============================================================================\n");
	printf("      myID      |    name       | spriority    |  task_or_not   | running?\n");
	//进程号，进程名，优先级，是否是系统进程，是否在运行
	printf("-----------------------------------------------------------------------------\n");
	for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )//逐个遍历
	{
		if ( proc_table[i].priority == 0) continue;//系统资源跳过
	printf("        %d           %s            %d               %d            yes\n", proc_table[i].pid, proc_table[i].p_name, proc_table[i].priority, proc_table[i].task_or_no);
	}
	printf("=============================================================================\n");
}

/*=============================================================================*
								sys_shutdown//模拟关机
===============================================================================*/
void sys_shutdown(PROCESS * p_proc)
{
	int i;
	disable_int();
	for ( i = 0 ; i < 16 ; ++i )
	{
		disable_irq(i);
	}
	while(1) {}
}
/*======================================================================*
							一些字符串方面的函数
 *======================================================================*/



/*======================================================================*
                               strtolower
 *======================================================================*/
void strtolower(char * src)
{
	while (*src)
	{
		if ((*src >= 'A') && (*src <= 'Z'))
		{
			*src += 'a' - 'A';
		}
		++src;
	}
}


/*======================================================================*
                               strcmp//字符串是否匹配
 *======================================================================*/
 int strcmp(char * src, char * dst)
{
	while ( *src && *dst && *src == *dst) { ++src, ++dst; };
	return *src - *dst;
}
/*=====================================================================
						//开机界面
======================================================================*/
void DisPlay()
{
	int color = 0x7f;
	clear();
// 	while(1);
	printf("\n\n\n\n\n\n\n\n\n");
	printf("                          Welcome to the YINIX \n");
	printf("\n\n\n\n\n\n\n\n\n");
	printf("                          Welcome to the YINIX \n");
	milli_delay(1000);
}

