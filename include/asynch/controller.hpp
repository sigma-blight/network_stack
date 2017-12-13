#ifndef     _ASYNCH_CONTROLLER_HPP_
#define     _ASYNCH_CONTROLLER_HPP_

#include <tuple>
#include <utility>
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
        constexpr decltype(auto) operator() (Args_ && ... args)
        {
            static_assert(std::is_invocable_v<Delegate_, decltype(args)...>,
                "Invalid invocation arguments");
                
            return std::invoke(delegate, std::forward<Args_>(args)...);
        }
    };
    
    template <typename Tag_, typename Delegate_>
    Event(Tag_, Delegate_ &&) -> Event<Tag_, std::decay_t<Delegate_>>;
    
    /////////////////////////////////
    //          Controller
    /////////////////////////////////
    
    template <typename... Events_>
    class Controller
    {
      private:
        static_assert((meta::is_same_template_type<Event, Events_>() && ...),
            "Requires all template parameters to be events");
            
        using tuple_t = std::tuple<Events_...>;
        tuple_t events;
        
      public:
        constexpr Controller(Events_ ... events) :
            events{ std::move(events)... }
        {}
        
        template <typename Tag_, typename... Args_>
        constexpr decltype(auto) operator() (Tag_, Args_ && ... args)
        {
            return get_event<Tag_>()(*this, std::forward<Args_>(args)...);
        }
        
      private:
        
        template <typename Tag_, size_t index_ = 0>
        constexpr decltype(auto) get_event()
        {
            // short circuit stop if index range is invalid
            static_assert(index_ < sizeof...(Events_), "Tag not found");
            
            auto & event = std::get<index_>(events);
            using event_t = std::decay_t<decltype(event)>;
            if constexpr (std::is_same_v<Tag_, typename event_t::tag_t>)
                return event;
            else return get_event<Tag_, index_ + 1>();
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
