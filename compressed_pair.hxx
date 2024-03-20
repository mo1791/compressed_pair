//  ------------------------------------
//      Copyright (C) 2018 MO ELomari
//  ------------------------------------

// The compressed pair class is very similar to std::pair, but if either of the
// template arguments are empty classes, then the "empty base-class
// optimization" is applied to compress the size of the pair.

#ifndef __COMPRESSED_PAIR_HXX__
#define __COMPRESSED_PAIR_HXX__

#include <tuple>
#include <type_traits>


namespace detail {

// type trait to test if a class is a specialization of another class
template <typename, template <typename...> class>
struct is_specialization_of: public std::false_type {};

template <template <typename...> class Primary, typename... ARGS>
struct is_specialization_of<Primary<ARGS...>, Primary>: public std::true_type {};

template <typename T, template<typename...> class Primary>
concept specialization_of = is_specialization_of<typename std::decay<T>::type, Primary>::value;


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

}  // namespace detail

/** END **/



// FORWARD DECL
template <typename, typename> class compressed_pair;

template <typename T1, typename T2, bool B1, bool B2> class compressed_pair_impl;


// COMPRESSED_PAIR HELPER

// #1 partial specialization where T1 and T2  != empty
template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, false, false>
{
   
public:
    using first_type  = T1;
    using second_type = T2;


public:
    // Default constructor. Value-initializes both elements of the pair, first and second
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : m_first(), m_second()
    {
    }

    // Initializes m_first with first and m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T> and std::is_copy_constructible<U> are both true
    constexpr compressed_pair_impl(
        const first_type& first,
        const second_type& second) noexcept(std::conjunction<
                                     std::is_nothrow_copy_constructible<T1>,
                                     std::is_nothrow_copy_constructible<T2>>::
                                     value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : m_first(first), m_second(second)
    {
    }

    // Initializes m_first with std::forward<U1>(first) and m_second with std::forward<U2>(second).
    // This constructor participates in overload resolution if and only if
    // std::is_constructible<T1, U1> and std::is_constructible<T2, U2> are both true.
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

    // Forwards the elements of tuple1 to the constructor of m_first
    // and forwards the elements of tuple2 to the constructor of m_second.
    template <template <typename...> class Tuple1, typename... Ts1,
              template <typename...> class Tuple2, typename... Ts2>
    constexpr compressed_pair_impl( std::piecewise_construct_t, const Tuple1<Ts1...>& tuple1, const Tuple2<Ts2...>& tuple2)
        noexcept(std::conjunction<std::is_nothrow_constructible<T1, Ts1...>,
                                  std::is_nothrow_constructible<T2, Ts2...>>::value)
        requires(std::conjunction<
                    detail::is_specialization_of<Tuple1<Ts1...>, std::tuple>,
                    detail::is_specialization_of<Tuple2<Ts2...>, std::tuple>,
                    std::is_constructible<T1, Ts1...>,
                    std::is_constructible<T2, Ts2...>>::value)
        : m_first(std::make_from_tuple<first_type>(tuple1))
        , m_second(std::make_from_tuple<second_type>(tuple2))
    {
    }

    //    template <typename Tuple1, typename Tuple2>
    //    constexpr compressed_pair_impl( std::piecewise_construct_t, Tuple1&& tuple1, Tuple2&& tuple2)
    //        noexcept(std::conjunction<
    //                             unpack_tuple<std::is_nothrow_constructible,T1, Tuple1>,
    //                             unpack_tuple<std::is_nothrow_constructible,T2, Tuple2>>::value)
    //        requires(std::conjunction<
    //                 detail::is_specialization_of<Tuple1, std::tuple>,
    //                 detail::is_specialization_of<Tuple2, std::tuple>,
    //                 unpack_tuple<std::is_constructible, T1, Tuple1>,
    //                 unpack_tuple<std::is_constructible, T2, Tuple2>>::value)
    //        : m_first( std::make_from_tuple<first_type>(tuple1) )
    //         , m_second( std::make_from_tuple<second_type>(tuple2) )
    //    {
    //    }


public:
    // access first element of a pair
    constexpr auto first() const&  noexcept -> const first_type&  { return m_first; }
    constexpr auto first()    &    noexcept ->       first_type&  { return m_first; }

    constexpr auto first() const&& noexcept -> const first_type&& { return std::move(m_first); }
    constexpr auto first()   &&    noexcept ->       first_type&& { return std::move(m_first); }


    // access second element of a pair
    constexpr auto second() const&  noexcept -> const second_type& { return m_second; }
    constexpr auto second()   &     noexcept ->       second_type& { return m_second; }
    
    constexpr auto second() const&& noexcept -> const second_type&& { return std::move(m_second); }
    constexpr auto second()   &&    noexcept ->       second_type&& { return std::move(m_second); }


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


// #2 partial specialization where T1 == empty and T2 != empty
template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, true, false> : private std::remove_cv<T1>::type
{
   
public:
    using first_type  = T1;
    using second_type = T2;


public:
    // Default constructor. Value-initializes both elements of the pair, first and second
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T1(), m_second()
    {
    }

    // Initializes m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_default_constructible<T> and std::is_copy_constructible<U> are both true
    constexpr compressed_pair_impl(const second_type& second) noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(), m_second(second)
    {
    }

    // Initializes m_first with first and m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T> and std::is_copy_constructible<U> are both true
    constexpr compressed_pair_impl(const first_type& first, const second_type& second)
        noexcept(std::conjunction<std::is_nothrow_copy_constructible<T1>,
                                  std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(first), m_second(second)
    {
    }

    // Initializes m_first with std::forward<U1>(first) and m_second with std::forward<U2>(second).
    // This constructor participates in overload resolution if and only if
    // std::is_constructible<T1, U1> and std::is_constructible<T2, U2> are both true.
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

    // Forwards the elements of tuple to the constructor of m_second
    template <template <typename...> class Tuple, typename... ARGS>
    constexpr compressed_pair_impl(std::piecewise_construct_t,const Tuple<ARGS...>& tuple)
        noexcept(std::conjunction<std::is_nothrow_default_constructible<T1>,
                                  std::is_nothrow_constructible< T2, ARGS...>>::value)
        requires(std::conjunction<
                    detail::is_specialization_of<Tuple<ARGS...>, std::tuple>,
                    std::is_default_constructible<T1>,
                    std::is_constructible<T2, ARGS...>>::value)
        : T1(), m_second(std::make_from_tuple<second_type>(tuple)) 
    {
    }


public:
    // access first element of a pair
    template <typename Self>
    constexpr auto first(this Self&& self) noexcept { return std::forward_like<Self>(self); }
    
    // access second element of a pair
    template <typename Self>
    constexpr auto second(this Self&& self) noexcept { return std::forward_like<Self>(self).m_second; }

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

// #1 partial specialization where T1 != empty and T1 == empty
template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, false, true> : private std::remove_cv<T2>::type
{
   
public:
    using first_type  = T1;
    using second_type = T2;


public:
    // Default constructor. Value-initializes both elements of the pair, first and second
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T2(), m_first()
    {
    }

    // Initializes m_first with first
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T1> and std::is_default_constructible<T2> are both true
    constexpr compressed_pair_impl(const first_type& first) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T2(), m_first(first)
    {
    }

    // Initializes m_first with first and m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T1> and std::is_copy_constructible<T2> are both true
    constexpr compressed_pair_impl(const first_type& first,const second_type& second)
        noexcept(std::conjunction<std::is_nothrow_copy_constructible<T1>,
                                  std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T2(second), m_first(first)
    {
    }

    // Initializes m_first with std::forward<U1>(first) and m_second with std::forward<U2>(second).
    // This constructor participates in overload resolution if and only if
    // std::is_constructible<T1, U1> and std::is_constructible<T2, U2> are both true.
    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : T2(std::forward_like<U2>(second))
        , m_first(std::forward_like<U1>(first))
    {
    }

    // Forwards the elements of tuple1 to the constructor of m_first
    template <template <typename...> class Tuple, typename... ARGS>
    constexpr compressed_pair_impl( std::piecewise_construct_t, const Tuple<ARGS...>& tuple)
        noexcept(std::conjunction<std::is_nothrow_constructible<T1, ARGS...>,
                                  std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<
                    detail::is_specialization_of<Tuple<ARGS...>, std::tuple>,
                    std::is_constructible<T1, ARGS...>,
                    std::is_default_constructible<T2>>::value)
        : T2(), m_first(std::make_from_tuple<first_type>(tuple))
    {
    }


public:
    // access first element of a pair
    template <typename Self>
    constexpr auto first(this Self&& self) noexcept { return std::forward_like<Self>(self).m_first; }


    // access second element of a pair
    template <typename Self>
    constexpr auto second(this Self&& self) noexcept { return std::forward_like<Self>(self); }


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


// #3 partial specialization where T1 == empty and  T2 == empty
template <typename T1, typename T2>
class compressed_pair_impl<T1, T2, true, true>  : private std::remove_cv<T1>::type
                                                , private std::remove_cv<T2>::type
{

public:
    using first_type  = T1;
    using second_type = T2;


public:
    //  Default constructor. Value-initializes both elements of the pair, first and second
    constexpr compressed_pair_impl() noexcept(
        std::conjunction<std::is_nothrow_default_constructible<T1>,
                         std::is_nothrow_default_constructible<T2>>::value)
        requires(std::conjunction<std::is_default_constructible<T1>,
                                  std::is_default_constructible<T2>>::value)
        : T1(), T2()
    {
    }

    // Initializes m_first with first and m_second with second
    // This constructor participates in overload resolution if and only if 
    // std::is_copy_constructible<T> and std::is_copy_constructible<U> are both true
    constexpr compressed_pair_impl(const first_type& first, const second_type& second)
        noexcept(std::conjunction<std::is_nothrow_copy_constructible<T1>,
                                  std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : T1(first), T2(second)
    {
    }

    // Initializes m_first with std::forward<E>(first) and m_second with std::forward<F>(second).
    // This constructor participates in overload resolution if and only if
    // std::is_constructible<T, E> and std::is_constructible<U, F> are both true.
    template <typename U1, typename U2>
    constexpr compressed_pair_impl(U1&& first, U2&& second) noexcept(
        std::conjunction<std::is_nothrow_constructible<T1, U1>,
                         std::is_nothrow_constructible<T2, U2>>::value)
        requires(std::conjunction<std::is_constructible<T1, U1>,
                                  std::is_constructible<T2, U2>>::value)
        : T1(std::forward_like<U1>(first))
        , T2(std::forward_like<U2>(second)) 
    {
    }


public:
    // access first element of a pair
    template <typename Self>
    constexpr auto first(this Self&& self) noexcept { return std::forward_like<Self>(self); }

    // access second element of a pair
    template <typename Self>
    constexpr auto second(this Self&& self) noexcept { return std::forward_like<Self>(self); }

public:
    void swap(compressed_pair_impl& other) noexcept(
        std::conjunction<std::is_nothrow_swappable<T1>,
                         std::is_nothrow_swappable<T2>>::value)
    {
        return void();
    }
};




// MAIN CLASS 
template <typename T1, typename T2>
class compressed_pair
    : private compressed_pair_impl<
          T1, T2,
          std::conjunction<std::is_empty<T1>, std::negation<std::is_final<T1>>>::value,
          std::conjunction<std::is_empty<T2>, std::negation<std::is_final<T2>>>::value>
{

public:
    using base_t = compressed_pair_impl<
        T1, T2,
        std::conjunction<std::is_empty<T1>, std::negation<std::is_final<T1>>>::value,
        std::conjunction<std::is_empty<T2>, std::negation<std::is_final<T2>>>::value>;

    using first_type  = T1;
    using second_type = T2;


public:
    using base_t::base_t;

public:
    // Copy constructor
    constexpr compressed_pair(const compressed_pair& other) noexcept(
        std::conjunction<std::is_nothrow_copy_constructible<T1>,
                         std::is_nothrow_copy_constructible<T2>>::value)
        requires(std::conjunction<std::is_copy_constructible<T1>,
                                  std::is_copy_constructible<T2>>::value)
        : base_t(other.first(), other.second())
    {
    }

    // Move constructor
    constexpr compressed_pair(compressed_pair&& other) noexcept(
        std::conjunction<std::is_nothrow_move_constructible<T1>,
                         std::is_nothrow_move_constructible<T2>>::value)
        requires(std::conjunction<std::is_move_constructible<T1>,
                                  std::is_move_constructible<T2>>::value)
        : base_t(std::move(other.first()), std::move(other.second()))
    {
    }

    // Copy assignment operator
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

    // Move assignment operator
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

    // access first element of a pair
    template <typename Self>
    constexpr auto first(this Self&& self) noexcept { return std::forward_like<Self>(self).base_t::first(); }


    // access second element of a pair
    template <typename Self>
    constexpr auto second(this Self&& self) noexcept { return std::forward_like<Self>(self).base_t::second(); }


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



// lexicographically compares the values in the compressed_pair ( Equality and Ordering Operators )

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto lexicographical_compare(const compressed_pair<T, U>& lhs,
                                       const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return std::tie(lhs.first(), lhs.second()) <
           std::tie(rhs.first(), rhs.second());
}

template <std::equality_comparable T, std::equality_comparable_with<T> U>
constexpr auto operator==(const compressed_pair<T, U>& lhs,
                          const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return std::tie(lhs.first(), lhs.second()) ==
           std::tie(rhs.first(), rhs.second());
}

template <std::equality_comparable T, std::equality_comparable_with<T> U>
constexpr auto operator!=(const compressed_pair<T, U>& lhs,
                          const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return not(lhs == rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator>(const compressed_pair<T, U>& lhs,
                         const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return lexicographical_compare(rhs, lhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator>=(const compressed_pair<T, U>& lhs,
                          const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return not(lhs < rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator<(const compressed_pair<T, U>& lhs,
                         const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return lexicographical_compare(lhs, rhs);
}

template <std::totally_ordered T, std::totally_ordered_with<T> U>
constexpr auto operator<=(const compressed_pair<T, U>& lhs,
                          const compressed_pair<T, U>& rhs) noexcept -> bool
{
    return not(rhs < lhs);
}




// C++ structured binding support
namespace std {

template <typename T, typename U>
struct tuple_size<::compressed_pair<T, U>> : public integral_constant<size_t, 2> {};

template <size_t Index, typename T, typename U>
struct tuple_element<Index, ::compressed_pair<T, U>> : public tuple_element<Index, tuple<T, U>> {};

}  // namespace std


template <std::size_t Index, detail::specialization_of<::compressed_pair> Compressed_Pair>
constexpr auto get(Compressed_Pair&& pair) noexcept -> decltype(auto)
{
    if constexpr ( Index == 0 ) return std::forward_like<Compressed_Pair>(pair).first();
    if constexpr ( Index == 1 ) return std::forward_like<Compressed_Pair>(pair).second();
}
#endif

