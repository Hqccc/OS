#include "FileSystem.h"

int main()
{
	cout << "----------------�ļ�ϵͳ�ļ�ģ��----------------" << endl;
	FileSystem file_system;
	//��ʼ���ļ�ϵͳ
	file_system.init_file_system();
	//���ļ�ϵͳ
	file_system.open_file_system();
	//�õ���ǰĿ¼
	Dentry cur_path = file_system.dir_tree.get_root()->data;
	//��������,����
	string in,cmd, param;

	while (true)
	{
		file_system.cur_tip();
		getline(cin,in);
		if (in != "")
		{
			//�õ�����Ͳ���
			cmd = file_system.get_cmd(in);
			param = file_system.get_param(in);
			//cout << cmd << "\t" << param << endl;
			switch (file_system.get_cmd_id(cmd))
			{
			case 0:
			{
				//�˳�����(���������)��ǰ������ű���
				file_system.exit();
				return 0;
				break;
			}
			case 1:
			{
				//��ʾ��ǰĿ¼����ı䵱ǰĿ¼��
				file_system.cd(param);
				break;
			}
			case 2:
			{
				//��ʾĿ¼�е��ļ�����Ŀ¼�б�
				file_system.dir(param);
				break;
			}
			case 3:
			{
				//����Ŀ¼
				file_system.mkdir_or_md(param);
				break;
			}
			case 4:
			{
				//������ʾ�����
				file_system.more(param);
				break;
			}
			case 5:
			{
				//��һ�ݻ����ļ����Ƶ���һ��λ�á�
				file_system.copy(in);
				break;
			}
			case 6:
			{

				//ɾ��һ��Ŀ¼��
				file_system.rmdir_or_rd(param);
				break;
			}
			case 7:
			{

				//���ļ��������ַ�����
				file_system.find(in);
				break;
			}
			case 8:
			{
				//��ʾ�ļ����ԡ�
				file_system.attrib(param);
				break;
			}
			case 9:
			{
				//ɾ��һ���������ļ���
				file_system.del(param);
				break;
			}
			case 10:
			{
				//�����ļ���Ŀ¼����
				file_system.xcopy(in);
				break;
			}
			case 11:
			{
				//c:\a.txt .    ������C���µ�a.txt���뵽��ǰĿ¼��
				file_system.import(param);
				break;
			}
			case 12:
			{
				//a.txt c:\   ����ǰĿ¼�µ�a.txt����������C�̡�
				file_system.eexport(param);
				break;
			}
			case 13:
			{
				//�ƶ��ļ����������ļ���Ŀ¼��
				file_system.move(in);
				break;
			}
			case 14:
			{
				//�ṩ����İ�����Ϣ
				file_system.help();
				break;
			}
			case 15:
			{
				//��ʾ������ϵͳʱ�䡣
				file_system._time();
				break;
			}
			case 16:
			{
				//��ʾ Windows �汾��
				file_system.ver();
				break;
			}
			case 17:
			{
				//�������ļ���
				file_system.rename(in);
				break;
			}
			case 18:
			{
				//������д���ַ����ļ�
				file_system.echo(param);
				break;
			}
			default:
			{
				cout << "�����ڸ�����������룡" << endl;
				break;
			}
			}
			continue;
		}
	}
	return 0;
}