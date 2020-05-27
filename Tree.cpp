#include "Tree.h"



Tree::Tree()
{
	root = new TreeNode;
	root->data.d_file_name = "root";
	root->data.d_file_type = '1';
	root->data.d_inode = 0;
	root->data.d_parent_id = -1;
	root->child = nullptr;
	root->next = nullptr;
	root->parent = nullptr;
	current = root;
	pos = 0;
}


Tree::~Tree()
{

}

void Tree::set_inode_id(int inode)
{
	inode_id = inode;
}

int Tree::get_inode_id()
{
	return inode_id;
}

TreeNode *Tree::get_root()
{
	return root;
}

void Tree::set_root(TreeNode *new_root)
{
	root = new_root;
}

int Tree::get_free_pos()
{
	int max = pos;
	for (int i = 0; i < DISK_SIZE / DATA_SIZE; i++)
	{
		if (inode_table[i].i_flag == '1' && inode_table[i].i_size > 0)
		{
			for (int j = 0; j < 12; j++)
			{
				if (max < inode_table[i].i_address[j])
				{
					max = inode_table[i].i_address[j];
				}
			}
		}
	}
	return max;
}

void Tree::set_free_pos(int pos)
{
	this->pos = pos;
}

bool Tree::md_dir(string * dirs_name, int dirs_len,int type)
{
	TreeNode *temp = root;
	int flags = 0;
	//dirs_nameΪ��������Ŀ¼��·������
	for (int i = 0; i < dirs_len; i++)
	{
		//����һ���ӽڵ㲻����Ŀ¼���򴴽�
		if (temp->child == nullptr)
		{
			//������Ŀ¼
			TreeNode *new_node = new TreeNode;
			new_node->child = nullptr;
			new_node->next = nullptr;
			new_node->parent = temp;
			new_node->data.d_file_name = dirs_name[i];
			new_node->data.d_parent_id = temp->data.d_inode;
			inode_id++;
			//���һ����Ŀ¼�����ļ�
			if (i == dirs_len - 1)
			{
				if (type)
				{
					new_node->data.d_file_type = '1';
				}
				else {
					new_node->data.d_file_type = '0';
					inode_table[inode_id].i_size = 1024;
					pos++;
					inode_table[inode_id].i_address[0] = pos;
				}
			}
			else {
				new_node->data.d_file_type = '1';
			}
			new_node->data.d_inode = inode_id;
			inode_table[inode_id].i_flag = '1';
			inode_table[inode_id].i_ctime = time(&t);
			//����Ŀ¼��
			temp->child = new_node;
			//������һ��
			temp = temp->child;
		}
		else {
			//����һ���ӽڵ����ͬ��Ŀ¼���������һ��Ŀ¼
			if (temp->child->data.d_file_name == dirs_name[i])
			{
				temp = temp->child;
				flags++;
			}
			else {
				//��ͬ�����Ƚ�ͬ�������ӽڵ�
				TreeNode *other = temp;
				//�Ƿ������һ��Ŀ¼
				int flag = false;

				while (other->next !=nullptr)
				{
					//ͬ�����������һ��Ŀ¼
					if (other->next->data.d_file_name == dirs_name[i])
					{
						temp = other->next;
						flag = true;
						flags++;
						break;
					}
					other = other->next;
				}
				//��ͬ����ͬ���ļ����򴴽�
				if (!flag)
				{
					temp = other;
					//������Ŀ¼
					TreeNode *new_node = new TreeNode;
					new_node->child = nullptr;
					new_node->next = nullptr;
					new_node->parent = temp;
					new_node->data.d_file_name = dirs_name[i];
					new_node->data.d_parent_id = temp->data.d_inode;
					inode_id++;
					//���һ����Ŀ¼�����ļ�
					if(i == dirs_len - 1)
					{
						if (type)
						{
							new_node->data.d_file_type = '1';
						}
						else {
							new_node->data.d_file_type = '0';
							inode_table[inode_id].i_size = 1024;
							pos++;
							inode_table[inode_id].i_address[0] = pos;
						}
					}
					else {
						new_node->data.d_file_type = '1';
					}
					new_node->data.d_inode = inode_id;
					inode_table[inode_id].i_flag = '1';
					inode_table[inode_id].i_ctime = time(&t);
					//����Ŀ¼��
					temp->next = new_node;
					//������һ��
					temp = temp->next;
				}
			}
		}
	}
	//cout << flags << endl;
	//Ҫ������Ŀ¼/·���Ѵ���
	if (flags == dirs_len)
	{
		return false;
	}
	else {
		return true;
	}
}

bool Tree::show_dir(string * dirs_name, int dirs_len)
{
	//��Ŀ¼�Ƿ����
	bool res = exist_dir(dirs_name, dirs_len);
	//cout << res << endl;
	if (res)
	{
		TreeNode *temp = get_dir_node(dirs_name, dirs_len);
		if (temp->child != nullptr)
		{
			print_dir(temp->child);
		}
		while (temp->next != nullptr)
		{
			print_dir(temp->next);
			temp = temp->next;
		}
		return true;
	}
	return false;
}

bool Tree::exist_dir(string * dirs_name, int dirs_len)
{
	//cout <<"exist" <<dirs_len << endl;
	TreeNode *temp = get_dir_node(dirs_name,dirs_len);
	if (temp != nullptr)
		return true;
	else
		return false;
}

Dentry Tree::get_pre_dir()
{
	if (current->parent != nullptr)
	{
		return current->parent->data;
	}
	else {
		return root->data;
	}
}

Dentry Tree::get_cur_dir()
{
	return current->data;
}

Dentry * Tree::get_cur_q(int &size)
{
	Dentry * a_cur;
	TreeNode *temp = current;
	int len = 0,i = 0;
	//���㳤��
	while (temp !=nullptr)
	{
		len++;
		temp = temp->parent;
	}
	a_cur = new Dentry[len];
	i = len;
	size = len;
	temp = current;
	//�����������
	while (temp != nullptr)
	{
		a_cur[i-1] = temp->data;
		i--;
		temp = temp->parent;
	}
	return a_cur;
}

string * Tree::get_cur_s(int & size)
{
	string * a_cur=nullptr;
	TreeNode *temp = current;
	int len = 0, i = 0;
	//���㳤��-������root
	while (temp->parent != nullptr)
	{
		len++;
		temp = temp->parent;
	}
	a_cur = new string[len];
	i = len;
	size = len;
	//cout << size << endl;
	temp = current;
	//�����������,ȥ��root
	while (temp->parent != nullptr)
	{
		a_cur[i - 1] = temp->data.d_file_name;
		//cout << a_cur[i - 1] << endl;
		i--;
		temp = temp->parent;
	}
	return a_cur;
}

TreeNode * Tree::get_dir_node(string * dirs_name, int dirs_len)
{
	//cout <<"node" <<dirs_len << endl;
	TreeNode *temp = root,*res = nullptr;
	int i;
	for (i= 0; i < dirs_len;)
	{
		if (temp->child != nullptr)
		{
			//����һ���ӽڵ�
			if (dirs_name[i] == temp->child->data.d_file_name)
			{
				//������һ��
				temp = temp->child;
				res = temp;
				i++;
			}
			else {
				//���ͬ�������ڵ�
				TreeNode *other = temp;
				int flag = 0;
				while (other->next != nullptr)
				{
					if (dirs_name[i] == other->next->data.d_file_name)
					{
						//������һ��
						temp = other->next;
						res = temp;
						i++;
						flag = 1;
						break;
					}
					other = other->next;
				}
				if (!flag)
				{
					//û���ҵ�
					break;
				}
			}
		}
		else {
			break;
		}
	}
	//˵��Ϊ��Ŀ¼��
	if (dirs_len == 0)
	{
		res = root;
	}
	if (dirs_len == i)
	{
		return res;
	}
	else {
		return nullptr;
	}
}

Inode Tree::get_inode(int id)
{
	//cout << "id" << id << endl;
	for (int i = 0; i < DISK_SIZE / DATA_SIZE; i++)
	{
		if (i == id)
		{
			return inode_table[i];
		}
	}
}

bool Tree::cd_dir(string * dirs_name, int dirs_len)
{
	//��Ŀ¼�Ƿ����
	bool res = exist_dir(dirs_name, dirs_len);
	//cout << "cd"<<res<<endl;
	if (res)
	{
		TreeNode *temp = get_dir_node(dirs_name,dirs_len);
		if (temp != nullptr)
		{
			current = temp;
			return true;
		}
		else
			return false;
	}
	return false;
}

bool Tree::cp_dir(string * source_dir, string * goal_dir, int source_len, int goal_len)
{
	bool source = exist_dir(source_dir,source_len);
	bool goal = exist_dir(goal_dir,goal_len);

	//cout << source << goal << endl;
	if (source&&goal)
	{
		TreeNode *temp_s = get_dir_node(source_dir, source_len),
			*temp_g = get_dir_node(goal_dir, goal_len);
		//cout << temp_s->data.d_file_name << temp_g->data.d_file_name << endl;
		//����ԴĿ¼
		TreeNode *res = copy(temp_s,temp_g->data.d_inode);
		//print(root);
		//cout << "copy" << endl;
		//print(res);
		//��Ŀ��Ŀ¼��Ѱ�ҿսڵ�
		if (temp_g->child == nullptr)
		{
			//���ӵ�һ���ӽڵ�
			temp_g->child = res->child;
			temp_g->child->parent = temp_g;
			//���������ӽڵ�
			while (res->next != nullptr)
			{
				temp_g->next = res->next;
				temp_g->next->parent = temp_g;
				res = res->next;
			}
		}
		else {
			//��Ŀ��Ŀ¼�Ŀսڵ�
			TreeNode *other = temp_g;
			while (other->next != nullptr)
			{
				other = other->next;
			}
			//����ԴĿ¼��ֱ�ӽڵ�
			//���ӵ�һ���ӽڵ�
			other->next = res->child;
			cout << other->next->data.d_file_name << endl;
			other->next->parent = temp_g;
			other = other->next;
			//���������ӽڵ�
			while (res->next != nullptr)
			{
				other->next = res->next;
				cout << other->next->data.d_file_name << endl;
				other->next->parent = temp_g;
				res = res->next;
			}
		}
		return true;
	}
	return false;
}

bool Tree::rm_dir(string * dirs_name, int dirs_len)
{	
	//��Ŀ¼�Ƿ����
	bool res = exist_dir(dirs_name, dirs_len);
	if (res)
	{
		TreeNode *temp = get_dir_node(dirs_name, dirs_len);
		//��Ϊ��Ŀ¼��ɾ��
		if (temp->child == nullptr && temp->next == nullptr)
		{
			//��Ϊ���ڵ�ĵ�һ���ӽڵ�
			if (temp == temp->parent->child)
			{
				temp->parent->child = nullptr;
			}
			else {
				//��Ϊ�����ӽڵ�
				TreeNode *other = temp->parent;
				while (other != nullptr)
				{
					if (temp == other)
					{
						other = nullptr;
					}
					other = other->next;
				}
			}
			delete temp;
			temp = nullptr;
			return true;
		}
	}
	return false;
}

void Tree::print(TreeNode *parent)
{
	if (parent == nullptr)
	{
		return;
	}
	cout << parent->data.d_parent_id <<"\t"<<parent->data.d_file_name << "\t"<<
		parent->data.d_file_type<<"\t" << parent->data.d_inode << endl;
	if (parent->child != nullptr)
	{
		print(parent->child);
	}
	if (parent->next != nullptr)
	{
		print(parent->next);
	}
}

TreeNode *Tree::copy(TreeNode * parent,int pid)
{
	TreeNode *new_node =nullptr,*new_child=nullptr,*new_next=nullptr;

	if (parent == nullptr)
	{
		return nullptr;
	}
	if (parent->child != nullptr)
	{
		new_child = copy(parent->child,pid++);
	}
	else {
		new_child = nullptr;
	}
	if (parent->next != nullptr)
	{
		new_next = copy(parent->next,pid++);
	}
	else {
		new_next = nullptr;
	}
	new_node = new TreeNode;
	new_node->child = new_child;
	new_node->next = new_next;
	new_node->data = parent->data;
	new_node->parent = parent->parent;
	//����������
	int old_id = inode_id;
	new_node->data.d_inode = ++inode_id;
	new_node->data.d_parent_id = pid;
	inode_table[inode_id].i_flag = '1';
	inode_table[inode_id].i_ctime = time(&t);
	//�����ļ�
	if (parent->data.d_file_type == '0')
	{
		inode_table[inode_id].i_size = 1024;
		inode_table[inode_id].i_address[0] = ++pos;
		//��������
		copy_str(old_id, pos);
	}
	return new_node;
}

void Tree::print_dir(TreeNode * node)
{
	Inode td = get_inode(node->data.d_inode);
	string st;
	if (node->data.d_file_type == '1')
	{
		st =  "dir\t";
	}
	else {
		st= "file\t";
	}
	struct tm *ptr;
	time_t lt;
	char str[80];
	lt = td.i_ctime;
	ptr = localtime(&lt);
	strftime(str, 100, "%F %T", ptr);
	cout << str << "\t" << st << "\t" << node->data.d_file_name << endl;
}

bool Tree::move_dir(string * source_dir, string * goal_dir, int s_len, int g_len)
{
	bool res = exist_dir(source_dir,s_len);
	if (res)
	{
		//�õ�ԴĿ¼�Ľڵ�
		TreeNode *s_node = get_dir_node(source_dir,s_len);
		//cout << s_node->data.d_file_name << endl;
		//����Ŀ��Ŀ¼
		md_dir(goal_dir,g_len,1);
		//�õ�Ŀ��Ŀ¼�Ľڵ�
		TreeNode *g_node = get_dir_node(goal_dir,g_len);
		//����ԴĿ¼�µ��ļ�
		if (s_node->child != nullptr)
		{
			g_node->child = s_node->child;
			g_node->child->parent = g_node;
		}
		TreeNode *other = s_node->next;
		while (other !=nullptr)
		{
			g_node->next = other;
			g_node->next->parent = g_node;
			other = other->next;
		}
		s_node->child = nullptr;
		s_node->next = nullptr;
		return true;
	}
	return false;
}

void Tree::copy_str(int s_id, int g_pos)
{
	FileStr *content = new FileStr;
	int ind = s_id;
	//�õ��洢���ݵĿ��λ��
	int pos = inode_table[ind].i_address[0];
	//cout << pos << endl;
	//��������λ��
	char c[DATA_SIZE];
	ifstream in(DISK_NAME, ios::in | ios::binary);
	if (!in)
	{
		cout << "Open disk file error!" << endl;
	}
	//cout << in << endl;
	in.seekg(sizeof(c)*pos);
	in.read((char*)content, sizeof(FileStr));
	in.close();
	//���¿�д������
	ofstream out(DISK_NAME, ios::in |ios::out | ios::binary);
	if (!out)
	{
		cout << "Open disk file error!" << endl;
	}
	out.seekp(sizeof(c)*g_pos);
	out.write((char*)content, sizeof(FileStr));
	out.close();
}
