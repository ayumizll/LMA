//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================


#include <libv/lma/lma.hpp>

struct DistanceFromCircleCost
{
  DistanceFromCircleCost(double xx, double yy) : xx_(xx), yy_(yy) {}
  bool operator()(double x, double y, double m, double& residual) const
  {
    const double 
      r = m * m,
      xp = xx_ - x,
      yp = yy_ - y;
    residual = r*r - xp*xp - yp*yp;
    return true;
  }
  double xx_, yy_;
};


int main(int , char **)
{
  std::vector<double> X;
  std::vector<double> Y;

  double x(4), y(2), r(2);

  std::default_random_engine generator;
  std::uniform_real_distribution<double> distrib_radius(r-0.1, r+0.1);
  std::uniform_real_distribution<double> distrib_angle(0., 2. * M_PI);

  for(size_t i = 0 ; i < 1000 ; ++i)
  {
    double radius = distrib_radius(generator);
    double angle = distrib_angle(generator);
    X.push_back(x + radius * cos(angle));
    Y.push_back(y + radius * sin(angle));
  }
  

  // Parameterize r as m^2 so that it can't be negative.
  x = y = r = 3;
  r = sqrt(r);

  double lambda = 0.001;
  double iteration_max = 5;
  lma::Solver<DistanceFromCircleCost> solver(lambda, iteration_max);

  for(size_t i = 0 ; i < X.size() ; ++i)
    solver.add(DistanceFromCircleCost(X[i], Y[i]), &x, &y, &r);

  solver.solve(lma::DENSE,lma::enable_verbose_output());

  r *= r;
  std::cout << "x,y,r = " << x << "," << y << "," << r << "\n";
  return 0;
}
