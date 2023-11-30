// Copyright (c) 2019-2023 Antony Polukhin.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP
#define BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP
#pragma once

#include <boost/pfr/detail/config.hpp>

#include <type_traits>

namespace boost { namespace pfr { namespace detail {

template <class T>
struct wrapper {
    T value;

    // This object serves as a link-time assert. If linker requires it, then
    // `unsafe_declval()` is used at runtime.
    // The wrapper class has external linkage while T has not sure.
    static wrapper<T> report_if_you_see_link_error_with_this_object;
};

// For returning non default constructible types. Do NOT use at runtime!
//
// GCCs std::declval may not be used in potentionally evaluated contexts,
// so we reinvent it.
template <class T>
constexpr T unsafe_declval() noexcept {
    typedef typename std::remove_reference<T>::type type;
    return wrapper<type>::report_if_you_see_link_error_with_this_object.value;
}

}}} // namespace boost::pfr::detail


#endif // BOOST_PFR_DETAIL_UNSAFE_DECLVAL_HPP

