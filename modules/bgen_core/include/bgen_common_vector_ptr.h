//
// Created by Luciano Silva on 17/03/2017.
//

#ifndef _bgen_common_vector_ptr_h_
#define _bgen_common_vector_ptr_h_

#include <vector>

using namespace std;

namespace bgen {
	namespace common {

		template < class _t >
		class vector_ptr {
		public:

			using value_type = _t;
			using reference = _t &;
			using vector_type = vector < _t >;

		private:

			vector_type * 	_source = nullptr;
			size_t 			_index;

		public:

			vector_ptr ( vector_type & source, size_t index ) :
				_source (&source),
				_index (index)
			{}

			inline reference operator-> () { return _source->operator[](_index); }
			inline reference operator * () { return _source->operator[](_index); }

			inline operator bool () const { return _source == nullptr; }

			inline void reset () {
				new () vector_ptr (*this);
			}
		};

	}
}

#endif //_bgen_common_vector_ptr_h_
