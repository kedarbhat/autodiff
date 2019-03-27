//           Copyright Matthew Pulver 2018 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include "test_autodiff.hpp"

using namespace boost::math::differentiation;

/*********************************************************************************************************************
 * special functions tests
 *********************************************************************************************************************/

BOOST_AUTO_TEST_SUITE(test_autodiff_6)

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_1_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> k_sampler{T{-1}, T{1}};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(),
                                           boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto k = k_sampler.next();
    auto phi = phi_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::ellint_1(make_fvar<T, m>(k)),
                        boost::math::ellint_1(k),
                        2.5e3 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_1(make_fvar<T, m>(k), make_fvar<T, m>(phi)),
        boost::math::ellint_1(k, phi), 1e4 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_2_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> k_sampler{-1, 1};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(),
                                           boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto k = k_sampler.next();
    auto phi = phi_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::ellint_2(make_fvar<T, m>(k)),
                        boost::math::ellint_2(k),
                        2.5e3 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_2(make_fvar<T, m>(k), make_fvar<T, m>(phi)),
        boost::math::ellint_2(k, phi), 2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_3_hpp, T, all_float_types) {
  using boost::math::nextafter;
  using boost::multiprecision::nextafter;

  using std::sin;
  using boost::math::differentiation::detail::sin;
  using boost::multiprecision::min;
  using std::min;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> k_sampler{-1, 1};
  test_detail::RandomSample<T> n_sampler{-2000, 2000};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(),
                                           boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto k = k_sampler.next();
    auto phi = phi_sampler.next();
    auto n = (min)((min)(n_sampler.next(), T(1) / (sin(phi) * sin(phi))),
                   nextafter(T(1), T(0)));
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_3(make_fvar<T, m>(k), make_fvar<T, m>(n),
                              make_fvar<T, m>(phi)),
        boost::math::ellint_3(k, n, phi),
        2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_d_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> k_sampler{-1, 1};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(),
                                           boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto k = k_sampler.next();
    auto phi = phi_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::ellint_d(make_fvar<T, m>(k)),
                        boost::math::ellint_d(k),
                        2.5e3 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_d(make_fvar<T, m>(k), make_fvar<T, m>(phi)),
        boost::math::ellint_d(k, phi), 2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_rf_hpp, T, all_float_types) {

  using boost::math::tools::max;
  using std::max;

  using boost::math::nextafter;
  using std::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;

  test_detail::RandomSample<T> x_sampler{0, 2000};
  test_detail::RandomSample<T> y_sampler{0, 2000};
  test_detail::RandomSample<T> z_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = nextafter(x_sampler.next(), ((std::numeric_limits<T>::max))());
    auto y = nextafter(y_sampler.next(), ((std::numeric_limits<T>::max))());
    auto z = nextafter(z_sampler.next(), ((std::numeric_limits<T>::max))());

    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_rf(make_fvar<T, m>(x), make_fvar<T, m>(y),
                               make_fvar<T, m>(z)),
        boost::math::ellint_rf(x, y, z), 2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_rc_hpp, T, all_float_types) {
  using boost::math::nextafter;
  using boost::math::tools::max;
  using std::max;
  using std::nextafter;
  using boost::math::signbit;
  using boost::multiprecision::signbit;
  using boost::math::fpclassify;
  using boost::multiprecision::fpclassify;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  test_detail::RandomSample<T> y_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto y = T(0);
    while (fpclassify(T(y)) == FP_ZERO) {
      y = (max)(y_sampler.next(),
                nextafter(T(0), T(signbit(y) ? -1 : 1)*((std::numeric_limits<T>::max))()));
    }

    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_rc(make_fvar<T, m>(x), make_fvar<T, m>(y)),
        boost::math::ellint_rc(x, y), 2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_rj_hpp, T, all_float_types) {
  using boost::math::nextafter;
  using boost::math::tools::max;
  using boost::math::fpclassify;
  using boost::multiprecision::fpclassify;
  using boost::math::signbit;
  using boost::multiprecision::signbit;

  using std::max;
  using std::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;

  test_detail::RandomSample<T> x_sampler{0, 2000};
  test_detail::RandomSample<T> y_sampler{0, 2000};
  test_detail::RandomSample<T> z_sampler{0, 2000};
  test_detail::RandomSample<T> p_sampler{-2000, 2000};

  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto y = (x != 0 ? 1 : 0) + y_sampler.next();
    auto z = ((x == 0 || y == 0) ? 1 : 0) + z_sampler.next();
    auto p = T(0);

    while (fpclassify(T(p)) == FP_ZERO) {
      p = (max)(p_sampler.next(),
                nextafter(T(0), T(signbit(p) ? -1 : 1) * ((std::numeric_limits<T>::max))()));
    }
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_rj(make_fvar<T, m>(x), make_fvar<T, m>(y),
                               make_fvar<T, m>(z), make_fvar<T, m>(p)),
        boost::math::ellint_rj(x, y, z, p),
        2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_rd_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  test_detail::RandomSample<T> y_sampler{0, 2000};
  test_detail::RandomSample<T> z_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto y = (x == 0 ? 1 : 0) + y_sampler.next();
    auto z = z_sampler.next();
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_rd(make_fvar<T, m>(x), make_fvar<T, m>(y),
                               make_fvar<T, m>(z)),
        boost::math::ellint_rd(x, y, z), 2.5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(ellint_rg_hpp, T, all_float_types) {

  using boost::math::nextafter;
  using std::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  test_detail::RandomSample<T> y_sampler{0, 2000};
  test_detail::RandomSample<T> z_sampler{0, 2000};

  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = nextafter(x_sampler.next(), ((std::numeric_limits<T>::max))());
    auto y = nextafter(y_sampler.next(), ((std::numeric_limits<T>::max))());
    auto z = z_sampler.next();
    BOOST_REQUIRE_CLOSE(
        boost::math::ellint_rg(make_fvar<T, m>(x), make_fvar<T, m>(y),
                               make_fvar<T, m>(z)),
        boost::math::ellint_rg(x, y, z), 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(erf_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();

    BOOST_REQUIRE_CLOSE(erf(make_fvar<T, m>(x)), boost::math::erf(x),
                        200 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(erfc(make_fvar<T, m>(x)), boost::math::erfc(x),
                        200 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(expint_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{1, 83};
  for (auto n :
      boost::irange(1u, static_cast<unsigned>(test_constants::n_samples))) {
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::expint(n, make_fvar<T, m>(x)),
                        boost::math::expint(n, x),
                        200 * test_constants::pct_epsilon());

    for (auto y : {-1, 1}) {
      BOOST_REQUIRE_CLOSE(boost::math::expint(make_fvar<T, m>(x * y)),
                          boost::math::expint(x * y),
                          200 * test_constants::pct_epsilon());
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
