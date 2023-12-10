#include "private_access/private_access.hpp"

#include <iostream>
#include <string>

class very_private
{
  private:
    const int i = 1;
    int j = 2;
    constexpr int f(int a) const
    {
        return a;
    }

    static const int s = 3;
    constexpr static int sf(int a)
    {
        return a;
    }
};

EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, i)
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, j)
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, f)
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, s)
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, sf)

class private_type_access
{
  private:
    class private_type
    {
        int i = 4;
    };
    private_type p;
};

namespace pa = privateaccess;

EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(private_type_access, p)
using private_type_p = pa::get_member_type_t<private_type_access, "p">;
EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(private_type_p, i)

int main()
{
    {
        // Access with member address pointer
        constexpr very_private v;
        constexpr auto member_address = pa::get_address<very_private, "i">();
        static_assert(v.*member_address == 1, "Bad direct address use");
    }
    {
        // Direct access with proxy object and operator*
        constexpr auto f = []() {
            very_private b;
            auto j_proxy = pa::make_proxy<"j">(b);
            *j_proxy = 4;
            return *j_proxy;
        };
        static_assert(f() == 4, "Bad assignment");
    }
    {
        // Proxy access
        constexpr auto j_proxy = pa::make_proxy<"j">(very_private{});
        static_assert(*j_proxy == 2, "Bad object access");
    }
    {
        // Function access
        constexpr auto f_proxy = pa::make_proxy<"f">(very_private{});
        static_assert(f_proxy(1) == 1, "Bad function access");
    }
    {
        // Access to static member
        constexpr auto static_address = pa::get_address<very_private, "s">();
        static_assert(*static_address == 3, "Bad static address use");
    }
    {
        // Access to static function
        constexpr auto static_address = pa::get_address<very_private, "sf">();
        static_assert((*static_address)(5) == 5, "Bad static address use");
    }

    {
        // Access to private member of private class
        static_assert(pa::make_proxy<"i">(*pa::make_proxy<"p">(private_type_access{}))() == 4,
                      "Bad private member type access.");
    }
}
