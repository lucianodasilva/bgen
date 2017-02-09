#pragma once 
#ifndef _bgen_indexed_vector_h_
#define _bgen_indexed_vector_h_

#include <vector>
#include <unordered_map>
#include <cinttypes>
#include <limits>

using namespace std; 

namespace bgen {

    const size_t indexed_vector_nindex = std::numeric_limits < size_t >::max();

    template < class _key_t, class _value_t >
    class indexed_vector {
    private:

        using map_type = unordered_map < _key_t, size_t >;
        using vector_type = vector < _value_t >;

        map_type _indexes;
        vector_type _vector;

    public:

        using value_type =      typename vector_type::value_type;
        using key_type =        typename map_type::key_type;
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
        using allocator_type =  typename vector_type::allocator_type;

        static const size_type nindex = indexed_vector_nindex;

        inline iterator begin() { return _vector.begin(); }
        inline const_iterator begin () const { return _vector.begin (); }

        inline iterator end() { return _vector.end (); }
        const_iterator end() const { return _vector.end (); }

        reverse_iterator rbegin() { return _vector.rbegin (); }
        const_reverse_iterator rbegin() const { return _vector.rend(); }

        reverse_iterator rend() { return _vector.rend(); }
        const_reverse_iterator rend() const{ return _vector.rend(); }

        inline void push_back (const key_type & key, const value_type & value) {
            size_type index = _vector.size();
            _indexes[key] = index;
            _vector.push_back (value);
        }

        template < class ... _tv >
        pair < size_type, reference > emplace_back (const key_type & key, _tv && ... args) {
            size_type index = _vector.size();
            _indexes[key] = index;
            _vector.emplace_back (args...);
            return { index, _vector.back () };
        }

        inline reference operator [] (size_type index) {
            return _vector[index];
        } 

        inline const_reference operator [] (size_type index) const {
            return _vector[index];
        }

        inline size_type find_index(const key_type & key) const {
            auto map_it = _indexes.find (key);

            if (map_it != _indexes.end())
                return map_it->second;
            else
                return nindex;
        }

        inline iterator find(const key_type & key) {
            size_type index = find_index (key);

            if (index == nindex)
                return _vector.end();
            else
                return _vector.begin () + index;
        }

        const_iterator find(const key_type & key) const {
            size_type index = find_index (key);

            if (index == nindex)
                return _vector.end();
            else
                return _vector.begin () + index;
        }

        pair < size_type, reference > find_or_emplace (const key_type & key) {
            size_type index = find_index(key);

            if (index == nindex) {
                return emplace_back (key);
            } else {
                return { index, _vector[index] };
            }
        }

    };

}

#endif