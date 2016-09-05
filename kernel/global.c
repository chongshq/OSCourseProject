
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
#include "file.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


PUBLIC	PROCESS	proc_table[NR_TASKS + NR_PROCS];

PUBLIC	TASK	task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TTY, "tty"}};
PUBLIC	TASK	user_proc_table[NR_PROCS] = {	{TestA, STACK_SIZE_TESTA, "TestA"}//,
						/*{TestB, STACK_SIZE_TESTB, "TestB"},
						{TestC, STACK_SIZE_TESTC, "TestC"}*/};

PUBLIC	char	task_stack[STACK_SIZE_TOTAL];

PUBLIC	TTY			tty_table[NR_CONSOLES];
PUBLIC	CONSOLE			console_table[NR_CONSOLES];

PUBLIC	t_pf_irq_handler	irq_table[NR_IRQ];

PUBLIC	t_sys_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_write};

/*-----------------File System-------------*/
/* ϵͳ�򿪱� */
PUBLIC	F_FILE	f_sys_opentbl[MAX_FILEOPEN];
/* �ļ�ϵͳ */
PUBLIC	F_SYS	f_sys;
/* �ļ�Ŀ¼ */
PUBLIC	F_DIR	f_dir;
/* ���б� */
PUBLIC	F_EPTBL	f_eptbl;

PUBLIC	t_fd	gfd;
