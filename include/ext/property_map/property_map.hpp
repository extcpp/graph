// Copyright - 2020 - Jan Christoph Uhde <Jan@UhdeJC.com>
#ifndef EXT_GRAPH_PROPERTY_MAP_PROPERTY_MAP_HEADER
#define EXT_GRAPH_PROPERTY_MAP_PROPERTY_MAP_HEADER 1

#include <iostream>
#include <iterator>
#include <vector>
#include <map>

//#include <ext/macros/assert.hpp>
//#include <ext/util/bit_tricks.hpp>
#include <concepts>

namespace ext { namespace graph {

template<typename ValueType, typename KeyType, typename Reference>
struct property_map_types {
    using value_type = ValueType;
    using key_type = KeyType;
    using reference = Reference;
};

template <typename T>
auto property_map_trait(T&&) -> property_map_types<typename T::value_type, typename T::key_type, typename T::reference>;



template <typename T, typename Trait = decltype(property_map_trait(std::declval<T>()))>
concept PropertyMap = requires(Trait t) {
    typename Trait::value_type;
    typename Trait::key_type;
    typename Trait::reference;
};

template <typename T, typename Trait = decltype(property_map_trait(std::declval<T>()))>
concept ReadablePropertyMap =
    PropertyMap<T> &&
    requires (T map, typename Trait::key_type key) { { get(map, key) } -> std::same_as<typename Trait::reference>; };

template <typename T, typename Trait = decltype(property_map_trait(std::declval<T>()))>
concept WriteablePropertyMap =
    PropertyMap<T> && ReadablePropertyMap<T> &&
    requires (T map, typename Trait::key_type key, typename Trait::value_type value) { { put(map, key, value) } -> std::same_as<void>; };

template <typename T, typename Trait = decltype(property_map_trait(std::declval<T>()))>
concept LValuePropertyMap =
    PropertyMap<T> && WriteablePropertyMap<T> &&
    ( std::is_same_v<typename Trait::reference, typename Trait::value_type const&> || std::is_same_v<typename T::reference, typename T::value_type&> );




// CRTP base class that will provide freestanding put / get
// functions, when used as base class.
template <typename Map>
struct put_get_base { };

template <typename Map, typename Trait = decltype(property_map_trait(std::declval<Map>()))>
typename Map::reference
get(const put_get_base<Map>& map, typename Trait::key_type const& key) {
    typename Trait::reference v = static_cast<const Map&>(map)[key];
    return v;
}

template <typename Map, typename Trait = decltype(property_map_trait(std::declval<Map>()))>
void
put(const put_get_base<Map>& map, typename Trait::key_type const& key, typename Trait::value_type const& value) {
    static_cast<const Map&>(map)[key] = value;
}



template <typename UniquePairAssociativeContainer, bool Const = false>
class associative_property_map : public put_get_base<associative_property_map<UniquePairAssociativeContainer, Const>> {
    using Container = UniquePairAssociativeContainer;
    std::conditional_t<Const, Container const*, Container*> _container;

public:
    using key_type = typename Container::key_type;
    using value_type = typename Container::value_type::second_type;
    using reference = std::conditional_t<Const, const value_type&, value_type&>;

    //associative_property_map() : m_c(0) { }
    associative_property_map(Container& c) : _container(&c) { }
    associative_property_map(Container const& c) : _container(&c) { }

    reference operator[](const key_type& k) const {
        if constexpr(Const) {
            return _container->find(k)->second;
        } else {
            return (*_container)[k];
        }
    }
};

template <typename T>
associative_property_map(T const&) -> associative_property_map<T, true>;

}}     // namespace ext::graph
#endif
