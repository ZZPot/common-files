#include "cmd_option.h"

cmd_option::cmd_option(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list) :
	_paramized(paramized)
{
	_option_name = option_name;
	is_set = false;
	_param = _T("");
	_acceptable = ac_list;
	_descr = descr;
}
bool cmd_option::Set(LPCTSTR param)
{
	if (_paramized)
		if (!param)
			return false;
		else
		{
			//Empty _acceptable means every parameter is fine
			if (!_acceptable.size())
			{
				_param = param;
				is_set = true;
				return true;
			}
			for (auto& acc : _acceptable)
			{
				if (acc == param)
				{
					_param = param;
					is_set = true;
					return true;
				}
			}
			is_set = false;
			return false;
		}
	is_set = true;
	return true;
}
std::tstring cmd_option::GetName()
{
	return _option_name;
}
bool cmd_option::IsParamized()
{
	return _paramized;
}
void cmd_option::Reset()
{
	is_set = false;
	_param.clear();
}
bool cmd_option::IsSet()
{
	return is_set;
}
int cmd_option::GetInt()
{
	return _ttoi(_param.c_str());
}
std::tstring cmd_option::GetString()
{
	return _param;
}
std::tstring cmd_option::GetOptionDescription()
{
	std::tstring res;
	res = _option_name;
	res += _T("\t");
	if (_paramized)
		res += _T("<param> "); // надо добавить именованый параметр
	res += _descr;
	return res;
}