#include <gtest/gtest.h>

#include <ext/property_map/property_map.hpp>

template <ext::graph::LValuePropertyMap T>
  void testLValuePropertyConcept(T& h)  {
      (void)h;
}

TEST(property_maps, associative_property_map) {
    {
        auto map = std::map<int,std::string>();
        ext::graph::associative_property_map pmap(map);

        testLValuePropertyConcept(pmap);
        put(pmap,1,"foo");

        auto& actual = get(pmap,1);
        ASSERT_EQ("foo",actual);

        actual.append("bar");

        auto actual_update = get(pmap,1);
        ASSERT_EQ("foobar",actual_update);
    }

    {
        auto map = std::map<int,std::string>();
        map[1] = "foo";

        auto const& cmap = map;
        ext::graph::associative_property_map cpmap(cmap);

        testLValuePropertyConcept(cpmap); // how can the constness be checked?
        //put(cpmap,1,3); // fails to compile -- good

        auto& actual = get(cpmap,1);
        ASSERT_EQ("foo",actual);

        //actual.append("bar"); // fails to compile -- good
    }



}
