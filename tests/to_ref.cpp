#include <libv/lma/ttt/traits/to_ref.hpp>
#include <libv/lma/ttt/traits/clement.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/assert.hpp>
#include <cassert>

using namespace ttt;
using namespace boost;

int main()
{

  int a = 5;
  const int b = 5;
  int *p = &a;
  const int *q = &a;

  //BOOST_MPL_ASSERT((boost::is_same<int,decltype(to_ref(1))>));

  BOOST_MPL_ASSERT((boost::is_same<int&,decltype(to_ref(a))>));
  BOOST_MPL_ASSERT((boost::is_same<const int&,decltype(to_ref(b))>));

  BOOST_MPL_ASSERT((boost::is_same<int&,decltype(to_ref(p))>));
  BOOST_MPL_ASSERT((boost::is_same<const int&,decltype(to_ref(q))>));

  assert( &b == &to_ref(b));
  assert( &a == &to_ref(p));

  int& c = to_ref(p);
  assert( &c == &a );
  return EXIT_SUCCESS;
}
