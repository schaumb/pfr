// Copyright (c) 2016-2022 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_PFR_DETAIL_CORE17_HPP
#define BOOST_PFR_DETAIL_CORE17_HPP

#include <boost/pfr/detail/core17_generated.hpp>
#include <boost/pfr/detail/fields_count.hpp>
#include <boost/pfr/detail/for_each_field_impl.hpp>
#include <boost/pfr/detail/rvalue_t.hpp>
#include <limits>

namespace boost { namespace pfr { namespace detail {

#ifndef _MSC_VER // MSVC fails to compile the following code, but compiles the structured bindings in core17_generated.hpp
struct do_not_define_std_tuple_size_for_me {
    bool test1 = true;
};

template <class T>
constexpr bool do_structured_bindings_work() noexcept { // ******************************************* IN CASE OF ERROR READ THE FOLLOWING LINES IN boost/pfr/detail/core17.hpp FILE:
    T val{};
    const auto& [a] = val; // ******************************************* IN CASE OF ERROR READ THE FOLLOWING LINES IN boost/pfr/detail/core17.hpp FILE:

    /****************************************************************************
    *
    * It looks like your compiler or Standard Library can not handle C++17
    * structured bindings.
    *
    * Workaround: Define BOOST_PFR_USE_CPP17 to 0
    * It will disable the C++17 features for Boost.PFR library.
    *
    * Sorry for the inconvenience caused.
    *
    ****************************************************************************/

    return a;
}

static_assert(
    do_structured_bindings_work<do_not_define_std_tuple_size_for_me>(),
    "====================> Boost.PFR: Your compiler can not handle C++17 structured bindings. Read the above comments for workarounds."
);
#endif // #ifndef _MSC_VER

template<class Getter, class Setter>
constexpr std::size_t get_integral_bit_size(Getter&& getter, Setter&& setter) {
    using type = std::invoke_result_t<Getter>;
    if constexpr (std::is_unsigned_v<type>) {
        setter(std::numeric_limits<type>::max());
        type num = getter();

        std::size_t res{1};
        while (num >>= 1) {
            ++res;
        }
        return res;
    } else {
        auto max = std::numeric_limits<type>::max();
        for (setter(max); getter() != max; )
            setter(max /= 2);

        std::size_t res{};
        while (max) {
            ++res;
            max /= 2;
        }
        return res;
    }
}

template<class Index, class Getter, class Setter>
constexpr bool clearly_is_a_bitfield(Getter&& getter, Setter&& setter) {
    using type = std::invoke_result_t<Getter, Index, std::false_type>;
    if constexpr (std::is_integral_v<type>) {
        if constexpr (std::is_same_v<type, bool>) {
            return false; // TODO bool
        } else {
            std::size_t bits = get_integral_bit_size([&] { return getter(Index{}, std::false_type{}); },
                                                     [&] (auto value) { setter(Index{}, value); });
            if (bits < sizeof(type) * CHAR_BIT) {
                return true;
            } else {
                return false; // TODO full integral bitfield
            }
        }
    } else  {
        return false; // only integral can be bitfield
    }
}


template<class Getter, class Setter, std::size_t ... Ix>
constexpr bool clearly_has_a_bitfield(Getter&& getter, Setter&& setter, std::index_sequence<Ix...>) {
    return (clearly_is_a_bitfield<size_t_<Ix>>(getter, setter) || ...);
}

template<typename Getter, std::size_t ... Ix>
constexpr auto structure_tuple(Getter, std::index_sequence<Ix...>) ->
    sequence_tuple::tuple<std::invoke_result_t<Getter, size_t_<Ix>, std::false_type>...>;

template<class T>
constexpr bool clearly_has_a_bitfield() {
    typedef size_t_<boost::pfr::detail::fields_count<T>()> fields_count_tag;
    typedef decltype(std::get<0>(boost::pfr::detail::structure_member_getter_setter(std::declval<T&>(), fields_count_tag{}))) Getter;
    typedef decltype(structure_tuple(std::declval<Getter>(), std::make_index_sequence<fields_count_tag{}>{})) TupleType;
    if constexpr (sizeof(T) == sizeof(TupleType)) {
        if constexpr(std::is_trivial_v<T>) {
            typedef size_t_<boost::pfr::detail::fields_count<T>()> fields_count_tag;
            T val{};
            auto&& [getter, setter] = boost::pfr::detail::structure_member_getter_setter(val, fields_count_tag{});

            return clearly_has_a_bitfield(getter, setter, std::make_index_sequence<fields_count_tag{}>{});
        } else {
            return false; // TODO not trivial
        }
    } else if (alignof(T) == alignof(TupleType)) {
        return true; // not packed structure, size mismatch => has bitfield
    } else {
        return false; // TODO packed structure
    }
}

template<class Getter, std::size_t ...Ix>
constexpr bool can_be_a_bitfield(std::index_sequence<Ix...>) {
    return (std::is_integral_v<std::invoke_result_t<Getter, size_t_<Ix>, std::false_type>> || ...);
}

template<class T>
constexpr bool can_be_a_bitfield() {
    typedef size_t_<boost::pfr::detail::fields_count<T>()> fields_count_tag;
    typedef decltype(std::get<0>(boost::pfr::detail::structure_member_getter_setter(std::declval<T&>(), fields_count_tag{}))) Getter;
    return can_be_a_bitfield<Getter>(std::make_index_sequence<fields_count_tag{}>{});
}

template<class Getter, std::size_t ... Ix>
constexpr auto tie_as_tuple(Getter&& getter, std::index_sequence<Ix...>) {
    return sequence_tuple::tuple<std::invoke_result_t<Getter, size_t_<Ix>, std::true_type>...>{
        getter(size_t_<Ix>{}, std::true_type{})...
    };
}

template <class T>
constexpr auto tie_as_tuple(T& val) noexcept {
  static_assert(
    !std::is_union<T>::value,
    "====================> Boost.PFR: For safety reasons it is forbidden to reflect unions. See `Reflection of unions` section in the docs for more info."
  );
  typedef size_t_<boost::pfr::detail::fields_count<T>()> fields_count_tag;
  auto&& getter = std::get<0>(boost::pfr::detail::structure_member_getter_setter(val, fields_count_tag{}));
  return tie_as_tuple(std::move(getter), std::make_index_sequence<fields_count_tag{}>{});
}

template <class T, class F, std::size_t... I>
void for_each_field_dispatcher(T& t, F&& f, std::index_sequence<I...>) {
    static_assert(
        !std::is_union<T>::value,
        "====================> Boost.PFR: For safety reasons it is forbidden to reflect unions. See `Reflection of unions` section in the docs for more info."
    );
    std::forward<F>(f)(
        detail::tie_as_tuple(t)
    );
}

}}} // namespace boost::pfr::detail

#endif // BOOST_PFR_DETAIL_CORE17_HPP
