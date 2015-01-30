//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

#include <libv/lma/lma.hpp>
#include <libv/core/miscmath.hpp>

using namespace lma;

int main()
{
  auto rosenbrock = [](double x, double y){ return v::pow<2>(1-x) + 100*v::pow<2>(y - v::pow<2>(x)); };
  auto functor = [&rosenbrock](double x, double y, double& r){ r = rosenbrock(x,y); return true; };
  // Initial solution
  double x(-1),y(-1);
  std::cout << " Initial parameters   : (" << x << "," << y << ") with an error of " << rosenbrock(x,y) <<  std::endl;
  Solver<decltype(functor)>()
  .add(functor,&x,&y)
  .solve(DENSE,enable_verbose_output());
  std::cout << " Optimized parameters : (" << x << "," << y << ") with an error of " << rosenbrock(x,y) <<  std::endl;
  return EXIT_SUCCESS;
}
