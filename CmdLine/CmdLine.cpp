#include "CmdLine.h"

CmdLine::CmdLine()
{
	is_set = false;
}
CmdLine::~CmdLine()
{
	is_set = false;
	for (cmd_option* cmdopt : options_list)
	{
		delete cmdopt;
	}
}
CmdLine::operator bool()
{
	return is_set;
}
bool CmdLine::AddOption(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list)
{
	cmd_option* existing_option = GetOption(option_name);
	if (existing_option)
		return false;
	options_list.push_back(new cmd_option(option_name, paramized, descr, ac_list));
	return true;
}
bool CmdLine::DeleteOption(LPCTSTR option_name)
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for (; i != options_list.end(); i++)
	{
		std::tstring temp = (*i)->GetName();
		if (!_tcscmp(option_name, temp.c_str()))
			break;
	}
	if (i == options_list.end())
		return false;
	delete* i;
	options_list.erase(i);
	return true;
}
bool CmdLine::SetCmd(LPCTSTR cmd_line)
{
	if ((!cmd_line) || (!_tcslen(cmd_line)))
		return false;
	ResetCmd();
	PTCHAR cmd_dup = _tcsdup(cmd_line);
	DWORD argc = BreakCmd(cmd_dup, nullptr, 0);
	free(cmd_dup);
	PTCHAR* args = new PTCHAR[argc];
	cmd_dup = _tcsdup(cmd_line);
	BreakCmd(cmd_dup, args, argc);
	is_set = true;
	for (unsigned i = 0; i < argc; i++)
	{
		cmd_option* cur_option = GetOption(args[i]);
		if (!cur_option)
		{
			is_set = false; // если встречается незнакомая опция, то cmd-строка считается недействительной
			break;
		}
		if (cur_option->IsParamized()) // проверим на параметр
		{
			if (i == argc - 1) // если последний, то провал
			{
				is_set = false;
				break;
			}
			cur_option->Set(args[i + 1]);
			i++;
		}
		else
			cur_option->Set();
	}
	delete[] args;
	free(cmd_dup);
	return is_set;
}
void CmdLine::ResetCmd()
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for (; i != options_list.end(); i++)
	{
		(*i)->Reset();
	}
	is_set = false;
}

bool CmdLine::Set(LPCTSTR option_name, LPCTSTR param)
{
	cmd_option* existing_option = GetOption(option_name);
	if (!existing_option)
		return false;
	existing_option->Set(param);
	return true;
}
bool CmdLine::Reset(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if (!existing_option)
		return false;
	existing_option->Reset();
	return true;
}
bool CmdLine::IsSet(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if (!existing_option)
		return false;
	return existing_option->IsSet();
}
int CmdLine::GetInt(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if (!existing_option)
		return 0;
	return existing_option->GetInt();
}
std::tstring CmdLine::GetString(LPCTSTR option_name)
{
	cmd_option* existing_option = GetOption(option_name);
	if (!existing_option)
		return _T("");
	return existing_option->GetString();
}
cmd_option* CmdLine::GetOption(LPCTSTR option_name)
{
	std::list<cmd_option*>::iterator i = options_list.begin();
	for (; i != options_list.end(); i++)
	{
		std::tstring temp = (*i)->GetName();
		if (!_tcscmp(option_name, temp.c_str()))
			break;
	}
	if (i == options_list.end())
		return NULL;
	return *i;
}
void CmdLine::ShowUsage()
{
	for (auto& opt : options_list)
	{
		_tprintf(_T("\t%s\n"), opt->GetOptionDescription().c_str());
	}
}