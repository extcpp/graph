#include <gtest/gtest.h>

#include <ext/property_map/property_map.hpp>

template <ext::graph::LValuePropertyMap T>
  void testLValuePropertyConcept(T& h)  {
      (void)h;
}

TEST(property_maps, associative_property_map) {
    {
        auto map = std::map<int,int>();
        ext::graph::associative_property_map pmap(map);

        testLValuePropertyConcept(pmap);
        put(pmap,1,2);
        auto actual = get(pmap,1);
        ASSERT_EQ(2,actual);
    }

    {
        auto map = std::map<int,int>();
        map[1] = 2;

        auto const& cmap = map;
        ext::graph::associative_property_map cpmap(cmap);

        testLValuePropertyConcept(cpmap); // how can the constness be checked?
        //put(cpmap,1,3); // fails to compile -- good

        auto actual = get(cpmap,1);
        ASSERT_EQ(2,actual);
    }



}
