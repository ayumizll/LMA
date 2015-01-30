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

#ifndef __OPTIMISATION2_FUNCTION_DERIVATIVE_DEFAULT_DERIVATIVE_HPP__
#define __OPTIMISATION2_FUNCTION_DERIVATIVE_DEFAULT_DERIVATIVE_HPP__

#include "default_derivative_tag.hpp"
#include "analytical_derivative.hpp"
#include "numerical_derivative.hpp"
#if __cplusplus > 201103L
  #include "automatic_derivative.hpp"
#endif
#include "detail_derivative.hpp"

namespace lma
{
  template<class Obs> struct ToDerivativeTag
  {
    typedef typename
    boost::mpl::if_<
		    boost::is_convertible<Obs*,NumericForward*>,
		    NumericForward,
		    typename boost::mpl::if_< 
					      boost::is_convertible<Obs*,Analytical*>,
					      Analytical,
					      typename boost::mpl::if_<
									boost::is_convertible<Obs*,Automatic*>,
									Automatic,
									NumericCentral// default numeric derivative is central
								      >::type
					    >::type
		   >::type type;
  };

  template<class ... T> void plz_no_warning(T...){}
  
  template<class tag> struct Derivator
  {
    typedef tag Tag;

    template<class Fonctor, class Tuple, class Jacob, class Erreur>
    static void derive(const lma::Function<Fonctor>& fonctor, const Tuple& tuple, Jacob& result, const Erreur& error, NumericForward) // numeric forward
    {
      NumericalDerivator<Tag>::derive(fonctor,tuple,result,error);
    }
    
    template<class Fonctor, class Tuple, class Jacob, class Erreur>
    static void derive(const lma::Function<Fonctor>& fonctor, const Tuple& tuple, Jacob& result, const Erreur&, NumericCentral) // numeric central
    {
      NumericalDerivator<Tag>::derive(fonctor,tuple,result);
    }
    
    template<class Fonctor, class Tuple, class Jacob, class Erreur>
    static void derive(const lma::Function<Fonctor>& fonctor, const Tuple& tuple, Jacob& result, const Erreur&, Analytical) // analytic
    {
      AnalyticalDerivator<Tag>::derive(fonctor,tuple,result);
    }
    
    
    template<class Fonctor, class Tuple, class Jacob, class Erreur>
    static void derive(const lma::Function<Fonctor>& fonctor, const Tuple& tuple, Jacob& result, const Erreur&, Automatic) // analytic
    {
      #if __cplusplus > 201103L
        AutomaticDerivator<Tag>::derive(fonctor,tuple,result);
      #else
        std::cerr << " Automatic derivative needs c++14" << std::endl;
        plz_no_warning(fonctor,tuple,result);
        abort();
      #endif
    }
    
  };
}

namespace ttt
{
  template<class Tag> struct Name<lma::Derivator<Tag>> { static std::string name(){ return std::string()+"Derivator<" + ttt::name<Tag>() + ">"; } };
}

#endif
