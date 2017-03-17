//
// Created by Luciano Silva on 17/03/2017.
//

#ifndef _bgen_common_pool_h_
#define _bgen_common_pool_h_

#include <vector>

#include "bgen_common.h"

namespace bgen {
	namespace common {

		template < class _t >
		class pool_span {
		private:

			vector < _t > * _source;
			size_t 			_start_index;
			size_t 			_end_index;

		public:

			inline pool_span ( vector < _t > & source, size_t start_index, size_t end_index ) :
				_source (&source),
				_start_index (start_index),
				_end_index (end_index)
			{}

			class iterator {
			private:
				vector < _t > * _source;
				size_t 			_index;
				size_t 			_end;
			public:

				inline iterator ( vector < _t > * source, size_t index, size_t end ) :
					_source (source),
					_index (index),
					_end (end)
				{}

				inline iterator operator++() {
					return { _source, ++_index, _end };
				}

				inline _t & operator * () {
					return _source->operator[](_index);
				}

				inline bool operator != (iterator const & v) const {
					return !(_source == v._source &&
							 _index == v._index &&
							 _end == v._end);
				}

			};

			iterator begin () {
				return { _source, _start_index, _end_index };
			}

			iterator end () {
				return { _source, _end_index, _end_index };
			}

		};

		template < class _t >
		class pool {
		public:

			using vector_type = vector < _t >;

			using value_type =      typename vector_type::value_type;
			using pointer =         typename vector_type::pointer;
			using const_pointer =   typename vector_type::const_pointer;
			using reference =       typename vector_type::reference;
			using const_reference = typename vector_type::const_reference;
			using iterator =        typename vector_type::iterator;
			using const_iterator =  typename vector_type::const_iterator;
			using reverse_iterator = typename vector_type::reverse_iterator;
			using const_reverse_iterator = typename vector_type::const_reverse_iterator;
			using size_type =       typename vector_type::size_type;
			using difference_type = typename vector_type::difference_type;

		private:

			vector_type _data;

		public:

			inline pool_span < _t > push_back ( const vector < _t > & items ) {
				_data.insert(_data.end (), items.begin (), items.end());
				return { _data, _data.size () - items.size (), _data.size () };
			}

			inline reference at (size_type index) {
				return _data.at(index);
			}

			inline reference operator[](size_type index) {
				return _data[index];
			}

			inline size_type size () const { return _data.size (); }

			inline iterator begin () { return _data.begin (); }
			inline iterator end () { return _data.end(); }

			inline const_iterator begin () const { return _data.begin (); }
			inline const_iterator end () const { return _data.end (); }

			inline reverse_iterator rbegin () { return _data.rbegin (); }
			inline reverse_iterator rend () { return _data.rend (); }

			inline const_reverse_iterator rbegin () const { return _data.rbegin (); }
			inline const_reverse_iterator rend () const { return _data.rend (); }

		};

	}
}

#endif //_bgen_common_pool_h_
