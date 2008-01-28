// test_nc_beta.cpp

// Copyright John Maddock 2008.

// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt
// or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifdef _MSC_VER
#pragma warning (disable:4127 4512)
#endif

#if !defined(TEST_FLOAT) && !defined(TEST_DOUBLE) && !defined(TEST_LDOUBLE) && !defined(TEST_REAL_CONCEPT)
#  define TEST_FLOAT
#  define TEST_DOUBLE
#  define TEST_LDOUBLE
#  define TEST_REAL_CONCEPT
#endif

#include <boost/math/concepts/real_concept.hpp> // for real_concept
#include <boost/math/distributions/non_central_beta.hpp> // for chi_squared_distribution
#include <boost/test/included/test_exec_monitor.hpp> // for test_main
#include <boost/test/floating_point_comparison.hpp> // for BOOST_CHECK_CLOSE

#include "functor.hpp"
#include "handle_test_result.hpp"

#include <iostream>
using std::cout;
using std::endl;
#include <limits>
using std::numeric_limits;

#define BOOST_CHECK_CLOSE_EX(a, b, prec, i) \
   {\
      unsigned int failures = boost::unit_test::results_collector.results( boost::unit_test::framework::current_test_case().p_id ).p_assertions_failed;\
      BOOST_CHECK_CLOSE(a, b, prec); \
      if(failures != boost::unit_test::results_collector.results( boost::unit_test::framework::current_test_case().p_id ).p_assertions_failed)\
      {\
         std::cerr << "Failure was at row " << i << std::endl;\
         std::cerr << std::setprecision(35); \
         std::cerr << "{ " << data[i][0] << " , " << data[i][1] << " , " << data[i][2];\
         std::cerr << " , " << data[i][3] << " , " << data[i][4] << " } " << std::endl;\
      }\
   }

#define BOOST_CHECK_EX(a, i) \
   {\
      unsigned int failures = boost::unit_test::results_collector.results( boost::unit_test::framework::current_test_case().p_id ).p_assertions_failed;\
      BOOST_CHECK(a); \
      if(failures != boost::unit_test::results_collector.results( boost::unit_test::framework::current_test_case().p_id ).p_assertions_failed)\
      {\
         std::cerr << "Failure was at row " << i << std::endl;\
         std::cerr << std::setprecision(35); \
         std::cerr << "{ " << data[i][0] << " , " << data[i][1] << " , " << data[i][2];\
         std::cerr << " , " << data[i][3] << " , " << data[i][4] << " } " << std::endl;\
      }\
   }

void expected_results()
{
   //
   // Define the max and mean errors expected for
   // various compilers and platforms.
   //
   const char* largest_type;
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
   if(boost::math::policies::digits<double, boost::math::policies::policy<> >() == boost::math::policies::digits<long double, boost::math::policies::policy<> >())
   {
      largest_type = "(long\\s+)?double|real_concept";
   }
   else
   {
      largest_type = "long double|real_concept";
   }
#else
   largest_type = "(long\\s+)?double|real_concept";
#endif

   //
   // Catch all cases come last:
   //
   add_expected_result(
      "[^|]*",                          // compiler
      "[^|]*",                          // stdlib
      "[^|]*",                          // platform
      largest_type,                     // test type(s)
      "[^|]*medium[^|]*",               // test data group
      "[^|]*", 700, 500);               // test function
   add_expected_result(
      "[^|]*",                          // compiler
      "[^|]*",                          // stdlib
      "[^|]*",                          // platform
      largest_type,                     // test type(s)
      "[^|]*large[^|]*",                // test data group
      "[^|]*", 9000, 2000);             // test function
   //
   // Finish off by printing out the compiler/stdlib/platform names,
   // we do this to make it easier to mark up expected error rates.
   //
   std::cout << "Tests run with " << BOOST_COMPILER << ", " 
      << BOOST_STDLIB << ", " << BOOST_PLATFORM << std::endl;
}

template <class RealType>
RealType naive_pdf(RealType v, RealType lam, RealType x)
{
   // TODO!
   return 0;
}

template <class RealType>
void test_spot(
     RealType a,     // alpha
     RealType b,     // beta
     RealType ncp,   // non-centrality param
     RealType cs,    // Chi Square statistic
     RealType P,     // CDF
     RealType Q,     // Complement of CDF
     RealType tol)   // Test tolerance
{
   boost::math::non_central_beta_distribution<RealType> dist(a, b, ncp);
   BOOST_CHECK_CLOSE(
      cdf(dist, cs), P, tol);
   try{/*
      BOOST_CHECK_CLOSE(
         pdf(dist, cs), naive_pdf(dist.degrees_of_freedom(), ncp, cs), tol * 50);*/
   }
   catch(const std::overflow_error&)
   {}
   if((P < 0.99) && (Q < 0.99))
   {
      //
      // We can only check this if P is not too close to 1,
      // so that we can guarentee Q is reasonably free of error:
      //
      BOOST_CHECK_CLOSE(
         cdf(complement(dist, cs)), Q, tol);
      BOOST_CHECK_CLOSE(
            quantile(dist, P), cs, tol * 10);
      BOOST_CHECK_CLOSE(
            quantile(complement(dist, Q)), cs, tol * 10);
   }
}

template <class RealType> // Any floating-point type RealType.
void test_spots(RealType)
{
   RealType tolerance = (std::max)(
      boost::math::tools::epsilon<RealType>(),
      (RealType)boost::math::tools::epsilon<double>() * 5) * 100;
   //
   // At float precision we need to up the tolerance, since 
   // the input values are rounded off to inexact quantities
   // the results get thrown off by a noticeable amount.
   //
   if(boost::math::tools::digits<RealType>() < 50)
      tolerance *= 50;
   if(boost::is_floating_point<RealType>::value != 1)
      tolerance *= 20; // real_concept special functions are less accurate

   cout << "Tolerance = " << tolerance << "%." << endl;

} // template <class RealType>void test_spots(RealType)

template <class T>
T nc_beta_cdf(T a, T b, T nc, T x)
{
   return cdf(boost::math::non_central_beta_distribution<T>(a, b, nc), x);
}

template <class T>
T nc_beta_ccdf(T a, T b, T nc, T x)
{
   return cdf(complement(boost::math::non_central_beta_distribution<T>(a, b, nc), x));
}

template <typename T>
void do_test_nc_chi_squared(T& data, const char* type_name, const char* test)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   std::cout << "Testing: " << test << std::endl;

   value_type (*fp1)(value_type, value_type, value_type, value_type) = nc_beta_cdf;
   boost::math::tools::test_result<value_type> result;

   result = boost::math::tools::test(
      data,
      bind_func(fp1, 0, 1, 2, 3),
      extract_result(4));
   handle_test_result(result, data[result.worst()], result.worst(),
      type_name, "CDF", test);

   fp1 = nc_beta_ccdf;
   result = boost::math::tools::test(
      data,
      bind_func(fp1, 0, 1, 2, 3),
      extract_result(5));
   handle_test_result(result, data[result.worst()], result.worst(),
      type_name, "CCDF", test);

   std::cout << std::endl;

}

template <typename T>
void quantile_sanity_check(T& data, const char* type_name, const char* test)
{
   typedef typename T::value_type row_type;
   typedef typename row_type::value_type value_type;

   //
   // Tests with type real_concept take rather too long to run, so
   // for now we'll disable them:
   //
   if(!boost::is_floating_point<value_type>::value)
      return;

   std::cout << "Testing: " << type_name << " quantile sanity check, with tests " << test << std::endl;

   //
   // These sanity checks test for a round trip accuracy of one half
   // of the bits in T, unless T is type float, in which case we check
   // for just one decimal digit.  The problem here is the sensitivity
   // of the functions, not their accuracy.  This test data was generated
   // for the forward functions, which means that when it is used as
   // the input to the inverses then it is necessarily inexact.  This rounding
   // of the input is what makes the data unsuitable for use as an accuracy check,
   // and also demonstrates that you can't in general round-trip these functions.
   // It is however a useful sanity check.
   //
   value_type precision = static_cast<value_type>(ldexp(1.0, 1-boost::math::policies::digits<value_type, boost::math::policies::policy<> >()/2)) * 100;
   if(boost::math::policies::digits<value_type, boost::math::policies::policy<> >() < 50)
      precision = 1;   // 1% or two decimal digits, all we can hope for when the input is truncated to float

   for(unsigned i = 0; i < data.size(); ++i)
   {
      if(data[i][4] == 0)
      {
         BOOST_CHECK(0 == quantile(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1], data[i][2]), data[i][4]));
      }
      else if(data[i][4] < 0.9999f)
      {
         value_type p = quantile(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1], data[i][2]), data[i][4]);
         value_type pt = data[i][3];
         BOOST_CHECK_CLOSE_EX(pt, p, precision, i);
      }
      if(data[i][5] == 0)
      {
         BOOST_CHECK(1 == quantile(complement(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1], data[i][2]), data[i][5])));
      }
      else if(data[i][5] < 0.9999f)
      {
         value_type p = quantile(complement(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1], data[i][2]), data[i][5]));
         value_type pt = data[i][3];
         BOOST_CHECK_CLOSE_EX(pt, p, precision, i);
      }
      /*
      if(boost::math::tools::digits<value_type>() > 50)
      {
         //
         // Sanity check mode, note this may well overflow
         // since we don't know how to compute PDF's (and hence the mode) 
         // for large values of the parameters, in addition accuracy of
         // the mode is at *best* the square root of the accuracy of the PDF:
         //
         try
         {
            value_type m = mode(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1]));
            value_type p = pdf(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1]), m);
            BOOST_CHECK_EX(pdf(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1]), m * (1 + sqrt(precision) * 10)) <= p, i);
            BOOST_CHECK_EX(pdf(boost::math::non_central_beta_distribution<value_type>(data[i][0], data[i][1]), m * (1 - sqrt(precision)) * 10) <= p, i);
         }
         catch(const std::overflow_error&)
         {
         }
         //
         // Sanity check degrees-of-freedom finder, don't bother at float
         // precision though as there's not enough data in the probability
         // values to get back to the correct degrees of freedom or 
         // non-cenrality parameter:
         //
         try{
            if((data[i][3] < 0.99) && (data[i][3] != 0))
            {
               BOOST_CHECK_CLOSE_EX(
                  boost::math::non_central_beta_distribution<value_type>::find_degrees_of_freedom(data[i][1], data[i][2], data[i][3]),
                  data[i][0], precision, i);
               BOOST_CHECK_CLOSE_EX(
                  boost::math::non_central_beta_distribution<value_type>::find_non_centrality(data[i][0], data[i][2], data[i][3]),
                  data[i][1], precision, i);
            }
            if((data[i][4] < 0.99) && (data[i][4] != 0))
            {
               BOOST_CHECK_CLOSE_EX(
                  boost::math::non_central_beta_distribution<value_type>::find_degrees_of_freedom(boost::math::complement(data[i][1], data[i][2], data[i][4])),
                  data[i][0], precision, i);
               BOOST_CHECK_CLOSE_EX(
                  boost::math::non_central_beta_distribution<value_type>::find_non_centrality(boost::math::complement(data[i][0], data[i][2], data[i][4])),
                  data[i][1], precision, i);
            }
         }
         catch(const std::exception& e)
         {
            BOOST_ERROR(e.what());
         }
      }
      */
   }
}

template <typename T>
void test_accuracy(T, const char* type_name)
{
#include "ncbeta.ipp"
    do_test_nc_chi_squared(ncbeta, type_name, "Non Central Beta, medium parameters");
    quantile_sanity_check(ncbeta, type_name, "Non Central Beta, medium parameters");

#include "ncbeta_big.ipp"
    do_test_nc_chi_squared(ncbeta_big, type_name, "Non Central Beta, large parameters");
    // quantile_sanity_check(ncbeta_big, type_name, "Non Central Beta, large parameters");
}

int test_main(int, char* [])
{
   BOOST_MATH_CONTROL_FP;
   // Basic sanity-check spot values.
   expected_results();
   // (Parameter value, arbitrarily zero, only communicates the floating point type).
#ifdef TEST_FLOAT
   test_spots(0.0F); // Test float.
#endif
#ifdef TEST_DOUBLE
   test_spots(0.0); // Test double.
#endif
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
#ifdef TEST_LDOUBLE
   test_spots(0.0L); // Test long double.
#endif
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x582))
#ifdef TEST_REAL_CONCEPT
   test_spots(boost::math::concepts::real_concept(0.)); // Test real concept.
#endif
#endif
#endif

#ifdef TEST_FLOAT
   test_accuracy(0.0F, "float"); // Test float.
#endif
#ifdef TEST_DOUBLE
   test_accuracy(0.0, "double"); // Test double.
#endif
#ifndef BOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS
#ifdef TEST_LDOUBLE
   test_accuracy(0.0L, "long double"); // Test long double.
#endif
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x582))
#ifdef TEST_REAL_CONCEPT
   test_accuracy(boost::math::concepts::real_concept(0.), "real_concept"); // Test real concept.
#endif
#endif
#endif
   return 0;
} // int test_main(int, char* [])

