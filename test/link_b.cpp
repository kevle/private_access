#include "link.hpp"

#include "private_access/private_access.hpp"

EXPLICIT_INSTANTION_PRIVATE_MEMBER_ACCESS(very_private, i)

namespace pa = privateaccess;

int b(const very_private& vp)
{
    return *pa::make_proxy<"i">(vp);
}
