#ifndef FA_h__
#define FA_h__

#include <set>
#include <map>
#include <type_traits>
#include <functional>
#include <utility>
#include "Automata.h"
#pragma once
class FA
{
public:
	using state = int;
	static std::string alphabet;
	using under_type = decltype(alphabet);
	using alphabet_type = under_type::value_type;
	using automata_type = Automata<state, alphabet_type, under_type>;

	explicit FA(const alphabet_type& d);
	~FA();

	void print(std::string name);
	bool check(under_type str);
	static void  set_alphabet(under_type _alphabet);

	//construct NFA
	void alternate_FA(FA _FA);
	void concat_FA(FA _FA);
	void closure();

	void to_mini_DFA();
	void determinate();
	void minimize();

private:
	automata_type ato_;
};

#endif // FA_h__
