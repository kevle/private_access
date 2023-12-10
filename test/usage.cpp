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