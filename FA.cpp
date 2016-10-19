#include "FA.h"
#include <limits>

std::string FA::alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

FA::FA(const alphabet_type& d)
	:ato_(d, alphabet)
{

}


FA::~FA()
{
}


void FA::print(std::string name)
{
	ato_.print(name);
}

bool FA::check(under_type str)
{
	auto acp = ato_.get_accepts();
	for (auto i = str.begin(); i != str.end(); ++i)
	{
		if (ato_.consume(*i))
		{
			if (acp.find(ato_.cur_state()) != acp.end() 
				&& std::distance(i, str.begin()) == std::distance(--str.end(), str.begin()))
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}

void FA::set_alphabet(under_type _alphabet)
{
	alphabet = _alphabet;
}

void FA::alternate_FA(FA _FA)
{
	ato_ = ato_ | _FA.ato_;
}

void FA::concat_FA(FA _FA)
{
	ato_ = ato_ + _FA.ato_;
}

void FA::closure()
{
	ato_ = ato_.closure();
}

void FA::to_mini_DFA()
{
	determinate();
	minimize();
}


void FA::determinate()
{
	ato_.determinate();
}

void FA::minimize()
{
	ato_.minimize();
}
