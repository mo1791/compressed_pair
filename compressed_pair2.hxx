//  ------------------------------------
//      Copyright (C) 2018 MO ELomari
//  ------------------------------------


// The compressed pair class is very similar to std::pair, but if either of the
// template arguments are empty classes, then the "empty base-class
// optimization" is applied to compress the size of the pair.
#ifndef __COMPRESSED_PAIR_2_HXX__
#define __COMPRESSED_PAIR_2_HXX__

#include <type_traits>
#include <iostream>



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


/** MAIN CLASS **/
template <typename T1, typename T2>  class compressed_pair
{
public:
    using first_type  = T1;
    using second_type = T2;


public:
    constexpr compressed_pair() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : m_first(), m_second()
    {
    }

    constexpr compressed_pair(const first_type& first) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : m_first(first), m_second()
    {
    }

    constexpr compressed_pair(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : m_first(), m_second(second)
    {
    }
    
    constexpr compressed_pair(
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
    constexpr compressed_pair(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : m_first(std::forward<U1>(first))
        , m_second(std::forward<U2>(second))
    {
    }
    
    template <template <typename...> class U1, typename... U1s,
              template <typename...> class U2, typename... U2s>
    constexpr pair(
        std::piecewise_construct_t, U1<U1s...> const& t1,
        U2<U2s...> const&
            t2) noexcept(std::conjunction<std::is_nothrow_constructible<T1,
                                                                        U1s...>,
                                          std::is_nothrow_constructible<
                                              T2, U2s...>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1s...>,
                                  std::is_constructible<T2, U2s...>>::value)
        : m_first(std::make_from_tuple<T1>(t1))
        , m_second(std::make_from_tuple<T2>(t2))
    {
    }

//    template <typename Tuple1, typename Tuple2>
//    constexpr pair(std::piecewise_construct_t, Tuple1&& tup1, Tuple2&& tup2) noexcept(
//        std::conjunction<
//            unpack_tuple<std::is_nothrow_constructible, T1, Tuple1>,
//            unpack_tuple<std::is_nothrow_constructible, T2, Tuple2>>::value)
//        requires(std::conjunction<
//                    unpack_tuple<std::is_constructible, T1, Tuple1>,
//                    unpack_tuple<std::is_constructible, T2, Tuple2>>::value)
//        : m_first(std::make_from_tuple<T1>(std::forward<Tuple1>(tup1)))
//        , m_second(std::make_from_tuple<T2>(std::forward<Tuple2>(tup2)))
//    {
//    }

public:
    constexpr compressed_pair(const compressed_pair& other) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : m_first(other.m_first), m_second(other.m_second)
    {
    }

    constexpr compressed_pair(compressed_pair&& other) noexcept(
        std::conjunction<std::is_nothrow_move_constructible<T1>,
                         std::is_nothrow_move_constructible<T2>>::value)
        requires(std::conjunction<std::is_move_constructible<T1>,
                                  std::is_move_constructible<T2>>::value)
        : m_first(std::move(other.m_first))
        , m_second(std::move(other.m_second))
    {
    }

    constexpr auto operator=(const compressed_pair& rhs) noexcept(
        std::conjunction<std::is_nothrow_copy_assignable<T1>,
                         std::is_nothrow_copy_assignable<T2>>::value)
        -> compressed_pair&
        requires(std::conjunction<std::is_copy_assignable<T1>,
                                  std::is_copy_assignable<T2>>::value)
    {
        m_first  = rhs.m_first;
        m_second = rhs.m_second;

        return *this;
    }

    constexpr auto operator=(compressed_pair&& rhs) noexcept(
        std::conjunction<std::is_nothrow_move_assignable<T1>,
                         std::is_nothrow_move_assignable<T2>>::value)
        -> compressed_pair&
        requires(std::conjunction<std::is_move_assignable<T1>,
                                  std::is_move_assignable<T2>>::value)
    {
        m_first  = std::move(rhs.m_first);
        m_second = std::move(rhs.m_second);

        return *this;
    }


public:
    constexpr auto first() const noexcept -> const first_type& { return m_first; }
    constexpr auto first()       noexcept ->       first_type& { return m_first; }

    constexpr auto second() const noexcept -> const second_type& { return m_second; }
    constexpr auto second()       noexcept ->       second_type& { return m_second; }


public:
    void swap(compressed_pair& other) noexcept(
        std::conjunction<std::is_nothrow_swappable<T1>,
                         std::is_nothrow_swappable<T2>>::value)
        requires(std::conjunction<std::is_swappable<T1>,
                                  std::is_swappable<T2>>::value)
    {
        using std::swap;
        if constexpr (std::conjunction<std::negation<std::is_empty<T1>>,
                                    std::negation<std::is_empty<T2>>>::value)
        {
            swap(m_first, other.first());
            swap(m_second, other.second());
            return void();
        }

        if constexpr (std::conjunction<std::negation<std::is_empty<T1>>,
                                    std::is_empty<T2>>::value)
        {
            swap(m_first, other.first());
            return void();
        }

        if constexpr (std::conjunction<std::is_empty<T1>,
                                    std::negation<std::is_empty<T2>>>::value)
        {
            swap(m_second, other.second());
            return void();
        }

        if constexpr (std::conjunction<std::is_empty<T1>,
                                    std::is_empty<T2>>::value)
        {
            return void();
        }
    }


private:
    [[no_unique_address]] T1 m_first;
    [[no_unique_address]] T2 m_second;
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