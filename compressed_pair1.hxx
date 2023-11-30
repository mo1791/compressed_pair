//  ------------------------------------
//      Copyright (C) 2018 MO ELomari
//  ------------------------------------

// The compressed pair class is very similar to std::pair, but if either of the
// template arguments are empty classes, then the "empty base-class
// optimization" is applied to compress the size of the pair.

#ifndef __COMPRESSED_PAIR_1_HXX__
#define __COMPRESSED_PAIR_1_HXX__

#include <type_traits>
#include <tuple>

/** UNPACK TUPLE **/

// unpack a std::tuple into a type trait and use its value.
// for cv std::tuple<U> the result is Trait<T, cv U...>::value.
// for cv std::tuple<U>& the result is Trait<T, cv U&...>::value.
// otherwise the result is false (because we don't know if std::get throws).

template <template <typename...> class Trait, typename T, typename Tuple>
struct unpack_tuple : public std::false_type {};

template <template <typename...> class Trait, typename T, typename... U>
struct unpack_tuple<Trait, T, std::tuple<U...>> : public Trait<T, U...> {};

template <template <typename...> class Trait, typename T, typename... U>
struct unpack_tuple<Trait, T, std::tuple<U...>&> : public Trait<T, U&...> {};

template <template <typename...> class Trait, typename T, typename... U>
struct unpack_tuple<Trait, T, const std::tuple<U...>>
    : public Trait<T, const U...> {};

template <template <typename...> class Trait, typename T, typename... U>
struct unpack_tuple<Trait, T, const std::tuple<U...>&>
    : public Trait<T, const U&...> {};

/** END **/

inline constexpr struct zero_then_variadic_args_t {
    constexpr zero_then_variadic_args_t() noexcept(true) = default;
} zero_then_variadic_args{};

inline constexpr struct one_then_variadic_args_t {
    constexpr one_then_variadic_args_t() noexcept(true) = default;
} one_then_variadic_args{};


// FORWARD DECL
template <typename, typename>  class compressed_pair;

template <typename T1, typename T2, bool B1, bool B2>  class compressed_pair_impl;



// COMPRESSED_PAIR HELPER
template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, false, false>
{

public:
    using first_type = T1;
    using second_type = T2;

public:
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : m_first(), m_second()
    {
    }

    constexpr compressed_pair_impl(const first_type& first) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : m_first(first), m_second()
    {
    }

    constexpr compressed_pair_impl(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : m_first(), m_second(second)
    {
    }

    constexpr compressed_pair_impl(
        const first_type& first,
        const second_type&
            second) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_copy_constructible<T1>,
                                     std::is_nothrow_copy_constructible<T2>>::
                                     value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : m_first(first), m_second(second)
    {

    }

    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : m_first(std::forward<U1>(first))
        , m_second(std::forward<U2>(second))
    {
    }


public:
    constexpr auto first() const noexcept -> const first_type& { return m_first; }
    constexpr auto first()       noexcept ->       first_type& { return m_first; }

    constexpr auto second() const noexcept -> const second_type& { return m_second; }
    constexpr auto second()       noexcept ->       second_type& { return m_second; }


public:
    void swap(compressed_pair_impl& other) noexcept(
        std::conjunction<std::is_nothrow_swappable<T1>,
                         std::is_nothrow_swappable<T2>>::value)
        requires(std::conjunction<std::is_swappable<T1>,
                                  std::is_swappable<T2>>::value)
    {
        using std::swap;
        swap(m_first, other.first());
        swap(m_second, other.second());
    }


private:
    T1 m_first;
    T2 m_second;
};



template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, true, false> : private T1
{

public:
    using first_type = T1;
    using second_type = T2;

public:
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T1(), m_second()
    {
    }

    constexpr compressed_pair_impl(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(), m_second(second)
    {
    }

    constexpr compressed_pair_impl(
        const first_type& first,
        const second_type&
            second) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_copy_constructible<T1>,
                                     std::is_nothrow_copy_constructible<T2>>::
                                     value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(first), m_second(second)
    {
    }

    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : T1(std::forward<U1>(first))
        , m_second(std::forward<U2>(second))
    {
    }


public:
    constexpr auto first() const noexcept -> const first_type& { return *this; }
    constexpr auto first()       noexcept ->       first_type& { return *this; }

    constexpr auto second() const noexcept -> const second_type& { return m_second; }
    constexpr auto second()       noexcept ->       second_type& { return m_second; }


public:
    void swap(compressed_pair_impl& other) noexcept(
        std::is_nothrow_swappable<T2>::value)
        requires(std::is_swappable<T2>::value)
    {
        using std::swap;
        swap(m_second, other.second());
    }


private:
    T2 m_second;
};



template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, false, true> : private T2
{
   
public:
    using first_type = T1;
    using second_type = T2;

public:
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T2(), m_first()
    {
    }

    constexpr compressed_pair_impl(const first_type& first) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T2(), m_first(first)
    {
    }

    constexpr compressed_pair_impl(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T2(second), m_first()
    {
    }

    constexpr compressed_pair_impl(
        const first_type& first,
        const second_type&
            second) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_copy_constructible<T1>,
                                     std::is_nothrow_copy_constructible<T2>>::
                                     value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T2(second), m_first(first)
    {
    }

    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : T2(std::forward<U2>(second))
        , m_first(std::forward<U1>(first))
    {
    }


public:
    constexpr auto first() const noexcept -> const first_type& { return m_first; }
    constexpr auto first()       noexcept ->       first_type& { return m_first; }

    constexpr auto second() const noexcept -> const second_type& { return *this; }
    constexpr auto second()       noexcept ->       second_type& { return *this; }


public:
    void swap(compressed_pair_impl& other) noexcept(
        std::is_nothrow_swappable<T1>::value)
        requires(std::is_swappable<T1>::value)
    {
        using std::swap;
        swap(m_first, other.first());
    }


private:
    T1 m_first;
};



template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, true, true> : private T1, private T2
{
   
public:
    using first_type = T1;
    using second_type = T2;
   
public:
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T1(), T2()
    {
    }

    constexpr compressed_pair_impl(const first_type& first) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T1(first), T2()
    {
    }

    constexpr compressed_pair_impl(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(), T2(second)
    {
    }

    constexpr compressed_pair_impl(
        const first_type& first,
        second_type&
            second) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_copy_constructible<T1>,
                                     std::is_nothrow_copy_constructible<T2>>::
                                     value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(first), T2(second)
    {
    }

    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : T1(std::forward<U1>(first))
        , T2(std::forward<U2>(second))
    {
    }


public:
    constexpr auto first() const noexcept -> const first_type& { return *this; }
    constexpr auto first()       noexcept ->       first_type& { return *this; }

    constexpr auto second() const noexcept -> const second_type& { return *this; }
    constexpr auto second()       noexcept ->       second_type& { return *this; }


public:
    void swap(compressed_pair_impl& other) noexcept(
        std::conjunction<std::is_nothrow_swappable<T1>,
                         std::is_nothrow_swappable<T2>>::value)
    {
        return void();
    }
};


template <typename T1, typename T2>
class compressed_pair
    : private compressed_pair_impl<
          T1, T2,
          std::conjunction<std::is_empty<T1>,
                           std::negation<std::is_final<T1>>>::value,
          std::conjunction<std::is_empty<T2>,
                           std::negation<std::is_final<T2>>>::value> 
{
   
public:
    using base_t = compressed_pair_impl<
        T1, T2,
        std::conjunction<std::is_empty<T1>,
                         std::negation<std::is_final<T1>>>::value,
        std::conjunction<std::is_empty<T2>,
                         std::negation<std::is_final<T2>>>::value>;

    using first_type = T1;
    using second_type = T2;

public:
    constexpr compressed_pair() noexcept(
        std::is_nothrow_default_constructible<base_t>::value)
        : base_t()
    {
    }

    constexpr compressed_pair(const first_type& first) noexcept(
        std::is_nothrow_constructible<base_t, T1>::value)
        : base_t(first)
    {
    }

    constexpr compressed_pair(const second_type& second) noexcept(
        std::is_nothrow_constructible<base_t, T2>::value)
        : base_t(second)
    {
    }

    constexpr compressed_pair(
        const first_type& first,
        const second_type&
            second) noexcept(std::is_nothrow_constructible<base_t, T1,
                                                           T2>::value)
        : base_t(first, second)
    {
    }

    template <typename U1, typename U2>
    constexpr compressed_pair(U1&& first, U2&& second) noexcept(
        std::is_nothrow_constructible<base_t, T1, T2>::value)
        : base_t(std::forward<U1>(first), std::forward<U2>(second))
    {
    }

    template <template <typename...> class Tuple1, typename... Ts1,
              template <typename...> class Tuple2, typename... Ts2>
    constexpr compressed_pair(
        std::piecewise_construct_t, const Tuple1<Ts1...>& tuple1,
        const Tuple2<Ts2...>&
            tuple2) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_constructible<T1, Ts1...>,
                                     std::is_nothrow_constructible<
                                         T2, Ts2...>>::value)
        requires(std::conjunction<std::is_constructible<T1, Ts1...>,
                                  std::is_constructible<T2, Ts2...>>::value)
        : base_t(std::make_from_tuple<first_type>(tuple1),
                 std::make_from_tuple<second_type>(tuple2))
    {
    }

//    template <typename Tuple1, typename Tuple2>
//    constexpr compressed_pair(
//        std::piecewise_construct_t, Tuple1&& tuple1,
//        Tuple2&&
//            tuple2) noexcept(std::
//                                 conjunction<
//                                     unpack_tuple<std::is_nothrow_constructible,
//                                                  T1, Tuple1>,
//                                     unpack_tuple<std::is_nothrow_constructible,
//                                                  T2, Tuple2>>::value)
//        requires(std::conjunction<
//                 unpack_tuple<std::is_constructible, T1, Tuple1>,
//                 unpack_tuple<std::is_constructible, T2, Tuple2>>::value)
//        : base_t(
//              std::make_from_tuple<first_type>(std::forward<Tuple1>(tuple1)),
//              std::make_from_tuple<second_type>(std::forward<Tuple2>(tuple2)))
//    {
//    }

public:
    constexpr compressed_pair(const compressed_pair& other) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : base_t(other.first(), other.second())
    {
    }

    constexpr compressed_pair(compressed_pair&& other) noexcept(
        std::conjunction<std::is_nothrow_move_constructible<T1>,
                         std::is_nothrow_move_constructible<T2>>::value)
        requires(std::conjunction<std::is_move_constructible<T1>,
                                  std::is_move_constructible<T2>>::value)
        : base_t(std::move(other.first()), std::move(other.second()))
    {
    }

    constexpr auto operator=(const compressed_pair& rhs) noexcept(
        std::conjunction<std::is_nothrow_copy_assignable<T1>,
                         std::is_nothrow_copy_assignable<T2>>::value)
        -> compressed_pair&
        requires(std::conjunction<std::is_copy_assignable<T1>,
                                  std::is_copy_assignable<T2>>::value)
    {
        base_t::first() = rhs.first();
        base_t::second() = rhs.second();

        return *this;
    }

    constexpr auto operator=(compressed_pair&& rhs) noexcept(
        std::conjunction<std::is_nothrow_move_assignable<T1>,
                         std::is_nothrow_move_assignable<T2>>::value)
        -> compressed_pair&
        requires(std::conjunction<std::is_move_assignable<T1>,
                                  std::is_move_assignable<T2>>::value)
    {
        base_t::first() = std::move(rhs.first());
        base_t::second() = std::move(rhs.second());

        return *this;
    }

public:
    constexpr auto first() const noexcept -> const first_type& { return base_t::first(); }
    constexpr auto first()       noexcept ->       first_type& { return base_t::first(); }

    constexpr auto second() const noexcept -> const second_type& { return base_t::second(); }
    constexpr auto second()       noexcept ->       second_type& { return base_t::second(); }


public:
    void swap(compressed_pair& other) noexcept(
        std::conjunction<std::is_nothrow_swappable<T1>,
                         std::is_nothrow_swappable<T2>>::value)
        requires(std::conjunction<std::is_swappable<T1>,
                                  std::is_swappable<T2>>::value)
    {
        base_t::swap(other);
    }
};


// partial specialisation for case where T1 == T2:
// there is only one version of the single-argument constructor, and this
// constructor initialises both values in the pair to the passed value.
template <typename T>
class compressed_pair<T, T>
    : private compressed_pair_impl<
          T, T,
          std::conjunction<std::is_empty<T>,
                           std::negation<std::is_final<T>>>::value,
          std::conjunction<std::is_empty<T>,
                           std::negation<std::is_final<T>>>::value>
{

public:
    using base_t = compressed_pair_impl<
        T, T,
        std::conjunction<std::is_empty<T>,
                         std::negation<std::is_final<T>>>::value,
        std::conjunction<std::is_empty<T>,
                         std::negation<std::is_final<T>>>::value>;

    using first_type = T;
    using second_type = T;


public:
    constexpr compressed_pair() noexcept(
        std::is_nothrow_default_constructible<base_t>::value)
        : base_t()
    {
    }

    template <typename U1, typename U2>
    constexpr compressed_pair(U1&& first, U2&& second) noexcept(
        std::is_nothrow_constructible<base_t, T, T>::value)
        : base_t(std::forward<U1>(first), std::forward<U2>(second))
    {
    }

    template <template <typename...> class Tuple1, typename... Ts1,
              template <typename...> class Tuple2, typename... Ts2>
    constexpr compressed_pair(
        std::piecewise_construct_t, const Tuple1<Ts1...>& tuple1,
        const Tuple2<Ts2...>&
            tuple2) noexcept(std::
                                 conjunction<
                                     std::is_nothrow_constructible<T, Ts1...>,
                                     std::is_nothrow_constructible<T, Ts2...>>::
                                     value)
        requires(std::conjunction<std::is_constructible<T, Ts1...>,
                                  std::is_constructible<T, Ts2...>>::value)
        : base_t(std::make_from_tuple<T>(tuple1),
                 std::make_from_tuple<T>(tuple2))
    {
    }


//    template <typename Tuple1, typename Tuple2>
//    constexpr compressed_pair(
//        std::piecewise_construct_t, Tuple1&& tuple1,
//        Tuple2&&
//            tuple2) noexcept(std::
//                                 conjunction<
//                                     unpack_tuple<std::is_nothrow_constructible,
//                                                  T, Tuple1>,
//                                     unpack_tuple<std::is_nothrow_constructible,
//                                                  T, Tuple2>>::value)
//        requires(std::conjunction<
//                 unpack_tuple<std::is_constructible, T, Tuple1>,
//                 unpack_tuple<std::is_constructible, T, Tuple2>>::value)
//        : base_t(std::make_from_tuple<T>(std::forward<Tuple1>(tuple1)),
//                 std::make_from_tuple<T>(std::forward<Tuple2>(tuple2))) {}


public:
    constexpr compressed_pair(const compressed_pair& other) noexcept(
        std::is_nothrow_copy_constructible<T>::value)
        requires(std::is_copy_constructible<T>::value)
        : base_t(other.first(), other.second())
    {
    }

    constexpr compressed_pair(compressed_pair&& other) noexcept(
        std::is_nothrow_move_constructible<T>::value)
        requires(std::is_move_constructible<T>::value)
        : base_t(std::move(other.first()), std::move(other.second()))
    {
    }

    constexpr auto operator=(const compressed_pair& rhs) noexcept(
        std::is_nothrow_copy_assignable<T>::value) -> compressed_pair&
        requires(std::is_copy_assignable<T>::value)
    {
        base_t::first() = rhs.first();
        base_t::second() = rhs.second();

        return *this;
    }

    constexpr auto operator=(compressed_pair&& rhs) noexcept(
        std::is_nothrow_move_assignable<T>::value) -> compressed_pair&
        requires(std::is_move_assignable<T>::value)
    {
        base_t::first() = std::move(rhs.first());
        base_t::second() = std::move(rhs.second());

        return *this;
    }

public:
    constexpr auto first() const noexcept -> const first_type& { return base_t::first(); }
    constexpr auto first()       noexcept ->       first_type& { return base_t::first(); }

    constexpr auto second() const noexcept -> const second_type& { return base_t::second(); }
    constexpr auto second()       noexcept ->       second_type& { return base_t::second(); }


public:
    void swap(compressed_pair& other) noexcept(
        std::is_nothrow_swappable<T>::value)
        requires(std::is_swappable<T>::value)
    {
        base_t::swap(other);
    }
};



// C++ structured binding support
namespace std
{
    template <typename T, typename U>
    struct tuple_size<::compressed_pair<T, U>> : integral_constant<size_t, 2> {};

    template <size_t Index, typename T, typename U>
    struct tuple_element<Index, ::compressed_pair<T, U>> : tuple_element<Index, tuple<T, U>> {};

}  // namespace std


template <std::size_t Index, typename T1, typename T2>
constexpr auto get(compressed_pair<T1, T2>& my_pair) ->
    typename std::tuple_element<Index, compressed_pair<T1, T2>>::type&
{
    if constexpr (Index == 0) return my_pair.first();
    if constexpr (Index == 1) return my_pair.second();
}

template <std::size_t Index, typename T1, typename T2>
constexpr auto get(const compressed_pair<T1, T2>& my_pair) ->
    typename std::tuple_element<Index, compressed_pair<T1, T2>>::type const&
{
    if constexpr (Index == 0) return my_pair.first();
    if constexpr (Index == 1) return my_pair.second();
}

template <std::size_t Index, typename T1, typename T2>
constexpr auto get(compressed_pair<T1, T2>&& my_pair) ->
    typename std::tuple_element<Index, compressed_pair<T1, T2>>::type&&
{
    if constexpr (Index == 0) return std::move(my_pair.first());
    if constexpr (Index == 1) return std::move(my_pair.second());
}

template <std::size_t Index, typename T1, typename T2>
constexpr auto get(const compressed_pair<T1, T2>&& my_pair) ->
    typename std::tuple_element<Index, compressed_pair<T1, T2>>::type const&&
{
    if constexpr (Index == 0) return std::move(my_pair.first());
    if constexpr (Index == 1) return std::move(my_pair.second());
}

#endif

