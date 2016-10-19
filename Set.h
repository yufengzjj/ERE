#pragma once
#include <set>
#include <initializer_list>
#include <algorithm>
#include <vector>
#include <utility>
template<typename _T>
class Set : public std::set<_T>
{
public:
	Set() {}
	Set(_T&& x) { std::set<_T>::insert(std::forward<_T>(x)); }
	Set(const _T& x) { std::set<_T>::insert(x); }
	Set(const Set& x) :std::set<_T>(x) {}
	Set(Set&& x) :std::set<_T>(std::forward<Set>(x)) {}
	Set(std::initializer_list<_T> il) :std::set<_T>(il) {}
	template <class InputIterator>
	Set(InputIterator first, InputIterator last)
		: std::set<_T>(first, last) {}

	Set & operator= (Set &&x)
	{
		std::set<_T>::operator =(std::forward<Set>(x));
		return *this;
	}

	//set operators
	Set operator&(Set const& s);
	Set operator-(Set const& s);
	Set operator|(Set const& s);
};


template <typename _T>
Set<_T> Set<_T>::operator&(Set const& s)
{
	std::vector<_T> r;
	r.resize(this->size() + s.size());
	auto it = std::set_intersection(this->begin(), this->end(),
		s.begin(), s.end(), r.begin());
	r.resize(it - r.begin());
	return Set<_T>(r.begin(), r.end());
}

template <typename _T>
Set<_T> Set<_T>::operator-(Set const& s)
{
	std::vector<_T> r;
	r.resize(this->size() + s.size());
	auto it = std::set_difference(this->begin(),
		this->end(), s.begin(), s.end(), r.begin());
	r.resize(it - r.begin());
	return Set<_T>(r.begin(), r.end());
}

template <typename _T>
Set<_T> Set<_T>::operator|(Set const& s)
{
	std::vector<_T> r;
	r.resize(this->size() + s.size());
	auto it = std::set_union(this->begin(), this->end(),
		s.begin(), s.end(), r.begin());
	r.resize(it - r.begin());
	return Set<_T>(r.begin(), r.end());
}
