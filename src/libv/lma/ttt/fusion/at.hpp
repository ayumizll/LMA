/**

\file
\author Datta Ramadasan
//==============================================================================
//         Copyright 2015 INSTITUT PASCAL UMR 6602 CNRS/Univ. Clermont II
//
//          Distributed under the Boost Software License, Version 1.0.
//                 See accompanying file LICENSE.txt or copy at
//                     http://www.boost.org/LICENSE_1_0.txt
//==============================================================================

*/

#ifndef __TTT_FUSION_AT_HPP__
#define __TTT_FUSION_AT_HPP__

#include <boost/fusion/include/at_key.hpp>
#include <libv/lma/global.hpp>

namespace ttt
{
  template<class A, class B, class Container> auto at(const Container& container) -> decltype(bf::at_key<B>(bf::at_key<A>(container)))
  {
    return bf::at_key<B>(bf::at_key<A>(container));
  }
}

#endif


