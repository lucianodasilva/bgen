#pragma once
#ifndef _bgen_common_small_vector_h_
#define _bgen_common_small_vector_h_

#include <cstdio>
#include <cinttypes>
#include <iterator>
#include <limits>
#include <type_traits>

namespace bgen {

	template<class _t, size_t _n>
	struct __typeless_array {
		uint8_t data[sizeof(_t) * _n];

		inline _t &operator[](size_t i) {
			return *reinterpret_cast < _t * > ((i * sizeof(_t)) + data);
		}

		inline const _t *location() const {
			return reinterpret_cast < const _t * > (+data);
		};

		inline _t *location() {
			return reinterpret_cast < _t * > (+data);
		};
	};

	template<class _t>
	struct small_vector_base {
	public:

		using value_type                = _t;
		using reference                = _t &;
		using const_reference            = _t const &;
		using pointer                    = _t *;
		using const_pointer            = _t const *;

		using size_type                = size_t;
		using difference_type            = ptrdiff_t;

		using iterator                    = pointer;
		using const_iterator            = const_pointer;

		using reverse_iterator            = std::reverse_iterator<iterator>;
		using const_reverse_iterator    = std::reverse_iterator<const_iterator>;

		inline iterator begin() noexcept { return _begin_ptr; }

		inline const_iterator begin() const noexcept { return _begin_ptr; }

		inline iterator end() noexcept { return _end_ptr; }

		inline const_iterator end() const noexcept { return _end_ptr; }

		inline reverse_iterator rbegin() noexcept { return --end(); }

		inline const_reverse_iterator rbegin() const noexcept { return --end(); }

		inline reverse_iterator rend() noexcept { return --begin(); }

		inline const_reverse_iterator rend() const noexcept { return --begin(); }

		inline const_iterator cbegin() const noexcept { return begin(); }

		inline const_iterator cend() const noexcept { return end(); }

		inline const_reverse_iterator crbegin() const noexcept { return rbegin(); }

		inline const_reverse_iterator crend() const noexcept { return rend(); }

		inline size_type size() const noexcept {
			return _end_ptr - _begin_ptr;
		}

		inline size_type max_size() const noexcept { return std::numeric_limits<size_t>::max(); }

		inline size_type capacity() const noexcept {
			return _capacity_ptr - _begin_ptr;
		}

		inline bool empty() const noexcept {
			return _begin_ptr == _end_ptr;
		}

		inline reference front() {
			if (empty())
				throw std::runtime_error("front() called for empty vector");
			return begin()[0];
		}

		inline const_reference front() const {
			if (empty())
				throw std::runtime_error("front() called for empty vector");
			return begin()[0];
		}

		inline reference back() {
			if (empty())
				throw std::runtime_error("back() called for empty vector");
			return end()[-1];
		}

		inline const_reference back() const {
			if (empty())
				throw std::runtime_error("back() called for empty vector");
			return end()[-1];
		}

		inline reference operator[](size_type n) {
			return begin()[n];
		}

		inline const_reference operator[](size_type n) const {
			return begin()[n];
		}

		small_vector_base &operator=(const small_vector_base &v) {
			if (this == &v)
				return *this;

			assign(v.begin(), v.end());

			return *this;
		}

		small_vector_base &operator=(std::initializer_list<value_type> il) {
			assign(il);
			return *this;
		}

		small_vector_base &operator=(small_vector_base &&v) noexcept {
			swap(v);
			return *this;
		}

		inline void reserve(size_type n) {
			if (n > capacity())
				grow(n);
		}

		inline void shrink_to_fit() noexcept {
			shrink(size());
		}

		template<class _input_it_t>
		inline void assign(_input_it_t first, _input_it_t last) {
			clear();

			auto elements = last - first;

			if (capacity() < elements)
				grow(elements);

			set_end(begin() + elements);
			std::uninitialized_copy(first, last, begin());
		}

		inline void assign(size_type n, const value_type &u) {
			clear();

			if (capacity() < n)
				grow(n);

			set_end(begin() + n);
			std::uninitialized_fill(begin(), end(), u);
		}

		inline void assign(std::initializer_list<value_type> il) {
			assign(il.begin(), il.end());
		}

		inline reference at(size_type n) {
			if (n >= size())
				throw std::out_of_range("small_vector");
			return begin()[n];
		}

		inline const_reference at(size_type n) const {
			if (n >= size())
				throw std::out_of_range("small_vector");
			return begin()[n];
		}

		inline void clear() noexcept {
			destroy_range(begin(), end());
			set_end(begin());
		}

		pointer data() noexcept {
			return _begin_ptr;
		}

		const const_pointer data() const noexcept {
			return _begin_ptr;
		}

		inline void push_back(const_reference x) {
			if (end() >= _capacity_ptr)
				grow();

			new(_end_ptr) _t(x);
			set_end(++end());
		}

		inline void push_back(value_type &&x) {
			if (end() >= _capacity_ptr)
				grow();

			new(end()) _t(std::move(x));
			set_end(end() + 1);
		}

		template<class... Args>
		inline void emplace_back(Args &&... args) {
			if (end() >= _capacity_ptr)
				grow();

			new(end()) _t(args...);
			set_end(end() + 1);
		}

		inline void pop_back() {
			if (empty())
				return;

			destroy_range(end() - 1, end());
			set_end(end() - 1);
		}

		template<class... Args>
		inline iterator emplace(const_iterator position, Args &&... args) {
			if (position == end()) {
				emplace_back(args...);
				return end() - 1;
			}

			if (position < begin() || position > end()) {
				throw std::out_of_range("emplace () position out of range");
			}

			size_type offset = position - begin();

			if (_end_ptr == _capacity_ptr)
				grow();

			iterator place = begin() + offset;

			// move items forward
			move_range_reverse(place, end(), end() + 1);

			new(place) _t(args...);

			return place;
		}

		inline iterator insert(const_iterator position, const value_type &x) {
			return emplace(position, x);
		}

		inline iterator insert(const_iterator position, value_type &&x) {
			return emplace(position, std::forward(x));
		}

		inline iterator insert(const_iterator position, size_type n, const value_type &x) {
			size_type offset = position - begin();

			if (position == end()) {
				insert(end(), n, x);
				return begin() + offset;
			}

			if (position < begin() || position > end())
				throw std::out_of_range("insert () position out of range");

			if (capacity() < (size() + n))
				grow(size() + n);

			iterator place = begin() + offset;

			// move items forward
			move_range_reverse(place, end(), end() + n);
			std::uninitialized_fill_n(place, n, x);

			return place;
		}

		template<class _input_it_t>
		iterator insert(const_iterator position, _input_it_t first, _input_it_t last) {

			size_type n = last - first;
			size_type offset = position - begin();

			if (position == end()) {
				insert(end(), first, last);
				return begin() + offset;
			}

			if (position < begin() || position > end())
				throw std::out_of_range("insert () position out of range");

			if (capacity() < (size() + n))
				grow(size() + n);

			auto place = begin() + offset;
			auto end_it = end();

			// move items forward
			move_range_reverse(place, end_it, end_it + n);
			std::copy(first, last, begin() + offset);

			return place;
		}

		inline iterator insert(const_iterator position, std::initializer_list<value_type> il) {
			return insert(position, il.begin(), il.end());
		}

		inline iterator erase(const_iterator position) {

			if (empty())
				return end();

			// is last item
			if (position == end() - 1) {
				pop_back();
				return end();
			}

			if (position < begin() || position > end())
				throw std::out_of_range("erase () position out of range");

			move_range(position + 1, end(), position);
			pop_back();

			return iterator(position);
		}

		inline iterator erase(const_iterator first, const_iterator last) {

			if (first < begin() || first > end())
				throw std::out_of_range("erase () out of range");

			if (last < first)
				throw std::out_of_range("erase () invalid range");

			if (last > end())
				throw std::out_of_range("erase () last past the end");

			iterator place = iterator(first);

			iterator last_place = move_range(last, end(), place);
			destroy_range(last_place, end());

			set_end(last_place);

			return place;
		}

		inline void resize(size_type sz) {
			resize(sz, _t());
		}

		inline void resize(size_type sz, const value_type &c) {
			if (sz > size())
				insert(end(), sz, c);
			else {
				shrink(sz);
			}
		}

		inline void swap(small_vector_base &v) {
			if (this == &v)
				return;

			// if both are "large"
			if (!is_small() && !v.is_small()) {
				// both "large" just swap all buffers
				swap_itrs(v);
			} else {

				// grow smaller one if no fit
				size_type
					this_size = this->size(),
					v_size = v.size();

				small_vector_base &smaller_v = *this;
				size_type larger_size = v_size;

				if (this_size < v_size) {
					smaller_v = v;
					larger_size = this_size;
				}

				if (smaller_v.capacity() < larger_size)
					smaller_v.grow(larger_size);

				// move stuff around
				set_end(begin() + v_size);
				v.set_end(v.begin() + this_size);

				move_range(
					begin(),
					begin() + larger_size,
					v.begin()
				);
			}
		}

		~small_vector_base() {
			destroy_range(begin(), end());
			if (!is_small())
				delete[] _begin_ptr;
		}

	protected:

		inline small_vector_base(size_type n) noexcept {
			_small = {};
			_begin_ptr = _small.location();
			_end_ptr = _begin_ptr;
			_capacity_ptr = _begin_ptr + n;
		}

		inline small_vector_base(size_type n, const_reference v) noexcept :
			small_vector_base(n) {
			assign(n, v);
		}

		small_vector_base(const small_vector_base &v, size_type self_size) :
			small_vector_base(self_size) {
			operator=(v);
		}

		small_vector_base(small_vector_base &&v, size_type self_size) noexcept {
			swap(v);
		}

		small_vector_base(std::initializer_list<value_type> il, size_type self_size) :
			small_vector_base(self_size) {
			assign(il);
		}

		template<class _input_it_t>
		small_vector_base(_input_it_t first, _input_it_t last, size_type self_size) :
			small_vector_base(self_size) {
			assign(first, last);
		}

		inline void grow(size_type n) {

			if (n <= capacity())
				return;

			auto new_begin = reinterpret_cast < pointer > (new uint8_t [sizeof (_t) * n]);
			auto data_size = size();

			if (std::is_trivially_copyable<_t>::value) {
				std::copy(begin(), end(), new_begin);
				destroy_range(begin(), end());
			} else {
				move_range(begin(), end(), new_begin);
			}

			if (!is_small())
				delete[] (begin());

			update_itrs(
				new_begin,
				new_begin + data_size,
				new_begin + n
			);
		}

		inline void grow() {
			grow(capacity() * 2);
		}

		inline void shrink(size_type n) {

			if (is_small() || n >= capacity())
				return;

			auto data_size = std::min(size(), n);
			auto cut_point = begin() + data_size;

			auto new_begin = reinterpret_cast < pointer > (new uint8_t [sizeof (_t) * n]);

			if (std::is_trivially_copyable<_t>::value) {
				std::copy(begin(), cut_point, new_begin);
				destroy_range(begin(), end());
			} else {
				move_range(begin(), cut_point, new_begin);
				destroy_range(cut_point, end());
			}

			delete[] (begin());

			update_itrs(
				new_begin, // begin
				new_begin + data_size, // end
				new_begin + n
			);
		}

		inline bool is_small() const {
			return _begin_ptr == _small.location();
		}

		inline static void destroy(pointer x) {
			x->~_t();
		}

		inline static void destroy_range(pointer b, pointer e) {
			while (b < e) {
				--e;
				e->~_t();
			}
		}

		template<class _input_it_t, class _output_it_t>
		inline static _output_it_t move_range(_input_it_t i, _input_it_t e, _output_it_t d) {
			for (; i < e; ++i, ++d) {
				*d = ::std::move(*i);
			}
			return d;
		};

		template<class _input_it_t, class _output_it_t>
		inline static _output_it_t move_range_reverse(_input_it_t i, _input_it_t e, _output_it_t d_e) {
			for (; i < e; --e, --d_e) {
				*d_e = ::std::move(*(e - 1));
			}
			return d_e;
		};

		inline void swap_itrs(iterator &begin_ptr, iterator &end_ptr, iterator &capacity_ptr) {
			std::swap(_begin_ptr, begin_ptr);
			std::swap(_end_ptr, end_ptr);
			std::swap(_capacity_ptr, capacity_ptr);
		}

		inline void swap_itrs(small_vector_base &v) {
			std::swap(_begin_ptr, v._begin_ptr);
			std::swap(_end_ptr, v._end_ptr);
			std::swap(_capacity_ptr, v._capacity_ptr);
		}

		inline void update_itrs(iterator begin_ptr, iterator end_ptr, iterator capacity_ptr) {
			_begin_ptr = begin_ptr;
			_end_ptr = end_ptr;
			_capacity_ptr = capacity_ptr;
		}

		inline void set_end(iterator end) {
			_end_ptr = end;
		}

		inline iterator capacity_it() {
			return _capacity_ptr;
		}

		pointer _begin_ptr;
		pointer _end_ptr;
		pointer _capacity_ptr;

		__typeless_array<_t, 1> _small;
		// reserved: do not define any variables after _first
	};

	template<class _t, size_t _n>
	struct small_vector : public small_vector_base<_t> {
	public:

		using value_type        = typename small_vector_base<_t>::value_type;
		using const_reference    = typename small_vector_base<_t>::const_reference;

		inline small_vector() noexcept :
			small_vector_base<_t>::small_vector_base(_n) {}

		inline small_vector(const_reference v) noexcept :
			small_vector_base<_t>::small_vector_base(_n, v) {
		}

		template < size_t _v_n >
		inline small_vector(const small_vector<_t, _v_n > & v ) :
			small_vector_base<_t>::small_vector_base(v, _n) {}

		inline small_vector ( const small_vector < _t, _n > & v ) :
			small_vector_base<_t>::small_vector_base(v, _n) {}

		inline small_vector(small_vector_base<_t> &&v) noexcept :
			small_vector_base<_t>::small_vector_base(std::forward(v), _n) {}

		inline small_vector(std::initializer_list<value_type> il) :
			small_vector_base<_t>::small_vector_base(il, _n) {}

		template<class InputIterator>
		inline small_vector(InputIterator first, InputIterator last) :
			small_vector_base<_t>::small_vector_base(first, last, _n) {}

	private:
		__typeless_array<_t, _n - 1> _small_data;
	};

}


#endif