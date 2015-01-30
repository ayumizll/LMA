//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================


#include "lma.hpp"
#include <iostream>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if (argc != 4 && argc != 5) 
  {
    std::cerr << "usage: test-lma3d <bal_problem> lambda nb_iteration_max\n";
    return 0;
  }

  call_lma(argv[1],atof(argv[2]),atoi(argv[3]));

}
