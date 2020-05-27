#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include<string>
#include <ctime>
#include <fstream>
#include <queue>
#include <sstream>
using namespace std;
//磁盘大小 1M 
#define DISK_SIZE 1024*1024
//数据块大小 B
#define DATA_SIZE 1024
//文件最大 KB
#define FILE_MAX_SIZE 12
//磁盘名
#define DISK_NAME "disk.dat"
//超级块
struct SuperBlock {
	//块的大小
	int s_block_size;
	//块的数目
	int s_block_count;
	//保留的块数量
	int s_reserve_count;
	//空闲块数量
	int s_free_count;
	//第一个数据块
	int s_first_data_block;
	//索引节点数
	int s_inode_count;
	//空闲的节点数
	int s_free_inode_count;
};
//索引节点,每1KB设置一个，共1024
struct Inode {
	//文件模式,0表示只读，1表示读写
	char i_mode;
	//文件大小，单位是 byte,
	int i_size;
	//文件创建时间
	time_t i_ctime;
	//文件访问时间
	time_t i_atime;
	//文件修改时间
	time_t i_mtime;
	//只支持直接索引，文件最大12KB
	int i_address[12];
	//索引节点是否可用,0可用，1不可用
	char i_flag;
};
//目录项
struct Dentry {
	//索引节点号
	int d_inode;
	//文件名
	string d_file_name;
	//文件类型，0表示普通文件，1表示目录文件
	char d_file_type;
	//父节点号--存储父亲节点号，便于写入文件和目录树的创建
	int d_parent_id;
};
//目录树节点
struct TreeNode {
	//数据域
	Dentry data;
	//第一个子节点
	TreeNode *child;
	//同层其他节点
	TreeNode *next;
	//父节点
	TreeNode *parent;
};
//文件内容
struct FileStr {
	string str;
};
//构建目录树
class Tree
{
private:
	//根节点
	TreeNode *root;
	//当前目录
	TreeNode *current;
	//时间
	time_t t;
	//索引节点号
	int inode_id;
	//可分配块位置
	int pos;
public:
	//索引表
	Inode inode_table[1024];
	//初始化树
	Tree();
	//销毁树
	~Tree();
	//设置索引节点号
	void set_inode_id(int inode);
	//返回索引节点号
	int get_inode_id();
	//返回根目录
	TreeNode *get_root();
	//设置根目录
	void set_root(TreeNode *new_root);
	//返回可分配块位置
	int get_free_pos();
	//设置可分配块位置
	void set_free_pos(int pos);
	//创建目录
	bool md_dir(string *dirs_name, int dirs_len, int type);
	//显示目录中的文件和子目录列表
	bool show_dir(string *dirs_name,int dirs_len);
	//目录或文件是否存在
	bool exist_dir(string *dirs_name, int dirs_len);
	//得到上一级目录
	 Dentry get_pre_dir();
	 //得到当前目录
	 Dentry get_cur_dir();
	 //得到当前目录数组--即路径
	 Dentry *get_cur_q(int &size);
	 //得到当前目录的string数组
	 string *get_cur_s(int &size);
	 //得到指定目录节点
	 TreeNode *get_dir_node(string * dirs_name, int dirs_len);
	 //得到指定索引节点号的文件或目录信息
	 Inode get_inode(int id);
	//打开指定目录
	bool cd_dir(string *dirs_name,int dirs_len);
	//复制目录
	bool cp_dir(string *source_dir,string *goal_dir, int source_len,int goal_len);
	//删除空目录
	bool rm_dir(string *dirs_name, int dirs_len);
	//前序遍历目录树--用于测试
	void print(TreeNode *parent);
	//复制子树
	TreeNode *copy(TreeNode *parent,int pid);
	//输出指定文件的详细信息
	void print_dir(TreeNode *node);
	//移动并重命名文件或目录
	bool move_dir(string *source_dir,string *goal_dir,int s_len,int g_len);
	//复制指定节点号的内容
	void copy_str(int s_id,int g_pos);
};

