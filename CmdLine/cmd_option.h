#include "../common.h"

class cmd_option
{
public:
	cmd_option(LPCTSTR option_name, bool paramized, std::tstring descr, std::list<std::tstring> ac_list = std::list<std::tstring>());
	std::tstring GetName();
	bool IsParamized();
	bool Set(LPCTSTR param = NULL);
	void Reset();
	bool IsSet();
	int GetInt();
	std::tstring GetString();
	std::tstring GetOptionDescription();
protected:
	std::tstring _option_name;
	bool is_set;
	const bool _paramized;
	std::tstring _descr;
	std::tstring _param;
	std::list<std::tstring> _acceptable;
};