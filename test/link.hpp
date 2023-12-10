#pragma once

class very_private
{
  private:
    int i = 0;
};

int a(const very_private& vp);
int b(const very_private& vp);
