// Copyrighted (C) 2015-2017 Antinet.org team, see file LICENCE-by-Antinet.txt

#pragma once

/**
@file stdplus is library of very common functions to be used in this and other projects,
stdplus/tab focuses on containers support
@owner rfree
@todo TODO@testcase - test cases for all functions here
*/

#include <memory>
#include <utils/check.hpp>
#include <utils/misc.hpp>

namespace stdplus {

/**
 * Check if elements of container are stored contiguously
 */
template <typename T>
struct is_stl_container_contiguously : std::false_type {
};

template <typename T, typename A>
struct is_stl_container_contiguously<std::vector<T, A>> : std::true_type {
};

template <typename T, size_t N>
struct is_stl_container_contiguously<std::array<T, N>> : std::true_type {
};

template <typename T>
struct is_stl_container_contiguously<std::basic_string<T>> : std::true_type {
};

/*
Copies n elements from tab_src container (starting from offset_src)
into tab_dst table (starting in it at index offset_dst).
Both containers must be SequenceContainer and also AddressLinearContainer - e.g. std::vector, std::array
Given ranged must be apart, so either:
a) must come from non-overlaping ranges that are both from same memory-area/container
b) must come from separate containers that do not share memory-area

@safe Function will try to avoid UB (by aborting or throwing) in all cases as long as both
containers tab_src tab_dst are in valid state.
If conditions regarding address-overlapping are not meet, then function at most throws exception (hard _check error),
how ever it can also do nothing if given invalid condition was not needed anyway (e.g. for n==0).
If other conditions are not meet (e.g. offset is bigger then size) function can instead abort.
*/
template <class T1, class T2>
typename T2::iterator copy_safe_apart(size_t n, const T1 & tab_src, T2 & tab_dst, size_t offset_src=0, size_t offset_dst=0) {
	_dbg5("copy (tab) n="<<n);
	if (n<1) return tab_dst.begin();

	// could write both blocks below with lambda, in C++17TODO when decomposition declarations is available

	// source
	auto src_rb = tab_src.begin() + offset_src; // rb = range begin
	_check_input( offset_src < tab_src.size() );
	_check_input( n <= tab_src.size() - offset_src ); // subtracting offset_src is valid since above
	auto src_rl = tab_src.begin() + offset_src + n -1; // range last
	_dbg5("Source     range src_rb="<<to_debug(src_rb)<<" ... src_rl="<<to_debug(src_rl));
	_check_abort( src_rl <= tab_src.end() );

	// dest
	auto dst_rb = tab_dst.begin() + offset_dst; // rb = range begin
	_check_input( offset_dst < tab_dst.size() );
	_check_input( n <= tab_dst.size() - offset_dst ); // subtracting offset_dst is valid since above
	auto dst_rl = tab_dst.begin() + offset_dst + n -1; // range last
	_dbg5("Destintion range dst_rb="<<to_debug(dst_rb)<<" ... dst_rl="<<to_debug(dst_rl));
	_check_abort( dst_rl <= tab_dst.end() );

	bool overlap = test_ranges_overlap_inclusive_noempty(src_rb, src_rl,  dst_rb, dst_rl);
	_dbg5("overlap=" << overlap);
	_check_input(!overlap);

	copy_iter_and_check_no_overlap( src_rb, src_rl,  dst_rb, n );
	_dbg5("Copy done.");

	return dst_rb;
}

/**
 * View for SequenceContainer and also ContiguousIterator(as defined for C++17)
 * e.g. std::vector<> will work.
 * The container view is valid until the container iterators are valid.
 */
	template<typename Tp>
	class container_view
	{
	public:
		using iterator = typename Tp::iterator;
		using const_iterator = typename Tp::const_iterator;
		container_view(iterator begin, iterator end)
			: m_begin(begin), m_end(end), m_size(end-begin){}
		iterator begin() {return m_begin;}
		iterator end() {return m_end;}
		const_iterator begin() const {return m_begin;}
		const_iterator end() const {return m_end;}
		size_t size() const {return m_size;}
	private:
		iterator m_begin;
		iterator m_end;
		size_t m_size;
	};

	template<typename T>
	class tab_view
	{
	public:
		using iterator = T*;
		using const_iterator = const T*;

		// TI = type of some iterator. it must be linear-memory. TODO: metaprogramming check
		// of course (begin..end] must be from same container and must be a valid, linear range
		template <typename TI>
		tab_view(TI begin, TI end)
				: m_begin(begin), m_end(end), m_size(end-begin) {
			if(end<begin) throw std::invalid_argument("Incorect begin or end (end<begin).");
		}

		template <typename TC> // TC = type of some container. it must be linear-memory. TODO: metaprogramming check
		tab_view(const TC & obj)
				: m_begin(obj.size()?&(obj.at(0)):nullptr), m_end(m_begin+obj.size()), m_size(obj.size()) { }

		template <typename TC> // TC = type of some container. it must be linear-memory. TODO: metaprogramming check
		tab_view(TC & obj)
				: m_begin(obj.size()?&(obj.at(0)):nullptr), m_end(m_begin+obj.size()), m_size(obj.size()) { }

		template <typename TC> // TC = type of some container. it must be linear-memory. TODO: metaprogramming check
		tab_view(const TC & obj, size_t begin, size_t end)
				: m_begin(&(obj.at(begin))), m_end(&(obj.at(end-1))+1), m_size(end-begin) {
			if(end<begin) throw std::invalid_argument("Incorect begin or end (end<begin).");
		}

		template <typename TC> // TC = type of some container. it must be linear-memory. TODO: metaprogramming check
		tab_view(TC & obj, size_t begin, size_t end)
				: m_begin(&(obj.at(begin))), m_end(&(obj.at(end-1))+1), m_size(end-begin) {
			if(end<begin) throw std::invalid_argument("Incorect begin or end (end<begin).");
		}

		T & at(size_t ix) {
			if (ix >= size() || ix < 0) throw std::out_of_range("Reading tab_view out of range");
			return *( m_begin + ix );
		}

		iterator begin() {return m_begin;}
		iterator end() {return m_end;}
		const_iterator begin() const {return m_begin;}
		const_iterator end() const {return m_end;}
		size_t size() const {return m_size;}
	private:
		iterator m_begin;
		iterator m_end;
		size_t m_size;
	};

} // namespace stdplus
