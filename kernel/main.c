
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
							������غ���
*=====================================================================*/
int AddProcess(int TaskID);//���һ�����̵���
t_bool KillProcess(int Pid);//����pid�Ž���
void  KernelError();//����
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
			if (AddProcess(task_proc_table[i]) < 0)//�����������Ҫ������
			{
				KernelError();//ֻ����ʾ������Ϣ
				while (1);
			} 
		}
	}
	p_proc_ready = proc_table;//���ݽ��̱�
	
	k_reenter = 0;
	ticks = 0;
	init_clock();

	restart();


	while(1){}
}


/*======================================================================*
                               ������غ���
 *======================================================================*/
 int AddProcess(int TaskID)
{
	int			i;
	t_8			privilege;
	t_8			rpl;
	int			eflags;
	for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i ) 	//���forѭ���Ƕ�ÿһ�����������
	{
		if (proc_table[i].priority == 0)	//������̵����ȼ���0�������table��λ�ÿ����ã����˳�
		{					//���̱�Ķ�����proc.h
			break;
		}
	}

	if ( i == MAX_TASKS_PROCS ) return -1;		//���ﵽ�����Ľ�����
	if ( task_whole_list[TaskID].task_or_no == TRUE )//����������������ΪtaskID��ϵͳ����Ļ�
	{
		privilege = PRIVILEGE_TASK;
		rpl = RPL_TASK;
		eflags = 0x1202;
	}
	else						//����������������ΪtaskID�����û�����Ļ�
	{
		privilege = PRIVILEGE_USER;		
		rpl = RPL_USER;
		eflags = 0x202;
	}
// 	printf("TaskID:%d, ProcID:%d", TaskID, i);
	//����task_whole_list�е��������̣���ӽ��̱�
	strcpy(proc_table[i].p_name, task_whole_list[TaskID].name);	// ����name of the process
	proc_table[i].pid	= i;			// �������
	//ȫ��ģ�����ϴ���p235���������ڳ�ʼ�����̱�
	proc_table[i].ldt_sel	= SELECTOR_LDT_FIRST + 8 * i;//�޸�ldtѡ����
	memcpy(&proc_table[i].ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
	proc_table[i].ldts[0].attr1 = DA_C | privilege << 5;	// change the DPL
	memcpy(&proc_table[i].ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
	proc_table[i].ldts[1].attr1 = DA_DRW | privilege << 5;	// change the DPL
	//�������ڵ��������Ĵ���
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
	return i;//����ռ�ý��̱��еĵڼ���
}


/*======================================================================*/
void  KernelError()
{
	disp_str("Kernel error, boot failed!\n");//������ʾ������Ϣ
}


/*======================================================================*/
t_bool KillProcess(int Pid)		  //ȡ������
{
	if (proc_table[Pid].priority == 0)//ԭ�����ǿ�
	{
		return FALSE;
	}
	proc_table[Pid].priority = 0;
	proc_table[Pid].p_name[0] = 0;
	return TRUE;
}


/*======================================================================*
								�����庯��
*======================================================================*/
void KillPro()
{
	clear();	//����
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
		readacommand(szCmd);	//��ȡһ��ָ��
		strtolower(szCmd);	//ȫ��תΪСд����Ƚ�
		if (!strcmp(szCmd, "kill"))
		{
			printf("[Enter the pro-ID here]#");
			readacommand(szCmd_id);
			if (_atoi(szCmd_id,&id))
			{
				if (proc_table[id].task_or_no)	//�����ϵͳ����
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
				AddProcess(0);//0����Ϊ����͵�λ�ÿ�ʼ��
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
		//a�����¼�ĸ������,b�����¼�û�������ĸ���,flaga��ʾ�ĸ����������Щ�ѱ��³�,flagb��ʾ�û��ĸ�������Щ���� 
		char error; 
		//printf("���ڿ�ʼ������Ϸ,�밴��ʾ���������ĸ�0~9������.������10�λ���.\n"); 
		printf("Let's start, please input 4 integers 0~9 in turn, you have 10 times.\n"); 
		//srand((unsigned)time(NULL));//����ϵͳ��ǰʱ��������������� 
		for (i = 0; i < 4; i++) 
		{

			a[i]=(int)(get_ticks() * 3.1415926 / (i + 1)) % 10;

		} 

		for( num = 0; num < 10; num ++) 
	{ 
			x = 0; y = 0;//���ͳ������ 
		for (i = 0; i < 4; i++) 
		{ 
			flaga[i] = 0;//��ʾ���������δ���³� 
			flagb[i] = 0;//��ʾ���û����벻�� 
			b[i] = -1; 
		while( b[i] == -1 ) 
			{ 
				//printf("���������²�ĵ�%d����:", i+1);

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
		x++;//���ֶ���λ�öԵĸ���x 
		flaga[i] = 1;//��ǰ��������³� 
		flagb[i] = 1;//��ǰ�û�����������ȷ�� 
	} 
} 
for (i = 0; i < 4; i++) 
{
	if (flaga[i] == 1) continue;//�����ǰ������Ѿ����³�,�ͱȽ���һ������� 
	for (j = 0; j < 4; j++) 
	{ 
		if ( (flagb[j] == 0) && (a[i] == b[j]) ) 
		{ 
		y++;//���ֶԵ�λ�ò��Եĸ���y��1 
		flagb[j] = 1;//��ʾ��ǰ�����ǶԵ�. 
		break; 
		} 
	} 
} 
if (x == 4) 
{ 
//printf("��ϲ��,�¶���!"); 
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
//printf("��Ǹ,����ȫ�¶Ե���%d����,���ֶԶ�λ�ò��Ե���%d����\n", x, y);
printf("Sorry,there are %d exact right numbers,and %d wrong location but right numbers\n", x, y); 

if (num == 9) 
	{
//printf("�Բ���,��ʧ����."); 
printf("Sorry,you failed\n"); 
milli_delay(2000);
clear();
printf("\n");
printf("                        ==================================\n");
printf("                             Number puzzle (vesion1.0)\n");
printf("                        ==================================\n");
	}
else 
//printf("������%d�λ���.\n", 9-num);
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
	clear();	//����
	//��ʾ��ӭ��Ϣ
	printf("                        ==================================\n");
	printf("                                    virtual Linux 1++                     \n");
	printf("                                 JiongJiongYouShen\n");
	printf("                             kernel 1.0.0 on a i386\n\n");
	printf("                                    welcome !\n");
	printf("                        ==================================\n");
	do{
		printf("[root@localhost root]#");
		readacommand(szCmd);//��ȡһ��ָ��
		strtolower(szCmd);
		if (!strcmp(szCmd, "shutdown"))//�ر�ϵͳ
		{
			break;
		}
		else if (!strcmp(szCmd, "prosinfo"))//�鿴ϵͳ���������
		{
			ProcessManage();
		}
		else if (!strcmp(szCmd, "help"))
		{
			Help();
		}
		else if (!strcmp(szCmd, "runzza"))//����һ��������ʾ��������Ϸ
		{
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (!strcmp(proc_table[i].p_name, task_whole_list[3].name))//��Ϊ�̶��ذ�										         //runzza������task_whole_list��3����
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
		else if(!strcmp(szCmd, "taskmanager"))//���ý��̹������
		{
			int i;
			for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )
			{
				if (!strcmp(proc_table[i].p_name, task_whole_list[4].name))//��Ϊ�̶��ذ�										                                 //runzza������task_whole_list��4����
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
								����
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
								��ʾģʽ����
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
	//���̺ţ������������ȼ����Ƿ���ϵͳ���̣��Ƿ�������
	printf("-----------------------------------------------------------------------------\n");
	for ( i = 0 ; i < MAX_TASKS_PROCS ; ++i )//�������
	{
		if ( proc_table[i].priority == 0) continue;//ϵͳ��Դ����
	printf("        %d           %s            %d               %d            yes\n", proc_table[i].pid, proc_table[i].p_name, proc_table[i].priority, proc_table[i].task_or_no);
	}
	printf("=============================================================================\n");
}

/*=============================================================================*
								sys_shutdown//ģ��ػ�
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
							һЩ�ַ�������ĺ���
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
                               strcmp//�ַ����Ƿ�ƥ��
 *======================================================================*/
 int strcmp(char * src, char * dst)
{
	while ( *src && *dst && *src == *dst) { ++src, ++dst; };
	return *src - *dst;
}
/*=====================================================================
						//��������
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

