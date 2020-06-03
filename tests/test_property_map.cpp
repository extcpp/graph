#include <gtest/gtest.h>

#include <ext/property_map/property_map.hpp>

template <ext::graph::LValuePropertyMap T>
  void testLValuePropertyConcept(T& h)  {
      (void)h;
}

TEST(property_map, create) {

  auto map = std::map<int,int>();
  ext::graph::associative_property_map pmap(map);

  testLValuePropertyConcept(pmap);

}
