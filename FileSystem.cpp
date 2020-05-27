#include "FileSystem.h"

FileSystem::FileSystem()
{
	dir_link = nullptr;
	dir_num = 0;
}


FileSystem::~FileSystem()
{

}

void FileSystem::init_file_system()
{
	//�������Ƿ��Ѿ�����
	ifstream in(DISK_NAME, ios::in | ios::binary);
	//���������򴴽�����ʼ��
	if (!in)
	{
		//cout << "Open disk file error!" << endl;
		in.close();
		//��������λ��
		char c[DATA_SIZE];
		ofstream out(DISK_NAME, ios::out | ios::binary);
		if (!out)
		{
			cout << "Create disk file error!" << endl;
		}
		//��ʼ������
		for (int i = 0; i < DISK_SIZE; i++)
		{
			out.write("0",1);
		}
		//��ʼ��������
		//��λByte
		super_block.s_block_size = DATA_SIZE;
		super_block.s_block_count = DISK_SIZE / DATA_SIZE;
		super_block.s_inode_count = DISK_SIZE / DATA_SIZE;
		super_block.s_free_inode_count = DATA_SIZE;
		//��ʼ��������
		for (int i = 0; i < super_block.s_inode_count; i++)
		{
			dir_tree.inode_table[i].i_flag = '0';
			dir_tree.inode_table[i].i_size = 0;
			dir_tree.inode_table[i].i_mode = '1';
			dir_tree.inode_table[i].i_atime = 0;
			dir_tree.inode_table[i].i_ctime = 0;
			dir_tree.inode_table[i].i_mtime = 0;
			for (int j = 0; j < FILE_MAX_SIZE; j++)
			{
				dir_tree.inode_table[i].i_address[j] = -1;
			}
		}
		//����������������+������
		super_block.s_reserve_count = sizeof(Inode)*super_block.s_inode_count / DATA_SIZE + 1;
		super_block.s_free_count = super_block.s_block_count - super_block.s_reserve_count;
		super_block.s_first_data_block = super_block.s_reserve_count + 1;
		//Ϊ���ڵ����
		dir_tree.set_inode_id(0);
		dir_tree.inode_table[dir_tree.get_inode_id()].i_flag = '1';
		time_t t;
		dir_tree.inode_table[dir_tree.get_inode_id()].i_ctime = time(&t);
		super_block.s_free_inode_count--;
		//��ָ�������ļ�ͷ
		out.seekp(0);
		//д�볬����
		out.write((char*)&super_block, sizeof(SuperBlock));
		//д��������
		out.seekp(sizeof(c));
		out.write((char*)dir_tree.inode_table, sizeof(dir_tree.inode_table));
		//д��Ŀ¼��-��ʱֻ�и�Ŀ¼
		out.seekp(super_block.s_first_data_block*sizeof(c));
		out.write((char*)dir_tree.get_root(), sizeof(Dentry));
		//���ÿɷ�����λ��
		dir_tree.set_free_pos(super_block.s_first_data_block+1);
		//�ر��ļ�
		out.close();
	}
	else {
		in.close();
	}
}

void FileSystem::open_file_system()
{
	//���ÿɷ�����λ��
	read();
	dir_tree.set_free_pos(super_block.s_first_data_block + dir_tree.get_free_pos());
	//cout << dir_tree.get_free_pos();
}

void FileSystem::exit()
{
	updated_super_block();
	write();
}

void FileSystem::_time()
{
	system("time");
}

void FileSystem::ver()
{
	cout << "FileSystem [�汾1.0.0]" << endl;
}

void FileSystem::rename(string old_new)
{
	if (old_new == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ����ļ����µ�����
		string old,_new;
		istringstream istr;
		istr.str(old_new);
		istr.clear();
		istr >> old;
		istr >> old;
		istr >> _new;
		//�õ�Ŀ¼����
		string *str;
		int len = 0;
		str = get_dir(old,len);
		TreeNode *temp = dir_tree.get_dir_node(str,len);
		if (temp != nullptr)
		{
			temp->data.d_file_name = _new;
		}
		else {
			cout << "ϵͳ�Ҳ���ָ��·��!" << endl;
		}
	}
}

void FileSystem::echo(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		int len = 0;
		//ת��Ϊ����·����string����
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.md_dir(s, len, 0);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
		else {
			TreeNode *temp = dir_tree.get_dir_node(s, len);
			if (temp != nullptr)
			{
				//�õ�������
				int ind = temp->data.d_inode;
				//cout << ind << endl;
				//�õ��洢���ݵĿ��λ��
				int pos = dir_tree.inode_table[ind].i_address[0];
				//cout << pos << endl;
				cout << "input:";
				FileStr *content = new FileStr;
				cin >>content->str;
				cin.get();
				//��������λ��
				char c[DATA_SIZE];
				fstream out(DISK_NAME, ios::in | ios::out | ios::binary);
				if (!out)
				{
					cout << "Create disk file error!" << endl;
				}
				//cout << in << endl;
				out.seekp(sizeof(c)*pos);
				out.write((char*)content,sizeof(FileStr));
				out.close();
			}
			else {
				cout << "Echo error!" << endl;
			}
		}
	}
}

void FileSystem::write()
{
	//��������λ��
	char c[DATA_SIZE];
	fstream out(DISK_NAME, ios::in |ios::out | ios::binary);
	if (!out)
	{
		cout << "Create disk file error!" << endl;
	}
	//д�볬����
	out.seekp(0);
	//super_block.
	out.write((char*)&super_block,sizeof(SuperBlock));
	//д��������
	out.seekp(sizeof(c));
	out.write((char*)dir_tree.inode_table,sizeof(dir_tree.inode_table));
	//д��Ŀ¼��
	out.seekp(sizeof(c)*super_block.s_first_data_block);
	//��Ŀ¼��д�����
	get_dir_tree(dir_tree.get_root());
	int len = q_dir.size();
	//cout << len << endl;
	for (int i = 0; i < len; i++)
	{
		TreeNode *temp = q_dir.front();
		q_dir.pop();
		//cout << temp->data.d_file_name << endl;
		out.write((char*)&temp->data, sizeof(Dentry));
	}
	out.close();
}

void FileSystem::read()
{
	//��������λ��
	char c[DATA_SIZE];
	ifstream in(DISK_NAME, ios::in | ios::binary);
	if (!in)
	{
		cout << "Open disk file error!" << endl;
	}
	//���볬����
	//��ָ�������ļ�ͷ
	in.seekg(0);
	//���볬����
	in.read((char*)&super_block, sizeof(SuperBlock));
	//��λ��������������������
	in.seekg(sizeof(c));
	in.read((char*)dir_tree.inode_table, sizeof(dir_tree.inode_table));
	//���������ڵ���
	dir_tree.set_inode_id(super_block.s_inode_count-super_block.s_free_inode_count);
	//��λ������������һ�����ݿ飬����Ŀ¼��
	in.seekg(super_block.s_first_data_block*sizeof(c));
	//Ŀ¼����Ŀ¼�����������ռ�õ������ڵ���
	dir_num = super_block.s_inode_count - super_block.s_free_inode_count;
	//cout << super_block.s_inode_count << super_block.s_free_inode_count << endl;
	//����Ŀ¼��
	dir_link = new Dentry[dir_num];
	in.read((char*)dir_link,sizeof(Dentry)*dir_num);	
	//����Ŀ¼��
	dir_tree.set_root(create_dir());
	//dir_tree.print(dir_tree.get_root());
	in.close();
}

void FileSystem::get_dir_tree(TreeNode * root)
{
	if (root == nullptr)
		return ;
	q_dir.push(root);
	if (root->child != nullptr)
		get_dir_tree(root->child);
	if (root->next != nullptr)
		get_dir_tree(root->next);
}

queue<Dentry> FileSystem::get_pid_d(int pid)
{
	queue<Dentry> q_d;
	for (int i = 0; i < dir_num; i++)
	{
		if (dir_link[i].d_parent_id == pid)
		{
			q_d.push(dir_link[i]);
		}
	}
	return q_d;
}

TreeNode * FileSystem::get_pid_node(int pid, TreeNode * root)
{
	if (root == nullptr)
	{
		return nullptr;
	}
	if (pid == root->data.d_parent_id)
	{
		return root;
	}
	if (root->child != nullptr)
	{
		return  get_pid_node(pid,root->child);
	}
	if (root->next != nullptr)
	{
		return  get_pid_node(pid, root->next);
	}
}

TreeNode *FileSystem::create_dir()
{
	TreeNode *root_node = nullptr,*t_root=new TreeNode;
	queue<Dentry> q;
	//�������ڵ�
	t_root->child = nullptr;
	t_root->next = nullptr;
	t_root->parent = nullptr;
	q = get_pid_d(-1);
	if (q.size())
	{
		t_root->data = q.front();
		q.pop();
	}
	root_node = t_root;

	for (int i = 0; i < dir_num; i++)
	{
		//��ȡһ��ڵ�
		q = get_pid_d(i);
		int len = q.size();
		//cout << len << endl;
		for (int j = 0; j < len; j++)
		{
			//��һ���ӽڵ�
			if (j == 0)
			{
				TreeNode *new_node = new TreeNode;
				new_node->child = nullptr;
				new_node->next = nullptr;
				new_node->parent = root_node;
				new_node->data = q.front();
				root_node->child = new_node;
				q.pop();
			}
			//ͬ�������ӽڵ�
			else {
				TreeNode *new_node = new TreeNode;
				new_node->child = nullptr;
				new_node->next = nullptr;
				new_node->parent = root_node;
				new_node->data = q.front();
				root_node->next = new_node;
				q.pop();
				//�������
				root_node = root_node->next;
			}
			//������һ��
			if (root_node != nullptr)
			{
				root_node = get_pid_node(i+1, t_root);
			}
		}
	}
	
	return t_root;
}

string FileSystem::get_cmd(string in)
{
	string str;
	istringstream istr;
	istr.clear();
	istr.str(in);
	istr >> str;
	return str;
}

string FileSystem::get_param(string in)
{
	string str1,str2;
	istringstream istr;
	istr.clear();
	istr.str(in);
	istr >> str1;
	istr >> str2;
	if (str1 == str2)
	{
		return "";
	}
	return str2;
}

string * FileSystem::get_dir(string str, int & len)
{
	//ȥ�����һ��/
	if (str[str.size() - 1] == '\\') str[str.size()-1] = '\0';

	for (int i = 0; i < str.size();i++)
	{
		if (str[i] == '\\')
		{
			str[i] = ' ';
		}
	}

	queue<string> strs;
	string s,*sarr = nullptr,*s_cur = nullptr;
	istringstream istr;
	istr.clear();
	istr.str(str);
	while (!istr.eof())
	{
		istr >> s;
		strs.push(s);
		//cout << "s"<< s << endl;
	}
	//�õ���ǰĿ¼string����
	s_cur = dir_tree.get_cur_s(len);
	int t = len;
	len += strs.size();
	sarr = new string[len];
	for (int i = 0; i < t; i++)
	{
		sarr[i] = s_cur[i];
		//cout << s_cur[i] << endl;
	}
	//ƴ��
	for (int i = t; i < len; i++)
	{
		sarr[i] = strs.front();
		strs.pop();
	}
	return sarr;
}

string * FileSystem::get_g_dir(string str, int & len)
{

	//ȥ�����һ��/
	if (str[str.size() - 1] == '\\') str[str.size() - 1] = '\0';

	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '\\')
		{
			str[i] = ' ';
		}
	}

	queue<string> strs;
	string s, *sarr = nullptr;
	istringstream istr;
	istr.clear();
	istr.str(str);
	while (!istr.eof())
	{
		istr >> s;
		strs.push(s);
		//cout << "s"<< s << endl;
	}
	len = strs.size();
	sarr = new string[len];
	for (int i = 0; i < len; i++)
	{
		sarr[i] = strs.front();
		strs.pop();
	}
	return sarr;
}

int FileSystem::get_cmd_id(string cmd)
{
	//���⴦��
	if (cmd == "md" || cmd == "MD")
	{
		cmd = "mkdir";
	}
	if (cmd == "rd" || cmd == "RD")
	{
		cmd = "rmdir";
	}
	//��ΪСдת��Ϊ��д
	char res[10];
	for (unsigned int i = 0,j = 0; i < cmd.length(); i++)
	{
		if (cmd[i] >= 'a' && cmd[i] <= 'z')
		{
			res[j] =  cmd[i] - 32;
			j++;
		}
		res[j] = '\0';
	}
	//��CMD�в��Ҷ�Ӧ�����
	string temp = res;
	for (int i = 0; i < 19; i++)
	{
		if (temp == CMD[i])
		{
			return  i;
		}
	}
	return -1;
}

void FileSystem::updated_super_block()
{
	super_block.s_free_inode_count = super_block.s_inode_count;
	super_block.s_free_count= super_block.s_block_count - super_block.s_reserve_count;
	for (int i = 0; i < super_block.s_inode_count; i++)
	{
		if (dir_tree.inode_table[i].i_flag == '1')
		{
			super_block.s_free_inode_count--;
		}
	}
	super_block.s_free_count = super_block.s_block_count - dir_tree.get_free_pos();
	//cout << super_block.s_free_count << endl;
}

void FileSystem::show_cur_dir()
{
	int len = 0;
	Dentry *temp = dir_tree.get_cur_q(len);
	for (int i = 0; i < len; i++)
	{
		if (i == 0)
		{
			cout << temp[i].d_file_name << ":\\";
		}
		else {
			cout << temp[i].d_file_name;
			if (i != len - 1)
			{
				cout << "\\";
			}
		}
	}
}

void FileSystem::cd(string param)
{
	if (param == "")
	{
		show_cur_dir();
		cout << ">" << endl<<endl;
	}
	else {
		if (param == "root:" || param =="root")
		{
			string *st = nullptr;
			dir_tree.cd_dir(st,0);
			show_cur_dir();
			cout << ">" << endl << endl;
		}
		else {
			int len = 0;
			//ת��Ϊ����·����string����
			//�Ӹ�Ŀ¼��ʼ
			string *s_f = get_g_dir(param, len);
			//cout << len << endl;
			bool res_f = dir_tree.cd_dir(s_f, len),res_p = false;
			//���������������Ƿ�����
			if (!res_f)
			{
				len = 0;
				//��ǰĿ¼��ʼ
				string *s_p = get_dir(param, len);
				res_p = dir_tree.cd_dir(s_p, len);
			}
			//������
			if (!res_f && !res_p)
			{
				cout << "ϵͳ�Ҳ���ָ��·����" << endl;
			}
		}
	}
}

void FileSystem::cur_tip()
{
	show_cur_dir();
	cout << ">";
}

void FileSystem::dir(string param)
{
	show_cur_dir();
	cout << "��Ŀ¼" << endl << endl;
	if (param == "")
	{
		//չʾ��ǰĿ¼�µ��ļ�
		int c_len = 0;
		string *s_c = dir_tree.get_cur_s(c_len);
		//cout << c_len << endl;
		/*string s[4] = { "a","b","c","d" }, s1[1] = {"a"};
		dir_tree.md_dir(s,4,1);*/
		//dir_tree.print(dir_tree.get_root());
		//dir_tree.print_dir(dir_tree.get_root()->child);
		dir_tree.show_dir(s_c, c_len);
	}
	else {
		int len = 0;
		//ת��Ϊ����·����string����
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.show_dir(s, len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::mkdir_or_md(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		int len = 0;
		//ת��Ϊ����·����string����
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.md_dir(s, len,1);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::more(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		int len = 0;
		//ת��Ϊ����·����string����
		string *s = get_dir(param, len);
		FileStr *content = new FileStr;
		//cout << len << endl;
		//�õ�Ŀ¼�ڵ�
		TreeNode *temp= dir_tree.get_dir_node(s, len);
		if (temp == nullptr)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
		else {
			//�õ�������
			int ind = temp->data.d_inode;
			//�õ��洢���ݵĿ��λ��
			int pos = dir_tree.inode_table[ind].i_address[0];
			//cout << pos << endl;
			//��������λ��
			char c[DATA_SIZE];
			ifstream in(DISK_NAME, ios::in| ios::binary);
			if (!in)
			{
				cout << "Open disk file error!" << endl;
			}
			//cout << in << endl;
			in.seekg(sizeof(c)*pos);
			in.read((char*)content, sizeof(FileStr));
			cout << content->str << endl;
			in.close();
		}
	}
}

void FileSystem::copy(string source_goal)
{
	if (source_goal == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ�Դ��ַ��Ŀ���ַ
		string source, goal;
		istringstream istr;
		istr.str(source_goal);
		istr.clear();
		istr >> source;
		istr >> source;
		istr >> goal;
		//cout << source << goal << endl;
		string *str_s, *str_g;
		int s_len = 0, g_len = 0;
		str_s = get_dir(source,s_len);
		str_g = get_dir(goal,g_len);
		bool res = dir_tree.cp_dir(str_s,str_g,s_len,g_len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::rmdir_or_rd(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		string *str;
		int len = 0;
		str = get_dir(param,len);
		bool res = dir_tree.rm_dir(str,len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::find(string str_path)
{
	if (str_path == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ��ַ�����Ŀ���ַ
		string str, path;
		istringstream istr;
		istr.str(str_path);
		istr.clear();
		istr >> str;
		istr >> str;
		istr >> path;
		//�õ�·������
		string *str_s;
		int len = 0;
		str_s = get_dir(path, len);
		//�õ�ָ��Ŀ¼�ڵ�
		TreeNode *cur = dir_tree.get_dir_node(str_s, len);
		FileStr *content = new FileStr;
		//cout << len << endl;
		if (cur == nullptr)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
		else {
			//�õ�������
			int ind = cur->data.d_inode;
			//�õ��洢���ݵĿ��λ��
			int pos = dir_tree.inode_table[ind].i_address[0];
			//��������λ��
			char c[DATA_SIZE];
			ifstream in(DISK_NAME, ios::in | ios::binary);
			if (!in)
			{
				cout << "Open disk file error!" << endl;
			}
			in.seekg(sizeof(c)*pos);
			in.read((char*)content, sizeof(FileStr));
			string::size_type idx;
			idx = content->str.find(str);
			//���ҵ�
			if (idx != string::npos)
				cout << content->str << endl;
			in.close();
		}
	}
}

void FileSystem::attrib(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		int len = 0;
		//ת��Ϊ����·����string����
		string *s = get_dir(param, len);
		//�ļ����ݽṹ�壬���ڶ������ļ���д
		FileStr *content = new FileStr;
		//cout << len << endl;
		//�õ�Ŀ¼�ڵ�
		TreeNode *temp = dir_tree.get_dir_node(s, len);
		if (temp == nullptr)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
		else {
			//����ļ���Ϣ
			dir_tree.print_dir(temp);
		}
	}
}

void FileSystem::del(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		string *str;
		int len = 0;
		str = get_dir(param, len);
		bool res = dir_tree.rm_dir(str, len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::xcopy(string source_goal)
{
	if (source_goal == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ�Դ��ַ��Ŀ���ַ
		string source, goal;
		istringstream istr;
		istr.str(source_goal);
		istr.clear();
		istr >> source;
		istr >> source;
		istr >> goal;
		//cout << source << goal << endl;
		string *str_s, *str_g;
		int s_len = 0, g_len = 0;
		str_s = get_dir(source, s_len);
		str_g = get_dir(goal, g_len);
		bool res = dir_tree.cp_dir(str_s, str_g, s_len, g_len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::import(string filename)
{
	if (filename == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ���ǰĿ¼��string����
		int len = 0;
		string *s_p = get_dir(filename, len), stemp,st;
		TreeNode *temp = dir_tree.get_dir_node(s_p, len);
		if (temp != nullptr)
		{
			//�����ⲿ�ļ�����
			ifstream in_f(filename);
			if (!in_f)
			{
				cout << "Open disk file error!" << endl;
			}
			while (!in_f.eof())
			{
				in_f >> st;
				stemp += st;
			}
			in_f.close();
			//�õ�������
			int ind = temp->data.d_inode;
			//cout << ind << endl;
			//�õ��洢���ݵĿ��λ��
			int pos = dir_tree.inode_table[ind].i_address[0];
			//cout << pos << endl;
			FileStr *content = new FileStr;
			content->str = stemp;
			//��������λ��
			char c[DATA_SIZE];
			fstream out(DISK_NAME, ios::in | ios::out | ios::binary);
			if (!out)
			{
				cout << "Create disk file error!" << endl;
			}
			//cout << in << endl;
			out.seekp(sizeof(c)*pos);
			out.write((char*)content, sizeof(FileStr));
			out.close();
			cout << "success!" << endl;
		}
		else {
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::eexport(string param)
{
	if (param == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ���ǰĿ¼��string����
		int len = 0;
		string *s_p = get_dir(param, len),stemp;
		//�õ���ǰ�ļ��Ľڵ�
		TreeNode *cur = dir_tree.get_dir_node(s_p,len);
		//cout << cur->data.d_file_name << endl;
		FileStr *content = new FileStr;
		//cout << len << endl;
		if (cur == nullptr)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
		else {
			//�õ�������
			int ind = cur->data.d_inode;
			//�õ��洢���ݵĿ��λ��
			int pos = dir_tree.inode_table[ind].i_address[0];
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
			stemp=content->str;
			in.close();
			ofstream out(cur->data.d_file_name);
			if (!out)
			{
				cout << "Open disk file error!" << endl;
			}
			out << stemp;
			out.close();
			cout << "success!" << endl;
		}
	}
}

void FileSystem::move(string source_goal)
{
	if (source_goal == "")
	{
		cout << "�����﷨����ȷ��" << endl;
	}
	else {
		//�õ�Դ��ַ��Ŀ���ַ
		string source, goal;
		istringstream istr;
		istr.str(source_goal);
		istr.clear();
		istr >> source;
		istr >> source;
		istr >> goal;
		//cout << source << goal << endl;
		string *str_s, *str_g;
		int s_len = 0, g_len = 0;
		str_s = get_dir(source, s_len);
		str_g = get_dir(goal, g_len);
		bool res = dir_tree.move_dir(str_s, str_g, s_len, g_len);
		if (!res)
		{
			cout << "ϵͳ�Ҳ���ָ��·����" << endl;
		}
	}
}

void FileSystem::help()
{
	cout << "cd" << "\t"<<"��ָ��Ŀ¼" << endl;
	cout<< "dir"<<"\t"<<"��ʾĿ¼�е��ļ�����Ŀ¼�б�"<<endl;
	cout << "mkdir or md" << "\t" << "����Ŀ¼" << endl;
	cout << "more" << "\t" << "��ȡָ���ļ�������" << endl;
	cout << "copy" << "\t" << "��һ���ļ����Ƶ���һ��Ŀ¼" << endl;
	cout << "rmdir_or_rd" << "\t" << "ɾ����Ŀ¼" << endl;
	cout << "find" << "\t" << "��ָ���ļ��������ַ���" << endl;
	cout << "attrib" << "\t" << "��ʾ�ļ�����" << endl;
	cout << "del" << "\t"<<"ɾ��һ���ļ�" << endl;
	cout << "xcopy" << "\t" << "�����ļ���Ŀ¼��" << endl;
	cout << "import" << "\t" << "�����ش����е�ָ���ı��ļ����뵱ǰĿ¼" << endl;
	cout << "export" << "\t" << "����ǰĿ¼�µ�ָ���ı��ļ����뱾�ش���" << endl;
	cout << "move" << "\t" << "�ƶ��ļ����������ļ���Ŀ¼" << endl;
	cout << "help" << "\t" << "�ṩ����İ�����Ϣ" << endl;
	cout << "time" << "\t" << "��ʾ������ϵͳʱ��" << endl;
	cout << "ver" << "\t" << "��ʾϵͳ�汾��Ϣ" << endl;
	cout << "rename" << "\t" << "�������ļ�" << endl;
	cout << "echo" << "\t" << "�����ļ���д������" << endl;
	cout << "exit" << "\t" << "�˳�ϵͳ" << endl;
}
