#pragma once
#include "Tree.h"
class FileSystem
{
private:
	//实现的命令
	string CMD[19] = {"EXIT", "CD","DIR","MKDIR","MORE","COPY","RMDIR","FIND","ATTRIB",
		"DEL","XCOPY","IMPORT","EXPORT","MOVE","HELP","TIME","VER","RENAME","ECHO"};
	//目录表，用于建立目录树
	Dentry *dir_link;
	//目录项数
	int dir_num;
	//目录队列
	queue <TreeNode*> q_dir;
public:
	//超级块
	SuperBlock super_block;
	//目录树
	Tree dir_tree;
	FileSystem();
	~FileSystem();
	//创建虚拟文件系统并初始化
	void init_file_system();
	//打开文件系统
	void open_file_system();
	//退出 CMD.EXE 程序(命令解释器)或当前批处理脚本。
	void exit();
	//写磁盘
	void write();
	//读磁盘
	void read();
	//将目录树存入队列
	void get_dir_tree(TreeNode *root);
	//得到指定父节点ID的目录项
	queue<Dentry> get_pid_d(int pid);
	//返回指定父节点ID的节点
	TreeNode* get_pid_node(int pid,TreeNode *root);
	//根据读入的目录表，创建目录树
	TreeNode *create_dir();
	//得到命令
	string get_cmd(string in);
	//得到参数
	string get_param(string in);
	//得到当前目录到输入目录的路径数组
	string *get_dir(string str,int &len);
	//得到根目录到输入目录的路径数组
	string *get_g_dir(string str,int &len);
	//得到命令号
	int get_cmd_id(string cmd);
	//更新超级块
	void updated_super_block();
	//显示当前目录
	void show_cur_dir();
	//显示当前目录名或改变当前目录。
	void cd(string param);
	//当前目录提示
	void cur_tip();
	//显示目录中的文件和子目录列表。
	void dir(string param);
	//创建目录
	void mkdir_or_md(string param);
	//逐屏显示输出。
	void more(string param);
	//将一份文件复制到另一个位置
	void copy(string source_goal);
	//删除一个目录。
	void rmdir_or_rd(string param);
	//在文件中搜索字符串。
	void find(string str_path);
	//显示或更改文件属性。
	void attrib(string param);
	//删除一个文件。
	void del(string param);
	//复制文件和目录树。
	void xcopy(string source_goal);
	//c:\a.txt .    将本地C盘下的a.txt导入到当前目录。
	void import(string filename);
	//a.txt c:\   将当前目录下的a.txt导出到本地C盘。
	void eexport(string param);
	//移动文件并重命名文件和目录。
	void move(string source_goal);
	//提供命令的帮助信息
	void help();
	//显示或设置系统时间。
	void _time();
	//显示 Windows 版本。
	void ver();
	//重命名文件。
	void rename(string old_new);
	//创建文件并写入内容
	void echo(string filename);
};

