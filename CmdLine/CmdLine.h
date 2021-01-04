#include "../common.h"
#include "cmd_option.h"
class CmdLine
{
public:
	CmdLine();
	virtual ~CmdLine();
	operator bool(); // ��������� �� ���������� �����
	bool AddOption(LPCTSTR option_name, // ������ ��� �����
		bool param, // ����������������� �� �����
		std::tstring descr, // �������� �����
		std::list<std::tstring> ac_list = std::list<std::tstring>()); // ������ ���������� ����������
	bool DeleteOption(LPCTSTR option_name);
	bool SetCmd(LPCTSTR cmd_line);
	void ResetCmd();
	bool Set(LPCTSTR option_name, LPCTSTR param = NULL);
	bool Reset(LPCTSTR option_name);
	bool IsSet(LPCTSTR option_name);
	int GetInt(LPCTSTR option_name);
	std::tstring GetString(LPCTSTR option_name);
	void ShowUsage(); // ������ �����, ��� ������� ������
protected:
	cmd_option* GetOption(LPCTSTR option_name);
	std::list<cmd_option*> options_list;
	bool is_set;
};