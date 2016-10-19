#pragma once

#include <set>
#include <forward_list>
#include <utility>
#include <memory>
#include <stack>
#include <iostream>
#include <array>
#include <limits>
#include <fstream>
#include <iterator>
#include <string>
#include <exception>
#include "third_part/ATMPL/optional/optional.hpp"
#include "Set.h"
template<typename _State = int, typename _Driver = char, typename _Sigma = std::string>
class Automata
{
public:
	enum { epsilon = 0 };
	enum { invalid_state = -1 };
	//the set must impl unique insert().
	using state_set = Set<_State>;
	Automata(_Sigma sigma);
	explicit Automata(const _Driver& d, _Sigma sigma);
	Automata(const Automata& ato_);
	Automata& operator=(const Automata& ato_);
	Automata& operator=(Automata&& ato_);
	Automata(Automata&& ato_);
	Automata clone() const;
	~Automata() {}

	_State gen_state();
	void add_edge(_State src, _State dest, _Driver d);
	void remove_edge(_State src, _State dest, _Driver d);
	void set_start(_State s);
	void set_accept(_State s);
	void set_accept(state_set s);
	void cancle_accept(_State s);
	void cancle_accept(state_set s);
	state_set get_accepts();
	_State get_start();
	state_set get_states();
	_State direct_to(_State src, _Driver d);
	state_set direct_to(state_set src, _Driver d);
	state_set e_closure(_State s);
	state_set e_closure(state_set s);

	//operations
	Automata operator|(Automata& ato_);
	Automata operator+(Automata& ato_);
	Automata closure();
	void determinate();
	void minimize();
	bool consume(_Driver d);
	_State cur_state();

	//port to third
	void print(std::string name);

	class AutomataException :public std::exception
	{
	public:
		AutomataException(std::string msg) :msg_(msg) {}
		virtual const char* what() const noexcept
		{
			return msg_.c_str();
		}
	private:
		std::string msg_;
	};
private:
	struct State :public std::enable_shared_from_this<State>
	{
		using direc_list = std::forward_list<
			std::pair<_Driver, State>>;
		using direc_ptr = std::shared_ptr<direc_list>;
		State(_State s, direc_ptr dptr)
		{
			state_ = s;
			directed_ = dptr;
		}
		friend bool operator==(const State& lhs, const State& rhs)
		{
			return lhs.state_ == rhs.state_;
		}
		friend bool operator<(const State& lhs, const State& rhs)
		{
			return lhs.state_ < rhs.state_;
		}

		_State state_;
		direc_ptr directed_;
	};
	typename Automata<_State, _Driver, _Sigma>::State add_state(_State s);
	ATMPL::optional<typename Automata<_State, _Driver, _Sigma>::State> find_state(_State s);
	typename Automata<_State, _Driver, _Sigma>::State::direc_ptr alloc_drc_ptr();
private:
	state_set accept_;
	_State start_;
	_State cur_state_;
	static _State seed_;
	_Sigma sigma_;

	std::forward_list<State> graph_;
};

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma /*= std::string*/>
_State Automata<_State, _Driver, _Sigma>::cur_state()
{
	return cur_state_;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma /*= std::string*/>
bool Automata<_State, _Driver, _Sigma>::consume(_Driver d)
{
	auto st = direct_to(cur_state_, d);
	if (st != invalid_state)
	{
		cur_state_ = st;
		return true;
	}
	return false;

}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma >
void Automata<_State, _Driver, _Sigma>::minimize()
{
	state_set origin;
	for (auto s : graph_)
	{
		origin.insert(s.state_);
	}
	std::forward_list<state_set> equivalence;
	using index = typename std::forward_list<state_set>::iterator::difference_type;
	auto eq_drt = [&](_State s, _Driver d)->index
	{
		_State s_ = direct_to(s, d);
		if (s_ == invalid_state)
		{
			s_ = s;
		}
		for (auto i = equivalence.begin(); i != equivalence.end(); ++i)
		{
			if (i->find(s_) != i->end())
			{
				return std::distance(equivalence.begin(), i);
			}
		}
		throw AutomataException("unexpected state" + std::to_string(s_));
	};
	equivalence.push_front(accept_);
	equivalence.push_front(origin - accept_);
	bool done = false;
	while (!done)//calculate equivalence state set
	{
		done = true;
		for (auto states = equivalence.begin(); states != equivalence.end(); ++states)
		{
			if (states->size() <= 1)
			{
				continue;
			}
			state_set apart_states;
			for (auto d : sigma_)
			{
				index first = eq_drt(*states->begin(), d);
				for (auto c = ++states->begin(); c != states->end(); ++c)
				{
					index cur = eq_drt(*c, d);
					if (cur != first)
					{
						apart_states.insert(*c);
					}
				}
				if (!apart_states.empty())
				{
					break;
				}
			}
			if (!apart_states.empty())
			{
				done = false;
				equivalence.push_front(apart_states);
				*states = *states - apart_states;
				break;
			}
		}
	}

	Automata<_State, _Driver, _Sigma> ato(sigma_);
	std::forward_list<_State> equivalence_state;
	auto eq_drive = [&](_State s, _Driver d)->ATMPL::optional<index>
	{
		_State s_ = direct_to(s, d);
		if (s_ == invalid_state)
		{
			return ATMPL::optional<index>();
		}
		for (auto i = equivalence.begin(); i != equivalence.end(); ++i)
		{
			if (i->find(s_) != i->end())
			{
				return std::distance(equivalence.begin(), i);
			}
		}
		throw AutomataException("unexpected state" + std::to_string(s_));
	};
	for (auto i = equivalence.begin(); i != equivalence.end(); ++i)
	{
		equivalence_state.push_front(gen_state());
	}
	//generate new automata from equivalence state set.
	for (auto eq = equivalence.begin(); eq != equivalence.end(); ++eq)
	{
		for (auto d : sigma_)
		{
			auto index_ = eq_drive(*eq->begin(), d);
			if (index_)
			{
				auto eq_state_src = equivalence_state.begin();
				auto eq_state_dst = equivalence_state.begin();
				std::advance(eq_state_src, std::distance(equivalence.begin(), eq));
				std::advance(eq_state_dst, *index_);
				ato.add_edge(*eq_state_src, *eq_state_dst, d);
			}
		}
		auto eq_state_i = equivalence_state.begin();
		std::advance(eq_state_i, std::distance(equivalence.begin(), eq));
		if (eq->find(start_) != eq->end())
		{
			ato.set_start(*eq_state_i);
		}
		else if (!(*eq & accept_).empty())
		{
			ato.set_accept(*eq_state_i);
		}

	}
	*this = ato;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
void Automata<_State, _Driver, _Sigma>::determinate()
{
	std::forward_list<state_set> walker;
	std::forward_list<std::pair<_State, state_set>> state_set2state;
	auto state_find = [&](state_set s)->ATMPL::optional<_State> {
		for (auto m : state_set2state)
		{
			if (m.second == s)
			{
				return m.first;
			}
		}
		return ATMPL::optional<_State>();
	};
	std::forward_list<state_set> traverse;
	Automata<_State, _Driver, _Sigma> ato(sigma_);
	//init
	state_set2state.push_front(std::make_pair(gen_state(), e_closure({ start_ })));
	walker.push_front(state_set2state.front().second);
	ato.set_start(state_set2state.front().first);
	traverse.push_front(state_set2state.front().second);
	//stop until no new sets.
	while (!walker.empty())
	{
		auto s = walker.front();
		walker.pop_front();

		for (auto d : sigma_)
		{
			auto st = direct_to(s, d);
			if (!st.empty())
			{
				st = e_closure(st);
				if (std::find(traverse.begin(), traverse.end(), st) == traverse.end())
				{
					traverse.push_front(st);
					walker.push_front(st);
				}

				auto start = state_find(s);
				if (start)
				{
					auto end = state_find(st);
					if (end)
					{
						ato.add_edge(*start, *end, d);
					}
					else
					{
						state_set2state.push_front(std::make_pair(gen_state(), st));
						ato.add_edge(*start, state_set2state.front().first, d);
					}

				}
			}

		}

	}

	//mark accept states
	state_set accept;
	for (auto a : state_set2state)
	{
		for (auto acp : accept_)
		{
			if (a.second.find(acp) != a.second.end())
			{
				accept.insert(a.first);
			}
		}

	}
	ato.set_accept(accept);
	*this = ato;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
Automata<_State, _Driver, _Sigma> Automata<_State, _Driver, _Sigma>::closure()
{
	auto ato = clone();
	auto start = gen_state();
	auto accept = gen_state();
	for (auto acp : ato.accept_)
	{
		ato.add_edge(acp, ato.start_, epsilon);
		ato.add_edge(acp, accept, epsilon);
	}
	ato.add_edge(start, ato.start_, epsilon);
	ato.add_edge(start, accept, epsilon);
	ato.set_start(start);
	ato.accept_.clear();
	ato.set_accept(accept);
	return ato;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
Automata<_State, _Driver, _Sigma>
Automata<_State, _Driver, _Sigma>::operator+(Automata& ato_)
{
	auto ato = clone();
	ato.graph_.sort();
	ato_.graph_.sort();
	ato.graph_.merge(ato_.graph_);
	ato.graph_.unique();
	for (auto accept : ato.accept_)
	{
		ato.add_edge(accept, ato_.start_, epsilon);
	}
	ato.accept_.clear();
	ato.set_accept(ato_.accept_);
	return ato;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
Automata<_State, _Driver, _Sigma>::Automata(_Sigma sigma)
	:start_(invalid_state),
	sigma_(sigma)
{
	//std::cout << "Default constructor" << std::endl;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
Automata<_State, _Driver, _Sigma>& Automata<_State, _Driver, _Sigma>::operator=(Automata&& ato_)
{
	//std::cout << "Move assignment operator" << std::endl;
	set_start(ato_.get_start());
	accept_.clear();
	set_accept(ato_.get_accepts());
	graph_ = ato_.graph_;
	sigma_ = ato_.sigma_;
	return *this;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
void Automata<_State, _Driver, _Sigma>::print(std::string name)
{
	std::ofstream out(name);
	out << "digraph{" << std::endl;
	out << R"(rankdir="LR";)" << std::endl;
	out << R"(fontname = "Microsoft YaHei";)" << std::endl;
	out << R"(node [shape = circle, fontname = "Microsoft YaHei"];)" << std::endl;
	out << R"(edge [fontname = "Microsoft YaHei"];)" << std::endl;
	out << R"(-1[style=invis];)" << std::endl;
	for (auto acp : accept_)
	{
		out << acp << R"( [ shape = doublecircle ];)" << std::endl;
	}
	out << "-1 -> " << get_start() << ";" << std::endl;
	for (auto state : graph_)
	{
		for (auto drct : *state.directed_)
		{
			if (drct.first == epsilon)
			{
				out << state.state_ << " -> " << drct.second.state_ << R"([ label = )" << "<&epsilon;>" << R"( ];)" << std::endl;
			}
			else
			{
				out << state.state_ << " -> " << drct.second.state_ << R"([ label = ")" << drct.first << R"(" ];)" << std::endl;
			}

		}
	}
	out << "}";
	out.flush();
	out.close();
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
typename Automata<_State, _Driver, _Sigma>::State::direc_ptr
Automata<_State, _Driver, _Sigma>::alloc_drc_ptr()
{
	return typename Automata<_State, _Driver, _Sigma>::State::direc_ptr
	(new typename Automata<_State, _Driver, _Sigma>::State::direc_list);
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
typename Automata<_State, _Driver, _Sigma>::State
Automata<_State, _Driver, _Sigma>::add_state(_State s)
{
	auto state = find_state(s);
	if (!state)
	{
		graph_.push_front(State(s, alloc_drc_ptr()));
		return graph_.front();
	}
	return *state;
}

template<typename _State, typename _Driver, typename _Sigma>
inline typename Automata<_State, _Driver, _Sigma>::state_set
Automata<_State, _Driver, _Sigma>::get_accepts()
{
	return accept_;
}

template<typename _State, typename _Driver, typename _Sigma>
inline _State Automata<_State, _Driver, _Sigma>::get_start()
{
	return start_;
}

template<typename _State, typename _Driver, typename _Sigma>
inline typename Automata<_State, _Driver, _Sigma>::state_set
Automata<_State, _Driver, _Sigma>::get_states()
{
	state_set st;
	for (auto s : graph_)
	{
		st.insert(s.state_);
	}
	return st;
}

template<typename _State, typename _Driver, typename _Sigma>
inline _State Automata<_State, _Driver, _Sigma>::direct_to(_State src, _Driver d)
{
	auto s = find_state(src);
	if (s)
	{
		for (auto edge : (*(*s).directed_))
		{
			if (edge.first == d)
			{
				return edge.second.state_;
			}
		}

	}
	return _State(invalid_state);
}

template<typename _State, typename _Driver, typename _Sigma>
inline typename Automata<_State, _Driver, _Sigma>::state_set
Automata<_State, _Driver, _Sigma>::direct_to(state_set src, _Driver d)
{
	state_set st;
	for (auto s : src)
	{
		st.insert(direct_to(s, d));
	}
	st.erase(invalid_state);
	return st;
}

template<typename _State, typename _Driver, typename _Sigma>
inline typename Automata<_State, _Driver, _Sigma>::state_set
Automata<_State, _Driver, _Sigma>::e_closure(_State s)
{
	state_set st;
	std::stack<_State> walker;
	walker.push(s);
	while (!walker.empty())
	{
		auto state = walker.top();
		walker.pop();
		auto node = find_state(state);
		if (node)
		{
			for (auto s_ : *((*node).directed_))
			{
				if (s_.first == epsilon && st.find(s_.second.state_) == st.end())
				{
					walker.push(s_.second.state_);
				}
			}
		}
		st.insert(state);
	}
	return st;
}

template<typename _State, typename _Driver, typename _Sigma>
inline typename Automata<_State, _Driver, _Sigma>::state_set
Automata<_State, _Driver, _Sigma>::e_closure(state_set s)
{
	state_set st;
	for (auto s_ : s)
	{
		auto st_ = e_closure(s_);
		st.insert(st_.begin(), st_.end());
	}
	return st;
}

template<typename _State, typename _Driver, typename _Sigma>
inline  Automata<_State, _Driver, _Sigma>
Automata<_State, _Driver, _Sigma>::operator|(Automata & ato_)
{
	auto ato = clone();
	ato.graph_.sort();
	ato_.graph_.sort();
	ato.graph_.merge(ato_.graph_);
	ato.graph_.unique();
	auto start = gen_state();
	auto end = gen_state();
	ato.add_edge(start, ato.start_, epsilon);
	ato.add_edge(start, ato_.start_, epsilon);
	ato.set_start(start);
	for (auto s : ato.accept_)
	{
		ato.add_edge(s, end, epsilon);
	}
	for (auto s : ato_.accept_)
	{
		ato.add_edge(s, end, epsilon);
	}
	ato.accept_.clear();
	ato.set_accept(end);
	return ato;
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
ATMPL::optional<typename Automata<_State, _Driver, _Sigma>::State>
Automata<_State, _Driver, _Sigma>::find_state(_State s)
{
	for (auto g : graph_)
	{
		if (g.state_ == s)
		{
			return g;
		}
	}
	return ATMPL::optional<typename Automata<_State, _Driver, _Sigma>::State>();
}

template<typename _State /*= int*/, typename _Driver /*= char*/, typename _Sigma>
void Automata<_State, _Driver, _Sigma>::add_edge(_State src, _State dest, _Driver d)
{
	State src_state = add_state(src);
	State dest_state = add_state(dest);
	std::pair<_Driver, State> edge = std::pair<_Driver, State>(d, dest_state);
	src_state.directed_->push_front(edge);
	src_state.directed_->unique();
}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::remove_edge(_State src, _State dest, _Driver d)
{
	auto src_state = find_state(src);
	auto dest_state = find_state(dest);
	if (src_state&&dest_state)
	{
		std::pair<_Driver, State> edge = std::pair<_Driver, State>(d, dest_state);
		(*src_state).directed_->remove(edge);
	}
}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::set_start(_State s)
{
	if (!find_state(s))
	{
		add_state(s);
	}
	start_ = s;
	cur_state_ = start_;
}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::set_accept(_State s)
{
	accept_.insert(s);
}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::set_accept(typename Automata<_State, _Driver, _Sigma>::state_set s)
{
	accept_.insert(s.begin(), s.end());
}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::cancle_accept(_State s)
{
	if (accept_.find() != accept_.end())
	{
		accept_.erase(s);
	}

}

template<typename _State, typename _Driver, typename _Sigma>
inline void Automata<_State, _Driver, _Sigma>::
cancle_accept(typename Automata<_State, _Driver, _Sigma>::state_set s)
{
	for (auto state : s)
	{
		cancle_accept(s);
	}
}

template<typename _State, typename _Driver, typename _Sigma>
inline Automata<_State, _Driver, _Sigma>::Automata(Automata && ato_)
{
	//std::cout << "Move constructors" << std::endl;
	set_start(ato_.get_start());
	set_accept(ato_.get_accepts());
	graph_ = ato_.graph_;
	sigma_ = ato_.sigma_;
}

template<typename _State, typename _Driver, typename _Sigma>
Automata<_State, _Driver, _Sigma> Automata<_State, _Driver, _Sigma>::clone() const
{
	//std::cout << "clone" << std::endl;
	Automata<_State, _Driver, _Sigma> ato(sigma_);
	ato.graph_.clear();
	for (auto g : graph_)
	{
		for (auto drc : (*g.directed_))
		{
			ato.add_edge(g.state_, drc.second.state_, drc.first);
		}
	}
	ato.set_start(const_cast<Automata<_State, _Driver, _Sigma>*>(this)->get_start());
	ato.set_accept(const_cast<Automata<_State, _Driver, _Sigma>*>(this)->get_accepts());
	return ato;
}

template<typename _State, typename _Driver, typename _Sigma>
_State Automata<_State, _Driver, _Sigma>::gen_state()
{
	auto state = seed_++;
	if (state == invalid_state)
	{
		seed_ = invalid_state + 1;
		state = seed_;
	}
	return state;
}

template<typename _State, typename _Driver, typename _Sigma>
_State Automata<_State, _Driver, _Sigma>::seed_ = Automata<_State, _Driver, _Sigma>::invalid_state + 1;

template<typename _State, typename _Driver, typename _Sigma>
Automata<_State, _Driver, _Sigma>::Automata(const _Driver& d, _Sigma sigma)
	:sigma_(sigma)
{
	//std::cout << "Converting constructor" << std::endl;
	auto start = gen_state();
	auto accept = gen_state();
	set_start(start);
	set_accept(accept);
	add_edge(start, accept, d);
}

template<typename _State, typename _Driver, typename _Sigma>
inline Automata<_State, _Driver, _Sigma>::Automata(const Automata & ato_)
{
	//std::cout << "Copy constructors" << std::endl;
	*this = ato_;
}

template<typename _State, typename _Driver, typename _Sigma>
inline  Automata<_State, _Driver, _Sigma> & Automata<_State, _Driver, _Sigma>::operator=(const Automata & ato_)
{
	//std::cout << "Copy assignment operator" << std::endl;
	auto ato = ato_.clone();
	graph_ = ato.graph_;
	sigma_ = ato_.sigma_;
	accept_.clear();
	set_start(const_cast<Automata<_State, _Driver, _Sigma>&>(ato).get_start());
	set_accept(const_cast<Automata<_State, _Driver, _Sigma>&>(ato).get_accepts());
	return *this;
}
