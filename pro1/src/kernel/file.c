#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "file.h"
#include "global.h"
#include "proto.h"

#ifdef __DB_ON_WIN_
	#include <stdio.h>
#endif

// 私有函数
PRIVATE t_blknum	__bmap(t_fd fd);
PRIVATE t_bool		__fchkname(char* name);
PRIVATE void		__clearFILE(t_fd fd);
PRIVATE int			__rdwr(int flag, t_fd fd, char* buf, int count);
PRIVATE t_bool		__same_file_name(char* name1, char* name2);
PRIVATE void		__set_name(char* dest, char* src);

#define F_NAME_NOT_EXIST	-1	// 文件名不存在
#define F_BLOCK_NULL		-1	// 查找块错误
#define F_FILE_NULL			-1	// 查找文件错误/空文件
#define F_FLAG_READ			0	// 读
#define F_FLAG_WRITE		1	// 写

//-------------------------初始化-------------------------
PUBLIC void InitFileSys()
{
	int i = 0;
	// 读取超级块信息
	f_sys.blk_total = BLOCK_TOTAL;  // 总块数 16MB / 512B
	f_sys.blk_dir = 0;				// 目录占用块数
	f_sys.blk_epttbl = 0;			// 空闲表占用块数
	f_sys.blk_ept = f_sys.blk_total
					- f_sys.blk_dir
					- f_sys.blk_epttbl
					- 1;	// 空闲块总数
	f_sys.deviceinfo = 0;	// 设备信息

	// 构造目录块
	f_dir.count = 0;

	// 构造空闲表
	f_eptbl.count = 1;
	f_eptbl.item[0].start = 1 + f_sys.blk_dir + f_sys.blk_epttbl;
	f_eptbl.item[0].start = f_sys.blk_ept;

	// 文件打开表
	for (i =0;i < MAX_FILEOPEN; i++)
	{
		f_sys_opentbl[i].id = F_FILE_NULL;
	}

	//printf("File System Initialized.\n");
	disp_str("File System Initialized.\n");
}

//-------------------------私有函数-------------------------
/**************************
 * 调用：bmap(t_fd fd)，fd 为文件描述符（文件打开表项的索引）
 * 返回：物理块号。比较是否超过了总块数返回-1
 * 过程：计算文件读写指针在文件内的块号（=off/块大小）
 ***/
PRIVATE t_blknum __bmap(t_fd fd)
{
	t_blknum blocknum;
	if (fd >= MAX_FILEOPEN)
		return F_BLOCK_NULL;
	
	blocknum = f_sys_opentbl[fd].offset / BLOCK_SIZE;

	// 检查是否超过总块数
	if (blocknum >= f_sys_opentbl[fd].fcb.blkcount)
		return F_BLOCK_NULL;

	blocknum = blocknum + f_sys_opentbl[fd].fcb.initNo;
	return blocknum;
}

/**************************
 * 调用：fchkname(char* name)
 * 返回：如果存在，返回ID，如果不存在，返回-1.
 * 过程：查找目录表，看是否存在要查找的文件名
 ***/
PRIVATE int __fchkname(char* name)
{
	// 到磁盘检查名称
	int i =0;
	for (i =0; i < f_dir.count; i++)
	{
		if(__same_file_name(f_dir.fcb[i].name, name))
			return i;
	}
	return F_NAME_NOT_EXIST;	// 名称不存在
}

PRIVATE t_bool __same_file_name(char* name1, char* name2)
{
	// 检查新名称与旧名称是否相同，同返回TURE
	int i = 0;
	for (i = 0; i<F_NAMELEN; i++)
	{
		if (name1[i]==0 && name2[i]==0)
			return TRUE;

		if (name1[i] != name2[i])
		{			
			return FALSE;	// 不相同
		}
	}
	return TRUE;
}

PRIVATE void __set_name(char* dest, char* src)
{
	int i =0;
	for (i = 0; i < F_NAMELEN; i++)
	{
		dest[i] = src[i];
	}
}

PRIVATE void __clearFILE(t_fd fd)
{
	f_sys_opentbl[fd].id = F_FILE_NULL;
}

PRIVATE int __rdwr(int flag, t_fd fd, char* buf, int count)
{
	int trans_count = 0;
	t_blknum blknum;
	if (fd >= MAX_FILEOPEN)
	{
		printf("The file dose not exist.\n");
		return 0;
	}

	blknum = __bmap(fd);	// 获得块号
	if (blknum == F_BLOCK_NULL)
	{
		printf("End of File.\n");
		return 0;
	}

	if (flag == F_FLAG_READ)
	{
		//读
		if(f_sys_opentbl[fd].offset + count >= f_sys_opentbl[fd].fcb.blkcount * BLOCK_SIZE)
			count = f_sys_opentbl[fd].fcb.blkcount * BLOCK_SIZE - f_sys_opentbl[fd].offset;
		if (count == 0)
			return 0;
			
		readMD(buf, blknum, f_sys_opentbl[fd].offset, count);

		if (strlen(buf)!=0)
			f_sys_opentbl[fd].offset = f_sys_opentbl[fd].offset + count;
			
		return strlen(buf);
	}
	else
	{
		// 超过总块数，则部分写入
		count = strlen(buf);
		if(count > f_sys_opentbl[fd].fcb.blkcount*BLOCK_SIZE-f_sys_opentbl[fd].offset)
		{
			count = f_sys_opentbl[fd].fcb.blkcount * BLOCK_SIZE - f_sys_opentbl[fd].offset;
		}
		//写
		//writeMD(int blocknum, void * src, int size);
		writeMD(blknum, buf, f_sys_opentbl[fd].offset, count);
		f_sys_opentbl[fd].offset = f_sys_opentbl[fd].offset + count;
		return strlen(buf);
	}
	return 0;
}
//--------------------------系统调用--------------------------
PUBLIC t_fd sys_fcreate(char* name, int blocknum)
{
	int i=0, j=0;
	t_blknum start=0;
	if (__fchkname(name)!=F_NAME_NOT_EXIST)
	{
		printf("The filename has already been used.\n");
		return F_FILE_NULL;
	}

	// 创建文件
	// 检查空闲块表，分配空间并清空（并且修改块表）
	for (i =0; i < f_eptbl.count; i++)
	{
		if (f_eptbl.item[i].size <= blocknum)
		{
			start = f_eptbl.item[i].start;
			f_eptbl.item[i].size = f_eptbl.item[i].size - blocknum;

			if (f_eptbl.item[i].size == 0 )
			{
				// 删除空闲表项
				for (j=i; j < f_eptbl.count-1; j++)
				{
					f_eptbl.item[j] = f_eptbl.item[j+1];
					f_eptbl.count--;
				}
			}
		}
	}
	
	// 增加一个目录项
	f_dir.fcb[f_dir.count].blkcount = blocknum;
	f_dir.fcb[f_dir.count].initNo = start;
	__set_name(f_dir.fcb[f_dir.count].name, name);
	f_dir.count++;

	printf("A new file has been created.\n");

	return sys_fopen(name);
}

PUBLIC t_fd sys_fopen(char* name)
{
	int i=0, id=F_FILE_NULL;

	id = __fchkname(name);
	if (id==F_NAME_NOT_EXIST)
	{
		printf("The file dose not exist.\n");
		return F_FILE_NULL;
	}

	for (i =0; i < MAX_FILEOPEN; i++)
	{
		if (f_sys_opentbl[i].id == F_FILE_NULL)
		{
			// 给f_sys_opentbl[i]赋值
			f_sys_opentbl[i].id = id;
			f_sys_opentbl[i].offset = 0;

			// 从磁盘拷贝FCB，位置可以根据id计算
			f_sys_opentbl[i].fcb = f_dir.fcb[id];
			__set_name(f_sys_opentbl[i].fcb.name, f_dir.fcb[id].name);

			printf("File opened.\n");
			return i;
		}
	}

	printf("Too many files have been opened. Please close first.\n");
	return F_FILE_NULL;
}

PUBLIC void sys_fclose(t_fd fd)
{
	if (fd >= MAX_FILEOPEN)
	{
		printf("The file does not exist.\n");
		return;
	}
	__clearFILE(fd);
	printf("File #%x closed.\n", fd);
}

PUBLIC void sys_fdelete(char* name)
{
	int i = 0, id=0;
	// 检查文件是否打开
	for (i = 0; i < MAX_FILEOPEN; i++)
	{
		if (__same_file_name(f_sys_opentbl[i].fcb.name, name) && f_sys_opentbl[i].id != F_FILE_NULL)
		{
			printf("The file is currently occupied.\n");
			return;
		}
	}

	id = __fchkname(name);
	if (id == F_NAME_NOT_EXIST)
	{
		printf("The file does not exist.\n");
		return;
	}
	// 删除

	// 增加空闲表项
	f_eptbl.item[f_eptbl.count].size = f_dir.fcb[id].blkcount;
	f_eptbl.item[f_eptbl.count].start = f_dir.fcb[id].initNo;
	f_eptbl.count++;

	f_sys.blk_ept += f_dir.fcb[id].blkcount;

	// 合并...

	// 删除目录项
	for (i=id; i<f_dir.count-1;i++)
	{
		f_dir.fcb[i] = f_dir.fcb[i+1];
		__set_name(f_dir.fcb[i].name, f_dir.fcb[i+1].name);
		f_dir.count--;
	}

	printf("File deleted.\n");
	return;
}

PUBLIC int sys_fread(t_fd fd, char* buf, int count)
{
	return __rdwr(F_FLAG_READ, fd, buf, count);
}

PUBLIC int sys_fwrite(t_fd fd, char* buf, int count)
{
	return __rdwr(F_FLAG_WRITE, fd, buf, count);
}

/**************************
 * 调用：frename(char* old_name, char* new_name)
 * 过程：如果old<>new，则调用name()检查新名称，如果存在则错误，如果不存在则改正
 ***/
PUBLIC void sys_frename(char* old_name, char* new_name)
{
	int i = 0, id = F_FILE_NULL;
	// 检查文件是否打开
	for (i = 0; i < MAX_FILEOPEN; i++)
	{
		if (__same_file_name(f_sys_opentbl[i].fcb.name, old_name) && f_sys_opentbl[i].id != F_FILE_NULL)
		{
			printf("The file is currently occupied.\n");
			return;
		}
	}

	// 检查新名称与旧名称是否相同
	if(__same_file_name(old_name, new_name))
		return;
	
	// 判断新名称是否存在
	if(__fchkname(new_name)!=F_NAME_NOT_EXIST)
	{
		printf("This name has already been used.\n");
		return;	
	}


	// 如果旧名称是否存在
	id = __fchkname(old_name);
	if (id == F_NAME_NOT_EXIST)
	{
		printf("The file does not exist.\n");
		return;	
	}

	// 在磁盘上的文件目录改名
	__set_name(f_dir.fcb[id].name, new_name);
	printf("A file has been renamed.\n");
}

PUBLIC void sys_fskbegin(t_fd fd)
{
	if (fd >= MAX_FILEOPEN)
		printf("The file dose not exist.\n");
	else
		f_sys_opentbl[fd].offset = 0;
}

PUBLIC void sys_flist()
{
	int i =0;
	//printf("-----FILE SYSTEM Infomation-----\n");
	for (i=0; i< f_dir.count; i++)
	{
		printf("  - ");
		printf(f_dir.fcb[i].name);
		printf("\n");
		//printf("%x\n", "400");
	}
	//printf("-----END-----\n\n");
}
