#include <boost/fusion/include/make_vector.hpp>
#include <libv/lma/lm/function/function.hpp>
#include <libv/lma/global.hpp>

struct F
{
  bool operator()(int , const float& , const double, double & res) const
  {
    res = 3.159;
    return true;
  }
};

int main()
{

  double d;
  F f;
  lma::Function<F> fun(f);
  
//   return (fun(lma::bf::make_vector(1,2.0f,3.0),d) && d == 3.159 ? EXIT_SUCCESS : EXIT_FAILURE);
  return EXIT_FAILURE;
}

