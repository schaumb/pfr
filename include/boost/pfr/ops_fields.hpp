// Copyright (c) 2016-2020 Antony Polukhin
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_PFR_OPS_FIELDS_HPP
#define BOOST_PFR_OPS_FIELDS_HPP
#pragma once

#include <boost/pfr/detail/config.hpp>

#include <boost/pfr/core.hpp>
#include <boost/pfr/detail/functional.hpp>

/// \file boost/pfr/ops_fields.hpp
/// Contains field-by-fields comparison and hash functions.
///
/// \b Example:
/// \code
///     #include <boost/pfr/ops_fields.hpp>
///     struct comparable_struct {      // No operators defined for that structure
///         int i; short s;
///     };
///     // ...
///
///     comparable_struct s1 {0, 1};
///     comparable_struct s2 {0, 2};
///     assert(boost::pfr::lt_fields(s1, s2));
/// \endcode
///
/// \podops for other ways to define operators and more details.
///
/// \b Synopsis:
namespace boost { namespace pfr {

    /// Does a field-by-field equality comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) == std::tie( _rhs-fields_ ) && tuple_size_v<T> == tuple_size_v<U>`
    template <class T, class U>
    bool eq_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::equal_impl>(lhs, rhs);
    }


    /// Does a field-by-field inequality comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) != std::tie( _rhs-fields_ ) || tuple_size_v<T> != tuple_size_v<U>`
    template <class T, class U>
    bool ne_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::not_equal_impl>(lhs, rhs);
    }

    /// Does a field-by-field greter comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) > std::tie( _rhs-fields_ ) || (std::tie( _lhs-fields_ ) == std::tie( _rhs-fields_ ) && tuple_size_v<T> > tuple_size_v<U>)`
    template <class T, class U>
    bool gt_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::greater_impl>(lhs, rhs);
    }


    /// Does a field-by-field less comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) < std::tie( _rhs-fields_ ) || (std::tie( _lhs-fields_ ) == std::tie( _rhs-fields_ ) && tuple_size_v<T> < tuple_size_v<U>)`
    template <class T, class U>
    bool lt_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::less_impl>(lhs, rhs);
    }


    /// Does a field-by-field greater equal comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) > std::tie( _rhs-fields_ ) || (std::tie( _lhs-fields_ ) == std::tie( _rhs-fields_ ) && tuple_size_v<T> >= tuple_size_v<U>)`
    template <class T, class U>
    bool ge_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::greater_equal_impl>(lhs, rhs);
    }


    /// Does a field-by-field less equal comparison.
    ///
    /// Let MIN be std::min(tuple_size_v<T>, tuple_size_v<U>). Let _lhs-fields_ and _rhs-fields_ be the tuples of MIN fields of lhs and rhs correspondingly.
    ///
    /// \returns `std::tie( _lhs-fields_ ) < std::tie( _rhs-fields_ ) || (std::tie( _lhs-fields_ ) == std::tie( _rhs-fields_ ) && tuple_size_v<T> <= tuple_size_v<U>)`
    template <class T, class U>
    bool le_fields(const T& lhs, const U& rhs) noexcept {
        return detail::binary_visit<detail::less_equal_impl>(lhs, rhs);
    }


    /// Does a field-by-field hashing.
    ///
    /// \returns combined hash of all the fields
    template <class T>
    std::size_t hash_fields(const T& x) {
        constexpr std::size_t fields_count_val = boost::pfr::detail::fields_count<std::remove_reference_t<T>>();
#if BOOST_PFR_USE_CPP17
        return detail::hash_impl<0, fields_count_val>::compute(detail::tie_as_tuple(x));
#else
        std::size_t result = 0;
        ::boost::pfr::detail::for_each_field_dispatcher(
            x,
            [&result](const auto& lhs) {
                // We can not reuse `fields_count_val` in lambda because compilers had issues with
                // passing constexpr variables into lambdas. Computing is again is the most portable solution.
                constexpr std::size_t fields_count_val_lambda = boost::pfr::detail::fields_count<std::remove_reference_t<T>>();
                result = detail::hash_impl<0, fields_count_val_lambda>::compute(lhs);
            },
            detail::make_index_sequence<fields_count_val>{}
        );

        return result;
#endif
    }
}} // namespace boost::pfr

#endif // BOOST_PFR_OPS_HPP
