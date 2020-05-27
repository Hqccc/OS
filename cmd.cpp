#include "FileSystem.h"

int main()
{
	cout << "----------------文件系统的简单模拟----------------" << endl;
	FileSystem file_system;
	//初始化文件系统
	file_system.init_file_system();
	//打开文件系统
	file_system.open_file_system();
	//得到当前目录
	Dentry cur_path = file_system.dir_tree.get_root()->data;
	//接受命令,参数
	string in,cmd, param;

	while (true)
	{
		file_system.cur_tip();
		getline(cin,in);
		if (in != "")
		{
			//得到命令和参数
			cmd = file_system.get_cmd(in);
			param = file_system.get_param(in);
			//cout << cmd << "\t" << param << endl;
			switch (file_system.get_cmd_id(cmd))
			{
			case 0:
			{
				//退出程序(命令解释器)或当前批处理脚本。
				file_system.exit();
				return 0;
				break;
			}
			case 1:
			{
				//显示当前目录名或改变当前目录。
				file_system.cd(param);
				break;
			}
			case 2:
			{
				//显示目录中的文件和子目录列表。
				file_system.dir(param);
				break;
			}
			case 3:
			{
				//创建目录
				file_system.mkdir_or_md(param);
				break;
			}
			case 4:
			{
				//逐屏显示输出。
				file_system.more(param);
				break;
			}
			case 5:
			{
				//将一份或多份文件复制到另一个位置。
				file_system.copy(in);
				break;
			}
			case 6:
			{

				//删除一个目录。
				file_system.rmdir_or_rd(param);
				break;
			}
			case 7:
			{

				//在文件中搜索字符串。
				file_system.find(in);
				break;
			}
			case 8:
			{
				//显示文件属性。
				file_system.attrib(param);
				break;
			}
			case 9:
			{
				//删除一个或数个文件。
				file_system.del(param);
				break;
			}
			case 10:
			{
				//复制文件和目录树。
				file_system.xcopy(in);
				break;
			}
			case 11:
			{
				//c:\a.txt .    将本地C盘下的a.txt导入到当前目录。
				file_system.import(param);
				break;
			}
			case 12:
			{
				//a.txt c:\   将当前目录下的a.txt导出到本地C盘。
				file_system.eexport(param);
				break;
			}
			case 13:
			{
				//移动文件并重命名文件和目录。
				file_system.move(in);
				break;
			}
			case 14:
			{
				//提供命令的帮助信息
				file_system.help();
				break;
			}
			case 15:
			{
				//显示或设置系统时间。
				file_system._time();
				break;
			}
			case 16:
			{
				//显示 Windows 版本。
				file_system.ver();
				break;
			}
			case 17:
			{
				//重命名文件。
				file_system.rename(in);
				break;
			}
			case 18:
			{
				//创建并写入字符到文件
				file_system.echo(param);
				break;
			}
			default:
			{
				cout << "不存在该命令，请检查输入！" << endl;
				break;
			}
			}
			continue;
		}
	}
	return 0;
}