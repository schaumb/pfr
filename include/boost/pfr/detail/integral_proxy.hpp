// Copyright (c) 2016-2022 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_DETAIL_BITFIELD_MEMBER_HPP
#define BOOST_PFR_DETAIL_BITFIELD_MEMBER_HPP
#pragma once

#include <boost/pfr/detail/config.hpp>

namespace boost { namespace pfr { namespace detail {

template<class Getter, class Setter>
class integral_proxy {
    using type = std::invoke_result_t<Getter, std::false_type>;
public:
    constexpr integral_proxy(Getter&& getter, Setter&& setter) noexcept
        : getter(getter)
        , setter(setter)
    {}

    constexpr operator type() const {
        return get();
    }

    constexpr operator type() {
        return get();
    }

    template<class = void>
    constexpr operator type&() {
        return getter(std::true_type{});
    }

    constexpr integral_proxy& operator=(type value) {
        setter(value);
        return *this;
    }

    constexpr bool operator==(const integral_proxy& rhs) const { return get() == rhs.get(); }
    constexpr bool operator<(const integral_proxy& rhs) const { return get() < rhs.get(); }
    constexpr bool operator==(const type& rhs) const { return get() == rhs; }
    constexpr bool operator<(const type& rhs) const { return get() < rhs; }


    friend constexpr bool operator!=(const integral_proxy& x, const integral_proxy& y) { return !static_cast<bool>(x == y); }
    friend constexpr bool operator>(const integral_proxy& x, const integral_proxy& y)  { return y < x; }
    friend constexpr bool operator<=(const integral_proxy& x, const integral_proxy& y) { return !static_cast<bool>(y < x); }
    friend constexpr bool operator>=(const integral_proxy& x, const integral_proxy& y) { return !static_cast<bool>(x < y); }
    friend constexpr bool operator==(const type& y, const integral_proxy& x) { return x == y; }
    friend constexpr bool operator!=(const type& y, const integral_proxy& x) { return !static_cast<bool>(x == y); }
    friend constexpr bool operator!=(const integral_proxy& y, const type& x) { return !static_cast<bool>(y == x); }
    friend constexpr bool operator>(const integral_proxy& x, const type& y) { return !(static_cast<bool>(x == y) || static_cast<bool>(x < y)); }
    friend constexpr bool operator<=(const integral_proxy& x, const type& y) { return !static_cast<bool>(x > y); }
    friend constexpr bool operator>=(const integral_proxy& x, const type& y) { return !static_cast<bool>(x < y); }
    friend constexpr bool operator>(const type& x, const integral_proxy& y)  { return y < x; }
    friend constexpr bool operator<(const type& x, const integral_proxy& y)  { return y > x; }
    friend constexpr bool operator<=(const type& x, const integral_proxy& y) { return !static_cast<bool>(y < x); }
    friend constexpr bool operator>=(const type& x, const integral_proxy& y) { return !static_cast<bool>(y > x); }

private:
    constexpr type get() const {
        return getter(std::false_type{});
    }

    Getter getter;
    Setter setter;
};

}}} // namespace boost::pfr::detail

#endif //BOOST_PFR_DETAIL_BITFIELD_MEMBER_HPP
