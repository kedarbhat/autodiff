//           Copyright Matthew Pulver 2018 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include "test_autodiff.hpp"

#include <boost/utility/identity_type.hpp>

using namespace boost::math::differentiation;

BOOST_AUTO_TEST_SUITE(test_autodiff_5)

BOOST_AUTO_TEST_CASE_TEMPLATE(beta_hpp, T, BOOST_IDENTITY_TYPE((mp11::mp_list<float, double>))) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> a_sampler{-2000, 2000};
  test_detail::RandomSample<T> b_sampler{-2000, 2000};
  test_detail::RandomSample<T> z_sampler{0, 1};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto a = a_sampler.next();
    auto b = b_sampler.next();
    auto z = z_sampler.next();
    {
      auto a_ = abs(a) + 1;
      auto b_ = abs(b) + 1;
      try {
        auto autodiff_v = boost::math::beta(
            make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
        auto anchor_v = boost::math::beta(a_, b_, z);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
        } else {
          BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
        }
      } catch (const boost::math::evaluation_error &) {
        BOOST_REQUIRE_THROW(boost::math::beta(make_fvar<T, m>(a_),
                                              make_fvar<T, m>(b_),
                                              make_fvar<T, m>(z)),
                            boost::wrapexcept<boost::math::evaluation_error>);
        BOOST_REQUIRE_THROW(boost::math::beta(a_, b_, z),
                            boost::wrapexcept<boost::math::evaluation_error>);
      }
      catch(...)  {
        std::cout << a_ << "\t" << b_ << "\t" << z << std::endl;
        std::rethrow_exception(std::current_exception());
      }

      try {
        auto autodiff_v = boost::math::betac(
            make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
        auto anchor_v = boost::math::betac(a_, b_, z);
        static_assert(std::is_same<decltype(anchor_v), T>::value, "");
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
        } else {
          BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
        }
      } catch (const boost::math::evaluation_error &) {
        BOOST_REQUIRE_THROW(boost::math::betac(make_fvar<T, m>(a_),
                                               make_fvar<T, m>(b_),
                                               make_fvar<T, m>(z)),
                            boost::wrapexcept<boost::math::evaluation_error>);
        BOOST_REQUIRE_THROW(boost::math::betac(a_, b_, z),
                            boost::wrapexcept<boost::math::evaluation_error>);
      } catch(...) {
        std::cout << a_ << "\t" << b_ << "\t" << z << std::endl;
        std::rethrow_exception(std::current_exception());
      }

      try {
        auto autodiff_v =
            boost::math::ibeta(make_fvar<T, m>(a_ - 1), make_fvar<T, m>(b_ - 1),
                               make_fvar<T, m>(z));
        auto anchor_v = boost::math::ibeta(a_ - 1, b_ - 1, z);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
        } else {
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
        }
      } catch (const boost::math::evaluation_error &) {
        BOOST_REQUIRE_THROW(boost::math::ibeta(make_fvar<T, m>(a_ - 1),
                                               make_fvar<T, m>(b_ - 1),
                                               make_fvar<T, m>(z)),
                            boost::wrapexcept<boost::math::evaluation_error>);
        BOOST_REQUIRE_THROW(boost::math::ibeta(a_ - 1, b_ - 1, z),
                            boost::wrapexcept<boost::math::evaluation_error>);
      }catch(...)  {
        std::cout << (a_-1) << "\t" << (b_-1) << "\t" << z << std::endl;
        std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibeta(make_fvar<T, m>(a_-1), make_fvar<T, m>(b_-1), make_fvar<T, m>(z))), boost::math::ibeta(a_-1, b_-1, z)) << std::endl;
        std::rethrow_exception(std::current_exception());
      }

      try {
        auto autodiff_v = boost::math::ibetac(make_fvar<T, m>(a_ - 1), make_fvar<T, m>(b_ - 1), make_fvar<T, m>(z));
        auto anchor_v = boost::math::ibetac(a_ - 1, b_ - 1, z);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
        } else {
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
        }
      } catch (const boost::math::evaluation_error &) {
        BOOST_REQUIRE_THROW(boost::math::ibetac(make_fvar<T, m>(a_ - 1),
                                                make_fvar<T, m>(b_ - 1),
                                                make_fvar<T, m>(z)),
                            boost::wrapexcept<boost::math::evaluation_error>);
        BOOST_REQUIRE_THROW(boost::math::ibetac(a_ - 1, b_ - 1, z),
                            boost::wrapexcept<boost::math::evaluation_error>);
      } catch(...)  {
        std::cout << (a_-1) << "\t" << (b_-1) << "\t" << z << std::endl;
        std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibetac(make_fvar<T, m>(a_-1), make_fvar<T, m>(b_-1), make_fvar<T, m>(z))), boost::math::ibetac(a_-1, b_-1, z)) << std::endl;
        std::rethrow_exception(std::current_exception());
      }

      try {
        auto autodiff_v = boost::math::ibeta_derivative(
            make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
        auto anchor_v = boost::math::ibeta_derivative(a_, b_, z);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
        } else {
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
        }
      } catch (const boost::math::evaluation_error &) {
        BOOST_REQUIRE_THROW(boost::math::ibeta_derivative(make_fvar<T, m>(a_),
                                                          make_fvar<T, m>(b_),
                                                          make_fvar<T, m>(z)),
                            boost::wrapexcept<boost::math::evaluation_error>);
        BOOST_REQUIRE_THROW(boost::math::ibeta_derivative(a_, b_, z),
                            boost::wrapexcept<boost::math::evaluation_error>);
      }catch(...)  {
        std::cout << a_ << "\t" << b_ << "\t" << z << std::endl;
        std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibeta_derivative<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z))), boost::math::ibeta_derivative<T>(a_, b_, z)) << std::endl;
        std::rethrow_exception(std::current_exception());
      }

      {
        try {
          auto autodiff_v = boost::math::ibeta_inv(
              make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibeta_inv<T>(a_, b_, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibeta_inv(make_fvar<T, m>(a_),
                                                     make_fvar<T, m>(b_),
                                                     make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibeta_inv(a_, b_, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        }catch(...)  {
          std::cout << a_ << "\t" << b_ << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibeta_inv<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z))), boost::math::ibeta_inv<T>(a_, b_, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }

        try {
          auto autodiff_v = boost::math::ibetac_inv<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibetac_inv<T>(a_, b_, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibetac_inv(make_fvar<T, m>(a_),
                                                      make_fvar<T, m>(b_),
                                                      make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibetac_inv(a_, b_, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        } catch(...)  {
          std::cout << a_ << "\t" << b_ << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibetac_inv<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z))), boost::math::ibetac_inv<T>(a_, b_, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }
      }
      {
        auto b_norm = b_sampler.normalize(abs(b));
        try {
          auto autodiff_v = boost::math::ibeta_inva(
              make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibeta_inva(a_, b_norm, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibeta_inva(make_fvar<T, m>(a_),
                                                      make_fvar<T, m>(b_norm),
                                                      make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibeta_inva(a_, b_norm, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        }catch(...)  {
          std::cout << a_ << "\t" << b_norm << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibeta_inva<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z))), boost::math::ibeta_inva<T>(a_, b_norm, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }

        try {
          auto autodiff_v = boost::math::ibetac_inva(
              make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibetac_inva(a_, b_norm, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibetac_inva(make_fvar<T, m>(a_),
                                                       make_fvar<T, m>(b_norm),
                                                       make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibetac_inva(a_, b_norm, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        }catch(...)  {
          std::cout << a_ << "\t" << b_norm << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibetac_inva<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z))), boost::math::ibetac_inva<T>(a_, b_norm, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }

        try {
          auto autodiff_v = boost::math::ibeta_invb(
              make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibeta_invb(a_, b_norm, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibeta_invb(make_fvar<T, m>(a_),
                                                      make_fvar<T, m>(b_norm),
                                                      make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibeta_invb(a_, b_norm, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        }catch(...)  {
          std::cout << a_ << "\t" << b_norm << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibeta_invb<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z))), boost::math::ibeta_invb<T>(a_, b_norm, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }

        try {
          auto autodiff_v = boost::math::ibetac_invb(
              make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
          auto anchor_v = boost::math::ibetac_invb(a_, b_norm, z);
          if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
            BOOST_REQUIRE_EQUAL(isZeroOrSubnormal(autodiff_v), isZeroOrSubnormal(anchor_v));
          } else {
            BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, test_constants::pct_epsilon());
          }
        } catch (const boost::math::evaluation_error &) {
          BOOST_REQUIRE_THROW(boost::math::ibetac_invb(make_fvar<T, m>(a_),
                                                       make_fvar<T, m>(b_norm),
                                                       make_fvar<T, m>(z)),
                              boost::wrapexcept<boost::math::evaluation_error>);
          BOOST_REQUIRE_THROW(boost::math::ibetac_invb(a_, b_norm, z),
                              boost::wrapexcept<boost::math::evaluation_error>);
        }catch(...)  {
          std::cout << a_ << "\t" << b_norm << "\t" << z << std::endl;
          std::cout << boost::math::float_distance(static_cast<T>(boost::math::ibetac_invb<autodiff_fvar<T, m>>(make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z))), boost::math::ibetac_invb<T>(a_, b_norm, z)) << std::endl;
          std::rethrow_exception(std::current_exception());
        }
      }
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(binomial_hpp, T, all_float_types) {

  using boost::multiprecision::min;
  using std::min;
  using std::fabs;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<unsigned> n_sampler{0u, 30u};
  test_detail::RandomSample<unsigned> r_sampler{0u, 30u};

  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto n = n_sampler.next();
    auto r = n == 0 ? 0 : (min)(r_sampler.next(), n - 1);

    // This is a hard function to test for type float;
    auto autodiff_v = std::is_same<T, float>::value ? make_fvar<T, m>(boost::math::binomial_coefficient<T>(n, r)) : boost::math::binomial_coefficient<T>(n, r);
    auto anchor_v = boost::math::binomial_coefficient<T>(n, r);
    BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cbrt_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_EQUAL(boost::math::cbrt(make_fvar<T, m>(x)),
                        boost::math::cbrt(x));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(chebyshev_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  {
    test_detail::RandomSample<unsigned> n_sampler{0u, 10u};
    test_detail::RandomSample<T> x_sampler{-2, 2};
    for (auto i : boost::irange(test_constants::n_samples)) {
      std::ignore = i;
      auto n = n_sampler.next();
      auto x = x_sampler.next();
      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_t(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_t(n, x), test_constants::pct_epsilon());
      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_u(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_u(n, x), test_constants::pct_epsilon());
      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_t_prime(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_t_prime(n, x), test_constants::pct_epsilon());

      // /usr/include/boost/math/special_functions/chebyshev.hpp:164:40: error:
      // cannot convert
      // ‘boost::boost::math::differentiation::autodiff_v1::detail::fvar<double,
      // 3>’ to ‘double’ in return
      // BOOST_REQUIRE_EQUAL(boost::math::chebyshev_clenshaw_recurrence(c.data(),c.size(),make_fvar<T,m>(0.20))
      // ,
      // boost::math::chebyshev_clenshaw_recurrence(c.data(),c.size(),static_cast<T>(0.20)));
      /*try {
        std::array<T, 4> c0{{14.2, -13.7, 82.3, 96}};
        BOOST_REQUIRE_EQUAL(boost::math::chebyshev_clenshaw_recurrence(c0.data(),
      c0.size(), make_fvar<T,m>(x)),
                                     boost::math::chebyshev_clenshaw_recurrence(c0.data(),
      c0.size(), x), 10* } catch (...) {
        std::rethrow_exception(std::exception_ptr(std::current_exception()));
      }*/
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cospi_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_EQUAL(boost::math::cos_pi(make_fvar<T, m>(x)),
                        boost::math::cos_pi(x));
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(digamma_hpp, T, all_float_types) {

  using boost::math::nextafter;
  using std::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-1, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = nextafter(x_sampler.next(), ((std::numeric_limits<T>::max))());
    auto autodiff_v = boost::math::digamma(make_fvar<T, m>(x));
    auto anchor_v = boost::math::digamma(x);
    BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
  }
}

BOOST_AUTO_TEST_SUITE_END()
