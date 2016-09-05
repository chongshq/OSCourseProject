#ifndef file_h__
#define file_h__

//#define __DB_ON_WIN_	// ��windows�µ���ʱʹ��

/***************�豸��������*****************/
// �趨������Ϊ16MB
#define	BLOCK_TOTAL	32768	// �����ܿ���
#define BLOCK_SIZE	512	// һ����Ĵ�С���ֽ�

/***************�ļ�ϵͳ��������***************/
// �ļ�ϵͳ����ļ������ļ�Ŀ¼���ڵ�����
#define MAX_ENTRY	32

// ���п���������
#define MAX_EPTITM	128

// ϵͳ�򿪱�������������һ���ܹ����ļ�����������
#define MAX_FILEOPEN	1

// FCB�У����Զεĳ��ȡ��ܿ���ռ��1���ֽڡ�
#define F_NAMELEN	8		// �ļ���
#define F_EXTLEN	3		// ��չ��
//#define F_BLKLEN	2		// ���
#define F_RESRV		2		// ����

#define MAX_BKCUNT	255		// ������

/***************��������(���ݽṹ)����***************/

//typedef short	t_fd;			// �ļ�������
//typedef	t_16	t_blknum;		// ���

// �ļ�Ŀ¼�FCB��, 16B
typedef struct s_fcb
{
	t_8		name[F_NAMELEN];	// �ļ���
	t_8		ext[F_EXTLEN];		// �ļ���չ��
	t_8		blkcount;			// �ܿ���
	t_16	initNo;				// ��ʼ���
	t_8		resrv[F_RESRV];		// ����
} F_FCB;

// �ļ�Ŀ¼
typedef struct s_dir
{
	F_FCB	fcb[MAX_ENTRY];	// Ŀ¼��
	t_8		count;				// �����ļ�����
} F_DIR;

// ���б���
typedef struct s_ept_item
{
	t_blknum	start;		// ��ʼ��
	t_blknum	size;		// ����
} F_EPTITM;

// ���б�
typedef struct s_eptbl
{
	F_EPTITM	item[MAX_EPTITM];
	t_blknum	count;
} F_EPTBL;

// ϵͳ�ļ��򿪱���
typedef struct s_File
{
	int		id;				// Ŀ¼����ţ���0��ʼ���
	F_FCB	fcb;			// Ŀ¼����
	t_16	offset;			// ���ֽ�Ϊ��λ�Ķ�дָ��
	//int		flag;			// fcb��Ϣ�Ƿ��޸�
} F_FILE;

// �ļ�ϵͳ��Ϣ�������飩
typedef struct s_filesys
{
	int		blk_total;		// �ļ�����ܿ���
	int		blk_dir;		// �ļ�Ŀ¼�Ŀ���
	int		blk_epttbl;		// ���п��Ŀ���
	int		blk_ept;		// ���п�����
	void*	deviceinfo;		//�豸��Ϣ
} F_SYS;

#endif // file_h__
