#pragma once
#include "Tree.h"
class FileSystem
{
private:
	//ʵ�ֵ�����
	string CMD[19] = {"EXIT", "CD","DIR","MKDIR","MORE","COPY","RMDIR","FIND","ATTRIB",
		"DEL","XCOPY","IMPORT","EXPORT","MOVE","HELP","TIME","VER","RENAME","ECHO"};
	//Ŀ¼�����ڽ���Ŀ¼��
	Dentry *dir_link;
	//Ŀ¼����
	int dir_num;
	//Ŀ¼����
	queue <TreeNode*> q_dir;
public:
	//������
	SuperBlock super_block;
	//Ŀ¼��
	Tree dir_tree;
	FileSystem();
	~FileSystem();
	//���������ļ�ϵͳ����ʼ��
	void init_file_system();
	//���ļ�ϵͳ
	void open_file_system();
	//�˳� CMD.EXE ����(���������)��ǰ������ű���
	void exit();
	//д����
	void write();
	//������
	void read();
	//��Ŀ¼���������
	void get_dir_tree(TreeNode *root);
	//�õ�ָ�����ڵ�ID��Ŀ¼��
	queue<Dentry> get_pid_d(int pid);
	//����ָ�����ڵ�ID�Ľڵ�
	TreeNode* get_pid_node(int pid,TreeNode *root);
	//���ݶ����Ŀ¼������Ŀ¼��
	TreeNode *create_dir();
	//�õ�����
	string get_cmd(string in);
	//�õ�����
	string get_param(string in);
	//�õ���ǰĿ¼������Ŀ¼��·������
	string *get_dir(string str,int &len);
	//�õ���Ŀ¼������Ŀ¼��·������
	string *get_g_dir(string str,int &len);
	//�õ������
	int get_cmd_id(string cmd);
	//���³�����
	void updated_super_block();
	//��ʾ��ǰĿ¼
	void show_cur_dir();
	//��ʾ��ǰĿ¼����ı䵱ǰĿ¼��
	void cd(string param);
	//��ǰĿ¼��ʾ
	void cur_tip();
	//��ʾĿ¼�е��ļ�����Ŀ¼�б�
	void dir(string param);
	//����Ŀ¼
	void mkdir_or_md(string param);
	//������ʾ�����
	void more(string param);
	//��һ���ļ����Ƶ���һ��λ��
	void copy(string source_goal);
	//ɾ��һ��Ŀ¼��
	void rmdir_or_rd(string param);
	//���ļ��������ַ�����
	void find(string str_path);
	//��ʾ������ļ����ԡ�
	void attrib(string param);
	//ɾ��һ���ļ���
	void del(string param);
	//�����ļ���Ŀ¼����
	void xcopy(string source_goal);
	//c:\a.txt .    ������C���µ�a.txt���뵽��ǰĿ¼��
	void import(string filename);
	//a.txt c:\   ����ǰĿ¼�µ�a.txt����������C�̡�
	void eexport(string param);
	//�ƶ��ļ����������ļ���Ŀ¼��
	void move(string source_goal);
	//�ṩ����İ�����Ϣ
	void help();
	//��ʾ������ϵͳʱ�䡣
	void _time();
	//��ʾ Windows �汾��
	void ver();
	//�������ļ���
	void rename(string old_new);
	//�����ļ���д������
	void echo(string filename);
};

