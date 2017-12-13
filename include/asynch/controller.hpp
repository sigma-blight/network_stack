#ifndef     _ASYNCH_CONTROLLER_HPP_
#define     _ASYNCH_CONTROLLER_HPP_

#include <utility>
#include <algorithm>
#include <functional> // std::invoke
#include <type_traits>

#include <meta/traits/is_same_template_type.hpp>

namespace asynch
{
    /////////////////////////////////
    //           Event
    /////////////////////////////////

    template <typename Tag_, typename Delegate_>
    class Event
    {
      public:
        using tag_t = Tag_;
        
      private:
        Delegate_ delegate;
        
      public:
        template <typename _Delegate_>
        constexpr Event(Tag_, _Delegate_ && _delegate) :
            delegate{ std::forward<_Delegate_>(delegate) }
        {}
        
        template <typename... Args_>
        constexpr void operator() (Args_ && ... args) const
        {
            static_assert(std::is_invocable_v<Delegate_, decltype(args)...>,
                "Invalid invocation arguments");
                
            std::invoke(delegate, std::forward<Args_>(args)...);
        }
    };
    
    template <typename Tag_, typename Delegate_>
    Event(Tag_, Delegate_ &&) -> Event<Tag_, std::decay_t<Delegate_>>;
    
    /////////////////////////////////
    //          Controller
    /////////////////////////////////
    
    template <typename... Events_>
    class Controller : private Events_...
    {
      private:
        static_assert((meta::is_same_template_type<Event, Events_>() && ...),
            "Requires all template parameters to be events");
        
      public:
        constexpr Controller(Events_ ... events) :
            Events_{ std::move(events) }...
        {}
        
        template <typename Tag_, typename... Args_>
        constexpr void operator() (Tag_, Args_ && ... args)
        {            
            get_event<Tag_, Events_...>()(*this, std::forward<Args_>(args)...);
        }
        
      private:
      
        template <typename Tag_, typename Event_, typename... Trail_>
        constexpr decltype(auto) get_event() const
        {
            if constexpr (std::is_same_v<Tag_, typename Event_::tag_t>)
                return static_cast<const Event_ &>(*this);
            else
                return get_event<Tag_, Trail_...>();
        }
    };

} // namespace asynch

#endif  //  _ASYNCH_CONTROLLER_HPP_





























//#ifndef     _NETWORK_STACK_SRC_CONTROLLER_HPP_
//#define     _NETWORK_STACK_SRC_CONTROLLER_HPP_



//namespace network
//{

//template <typename Char_, Char_... chars_>
//struct String {};

//template <typename Char_, Char_... chars_>
//constexpr auto operator "" _s () { return String<Char_, chars_...>{}; }

//template <typename String_, typename Delegate_>
//class Event
//{
//  private:

//    Delegate_ delegate;

//  public:

//    template <typename Delegate_arg_>
//    Event(String_, Delegate_arg_ && _delegate) :
//        delegate{ std::forward<Delegate_arg_>(_delegate) }
//    {}

//    template <typename... Args_>
//    void operator() (Args_ && ... args)
//    {
//        delegate(std::forward<Args_>(args)...);
//    }

//    template <typename Other_string_>
//    constexpr bool is_string() { return std::is_same_v<String_, Other_string_>; }
//};

//template <typename String_, typename Delegate_arg_>
//Event(String_, Delegate_arg_ &&) -> Event<String_, std::decay_t<Delegate_arg_>>;

//template <typename... Events_>
//class Controller
//{
//  private:

//    std::tuple<Events_...> events;

//  public:

//    Controller(Events_ && ... _events) :
//        events{ std::move(_events)... }
//    {}

//    template <typename String_, typename... Args_>
//    void call_event(String_, Args_ && ... args)
//    {
//        get_event<String_>()(*this, std::forward<Args_>(args)...);
//    }

//  private:

//    template <typename String_, size_t index_ = 0>
//    constexpr auto get_event()
//    {
//        static_assert(index_ != sizeof...(Events_), "Invalid event, not found");

//        auto event = std::get<index_>(events);
//        if constexpr (event.template is_string<String_>())
//            return event;
//         else
//             return get_event<String_, index_ + 1>();
//    }
//};

//}

//#endif // _NETWORK_STACK_SRC_CONTROLLER_HPP_
