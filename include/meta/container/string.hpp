#ifndef     _META_CONTAINER_STRING_HPP_
#define     _META_CONTAINER_STRING_HPP_

namespace meta
{
    template <typename Char_, Char_... chars_>
    struct String
    {
        using char_t = Char_;
        constexpr auto size() noexcept { return sizeof...(chars_); }
    };
    
    inline namespace literals
    {

        template <typename Char_, Char_... chars_>
        constexpr auto operator "" _s ()
        { return String<Char_, chars_...>{}; }

    } // inline namespace literals    
} // namespace meta

#endif  //  _META_CONTAINER_STRING_HPP_
