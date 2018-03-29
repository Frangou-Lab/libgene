// -*- C++ -*-
//===------------------------ string_view ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_LFTS_STRING_VIEW
#define _LIBCPP_LFTS_STRING_VIEW

#include <string>
#include <algorithm>
#include <iterator>
#include <ostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <memory>
#include <climits>
#include <limits>
#include <algorithm>
#include <iterator>

namespace experimental {

#ifndef _LIBCPP_ASSERT
#define _LIBCPP_ASSERT(x, m) ((x) ? (void)0 : (std::fprintf(stderr, "%s\n", m), std::abort()))
#endif

    template <class _Size>
    inline _Size
    loadword(const void* __p)
    {
        _Size __r;
        std::memcpy(&__r, __p, sizeof(__r));
        return __r;
    }
    
    template <class _BinaryPredicate, class _RandomAccessIterator1, class _RandomAccessIterator2>
    _RandomAccessIterator1
    search(_RandomAccessIterator1 __first1, _RandomAccessIterator1 __last1,
           _RandomAccessIterator2 __first2, _RandomAccessIterator2 __last2, _BinaryPredicate __pred,
           std::random_access_iterator_tag, std::random_access_iterator_tag)
    {
        typedef typename std::iterator_traits<_RandomAccessIterator1>::difference_type _D1;
        typedef typename std::iterator_traits<_RandomAccessIterator2>::difference_type _D2;
        // Take advantage of knowing source and pattern lengths.  Stop short when source is smaller than pattern
        _D2 __len2 = __last2 - __first2;
        if (__len2 == 0)
            return __first1;
        _D1 __len1 = __last1 - __first1;
        if (__len1 < __len2)
            return __last1;
        const _RandomAccessIterator1 __s = __last1 - (__len2 - 1);  // Start of pattern match can't go beyond here
        while (true)
        {
            for (_D1 __loop_unroll = (__s - __first1) / 4; __loop_unroll > 0; --__loop_unroll)
            {
                if (__pred(*__first1, *__first2))
                    goto __phase2;
                if (__pred(*++__first1, *__first2))
                    goto __phase2;
                if (__pred(*++__first1, *__first2))
                    goto __phase2;
                if (__pred(*++__first1, *__first2))
                    goto __phase2;
                ++__first1;
            }
            switch (__s - __first1)
            {
                case 3:
                    if (__pred(*__first1, *__first2))
                        break;
                    ++__first1;
                case 2:
                    if (__pred(*__first1, *__first2))
                        break;
                    ++__first1;
                case 1:
                    if (__pred(*__first1, *__first2))
                        break;
                case 0:
                    return __last1;
            }
        __phase2:
            _RandomAccessIterator1 __m1 = __first1;
            _RandomAccessIterator2 __m2 = __first2;
            ++__m2;
            ++__m1;
            for (_D2 __loop_unroll = (__last2 - __m2) / 4; __loop_unroll > 0; --__loop_unroll)
            {
                if (!__pred(*__m1, *__m2))
                    goto __continue;
                if (!__pred(*++__m1, *++__m2))
                    goto __continue;
                if (!__pred(*++__m1, *++__m2))
                    goto __continue;
                if (!__pred(*++__m1, *++__m2))
                    goto __continue;
                ++__m1;
                ++__m2;
            }
            switch (__last2 - __m2)
            {
                case 3:
                    if (!__pred(*__m1, *__m2))
                        break;
                    ++__m1;
                    ++__m2;
                case 2:
                    if (!__pred(*__m1, *__m2))
                        break;
                    ++__m1;
                    ++__m2;
                case 1:
                    if (!__pred(*__m1, *__m2))
                        break;
                case 0:
                    return __first1;
            }
        __continue:
            ++__first1;
        }
    }
    
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_find(const _CharT *__p, _SizeT __sz,
               _CharT __c, _SizeT __pos) noexcept
    {
        if (__pos >= __sz)
            return __npos;
        const _CharT* __r = _Traits::find(__p + __pos, __sz - __pos, __c);
        if (__r == 0)
            return __npos;
        return static_cast<_SizeT>(__r - __p);
    }
    
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_find(const _CharT *__p, _SizeT __sz,
             const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
    {
        if (__pos > __sz || __sz - __pos < __n)
            return __npos;
        if (__n == 0)
            return __pos;
        const _CharT* __r =
        search(__p + __pos, __p + __sz,
                    __s, __s + __n, _Traits::eq,
                    std::random_access_iterator_tag(), std::random_access_iterator_tag());
        if (__r == __p + __sz)
            return __npos;
        return static_cast<_SizeT>(__r - __p);
    }
    
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_rfind(const _CharT *__p, _SizeT __sz,
                _CharT __c, _SizeT __pos) noexcept
    {
        if (__sz < 1)
            return __npos;
        if (__pos < __sz)
            ++__pos;
        else
            __pos = __sz;
        for (const _CharT* __ps = __p + __pos; __ps != __p;)
        {
            if (_Traits::eq(*--__ps, __c))
                return static_cast<_SizeT>(__ps - __p);
        }
        return __npos;
    }
    
    template <class _BinaryPredicate, class _RandomAccessIterator1, class _RandomAccessIterator2>
     _RandomAccessIterator1
    find_end(_RandomAccessIterator1 __first1, _RandomAccessIterator1 __last1,
             _RandomAccessIterator2 __first2, _RandomAccessIterator2 __last2, _BinaryPredicate __pred,
             std::random_access_iterator_tag, std::random_access_iterator_tag)
    {
        // Take advantage of knowing source and pattern lengths.  Stop short when source is smaller than pattern
        typename std::iterator_traits<_RandomAccessIterator2>::difference_type __len2 = __last2 - __first2;
        if (__len2 == 0)
            return __last1;
        typename std::iterator_traits<_RandomAccessIterator1>::difference_type __len1 = __last1 - __first1;
        if (__len1 < __len2)
            return __last1;
        const _RandomAccessIterator1 __s = __first1 + (__len2 - 1);  // End of pattern match can't go before here
        _RandomAccessIterator1 __l1 = __last1;
        _RandomAccessIterator2 __l2 = __last2;
        --__l2;
        while (true)
        {
            while (true)
            {
                if (__s == __l1)
                    return __last1;
                if (__pred(*--__l1, *__l2))
                    break;
            }
            _RandomAccessIterator1 __m1 = __l1;
            _RandomAccessIterator2 __m2 = __l2;
            while (true)
            {
                if (__m2 == __first2)
                    return __m1;
                // no need to check range on __m1 because __s guarantees we have enough source
                if (!__pred(*--__m1, *--__m2))
                {
                    break;
                }
            }
        }
    }
    
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_rfind(const _CharT *__p, _SizeT __sz,
                const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
    {
        __pos = std::min(__pos, __sz);
        if (__n < __sz - __pos)
            __pos += __n;
        else
            __pos = __sz;
        const _CharT* __r = find_end(__p, __p + __pos, __s, __s + __n, _Traits::eq,
                                     std::random_access_iterator_tag(), std::random_access_iterator_tag());
        if (__n > 0 && __r == __p + __pos)
            return __npos;
        return static_cast<_SizeT>(__r - __p);
    }

    
    template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
     _ForwardIterator1
    find_first_of_ce(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
                       _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred)
    {
        for (; __first1 != __last1; ++__first1)
            for (_ForwardIterator2 __j = __first2; __j != __last2; ++__j)
                if (__pred(*__first1, *__j))
                    return __first1;
        return __last1;
    }
    
    // __str_find_first_of
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_find_first_of(const _CharT *__p, _SizeT __sz,
                        const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
    {
        if (__pos >= __sz || __n == 0)
            return __npos;
        const _CharT* __r = find_first_of_ce
        (__p + __pos, __p + __sz, __s, __s + __n, _Traits::eq );
        if (__r == __p + __sz)
            return __npos;
        return static_cast<_SizeT>(__r - __p);
    }
    
    
    // __str_find_last_of
    template<class _CharT, class _SizeT, class _Traits, _SizeT __npos>
    _SizeT 
    str_find_last_of(const _CharT *__p, _SizeT __sz,
                    const _CharT* __s, _SizeT __pos, _SizeT __n) noexcept
    {
        if (__n != 0)
        {
            if (__pos < __sz)
                ++__pos;
            else
                __pos = __sz;
            for (const _CharT* __ps = __p + __pos; __ps != __p;)
            {
                const _CharT* __r = _Traits::find(__s, __n, *--__ps);
                if (__r)
                    return static_cast<_SizeT>(__ps - __p);
            }
        }
        return __npos;
    }
    
    // We use murmur2 when size_t is 32 bits, and cityhash64 when size_t
    // is 64 bits.  This is because cityhash64 uses 64bit x 64bit
    // multiplication, which can be very slow on 32-bit systems.
    template <class _Size, size_t = sizeof(_Size)*CHAR_BIT>
    struct murmur2_or_cityhash;
    
    template <class _Size>
    struct murmur2_or_cityhash<_Size, 32>
    {
        _Size operator()(const void* __key, _Size __len);
    };
    
    // murmur2
    template <class _Size>
    _Size
    murmur2_or_cityhash<_Size, 32>::operator()(const void* __key, _Size __len)
    {
        const _Size __m = 0x5bd1e995;
        const _Size __r = 24;
        _Size __h = __len;
        const unsigned char* __data = static_cast<const unsigned char*>(__key);
        for (; __len >= 4; __data += 4, __len -= 4)
        {
            _Size __k = loadword<_Size>(__data);
            __k *= __m;
            __k ^= __k >> __r;
            __k *= __m;
            __h *= __m;
            __h ^= __k;
        }
        switch (__len)
        {
            case 3:
                __h ^= __data[2] << 16;
            case 2:
                __h ^= __data[1] << 8;
            case 1:
                __h ^= __data[0];
                __h *= __m;
        }
        __h ^= __h >> 13;
        __h *= __m;
        __h ^= __h >> 15;
        return __h;
    }
    
    template <class _Size>
    struct murmur2_or_cityhash<_Size, 64>
    {
        _Size operator()(const void* __key, _Size __len);
        
    private:
        // Some primes between 2^63 and 2^64.
        static const _Size __k0 = 0xc3a5c85c97cb3127ULL;
        static const _Size __k1 = 0xb492b66fbe98f273ULL;
        static const _Size __k2 = 0x9ae16a3b2f90404fULL;
        static const _Size __k3 = 0xc949d7c7509e6557ULL;
        
        static _Size __rotate(_Size __val, int __shift) {
            return __shift == 0 ? __val : ((__val >> __shift) | (__val << (64 - __shift)));
        }
        
        static _Size __rotate_by_at_least_1(_Size __val, int __shift) {
            return (__val >> __shift) | (__val << (64 - __shift));
        }
        
        static _Size __shift_mix(_Size __val) {
            return __val ^ (__val >> 47);
        }
        
        static _Size __hash_len_16(_Size __u, _Size __v) {
            const _Size __mul = 0x9ddfea08eb382d69ULL;
            _Size __a = (__u ^ __v) * __mul;
            __a ^= (__a >> 47);
            _Size __b = (__v ^ __a) * __mul;
            __b ^= (__b >> 47);
            __b *= __mul;
            return __b;
        }
        
        static _Size __hash_len_0_to_16(const char* __s, _Size __len) {
            if (__len > 8) {
                const _Size __a = loadword<_Size>(__s);
                const _Size __b = loadword<_Size>(__s + __len - 8);
                return __hash_len_16(__a, __rotate_by_at_least_1(__b + __len, __len)) ^ __b;
            }
            if (__len >= 4) {
                const uint32_t __a = loadword<uint32_t>(__s);
                const uint32_t __b = loadword<uint32_t>(__s + __len - 4);
                return __hash_len_16(__len + (__a << 3), __b);
            }
            if (__len > 0) {
                const unsigned char __a = __s[0];
                const unsigned char __b = __s[__len >> 1];
                const unsigned char __c = __s[__len - 1];
                const uint32_t __y = static_cast<uint32_t>(__a) +
                (static_cast<uint32_t>(__b) << 8);
                const uint32_t __z = __len + (static_cast<uint32_t>(__c) << 2);
                return __shift_mix(__y * __k2 ^ __z * __k3) * __k2;
            }
            return __k2;
        }
        
        static _Size __hash_len_17_to_32(const char *__s, _Size __len) {
            const _Size __a = loadword<_Size>(__s) * __k1;
            const _Size __b = loadword<_Size>(__s + 8);
            const _Size __c = loadword<_Size>(__s + __len - 8) * __k2;
            const _Size __d = loadword<_Size>(__s + __len - 16) * __k0;
            return __hash_len_16(__rotate(__a - __b, 43) + __rotate(__c, 30) + __d,
                                 __a + __rotate(__b ^ __k3, 20) - __c + __len);
        }
        
        // Return a 16-byte hash for 48 bytes.  Quick and dirty.
        // Callers do best to use "random-looking" values for a and b.
        static std::pair<_Size, _Size> __weak_hash_len_32_with_seeds(_Size __w, _Size __x,
                                                                     _Size __y, _Size __z,
                                                                     _Size __a, _Size __b) {
            __a += __w;
            __b = __rotate(__b + __a + __z, 21);
            const _Size __c = __a;
            __a += __x;
            __a += __y;
            __b += __rotate(__a, 44);
            return std::pair<_Size, _Size>(__a + __z, __b + __c);
        }
        
        // Return a 16-byte hash for s[0] ... s[31], a, and b.  Quick and dirty.
        static std::pair<_Size, _Size> __weak_hash_len_32_with_seeds(
                                                                const char* __s, _Size __a, _Size __b) {
            return __weak_hash_len_32_with_seeds(loadword<_Size>(__s),
                                                 loadword<_Size>(__s + 8),
                                                 loadword<_Size>(__s + 16),
                                                 loadword<_Size>(__s + 24),
                                                 __a,
                                                 __b);
        }
        
        // Return an 8-byte hash for 33 to 64 bytes.
        static _Size __hash_len_33_to_64(const char *__s, size_t __len) {
            _Size __z = loadword<_Size>(__s + 24);
            _Size __a = loadword<_Size>(__s) +
            (__len + loadword<_Size>(__s + __len - 16)) * __k0;
            _Size __b = __rotate(__a + __z, 52);
            _Size __c = __rotate(__a, 37);
            __a += loadword<_Size>(__s + 8);
            __c += __rotate(__a, 7);
            __a += loadword<_Size>(__s + 16);
            _Size __vf = __a + __z;
            _Size __vs = __b + __rotate(__a, 31) + __c;
            __a = loadword<_Size>(__s + 16) + loadword<_Size>(__s + __len - 32);
            __z += loadword<_Size>(__s + __len - 8);
            __b = __rotate(__a + __z, 52);
            __c = __rotate(__a, 37);
            __a += loadword<_Size>(__s + __len - 24);
            __c += __rotate(__a, 7);
            __a += loadword<_Size>(__s + __len - 16);
            _Size __wf = __a + __z;
            _Size __ws = __b + __rotate(__a, 31) + __c;
            _Size __r = __shift_mix((__vf + __ws) * __k2 + (__wf + __vs) * __k0);
            return __shift_mix(__r * __k0 + __vs) * __k2;
        }
    };
    
    // cityhash64
    template <class _Size>
    _Size
    murmur2_or_cityhash<_Size, 64>::operator()(const void* __key, _Size __len)
    {
        const char* __s = static_cast<const char*>(__key);
        if (__len <= 32) {
            if (__len <= 16) {
                return __hash_len_0_to_16(__s, __len);
            } else {
                return __hash_len_17_to_32(__s, __len);
            }
        } else if (__len <= 64) {
            return __hash_len_33_to_64(__s, __len);
        }
        
        // For strings over 64 bytes we hash the end first, and then as we
        // loop we keep 56 bytes of state: v, w, x, y, and z.
        _Size __x = loadword<_Size>(__s + __len - 40);
        _Size __y = loadword<_Size>(__s + __len - 16) +
        loadword<_Size>(__s + __len - 56);
        _Size __z = __hash_len_16(loadword<_Size>(__s + __len - 48) + __len,
                                  loadword<_Size>(__s + __len - 24));
        std::pair<_Size, _Size> __v = __weak_hash_len_32_with_seeds(__s + __len - 64, __len, __z);
        std::pair<_Size, _Size> __w = __weak_hash_len_32_with_seeds(__s + __len - 32, __y + __k1, __x);
        __x = __x * __k1 + loadword<_Size>(__s);
        
        // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
        __len = (__len - 1) & ~static_cast<_Size>(63);
        do {
            __x = __rotate(__x + __y + __v.first + loadword<_Size>(__s + 8), 37) * __k1;
            __y = __rotate(__y + __v.second + loadword<_Size>(__s + 48), 42) * __k1;
            __x ^= __w.second;
            __y += __v.first + loadword<_Size>(__s + 40);
            __z = __rotate(__z + __w.first, 33) * __k1;
            __v = __weak_hash_len_32_with_seeds(__s, __v.second * __k1, __x + __w.first);
            __w = __weak_hash_len_32_with_seeds(__s + 32, __z + __w.second,
                                                __y + loadword<_Size>(__s + 16));
            std::swap(__z, __x);
            __s += 64;
            __len -= 64;
        } while (__len != 0);
        return __hash_len_16(
                             __hash_len_16(__v.first, __w.first) + __shift_mix(__y) * __k1 + __z,
                             __hash_len_16(__v.second, __w.second) + __x);
    }

    
template<class _Ptr>
size_t do_string_hash(_Ptr __p, _Ptr __e)
{
    typedef typename std::iterator_traits<_Ptr>::value_type value_type;
    return murmur2_or_cityhash<size_t>()(__p, (__e-__p)*sizeof(value_type));
}

template <class _CharT, class _OutputIterator>
_OutputIterator
pad_and_output(_OutputIterator __s,
                const _CharT* __ob, const _CharT* __op, const _CharT* __oe,
                std::ios_base& __iob, _CharT __fl)
{
    std::streamsize __sz = __oe - __ob;
    std::streamsize __ns = __iob.width();
    if (__ns > __sz)
        __ns -= __sz;
    else
        __ns = 0;
    for (;__ob < __op; ++__ob, ++__s)
        *__s = *__ob;
    for (; __ns; --__ns, ++__s)
        *__s = __fl;
    for (; __ob < __oe; ++__ob, ++__s)
        *__s = *__ob;
    __iob.width(0);
    return __s;
}
    
template<class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
put_character_sequence(std::basic_ostream<_CharT, _Traits>& __os,
                             const _CharT* __str, size_t __len)
{
    try
    {
        typename std::basic_ostream<_CharT, _Traits>::sentry __s(__os);
        if (__s)
        {
            typedef std::ostreambuf_iterator<_CharT, _Traits> _Ip;
            if (pad_and_output(_Ip(__os),
                                 __str,
                                 (__os.flags() & std::ios_base::adjustfield) == std::ios_base::left ?
                                 __str + __len :
                                 __str,
                                 __str + __len,
                                 __os,
                                 __os.fill()).failed())
                __os.setstate(std::ios_base::badbit | std::ios_base::failbit);
        }
    }
    catch (...)
    {
        __os.setstate(std::ios_base::badbit);
    }
    return __os;
}

    
template<class _CharT, class _Traits = std::char_traits<_CharT> >
class basic_string_view {
public:
    // types
    typedef _Traits                                    traits_type;
    typedef _CharT                                     value_type;
    typedef const _CharT*                              pointer;
    typedef const _CharT*                              const_pointer;
    typedef const _CharT&                              reference;
    typedef const _CharT&                              const_reference;
    typedef const_pointer                              const_iterator; // See [string.view.iterators]
    typedef const_iterator                             iterator;
    typedef std::reverse_iterator<const_iterator>      const_reverse_iterator;
    typedef const_reverse_iterator                     reverse_iterator;
    typedef size_t                                     size_type;
    typedef ptrdiff_t                                  difference_type;
    static const size_type npos = -1; // size_type(-1);
    
    // [string.view.cons], construct/copy
     
    basic_string_view() noexcept : __data (nullptr), __size(0) {}
    
     
    basic_string_view(const basic_string_view&) noexcept = default;
    
    
    basic_string_view& operator=(const basic_string_view&) noexcept = default;
    
    template<class _Allocator>
    basic_string_view(const std::basic_string<_CharT, _Traits, _Allocator>& __str) noexcept
    : __data (__str.data()), __size(__str.size()) {}
    
     
    basic_string_view(const _CharT* __s, size_type __len)
    : __data(__s), __size(__len)
    {
        //             _LIBCPP_ASSERT(__len == 0 || __s != nullptr, "string_view::string_view(_CharT *, size_t): recieved nullptr");
    }
    
     
    basic_string_view(const _CharT* __s)
    : __data(__s), __size(_Traits::length(__s)) {}
    
    // [string.view.iterators], iterators
     
    const_iterator begin()  const noexcept { return cbegin(); }
    
     
    const_iterator end()    const noexcept { return cend(); }
    
     
    const_iterator cbegin() const noexcept { return __data; }
    
     
    const_iterator cend()   const noexcept { return __data + __size; }
    
    
    const_reverse_iterator rbegin()   const noexcept { return const_reverse_iterator(cend()); }
    
    
    const_reverse_iterator rend()     const noexcept { return const_reverse_iterator(cbegin()); }
    
    
    const_reverse_iterator crbegin()  const noexcept { return const_reverse_iterator(cend()); }
    
    
    const_reverse_iterator crend()    const noexcept { return const_reverse_iterator(cbegin()); }
    
    // [string.view.capacity], capacity
     
    size_type size()     const noexcept { return __size; }
    
     
    size_type length()   const noexcept { return __size; }
    
     
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max(); }
    
     bool 
    empty()         const noexcept { return __size == 0; }
    
    // [string.view.access], element access
     
    const_reference operator[](size_type __pos) const { return __data[__pos]; }
    
     
    const_reference at(size_type __pos) const
    {
        return __pos >= size()
        ? (throw std::out_of_range("string_view::at"), __data[0])
        : __data[__pos];
    }
    
     
    const_reference front() const
    {
        return _LIBCPP_ASSERT(!empty(), "string_view::front(): string is empty"), __data[0];
    }
    
     
    const_reference back() const
    {
        return _LIBCPP_ASSERT(!empty(), "string_view::back(): string is empty"), __data[__size-1];
    }
    
     
    const_pointer data() const noexcept { return __data; }
    
    // [string.view.modifiers], modifiers:
     
    void clear() noexcept
    {
        __data = nullptr;
        __size = 0;
    }
    
     
    void remove_prefix(size_type __n) noexcept
    {
        _LIBCPP_ASSERT(__n <= size(), "remove_prefix() can't remove more than size()");
        __data += __n;
        __size -= __n;
    }
    
     
    void remove_suffix(size_type __n) noexcept
    {
        _LIBCPP_ASSERT(__n <= size(), "remove_suffix() can't remove more than size()");
        __size -= __n;
    }
    
     
    void swap(basic_string_view& __other) noexcept
    {
        const value_type *__p = __data;
        __data = __other.__data;
        __other.__data = __p;
        
        size_type __sz = __size;
        __size = __other.__size;
        __other.__size = __sz;
        //             std::swap( __data, __other.__data );
        //             std::swap( __size, __other.__size );
    }
    
    // [string.view.ops], string operations:
    template<class _Allocator>
    
    explicit operator std::basic_string<_CharT, _Traits, _Allocator>() const
    { return std::basic_string<_CharT, _Traits, _Allocator>( begin(), end()); }
    
    template<class _Allocator = std::allocator<_CharT> >
    
    std::basic_string<_CharT, _Traits, _Allocator>
    to_string( const _Allocator& __a = _Allocator()) const
    { return std::basic_string<_CharT, _Traits, _Allocator> ( begin(), end(), __a ); }
    
    size_type copy(_CharT* __s, size_type __n, size_type __pos = 0) const
    {
        if ( __pos > size())
            throw std::out_of_range("string_view::copy");
        size_type __rlen = std::min( __n, size() - __pos );
        std::copy_n(begin() + __pos, __rlen, __s );
        return __rlen;
    }
    
    
    basic_string_view substr(size_type __pos = 0, size_type __n = npos) const
    {
        //             if (__pos > size())
        //                 throw out_of_range("string_view::substr");
        //             size_type __rlen = std::min( __n, size() - __pos );
        //             return basic_string_view(data() + __pos, __rlen);
        return __pos > size()
        ? throw std::out_of_range("string_view::substr")
        : basic_string_view(data() + __pos, std::min(__n, size() - __pos));
    }
    
     int compare(basic_string_view __sv) const noexcept
    {
        size_type __rlen = std::min( size(), __sv.size());
        int __retval = _Traits::compare(data(), __sv.data(), __rlen);
        if ( __retval == 0 ) // first __rlen chars matched
            __retval = size() == __sv.size() ? 0 : ( size() < __sv.size() ? -1 : 1 );
        return __retval;
    }
    
     
    int compare(size_type __pos1, size_type __n1, basic_string_view __sv) const
    {
        return substr(__pos1, __n1).compare(__sv);
    }
    
     
    int compare(                       size_type __pos1, size_type __n1,
                basic_string_view _sv, size_type __pos2, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(_sv.substr(__pos2, __n2));
    }
    
     
    int compare(const _CharT* __s) const
    {
        return compare(basic_string_view(__s));
    }
    
     
    int compare(size_type __pos1, size_type __n1, const _CharT* __s) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s));
    }
    
     
    int compare(size_type __pos1, size_type __n1, const _CharT* __s, size_type __n2) const
    {
        return substr(__pos1, __n1).compare(basic_string_view(__s, __n2));
    }
    
    // find
     
    size_type find(basic_string_view __s, size_type __pos = 0) const noexcept
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): recieved nullptr");
        return str_find<value_type, size_type, traits_type, npos>
        (data(), size(), __s.data(), __pos, __s.size());
    }
    
     
    size_type find(_CharT __c, size_type __pos = 0) const noexcept
    {
        return str_find<value_type, size_type, traits_type, npos>
        (data(), size(), __c, __pos);
    }
    
     
    size_type find(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find(): recieved nullptr");
        return str_find<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, __n);
    }
    
     
    size_type find(const _CharT* __s, size_type __pos = 0) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find(): recieved nullptr");
        return str_find<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, traits_type::length(__s));
    }
    
    // rfind
     
    size_type rfind(basic_string_view __s, size_type __pos = npos) const noexcept
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find(): recieved nullptr");
        return str_rfind<value_type, size_type, traits_type, npos>
        (data(), size(), __s.data(), __pos, __s.size());
    }
    
     
    size_type rfind(_CharT __c, size_type __pos = npos) const noexcept
    {
        return str_rfind<value_type, size_type, traits_type, npos>
        (data(), size(), __c, __pos);
    }
    
     
    size_type rfind(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::rfind(): recieved nullptr");
        return str_rfind<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, __n);
    }
    
     
    size_type rfind(const _CharT* __s, size_type __pos=npos) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::rfind(): recieved nullptr");
        return str_rfind<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, traits_type::length(__s));
    }
    
    // find_first_of
     
    size_type find_first_of(basic_string_view __s, size_type __pos = 0) const noexcept
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_first_of(): recieved nullptr");
        return str_find_first_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s.data(), __pos, __s.size());
    }
    
     
    size_type find_first_of(_CharT __c, size_type __pos = 0) const noexcept
    { return find(__c, __pos); }
    
     
    size_type find_first_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_first_of(): recieved nullptr");
        return str_find_first_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, __n);
    }
    
     
    size_type find_first_of(const _CharT* __s, size_type __pos=0) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_first_of(): recieved nullptr");
        return str_find_first_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, traits_type::length(__s));
    }
    
    // find_last_of
     
    size_type find_last_of(basic_string_view __s, size_type __pos=npos) const noexcept
    {
        _LIBCPP_ASSERT(__s.size() == 0 || __s.data() != nullptr, "string_view::find_last_of(): recieved nullptr");
        return str_find_last_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s.data(), __pos, __s.size());
    }
    
     
    size_type find_last_of(_CharT __c, size_type __pos = npos) const noexcept
    { return rfind(__c, __pos); }
    
     
    size_type find_last_of(const _CharT* __s, size_type __pos, size_type __n) const
    {
        _LIBCPP_ASSERT(__n == 0 || __s != nullptr, "string_view::find_last_of(): recieved nullptr");
        return str_find_last_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, __n);
    }
    
     
    size_type find_last_of(const _CharT* __s, size_type __pos=npos) const
    {
        _LIBCPP_ASSERT(__s != nullptr, "string_view::find_last_of(): recieved nullptr");
        return str_find_last_of<value_type, size_type, traits_type, npos>
        (data(), size(), __s, __pos, traits_type::length(__s));
    }
    
private:
    const   value_type* __data;
    size_type           __size;
};


// [string.view.comparison]
// operator ==
template<class _CharT, class _Traits>
bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}

template<class _CharT, class _Traits>
bool operator==(basic_string_view<_CharT, _Traits> __lhs,
                typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}

template<class _CharT, class _Traits>
bool operator==(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size()) return false;
    return __lhs.compare(__rhs) == 0;
}


// operator !=
template<class _CharT, class _Traits>
 
bool operator!=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}

template<class _CharT, class _Traits>
bool operator!=(basic_string_view<_CharT, _Traits> __lhs,
                typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}

template<class _CharT, class _Traits>
bool operator!=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    if ( __lhs.size() != __rhs.size())
        return true;
    return __lhs.compare(__rhs) != 0;
}


// operator <
template<class _CharT, class _Traits>
bool operator<(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT, class _Traits>
bool operator<(basic_string_view<_CharT, _Traits> __lhs,
               typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    return __lhs.compare(__rhs) < 0;
}

template<class _CharT, class _Traits>
bool operator<(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
               basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) < 0;
}


// operator >
template<class _CharT, class _Traits>
bool operator> (basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) > 0;
}

template<class _CharT, class _Traits>
bool operator>(basic_string_view<_CharT, _Traits> __lhs,
               typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    return __lhs.compare(__rhs) > 0;
}

template<class _CharT, class _Traits>
bool operator>(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
               basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) > 0;
}


// operator <=
template<class _CharT, class _Traits>
bool operator<=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) <= 0;
}

template<class _CharT, class _Traits>
bool operator<=(basic_string_view<_CharT, _Traits> __lhs,
                typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    return __lhs.compare(__rhs) <= 0;
}

template<class _CharT, class _Traits>
bool operator<=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) <= 0;
}


// operator >=
template<class _CharT, class _Traits>
bool operator>=(basic_string_view<_CharT, _Traits> __lhs, basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) >= 0;
}


template<class _CharT, class _Traits>
bool operator>=(basic_string_view<_CharT, _Traits> __lhs,
                typename std::common_type<basic_string_view<_CharT, _Traits> >::type __rhs) noexcept
{
    return __lhs.compare(__rhs) >= 0;
}

template<class _CharT, class _Traits>
bool operator>=(typename std::common_type<basic_string_view<_CharT, _Traits> >::type __lhs,
                basic_string_view<_CharT, _Traits> __rhs) noexcept
{
    return __lhs.compare(__rhs) >= 0;
}


// [string.view.io]
template<class _CharT, class _Traits>
std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, basic_string_view<_CharT, _Traits> __sv)
{
    return put_character_sequence(__os, __sv.data(), __sv.size());
}

typedef basic_string_view<char>     string_view;
typedef basic_string_view<char16_t> u16string_view;
typedef basic_string_view<char32_t> u32string_view;
typedef basic_string_view<wchar_t>  wstring_view;

}

namespace std {

template <class _CharT, class _Iter, class _Traits=char_traits<_CharT>>
struct quoted_output_proxy
{
    _Iter  __first;
    _Iter  __last;
    _CharT  __delim;
    _CharT  __escape;
    
    quoted_output_proxy(_Iter __f, _Iter __l, _CharT __d, _CharT __e)
    : __first(__f), __last(__l), __delim(__d), __escape(__e) {}
    //  This would be a nice place for a string_ref
};
    
//// [string.view.hash]
//// Shamelessly stolen from <string>
//template<class _CharT, class _Traits>
//struct hash<experimental::basic_string_view<_CharT, _Traits> >
//: public unary_function<experimental::basic_string_view<_CharT, _Traits>, size_t>
//{
//    size_t operator()(const experimental::basic_string_view<_CharT, _Traits>& __val) const noexcept;
//};
//
//template<class _CharT, class _Traits>
//size_t
//hash<experimental::basic_string_view<_CharT, _Traits> >::operator()(const experimental::basic_string_view<_CharT, _Traits>& __val) const noexcept
//{
//    return do_string_hash(__val.data(), __val.data() + __val.size());
//}

template <class _CharT, class _Traits>
quoted_output_proxy<_CharT, const _CharT *, _Traits>
quoted (experimental::basic_string_view <_CharT, _Traits> __sv,
        _CharT __delim = _CharT('"'), _CharT __escape=_CharT('\\'))
{
    return quoted_output_proxy<_CharT, const _CharT *, _Traits>
    ( __sv.data(), __sv.data() + __sv.size(), __delim, __escape );
}

}

#endif // _LIBCPP_LFTS_STRING_VIEW
