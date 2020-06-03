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

    template <typename T>
    concept PropertyMap = requires(T map) {
        typename T::value_type;
        typename T::key_type;
        typename T::reference;
    };

    // TODO add traits to allow the implementation of put / get
    // for arbritary types without the need of a wrapper class

    template <typename T>
    concept ReadablePropertyMap =
        PropertyMap<T> &&
        requires (T map, typename T::key_type key) { { get(map, key) } -> std::same_as<typename T::reference>; };

    template <typename T>
    concept WriteablePropertyMap =
        PropertyMap<T> && ReadablePropertyMap<T> &&
        requires (T map, typename T::key_type key, typename T::value_type value) { { put(map, key, value) } -> std::same_as<void>; };

    template <typename T>
    concept LValuePropertyMap =
        PropertyMap<T> && WriteablePropertyMap<T> &&
        ( std::is_same_v<typename T::reference, typename T::value_type const&> || std::is_same_v<typename T::reference, typename T::value_type&> );

  // CRTP base class that will provide freestanding put / get
  // functions, when used as base class.

  template <typename Map>
  struct put_get_base { };

  template <typename Map>
  typename Map::reference
  get(const put_get_base<Map>& pa, typename Map::key_type const& k) {
    typename Map::reference v = static_cast<const Map&>(pa)[k];
    return v;
  }

  template <typename Map>
  void
  put(const put_get_base<Map>& pa, typename Map::key_type const& k, typename Map::value_type const& v) {
    static_cast<const Map&>(pa)[k] = v;
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

    reference operator[](const key_type& k) const {
        if constexpr(Const) {
            return _container->find(k)->second;
        } else {
            return (*_container)[k];
        }
    }

  };

}}     // namespace ext::graph
#endif
