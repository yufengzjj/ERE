#include "BREParser.h"
#include <algorithm>
BREParser::BREParser()
	:m_pos(-1)
{
}


BREParser::~BREParser()
{
}

FA BREParser::parse(std::string _src)
{
	m_rexpr = _src;
	auto fa = RE();
	if (m_pos < _src.length()-1)
	{
		throw BREParserException("abort unexpected at offset:" + std::to_string(m_pos + 1));
	}
	dispatch(event::NFA_complete, fa);
	fa.determinate();
	dispatch(event::NFA_determinated, fa);
	fa.minimize();
	dispatch(event::DFA_minimized, fa);
	return fa;

}

void BREParser::register_event(event e_, event_handler h_)
{
	event_center_.push_front(std::make_pair(e_, h_));
}

void BREParser::dispatch(event e_, ATMPL::any msg_)
{
	for (auto ev : event_center_)
	{
		if (ev.first == e_)
		{
			ev.second(e_, msg_);
		}
	}
}

FA BREParser::RE()
{
	return expr();
}

FA BREParser::expr()
{
	FA _fa = term();
	if (peek_next() == '|')
	{
		next();
		_fa.alternate_FA(expr());
	}
	return _fa;
}

FA BREParser::term()
{
	FA _fa = factor();
	while (peek_next() != '|' && peek_next() != ')' && has_next())
	{
		_fa.concat_FA(factor());
	}
	return _fa;
}

FA BREParser::factor()
{
	FA _fa = item();
	while (peek_next() == '*')
	{
		next();
		_fa.closure();
	}
	return _fa;
}

FA BREParser::item()
{
	if (peek_next() == '(')
	{
		next();
		auto _fa = expr();
		if (peek_next() != ')')
		{
			//missing ')'
			throw BREParserException(("missing ')' at offset:" + std::to_string(m_pos + 1)).c_str());
		}
		next();
		return _fa;
	}
	else if (std::find(FA::alphabet.begin(), FA::alphabet.end(),peek_next()) != FA::alphabet.end())
	{
		return sym();
	}
	throw BREParserException(("unknown letter found at offset:" + std::to_string(m_pos + 1)).c_str());
}

FA BREParser::sym()
{
	FA _fa(peek_next());
	next();
	return _fa;
}

char BREParser::next()
{
	char _next_char = peek_next();
	if (_next_char != eof)
	{
		++m_pos;
	}
	return _next_char;
}

char BREParser::peek_next()
{
	pos next_pos = m_pos + 1;
	if (next_pos < m_rexpr.length())
	{
		return m_rexpr.at(next_pos);
	}
	return eof;
}

bool BREParser::has_next()
{
	if (peek_next() != eof)
	{
		return true;
	}
	return false;
}

