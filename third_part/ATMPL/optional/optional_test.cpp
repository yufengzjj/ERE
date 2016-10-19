#include <iostream>
#include "optional.hpp"
ATMPL::optional<int> optional_ret()
{
	ATMPL::optional<int> a(15);
	return a;
}
int optional_test()
{
	//constructor test
	ATMPL::optional<int> explicit_conv_T(12);
	ATMPL::optional<int> copy_constr(explicit_conv_T);
	ATMPL::optional<int> move_constr(optional_ret());
	ATMPL::optional<int> asign_copy;
	asign_copy = move_constr;
	ATMPL::optional<int> asign_move;
	asign_move = optional_ret();

	//cast test
	ATMPL::optional<int> int_ = 1;
	int_ = 'f';
	ATMPL::optional<char> char_ = 's';
	int_ = char_;
	std::cout << *int_<<"\n";
	return 0;
}