
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"
#include "keyboard.h"

PUBLIC	PROCESS			proc_table[MAX_TASKS_PROCS];
PUBLIC	TASK	task_whole_list[] =	{//函数入口，进程名，是否任务，首tty，优先级
										{Newprocess,"Newpros", FALSE, 2, 0 },
										{task_tty,	"IO-Main", TRUE,  0, 15},//优先级最高
										{ProMain,	"ProMain", TRUE,  0, 10},
										{Zza,		"Anzzaer", FALSE, 1, 5 },
										{KillPro,	"KillPro", FALSE, 3, 10},
										{GraphicMode, "GraphicMode", FALSE, 2, 5}
									};

PUBLIC	int		task_proc_table[NR_TASKS_PROCS]	=	{1, 2};//专指IO，ProMain

PUBLIC	char			task_stack[STACK_SIZE_TOTAL];

PUBLIC	t_pf_irq_handler	irq_table[NR_IRQ];
PUBLIC	TTY			tty_table[NR_CONSOLES];
PUBLIC	CONSOLE			console_table[NR_CONSOLES];
PUBLIC t_8		_VGAMemCopy[0xC0000-0xA0000];

PUBLIC	t_sys_call		sys_call_table[NR_SYS_CALL] = {//系统调用函数表
														sys_get_ticks, 
														sys_write, 
														sys_getchar, 
														sys_clear, 
														sys_shutdown, 
														sys_echoon, 
														sys_echooff, 
														sys_accepton, 
														sys_acceptoff, 
														sys_flush,
														sys_dispmode
													   };

PUBLIC unsigned int _portlist[29] = {
	0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,
	0x3C2,0x3C4,0x3C4,0x3C4,0x3CE,
	0x3CE,0x3D4,0x3D4,0x3D4,0x3D4,
	0x3D4,0x3D4,0x3D4,0x3D4,0x3D4,
	0x3D4,0x3D4,0x3D4,0x3D4,0x3D4,
	0x3D4,0x3D4,0x3D4,0x3D4};

PUBLIC char _indexlist[29] = {
		0x10,0x11,0x12,0x13,0x14,
		0x00,0x01,0x03,0x04,0x05,
		0x06,0x00,0x01,0x02,0x03,
		0x04,0x05,0x06,0x07,0x08,
		0x09,0x10,0x11,0x12,0x13,
		0x14,0x15,0x16,0x17};

PUBLIC t_8 _VGAMemCopy[0xC0000-0xA0000];

PUBLIC int map[9][9];

PUBLIC	int Area[20][20];
PUBLIC	int Mask[20][20];
