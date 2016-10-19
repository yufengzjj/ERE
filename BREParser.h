#ifndef BREParser_h__
#define BREParser_h__
#include <string>
#include <functional>
#include <tuple>
#include <type_traits>
#include <exception>
#include "third_part/ATMPL/any/any.hpp"
#include "FA.h"
#pragma once
class BREParser
{
public:
	BREParser();
	~BREParser();
	FA parse(std::string _src);
	class BREParserException :public std::exception
	{
	public:
		BREParserException(std::string msg):msg_(msg){}
		virtual const char* what() const noexcept
		{
			return msg_.c_str();
		}
	private:
		std::string msg_;
	};
	enum event
	{
		NFA_complete,
			NFA_determinated,
			DFA_minimized
	};
	using event_handler = std::function<void(event, ATMPL::any)>;
	void register_event(event e_, event_handler h_);
private:
	void dispatch(event e_, ATMPL::any msg_);
private:
	//note: the current letter view as processed.
	FA RE();
	FA expr();
	FA term();
	FA factor();
	FA item();
	FA sym();

	//move one step forward.
	//return eof = '\0' if no next letter.
	char next();

	//get next letter but not move forward.
	//return eof = '\0' if no next letter.
	char peek_next();

	bool has_next();

	using pos = unsigned int;
	char eof = '\0';

	std::string m_rexpr;
	pos m_pos;

	std::forward_list<std::pair<event, event_handler>> event_center_;

};
#endif // BREParser_h__

