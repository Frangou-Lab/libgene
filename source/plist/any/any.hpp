// -*- C++ -*-
//===------------------------------ any -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_EXPERIMENTAL_ANY
#define _LIBCPP_EXPERIMENTAL_ANY

#include <memory>
#include <new>
#include <typeinfo>
#include <type_traits>
#include <cstdlib>

namespace experimental {

class bad_any_cast : public std::bad_cast
{
public:
    virtual const char* what() const noexcept
    {
        return "bad_cast";
    }
};

static inline void __throw_bad_any_cast()
{
    throw bad_any_cast();
}

// Forward declarations
class any;

template <class _ValueType>
    typename std::add_pointer<typename std::add_const<_ValueType>::type>::type
any_cast(any const *) noexcept;

template <class _ValueType>
typename std::add_pointer<_ValueType>::type

any_cast(any *) noexcept;

namespace __any_imp
{
    template <class _Alloc>
    class allocator_destructor
    {
        typedef std::allocator_traits<_Alloc> __alloc_traits;
    public:
        typedef typename __alloc_traits::pointer pointer;
        typedef typename __alloc_traits::size_type size_type;
    private:
        _Alloc& __alloc_;
        size_type __s_;
    public:
        allocator_destructor(_Alloc& __a, size_type __s) noexcept
        : __alloc_(__a), __s_(__s) {}
        void operator()(pointer __p) noexcept
        {__alloc_traits::deallocate(__alloc_, __p, __s_);}
    };

    
  typedef typename std::aligned_storage<3*sizeof(void*), std::alignment_of<void*>::value>::type
    _Buffer;

  template <class _Tp>
  struct _IsSmallObject
    : public std::integral_constant<bool
        , sizeof(_Tp) <= sizeof(_Buffer)
          && std::alignment_of<_Buffer>::value
             % std::alignment_of<_Tp>::value == 0
          && std::is_nothrow_move_constructible<_Tp>::value
        >
  {};

  enum class _Action
  {
    _Destroy,
    _Copy,
    _Move,
    _Get,
    _TypeInfo
  };

  template <class _Tp>
  struct _SmallHandler;

  template <class _Tp>
  struct _LargeHandler;

  template <class _Tp>
  using _Handler = typename std::conditional<_IsSmallObject<_Tp>::value
                                      , _SmallHandler<_Tp>
                                      , _LargeHandler<_Tp>
                                    >::type;
  template <class _ValueType>
  using _EnableIfNotAny = typename
    std::enable_if<
      !std::is_same<typename std::decay<_ValueType>::type, any>::value
    >::type;

} // namespace __any_imp

class any
{
public:
  // 6.3.1 any construct/destruct
  
  any() noexcept : __h(nullptr) {}

  
  any(any const & __other) : __h(nullptr)
  {
    if (__other.__h) __other.__call(_Action::_Copy, this);
  }

  
  any(any && __other) noexcept : __h(nullptr)
  {
    if (__other.__h) __other.__call(_Action::_Move, this);
  }

  template <
      class _ValueType
    , class = __any_imp::_EnableIfNotAny<_ValueType>
    >
  
  any(_ValueType && __value);

  
  ~any()
  {
    this->clear();
  }

  // 6.3.2 any assignments
  
  any & operator=(any const & __rhs)
  {
    any(__rhs).swap(*this);
    return *this;
  }

  
  any & operator=(any && __rhs) noexcept
  {
    any(std::move(__rhs)).swap(*this);
    return *this;
  }

  template <
      class _ValueType
    , class = __any_imp::_EnableIfNotAny<_ValueType>
    >
  
  any & operator=(_ValueType && __rhs);

  // 6.3.3 any modifiers
  
  void clear() noexcept
  {
    if (__h) this->__call(_Action::_Destroy);
  }

  
  void swap(any & __rhs) noexcept;

  // 6.3.4 any observers
  
  bool empty() const noexcept
  {
    return __h == nullptr;
  }
    
  const std::type_info & type() const noexcept
  {
    if (__h) {
        return *static_cast<std::type_info const *>(this->__call(_Action::_TypeInfo));
    } else {
        return typeid(void);
    }
  }

private:
    typedef __any_imp::_Action _Action;

    typedef void* (*_HandleFuncPtr)(_Action, any const *, any *, const std::type_info *);

    union _Storage
    {
        void *  __ptr;
        __any_imp::_Buffer __buf;
    };

    
    void * __call(_Action __a, any * __other = nullptr,
                  std::type_info const * __info = nullptr) const
    {
        return __h(__a, this, __other, __info);
    }

    
    void * __call(_Action __a, any * __other = nullptr,
                  std::type_info const * __info = nullptr)
    {
        return __h(__a, this, __other, __info);
    }

    template <class>
    friend struct __any_imp::_SmallHandler;
    template <class>
    friend struct __any_imp::_LargeHandler;

    template <class _ValueType>
    friend typename std::add_pointer<typename std::add_const<_ValueType>::type>::type
    any_cast(any const *) noexcept;

    template <class _ValueType>
    friend typename std::add_pointer<_ValueType>::type
    any_cast(any *) noexcept;

    _HandleFuncPtr __h;
    _Storage __s;
};

namespace __any_imp
{

  template <class _Tp>
  struct _SmallHandler
  {
     
     static void* __handle(_Action __act, any const * __this, any * __other,
                           std::type_info const * __info)
     {
        switch (__act)
        {
        case _Action::_Destroy:
          __destroy(const_cast<any &>(*__this));
          return nullptr;
        case _Action::_Copy:
            __copy(*__this, *__other);
            return nullptr;
        case _Action::_Move:
          __move(const_cast<any &>(*__this), *__other);
          return nullptr;
        case _Action::_Get:
            return __get(const_cast<any &>(*__this), __info);
        case _Action::_TypeInfo:
          return __type_info();
        }
    }

    template <class _Up>
    
    static void __create(any & __dest, _Up && __v)
    {
        ::new (static_cast<void*>(&__dest.__s.__buf)) _Tp(std::forward<_Up>(__v));
        __dest.__h = &_SmallHandler::__handle;
    }

  private:
     
    static void __destroy(any & __this)
    {
        _Tp & __value = *static_cast<_Tp *>(static_cast<void*>(&__this.__s.__buf));
        __value.~_Tp();
        __this.__h = nullptr;
    }

     
    static void __copy(any const & __this, any & __dest)
    {
        _SmallHandler::__create(__dest, *static_cast<_Tp const *>(
            static_cast<void const *>(&__this.__s.__buf)));
    }

     
    static void __move(any & __this, any & __dest)
    {
        _SmallHandler::__create(__dest, std::move(
            *static_cast<_Tp*>(static_cast<void*>(&__this.__s.__buf))));
        __destroy(__this);
    }

     
    static void* __get(any & __this, std::type_info const * __info)
    {
        if (typeid(_Tp) == *__info) {
            return static_cast<void*>(&__this.__s.__buf);
        }
        return nullptr;
    }

     
    static void* __type_info()
    {
        return const_cast<void*>(static_cast<void const *>(&typeid(_Tp)));
    }
  };

  template <class _Tp>
  struct _LargeHandler
  {
    
    static void* __handle(_Action __act, any const * __this, any * __other,
                          std::type_info const * __info)
    {
        switch (__act)
        {
        case _Action::_Destroy:
          __destroy(const_cast<any &>(*__this));
          return nullptr;
        case _Action::_Copy:
          __copy(*__this, *__other);
          return nullptr;
        case _Action::_Move:
          __move(const_cast<any &>(*__this), *__other);
          return nullptr;
        case _Action::_Get:
            return __get(const_cast<any &>(*__this), __info);
        case _Action::_TypeInfo:
          return __type_info();
        }
    }

    template <class _Up>
    
    static void __create(any & __dest, _Up && __v)
    {
        typedef std::allocator<_Tp> _Alloc;
        typedef allocator_destructor<_Alloc> _Dp;
        _Alloc __a;
        std::unique_ptr<_Tp, _Dp> __hold(__a.allocate(1), _Dp(__a, 1));
        ::new ((void*)__hold.get()) _Tp(std::forward<_Up>(__v));
        __dest.__s.__ptr = __hold.release();
        __dest.__h = &_LargeHandler::__handle;
    }

  private:

     
    static void __destroy(any & __this)
    {
        delete static_cast<_Tp*>(__this.__s.__ptr);
        __this.__h = nullptr;
    }

     
    static void __copy(any const & __this, any & __dest)
    {
        _LargeHandler::__create(__dest, *static_cast<_Tp const *>(__this.__s.__ptr));
    }

     
    static void __move(any & __this, any & __dest)
    {
      __dest.__s.__ptr = __this.__s.__ptr;
      __dest.__h = &_LargeHandler::__handle;
      __this.__h = nullptr;
    }

     
    static void* __get(any & __this, std::type_info const * __info)
    {
        if (typeid(_Tp) == *__info) {
            return static_cast<void*>(__this.__s.__ptr);
        }
        return nullptr;
    }

     
    static void* __type_info()
    {
        return const_cast<void*>(static_cast<void const *>(&typeid(_Tp)));
    }
  };

} // namespace __any_imp


template <class _ValueType, class>
any::any(_ValueType && __v) : __h(nullptr)
{
  typedef typename std::decay<_ValueType>::type _Tp;
  static_assert(std::is_copy_constructible<_Tp>::value,
                "_ValueType must be CopyConstructible.");
  typedef __any_imp::_Handler<_Tp> _HandlerType;
  _HandlerType::__create(*this, std::forward<_ValueType>(__v));
}

template <class _ValueType, class>
any & any::operator=(_ValueType && __v)
{
  typedef typename std::decay<_ValueType>::type _Tp;
  static_assert(std::is_copy_constructible<_Tp>::value,
                "_ValueType must be CopyConstructible.");
  any(std::forward<_ValueType>(__v)).swap(*this);
  return *this;
}

inline
void any::swap(any & __rhs) noexcept
{
    if (__h && __rhs.__h) {
        any __tmp;
        __rhs.__call(_Action::_Move, &__tmp);
        this->__call(_Action::_Move, &__rhs);
        __tmp.__call(_Action::_Move, this);
    }
    else if (__h) {
        this->__call(_Action::_Move, &__rhs);
    }
    else if (__rhs.__h) {
        __rhs.__call(_Action::_Move, this);
    }
}

// 6.4 Non-member functions

inline 
void swap(any & __lhs, any & __rhs) noexcept
{
    __lhs.swap(__rhs);
}

template <class _ValueType>

_ValueType any_cast(any const & __v)
{
    static_assert(
        std::is_reference<_ValueType>::value
        || std::is_copy_constructible<_ValueType>::value,
        "_ValueType is required to be a reference or a CopyConstructible type.");
    typedef typename std::add_const<typename std::remove_reference<_ValueType>::type>::type
            _Tp;
    _Tp * __tmp = any_cast<_Tp>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return *__tmp;
}

template <class _ValueType>

_ValueType any_cast(any & __v)
{
    static_assert(
        std::is_reference<_ValueType>::value
        || std::is_copy_constructible<_ValueType>::value,
        "_ValueType is required to be a reference or a CopyConstructible type.");
    typedef typename std::remove_reference<_ValueType>::type _Tp;
    _Tp * __tmp = any_cast<_Tp>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return *__tmp;
}

template <class _ValueType>

_ValueType any_cast(any && __v)
{
    static_assert(
        std::is_reference<_ValueType>::value
        || std::is_copy_constructible<_ValueType>::value,
        "_ValueType is required to be a reference or a CopyConstructible type.");
    typedef typename std::remove_reference<_ValueType>::type _Tp;
    _Tp * __tmp = any_cast<_Tp>(&__v);
    if (__tmp == nullptr)
        __throw_bad_any_cast();
    return *__tmp;
}

template <class _ValueType>
inline
typename std::add_pointer<typename std::add_const<_ValueType>::type>::type
any_cast(any const * __any) noexcept
{
    static_assert(!std::is_reference<_ValueType>::value,
                  "_ValueType may not be a reference.");
    return any_cast<_ValueType>(const_cast<any *>(__any));
}

template <class _ValueType>
typename std::add_pointer<_ValueType>::type
any_cast(any * __any) noexcept
{
    using __any_imp::_Action;
    static_assert(!std::is_reference<_ValueType>::value,
                  "_ValueType may not be a reference.");
    typedef typename std::add_pointer<_ValueType>::type _ReturnType;
    if (__any && __any->__h) {

        return static_cast<_ReturnType>(
            __any->__call(_Action::_Get, nullptr, &typeid(_ValueType)
        ));

    }
    return nullptr;
}

} // namespace exp
    
#endif // _LIBCPP_EXPERIMENTAL_ANY
