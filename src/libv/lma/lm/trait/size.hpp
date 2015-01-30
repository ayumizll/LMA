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

#ifndef __OPTIMISATION2_TRAIT_SIZE_HPP__
#define __OPTIMISATION2_TRAIT_SIZE_HPP__

#include <cstddef>
#include <utility>
#include <libv/core/tag.hpp>
#include <type_traits>
#include <boost/mpl/bool.hpp>
#include <array>

namespace lma
{
  template<class T, class = void> struct Size;

  template<class T, size_t N> struct Size< T[N] > { static const size_t value = N;};
  
  template<class T> struct Size<T&>       { static const std::size_t value = Size<T>::value; };
  template<class T> struct Size<const T>  { static const std::size_t value = Size<T>::value; };
  template<class T> struct Size<T*>       { static const std::size_t value = Size<T>::value; };
  template<class T> struct Size<const T*> { static const std::size_t value = Size<T>::value; };
  template<class T> struct Size<const T&> { static const std::size_t value = Size<T>::value; };


  template<class T, size_t N> struct Size< std::array<T,N> > { enum { value = N } ; };

  template<class T> struct Size< std::pair<T,bool> > { static const std::size_t value = Size<T>::value; };

  template<> struct Size< float >         { static const std::size_t value = 1; };
  template<> struct Size< int >           { static const std::size_t value = 1; };
  template<> struct Size< double >        { static const std::size_t value = 1; };
  template<int I> struct Size<v::numeric_tag<I>> { static const std::size_t value = I; };
  

  template<class X, class R = void> struct SizeIsDefined : boost::mpl::false_ {};
  template<class X> struct SizeIsDefined< X, typename std::enable_if<sizeof(Size<X>)>::type > : boost::mpl::true_ {};
}

#endif
