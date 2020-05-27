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
	//读磁盘是否已经存在
	ifstream in(DISK_NAME, ios::in | ios::binary);
	//若不存在则创建并初始化
	if (!in)
	{
		//cout << "Open disk file error!" << endl;
		in.close();
		//用作计算位置
		char c[DATA_SIZE];
		ofstream out(DISK_NAME, ios::out | ios::binary);
		if (!out)
		{
			cout << "Create disk file error!" << endl;
		}
		//初始化磁盘
		for (int i = 0; i < DISK_SIZE; i++)
		{
			out.write("0",1);
		}
		//初始化超级块
		//单位Byte
		super_block.s_block_size = DATA_SIZE;
		super_block.s_block_count = DISK_SIZE / DATA_SIZE;
		super_block.s_inode_count = DISK_SIZE / DATA_SIZE;
		super_block.s_free_inode_count = DATA_SIZE;
		//初始化索引表
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
		//保留块数即超级块+索引区
		super_block.s_reserve_count = sizeof(Inode)*super_block.s_inode_count / DATA_SIZE + 1;
		super_block.s_free_count = super_block.s_block_count - super_block.s_reserve_count;
		super_block.s_first_data_block = super_block.s_reserve_count + 1;
		//为根节点分配
		dir_tree.set_inode_id(0);
		dir_tree.inode_table[dir_tree.get_inode_id()].i_flag = '1';
		time_t t;
		dir_tree.inode_table[dir_tree.get_inode_id()].i_ctime = time(&t);
		super_block.s_free_inode_count--;
		//将指针置于文件头
		out.seekp(0);
		//写入超级块
		out.write((char*)&super_block, sizeof(SuperBlock));
		//写入索引表
		out.seekp(sizeof(c));
		out.write((char*)dir_tree.inode_table, sizeof(dir_tree.inode_table));
		//写入目录树-此时只有根目录
		out.seekp(super_block.s_first_data_block*sizeof(c));
		out.write((char*)dir_tree.get_root(), sizeof(Dentry));
		//设置可分配块的位置
		dir_tree.set_free_pos(super_block.s_first_data_block+1);
		//关闭文件
		out.close();
	}
	else {
		in.close();
	}
}

void FileSystem::open_file_system()
{
	//设置可分配块的位置
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
	cout << "FileSystem [版本1.0.0]" << endl;
}

void FileSystem::rename(string old_new)
{
	if (old_new == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到就文件和新的名字
		string old,_new;
		istringstream istr;
		istr.str(old_new);
		istr.clear();
		istr >> old;
		istr >> old;
		istr >> _new;
		//得到目录数组
		string *str;
		int len = 0;
		str = get_dir(old,len);
		TreeNode *temp = dir_tree.get_dir_node(str,len);
		if (temp != nullptr)
		{
			temp->data.d_file_name = _new;
		}
		else {
			cout << "系统找不到指定路径!" << endl;
		}
	}
}

void FileSystem::echo(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		int len = 0;
		//转换为绝对路径的string数组
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.md_dir(s, len, 0);
		if (!res)
		{
			cout << "系统找不到指定路径！" << endl;
		}
		else {
			TreeNode *temp = dir_tree.get_dir_node(s, len);
			if (temp != nullptr)
			{
				//得到索引号
				int ind = temp->data.d_inode;
				//cout << ind << endl;
				//得到存储内容的块的位置
				int pos = dir_tree.inode_table[ind].i_address[0];
				//cout << pos << endl;
				cout << "input:";
				FileStr *content = new FileStr;
				cin >>content->str;
				cin.get();
				//用作计算位置
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
	//用作计算位置
	char c[DATA_SIZE];
	fstream out(DISK_NAME, ios::in |ios::out | ios::binary);
	if (!out)
	{
		cout << "Create disk file error!" << endl;
	}
	//写入超级块
	out.seekp(0);
	//super_block.
	out.write((char*)&super_block,sizeof(SuperBlock));
	//写入索引表
	out.seekp(sizeof(c));
	out.write((char*)dir_tree.inode_table,sizeof(dir_tree.inode_table));
	//写入目录树
	out.seekp(sizeof(c)*super_block.s_first_data_block);
	//将目录树写入队列
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
	//用作计算位置
	char c[DATA_SIZE];
	ifstream in(DISK_NAME, ios::in | ios::binary);
	if (!in)
	{
		cout << "Open disk file error!" << endl;
	}
	//读入超级快
	//将指针置于文件头
	in.seekg(0);
	//读入超级块
	in.read((char*)&super_block, sizeof(SuperBlock));
	//定位到索引区，读入索引表
	in.seekg(sizeof(c));
	in.read((char*)dir_tree.inode_table, sizeof(dir_tree.inode_table));
	//设置索引节点数
	dir_tree.set_inode_id(super_block.s_inode_count-super_block.s_free_inode_count);
	//定位到数据区，第一个数据块，读入目录树
	in.seekg(super_block.s_first_data_block*sizeof(c));
	//目录树中目录项的数量等于占用的索引节点数
	dir_num = super_block.s_inode_count - super_block.s_free_inode_count;
	//cout << super_block.s_inode_count << super_block.s_free_inode_count << endl;
	//读入目录树
	dir_link = new Dentry[dir_num];
	in.read((char*)dir_link,sizeof(Dentry)*dir_num);	
	//创建目录树
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
	//创建根节点
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
		//获取一层节点
		q = get_pid_d(i);
		int len = q.size();
		//cout << len << endl;
		for (int j = 0; j < len; j++)
		{
			//第一个子节点
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
			//同级其余子节点
			else {
				TreeNode *new_node = new TreeNode;
				new_node->child = nullptr;
				new_node->next = nullptr;
				new_node->parent = root_node;
				new_node->data = q.front();
				root_node->next = new_node;
				q.pop();
				//继续添加
				root_node = root_node->next;
			}
			//进入下一层
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
	//去除最后一个/
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
	//得到当前目录string数组
	s_cur = dir_tree.get_cur_s(len);
	int t = len;
	len += strs.size();
	sarr = new string[len];
	for (int i = 0; i < t; i++)
	{
		sarr[i] = s_cur[i];
		//cout << s_cur[i] << endl;
	}
	//拼接
	for (int i = t; i < len; i++)
	{
		sarr[i] = strs.front();
		strs.pop();
	}
	return sarr;
}

string * FileSystem::get_g_dir(string str, int & len)
{

	//去除最后一个/
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
	//特殊处理
	if (cmd == "md" || cmd == "MD")
	{
		cmd = "mkdir";
	}
	if (cmd == "rd" || cmd == "RD")
	{
		cmd = "rmdir";
	}
	//若为小写转换为大写
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
	//在CMD中查找对应命令号
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
			//转换为绝对路径的string数组
			//从根目录开始
			string *s_f = get_g_dir(param, len);
			//cout << len << endl;
			bool res_f = dir_tree.cd_dir(s_f, len),res_p = false;
			//若不是向上则检查是否向下
			if (!res_f)
			{
				len = 0;
				//当前目录开始
				string *s_p = get_dir(param, len);
				res_p = dir_tree.cd_dir(s_p, len);
			}
			//均不是
			if (!res_f && !res_p)
			{
				cout << "系统找不到指定路径！" << endl;
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
	cout << "的目录" << endl << endl;
	if (param == "")
	{
		//展示当前目录下的文件
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
		//转换为绝对路径的string数组
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.show_dir(s, len);
		if (!res)
		{
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::mkdir_or_md(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		int len = 0;
		//转换为绝对路径的string数组
		string *s = get_dir(param, len);
		//cout << len << endl;
		bool res = dir_tree.md_dir(s, len,1);
		if (!res)
		{
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::more(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		int len = 0;
		//转换为绝对路径的string数组
		string *s = get_dir(param, len);
		FileStr *content = new FileStr;
		//cout << len << endl;
		//得到目录节点
		TreeNode *temp= dir_tree.get_dir_node(s, len);
		if (temp == nullptr)
		{
			cout << "系统找不到指定路径！" << endl;
		}
		else {
			//得到索引号
			int ind = temp->data.d_inode;
			//得到存储内容的块的位置
			int pos = dir_tree.inode_table[ind].i_address[0];
			//cout << pos << endl;
			//用作计算位置
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
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到源地址和目标地址
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
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::rmdir_or_rd(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		string *str;
		int len = 0;
		str = get_dir(param,len);
		bool res = dir_tree.rm_dir(str,len);
		if (!res)
		{
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::find(string str_path)
{
	if (str_path == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到字符串和目标地址
		string str, path;
		istringstream istr;
		istr.str(str_path);
		istr.clear();
		istr >> str;
		istr >> str;
		istr >> path;
		//得到路径数组
		string *str_s;
		int len = 0;
		str_s = get_dir(path, len);
		//得到指定目录节点
		TreeNode *cur = dir_tree.get_dir_node(str_s, len);
		FileStr *content = new FileStr;
		//cout << len << endl;
		if (cur == nullptr)
		{
			cout << "系统找不到指定路径！" << endl;
		}
		else {
			//得到索引号
			int ind = cur->data.d_inode;
			//得到存储内容的块的位置
			int pos = dir_tree.inode_table[ind].i_address[0];
			//用作计算位置
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
			//若找到
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
		cout << "命令语法不正确！" << endl;
	}
	else {
		int len = 0;
		//转换为绝对路径的string数组
		string *s = get_dir(param, len);
		//文件内容结构体，便于二进制文件读写
		FileStr *content = new FileStr;
		//cout << len << endl;
		//得到目录节点
		TreeNode *temp = dir_tree.get_dir_node(s, len);
		if (temp == nullptr)
		{
			cout << "系统找不到指定路径！" << endl;
		}
		else {
			//输出文件信息
			dir_tree.print_dir(temp);
		}
	}
}

void FileSystem::del(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		string *str;
		int len = 0;
		str = get_dir(param, len);
		bool res = dir_tree.rm_dir(str, len);
		if (!res)
		{
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::xcopy(string source_goal)
{
	if (source_goal == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到源地址和目标地址
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
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::import(string filename)
{
	if (filename == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到当前目录的string数组
		int len = 0;
		string *s_p = get_dir(filename, len), stemp,st;
		TreeNode *temp = dir_tree.get_dir_node(s_p, len);
		if (temp != nullptr)
		{
			//读入外部文件内容
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
			//得到索引号
			int ind = temp->data.d_inode;
			//cout << ind << endl;
			//得到存储内容的块的位置
			int pos = dir_tree.inode_table[ind].i_address[0];
			//cout << pos << endl;
			FileStr *content = new FileStr;
			content->str = stemp;
			//用作计算位置
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
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::eexport(string param)
{
	if (param == "")
	{
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到当前目录的string数组
		int len = 0;
		string *s_p = get_dir(param, len),stemp;
		//得到当前文件的节点
		TreeNode *cur = dir_tree.get_dir_node(s_p,len);
		//cout << cur->data.d_file_name << endl;
		FileStr *content = new FileStr;
		//cout << len << endl;
		if (cur == nullptr)
		{
			cout << "系统找不到指定路径！" << endl;
		}
		else {
			//得到索引号
			int ind = cur->data.d_inode;
			//得到存储内容的块的位置
			int pos = dir_tree.inode_table[ind].i_address[0];
			//cout << pos << endl;
			//用作计算位置
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
		cout << "命令语法不正确！" << endl;
	}
	else {
		//得到源地址和目标地址
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
			cout << "系统找不到指定路径！" << endl;
		}
	}
}

void FileSystem::help()
{
	cout << "cd" << "\t"<<"打开指定目录" << endl;
	cout<< "dir"<<"\t"<<"显示目录中的文件和子目录列表"<<endl;
	cout << "mkdir or md" << "\t" << "创建目录" << endl;
	cout << "more" << "\t" << "读取指定文件的内容" << endl;
	cout << "copy" << "\t" << "将一个文件复制到另一个目录" << endl;
	cout << "rmdir_or_rd" << "\t" << "删除空目录" << endl;
	cout << "find" << "\t" << "在指定文件中搜索字符串" << endl;
	cout << "attrib" << "\t" << "显示文件属性" << endl;
	cout << "del" << "\t"<<"删除一个文件" << endl;
	cout << "xcopy" << "\t" << "复制文件或目录树" << endl;
	cout << "import" << "\t" << "将本地磁盘中的指定文本文件导入当前目录" << endl;
	cout << "export" << "\t" << "将当前目录下的指定文本文件导入本地磁盘" << endl;
	cout << "move" << "\t" << "移动文件并重命名文件和目录" << endl;
	cout << "help" << "\t" << "提供命令的帮助信息" << endl;
	cout << "time" << "\t" << "显示或设置系统时间" << endl;
	cout << "ver" << "\t" << "显示系统版本信息" << endl;
	cout << "rename" << "\t" << "重命名文件" << endl;
	cout << "echo" << "\t" << "创建文件并写入内容" << endl;
	cout << "exit" << "\t" << "退出系统" << endl;
}
