#ifndef file_h__
#define file_h__

//#define __DB_ON_WIN_	// 在windows下调试时使用

/***************设备常量定义*****************/
// 设定：磁盘为16MB
#define	BLOCK_TOTAL	32768	// 磁盘总块数
#define BLOCK_SIZE	512	// 一个块的大小，字节

/***************文件系统常量定义***************/
// 文件系统最大文件数（文件目录最大节点数）
#define MAX_ENTRY	32

// 空闲块表最大项数
#define MAX_EPTITM	128

// 系统打开表项最大个数。（一次能够打开文件的最大个数）
#define MAX_FILEOPEN	1

// FCB中，各自段的长度。总块数占用1个字节。
#define F_NAMELEN	8		// 文件名
#define F_EXTLEN	3		// 扩展名
//#define F_BLKLEN	2		// 块号
#define F_RESRV		2		// 保留

#define MAX_BKCUNT	255		// 最大块数

/***************数据类型(数据结构)定义***************/

//typedef short	t_fd;			// 文件描述符
//typedef	t_16	t_blknum;		// 块号

// 文件目录项（FCB）, 16B
typedef struct s_fcb
{
	t_8		name[F_NAMELEN];	// 文件名
	t_8		ext[F_EXTLEN];		// 文件扩展名
	t_8		blkcount;			// 总块数
	t_16	initNo;				// 起始块号
	t_8		resrv[F_RESRV];		// 保留
} F_FCB;

// 文件目录
typedef struct s_dir
{
	F_FCB	fcb[MAX_ENTRY];	// 目录项
	t_8		count;				// 已有文件个数
} F_DIR;

// 空闲表项
typedef struct s_ept_item
{
	t_blknum	start;		// 起始块
	t_blknum	size;		// 长度
} F_EPTITM;

// 空闲表
typedef struct s_eptbl
{
	F_EPTITM	item[MAX_EPTITM];
	t_blknum	count;
} F_EPTBL;

// 系统文件打开表项
typedef struct s_File
{
	int		id;				// 目录表项号，从0开始编号
	F_FCB	fcb;			// 目录表项
	t_16	offset;			// 以字节为单位的读写指针
	//int		flag;			// fcb信息是否被修改
} F_FILE;

// 文件系统信息（超级块）
typedef struct s_filesys
{
	int		blk_total;		// 文件卷的总块数
	int		blk_dir;		// 文件目录的块数
	int		blk_epttbl;		// 空闲块表的块数
	int		blk_ept;		// 空闲块总数
	void*	deviceinfo;		//设备信息
} F_SYS;

#endif // file_h__
