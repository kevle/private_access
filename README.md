# Access private class members

This library is an experimental exercise to non-intrusively access private members with the least painful syntax I could come up with in C++20.

Accessing private members can be useful in situations where you want to unit test implementation details without changing the class definition or for serialization routines.

The library can also be used to access class internals of code external to your own codebase ( there be dragons... ).
This [talk](https://github.com/cpp-aachen/cpp-aachen.github.io/blob/main/archive/2023-11-29/access_private_members/AccessPrivateMembers.pdf) from my [local C++ user group](https://cpp-aachen.github.io/) is what inspired me to test how far I could go.

Right before putting this experiment on github, I found a library from 2016 that basically does the same thing with C++11: [martong/access_private](https://github.com/martong/access_private)

The only "advantage" of the C++20 implementation probably is the capability to access private types and members of private nested types, as shown in the [test](./test/compile_test.cpp#private_type_access).

# Usage example

```c++
class very_private
{
  private:
    int i = 1;
    void fi(int new_i)
    {
        i = new_i;
    };
};

#include "private_access/private_access.hpp"

#include <iostream>

EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, i)
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, fi)

namespace pa = privateaccess;

void use_i(const very_private& v)
{
    // Get reference to i
    const auto& i = pa::make_proxy<"i">(v)();
    std::cout << "i: " << i << '\n';
}

void set_i(very_private& v, int new_i)
{
    // Create proxy object and call referenced function
    auto f_proxy = pa::make_proxy<"fi">(v);
    f_proxy(new_i);
}

int main()
{
    very_private vp;
    set_i(vp, 42);
    use_i(vp);
}
```

You can find some more examples in [compile_tests.cpp](test/compile_test.cpp).

# Implementation strategy

In explicit instantiations the private addresses of members can be referenced legally. These addresses get stored with help of a tag. 

Thanks to C++20, the tag's types depend on a compile-time string that can used to directly reference members by their name without complicated macros.
With a compile-time hash from a compile-time string this could probably also be made to work with C++17 using a very similar syntax.

This library tries to keep usage of macros to a minimum and you could also write the instantiations manually like so:

```c++
namespace
{
namespace privateaccess
{
    template struct access_address<class_name, "member_name"_cs, &class_name::member_name>;
    constexpr auto get_address(tag_t<class_name, "member_name"_cs>);
} // namespace privateaccess 
}
```

# References

Original implementation idea:
https://bloglitb.blogspot.com/2010/07/access-to-private-members-thats-easy.html

Refined implementation:
https://bloglitb.blogspot.com/2011/12/access-to-private-members-safer.html

Some more refinements proposed here:
http://cpp.kjx.cz/private_backdoor.html

A very similar C++11 implementation: [martong/access_private](https://github.com/martong/access_private)
