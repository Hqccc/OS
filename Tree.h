#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include<string>
#include <ctime>
#include <fstream>
#include <queue>
#include <sstream>
using namespace std;
//���̴�С 1M 
#define DISK_SIZE 1024*1024
//���ݿ��С B
#define DATA_SIZE 1024
//�ļ���� KB
#define FILE_MAX_SIZE 12
//������
#define DISK_NAME "disk.dat"
//������
struct SuperBlock {
	//��Ĵ�С
	int s_block_size;
	//�����Ŀ
	int s_block_count;
	//�����Ŀ�����
	int s_reserve_count;
	//���п�����
	int s_free_count;
	//��һ�����ݿ�
	int s_first_data_block;
	//�����ڵ���
	int s_inode_count;
	//���еĽڵ���
	int s_free_inode_count;
};
//�����ڵ�,ÿ1KB����һ������1024
struct Inode {
	//�ļ�ģʽ,0��ʾֻ����1��ʾ��д
	char i_mode;
	//�ļ���С����λ�� byte,
	int i_size;
	//�ļ�����ʱ��
	time_t i_ctime;
	//�ļ�����ʱ��
	time_t i_atime;
	//�ļ��޸�ʱ��
	time_t i_mtime;
	//ֻ֧��ֱ���������ļ����12KB
	int i_address[12];
	//�����ڵ��Ƿ����,0���ã�1������
	char i_flag;
};
//Ŀ¼��
struct Dentry {
	//�����ڵ��
	int d_inode;
	//�ļ���
	string d_file_name;
	//�ļ����ͣ�0��ʾ��ͨ�ļ���1��ʾĿ¼�ļ�
	char d_file_type;
	//���ڵ��--�洢���׽ڵ�ţ�����д���ļ���Ŀ¼���Ĵ���
	int d_parent_id;
};
//Ŀ¼���ڵ�
struct TreeNode {
	//������
	Dentry data;
	//��һ���ӽڵ�
	TreeNode *child;
	//ͬ�������ڵ�
	TreeNode *next;
	//���ڵ�
	TreeNode *parent;
};
//�ļ�����
struct FileStr {
	string str;
};
//����Ŀ¼��
class Tree
{
private:
	//���ڵ�
	TreeNode *root;
	//��ǰĿ¼
	TreeNode *current;
	//ʱ��
	time_t t;
	//�����ڵ��
	int inode_id;
	//�ɷ����λ��
	int pos;
public:
	//������
	Inode inode_table[1024];
	//��ʼ����
	Tree();
	//������
	~Tree();
	//���������ڵ��
	void set_inode_id(int inode);
	//���������ڵ��
	int get_inode_id();
	//���ظ�Ŀ¼
	TreeNode *get_root();
	//���ø�Ŀ¼
	void set_root(TreeNode *new_root);
	//���ؿɷ����λ��
	int get_free_pos();
	//���ÿɷ����λ��
	void set_free_pos(int pos);
	//����Ŀ¼
	bool md_dir(string *dirs_name, int dirs_len, int type);
	//��ʾĿ¼�е��ļ�����Ŀ¼�б�
	bool show_dir(string *dirs_name,int dirs_len);
	//Ŀ¼���ļ��Ƿ����
	bool exist_dir(string *dirs_name, int dirs_len);
	//�õ���һ��Ŀ¼
	 Dentry get_pre_dir();
	 //�õ���ǰĿ¼
	 Dentry get_cur_dir();
	 //�õ���ǰĿ¼����--��·��
	 Dentry *get_cur_q(int &size);
	 //�õ���ǰĿ¼��string����
	 string *get_cur_s(int &size);
	 //�õ�ָ��Ŀ¼�ڵ�
	 TreeNode *get_dir_node(string * dirs_name, int dirs_len);
	 //�õ�ָ�������ڵ�ŵ��ļ���Ŀ¼��Ϣ
	 Inode get_inode(int id);
	//��ָ��Ŀ¼
	bool cd_dir(string *dirs_name,int dirs_len);
	//����Ŀ¼
	bool cp_dir(string *source_dir,string *goal_dir, int source_len,int goal_len);
	//ɾ����Ŀ¼
	bool rm_dir(string *dirs_name, int dirs_len);
	//ǰ�����Ŀ¼��--���ڲ���
	void print(TreeNode *parent);
	//��������
	TreeNode *copy(TreeNode *parent,int pid);
	//���ָ���ļ�����ϸ��Ϣ
	void print_dir(TreeNode *node);
	//�ƶ����������ļ���Ŀ¼
	bool move_dir(string *source_dir,string *goal_dir,int s_len,int g_len);
	//����ָ���ڵ�ŵ�����
	void copy_str(int s_id,int g_pos);
};

