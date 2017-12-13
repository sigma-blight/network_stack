#ifndef     _META_TRAITS_IS_SAME_TEMPLATE_TYPE_HPP_
#define     _META_TRAITS_IS_SAME_TEMPLATE_TYPE_HPP_

#include <type_traits>

namespace meta
{
    //////////////////////////////////////////
    //          Is Template Type
    //////////////////////////////////////////

    namespace _detail
    {
    
        template <typename Type_>
        struct IsTemplateType : std::false_type {};
        
        template <template<typename...> typename Template_, typename... Types_>
        struct IsTemplateType<Template_<Types_...>> : std::true_type {};
        
    } // private namespace _detail

    template <typename Type_>
    constexpr auto is_template_type() noexcept
    {
        return _detail::IsTemplateType<Type_>{};
    }
    
    //////////////////////////////////////////
    //        Is Same Template Type
    //////////////////////////////////////////
    
    //  is_same_template_type<template, template>()
    
    namespace _detail
    {
    
        template <template<typename...> typename, template<typename...> typename>
        struct IsSameTemplateType : std::false_type {};
        
        template <template<typename...> typename Template_>
        struct IsSameTemplateType<Template_, Template_> : std::true_type {};
    
    } // private namespace _detail
    
    template <template<typename...> typename LHS_, template<typename...> typename RHS_>
    constexpr auto is_same_template_type() noexcept
    {
        return _detail::IsSameTemplateType<LHS_, RHS_>{};
    }
    
    //  is_same_template_type<template, type>()
    
    namespace _detail
    {
    
        template <template<typename...> typename LHS_,
                  template<typename...> typename RHS_,
                  typename... Types_>
        constexpr auto is_same_template_type(RHS_<Types_...>) noexcept
        {
            return meta::is_same_template_type<LHS_, RHS_>();
        }
    
    } // private namespace _detail
    
    template <template<typename...> typename Template_, typename Type_>
    constexpr auto is_same_template_type() noexcept
    {
        if constexpr (is_template_type<Type_>())
            return decltype(_detail::is_same_template_type<Template_>(std::declval<Type_>())){};
        else return std::false_type{};
    }
    
    //  is_same_template_type<type, type>()
    
    namespace _detail
    {
    
        template <template<typename...> typename LHS_,
                  template<typename...> typename RHS_,
                  typename... LHS_Args_,
                  typename... RHS_Args_>
        constexpr auto is_same_template_type(LHS_<LHS_Args_...>, RHS_<RHS_Args_...>) noexcept
        {
            return is_same_template_type<LHS_, RHS_>();
        }
    
    } // private namespace _detail
    
    template <typename LHS_, typename RHS_>
    constexpr auto is_same_template_type() noexcept
    {
        if constexpr (is_template_type<LHS_>() && is_template_type<RHS_>())
            return decltype(_detail::is_same_template_type(std::declval<LHS_>(), std::declval<RHS_>())){};
        else return std::false_type{};
    }
    
} // namespace meta

#endif  //  _META_TRAITS_IS_SAME_TEMPLATE_TYPE_HPP_
