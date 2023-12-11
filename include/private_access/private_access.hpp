#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <type_traits>

namespace
{

namespace privateaccess
{
template <size_t N> using string_array = std::array<char, N>;
namespace impl
{
// Conversion of raw char array => string_array
template <size_t N, typename Iterator> static constexpr auto init(Iterator begin, Iterator end) noexcept
{
    string_array<N> os;
    std::copy(begin, end, os.begin());
    return os;
}
template <size_t M> static constexpr auto init(const char (&c)[M]) noexcept
{
    // Exclude null terminator
    return init<M - 1>(c, c + M - 1);
}
} // namespace impl

template <size_t N> struct const_string
{
    static constexpr auto size = N;

    const string_array<N> chars = {};

    template <size_t M> constexpr const_string(const char (&string)[M]) noexcept : chars(impl::init(string))
    {
        static_assert(M - 1 == N);
    }
};
template <size_t N> const_string(const char (&string)[N]) -> const_string<N - 1>;

template <const_string s> static constexpr decltype(auto) operator"" _cs()
{
    return s;
}

template <typename T> struct member_ptr_type_extractor;
template <typename class_t, typename member_t> struct member_ptr_type_extractor<member_t(class_t::*)>
{
    using member_type = member_t;
    using class_type = class_t;
};

template <typename T> using get_member_type_from_ptr_t = typename member_ptr_type_extractor<T>::member_type;

template <typename in_t, typename out_t>
using copy_const_t = std::conditional_t<std::is_const_v<std::remove_reference_t<in_t>>, std::add_const_t<out_t>, out_t>;

template <class class_t, typename member_t>
static constexpr bool is_member_ptr = std::is_member_object_pointer_v<member_t std::remove_reference_t<class_t>::*>;

template <class class_t, typename member_t> struct proxy_value_access
{
    class_t obj;
    member_t std::remove_reference_t<class_t>::*ptr;

    using pointer_t = std::add_pointer_t<copy_const_t<class_t, member_t>>;
    using ref_t = std::add_lvalue_reference_t<copy_const_t<class_t, member_t>>;

    using cpointer_t = std::add_pointer_t<std::add_const_t<member_t>>;
    using cref_t = std::add_lvalue_reference_t<std::add_const_t<member_t>>;

    constexpr operator cref_t() const noexcept
    {
        return obj.*ptr;
    }
    constexpr cref_t operator()() const
    {
        return this->operator cref_t();
    }
    constexpr cref_t operator*() const noexcept(noexcept(*std::declval<pointer_t>()))
    {
        return this->operator cref_t();
    }
    constexpr cpointer_t operator->() const noexcept
    {
        return std::addressof(this->operator cref_t());
    }

    constexpr operator ref_t() noexcept
    {
        return obj.*ptr;
    }
    constexpr ref_t operator()()
    {
        return this->operator ref_t();
    }
    constexpr ref_t operator*() noexcept(noexcept(*std::declval<pointer_t>()))
    {
        return this->operator ref_t();
    }
    constexpr pointer_t operator->() noexcept
    {
        return std::addressof(this->operator ref_t());
    }
};

template <class class_t, typename member_t> struct proxy_function_access
{
    class_t obj;
    member_t std::remove_reference_t<class_t>::*ptr;

    template <class... args>
    constexpr auto operator()(args&&... a) noexcept(noexcept((obj.*ptr)(std::forward<args>(a)...)))
        -> decltype((obj.*ptr)(std::forward<args>(a)...))
    {
        return (obj.*ptr)(std::forward<args>(a)...);
    }
    template <class... args>
    constexpr auto operator()(args&&... a) const noexcept(noexcept((obj.*ptr)(std::forward<args>(a)...)))
        -> decltype((obj.*ptr)(std::forward<args>(a)...))
    {
        return (obj.*ptr)(std::forward<args>(a)...);
    }
};

template <class class_t, typename member_t>
using proxy_access = std::conditional_t<is_member_ptr<class_t, member_t>, proxy_value_access<class_t, member_t>,
                                        proxy_function_access<class_t, member_t>>;

template <typename class_t, const_string s> struct tag_t
{
};

template <typename class_t, const_string s, auto member_ptr> struct access_address
{
    friend constexpr auto get_address(tag_t<class_t, s>)
    {
        return member_ptr;
    }
};

template <const_string s, typename class_t> constexpr auto make_proxy(class_t&& obj)
{
    using raw_class_t = std::remove_cvref_t<class_t>;
    constexpr auto tag = tag_t<raw_class_t, s>{};
    using member_t = get_member_type_from_ptr_t<decltype(get_address(tag))>;
    return proxy_access<class_t, member_t>{std::forward<class_t>(obj), get_address(tag)};
}

template <typename class_t, const_string s> constexpr auto get_address()
{
    return get_address(tag_t<class_t, s>{});
}

template <typename class_t, const_string s>
using get_member_type_t = get_member_type_from_ptr_t<decltype(get_address<class_t, s>())>;

template <typename T> struct macro_type_expander
{
    using type = T;
};
template <typename S, typename T> struct macro_type_expander<S(T)>
{
    using type = T;
};

template <typename T> using macro_type_expander_t = typename macro_type_expander<T>::type;

} // namespace privateaccess
} // namespace

#define EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(class_name, member_name)                                             \
    namespace                                                                                                          \
    {                                                                                                                  \
    namespace privateaccess                                                                                            \
    {                                                                                                                  \
    template struct access_address<macro_type_expander_t<void(class_name)>, #member_name##_cs,                         \
                                   &macro_type_expander_t<void(class_name)>::member_name>;                             \
    constexpr auto get_address(tag_t<macro_type_expander_t<void(class_name)>, #member_name##_cs>);                       \
    } /* namespace privateaccess */                                                                                    \
    }
