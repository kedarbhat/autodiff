#include "test_autodiff.hpp"

BOOST_AUTO_TEST_SUITE(test_autodiff_8)

BOOST_AUTO_TEST_CASE_TEMPLATE(hermite_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-200, 200};
  for (auto i : boost::irange(14u)) {
    auto x = x_sampler.next();
    auto autodiff_v = boost::math::hermite(i, make_fvar<T, m>(x));
    auto anchor_v = boost::math::hermite(i, x);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(heuman_lambda_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-1, 1};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(), boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto phi = phi_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::heuman_lambda(make_fvar<T, m>(x), make_fvar<T, m>(phi)),
                        boost::math::heuman_lambda(x, phi), 250 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(hypot_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  test_detail::RandomSample<T> y_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto y = y_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::hypot(make_fvar<T, m>(x), make_fvar<T, m>(y)), boost::math::hypot(x, y),
                        50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(jacobi_elliptic_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> k_sampler{0, 1};
  test_detail::RandomSample<T> theta_sampler{-100, 100};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto k = k_sampler.next();
    auto theta = theta_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_cd(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_cd(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_cn(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_cn(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_cs(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_cs(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_dc(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_dc(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_dn(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_dn(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_ds(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_ds(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_nc(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_nc(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_nd(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_nd(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_ns(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_ns(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_sc(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_sc(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_sd(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_sd(k, theta), 50 * test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_sn(make_fvar<T, m>(k), make_fvar<T, m>(theta)),
                        boost::math::jacobi_sn(k, theta), 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(jacobi_zeta_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-1, 1};
  test_detail::RandomSample<T> phi_sampler{-boost::math::constants::two_pi<T>(), boost::math::constants::two_pi<T>()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    auto phi = phi_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::jacobi_zeta(make_fvar<T, m>(x), make_fvar<T, m>(phi)),
                        boost::math::jacobi_zeta(x, phi), 1.25e4 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(laguerre_hpp, T, all_float_types) {
  using boost::multiprecision::min;
  using std::min;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<unsigned> n_sampler{1, 50};
  test_detail::RandomSample<unsigned> r_sampler{0, 50};
  test_detail::RandomSample<T> x_sampler{0, 50};

  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto n = n_sampler.next();
    auto r = (min)(n - 1, r_sampler.next());
    auto x = x_sampler.next();

    {
      auto autodiff_v = static_cast<T>(boost::math::laguerre(n, make_fvar<T, m>(x)));
      auto anchor_v = boost::math::laguerre(n, x);
      if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
        BOOST_REQUIRE_SMALL(autodiff_v, std::numeric_limits<T>::epsilon());
        BOOST_REQUIRE_SMALL(anchor_v, std::numeric_limits<T>::epsilon());
      } else {
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50*test_constants::pct_epsilon());
      }
    }
    {
      auto autodiff_v = static_cast<T>(boost::math::laguerre(n, r, make_fvar<T, m>(x)));
      auto anchor_v = boost::math::laguerre(n, r, x);
      if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
        BOOST_REQUIRE_SMALL(autodiff_v, std::numeric_limits<T>::epsilon());
        BOOST_REQUIRE_SMALL(anchor_v, std::numeric_limits<T>::epsilon());
      } else {
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50*test_constants::pct_epsilon());
      }
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(lambert_w_hpp, T, all_float_types) {
  using boost::math::nextafter;
  using boost::math::tools::max;
  using boost::multiprecision::min;
  using std::max;
  using std::min;
  using std::nextafter;
  using std::fabs;
  using boost::math::differentiation::detail::fabs;
  using boost::multiprecision::fabs;

  using promoted_t = promote<T, double>;
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{static_cast<T>(-1 / std::exp(-1)), ((std::numeric_limits<T>::max))()};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    {
      auto x_ = (min<T>)(static_cast<T>(((max<promoted_t>))(-exp(promoted_t(-1)), promoted_t(x))),
                         ((std::numeric_limits<T>::max))());
      {
        auto autodiff_v = static_cast<T>(boost::math::lambert_w0(make_fvar<T, m>(x_)));
        auto anchor_v = boost::math::lambert_w0(x_);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_SMALL(autodiff_v, std::numeric_limits<T>::epsilon());
          BOOST_REQUIRE_SMALL(anchor_v, std::numeric_limits<T>::epsilon());
        } else {
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50*test_constants::pct_epsilon());
        }
      }
      {
        auto autodiff_v = static_cast<T>(boost::math::lambert_w0_prime(make_fvar<T, m>(x_)));
        auto anchor_v = boost::math::lambert_w0_prime(x_);
        if (isZeroOrSubnormal(autodiff_v) || isZeroOrSubnormal(anchor_v)) {
          BOOST_REQUIRE_SMALL(autodiff_v, std::numeric_limits<T>::epsilon());
          BOOST_REQUIRE_SMALL(anchor_v, std::numeric_limits<T>::epsilon());
        } else {
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50*test_constants::pct_epsilon());
        }
      }
    }

    {
      auto x_ = nextafter(static_cast<T>(nextafter(((max))(static_cast<promoted_t>(-exp(-1)), -fabs(promoted_t(x))),
                                                   ((std::numeric_limits<promoted_t>::max))())),
                          ((std::numeric_limits<T>::max))());
      x_ = (max)(static_cast<T>(-0.36), x_);
      BOOST_REQUIRE_CLOSE(boost::math::lambert_wm1(make_fvar<T, m>(x_)), boost::math::lambert_wm1(x_),
                          50*test_constants::pct_epsilon());
      BOOST_REQUIRE_CLOSE(boost::math::lambert_wm1_prime(make_fvar<T, m>(x_)), boost::math::lambert_wm1_prime(x_),
                          50*test_constants::pct_epsilon());
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(log1p_hpp, T, all_float_types) {
  using boost::math::log1p;
  using boost::multiprecision::log1p;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-1, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(log1p(make_fvar<T, m>(x)), log1p(x),
                        50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(next_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  for (auto i : boost::irange(test_constants::n_samples)) {
    BOOST_REQUIRE_CLOSE(boost::math::float_next(make_fvar<T, m>(static_cast<T>(i))),
                        boost::math::float_next(static_cast<T>(i)), test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_prior(make_fvar<T, m>(static_cast<T>(i))),
                        boost::math::float_prior(static_cast<T>(i)), test_constants::pct_epsilon());

    BOOST_REQUIRE_CLOSE(boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(1))),
                        boost::math::nextafter(static_cast<T>(i), static_cast<T>(1)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(i + 2))),
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), static_cast<T>(i + 2)),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(i + 1))),
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), static_cast<T>(i + 2)),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(-1))),
                        boost::math::nextafter(static_cast<T>(i), static_cast<T>(-1)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(-1 * (i + 2)))),
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), static_cast<T>(-1 * (i + 2))),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(-1 * (i + 1)))),
        boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), static_cast<T>(-1 * (i + 2))),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::nextafter(make_fvar<T, m>(static_cast<T>(i)), make_fvar<T, m>(static_cast<T>(i))),
                        ((make_fvar<T, m>(static_cast<T>(i)))), test_constants::pct_epsilon());

    BOOST_REQUIRE_CLOSE(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), 1),
                        boost::math::float_advance(static_cast<T>(i), 1), test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), i + 2),
                        boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), i + 2),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), i + 1),
        boost::math::float_advance(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), i + 2), -1),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), -1),
                        boost::math::float_advance(static_cast<T>(i), -1), test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(
        boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), -i - 1),
        boost::math::float_advance(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), -i - 2), 1),
        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_advance(make_fvar<T, m>(static_cast<T>(i)), 0),
                        ((make_fvar<T, m>(static_cast<T>(i)))), test_constants::pct_epsilon());

    BOOST_REQUIRE_CLOSE(boost::math::float_distance(make_fvar<T, m>(static_cast<T>(i)), static_cast<T>(i)),
                        static_cast<T>(0), test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_distance(boost::math::float_next(make_fvar<T, m>(static_cast<T>(i))),
                                                    make_fvar<T, m>(static_cast<T>(i))),
                        ((make_fvar<T, m>(-1))), test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::float_distance(boost::math::float_prior(make_fvar<T, m>(static_cast<T>(i))),
                                                    make_fvar<T, m>(static_cast<T>(i))),
                        ((make_fvar<T, m>(1))), test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(owens_t_hpp, T, all_float_types) {
  BOOST_MATH_STD_USING;
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> h_sampler{-2000, 2000};
  test_detail::RandomSample<T> a_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto h = h_sampler.next();
    auto a = a_sampler.next();
    auto autodiff_v = boost::math::owens_t(make_fvar<T, m>(h), make_fvar<T, m>(a));
    auto anchor_v = boost::math::owens_t(h, a);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 1e5 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(pow_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  for (auto i : boost::irange(10)) {
    BOOST_REQUIRE_CLOSE(boost::math::pow<0>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<0>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<1>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<1>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<2>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<2>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<3>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<3>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<4>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<4>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<5>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<5>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<6>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<6>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<7>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<7>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<8>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<8>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
    BOOST_REQUIRE_CLOSE(boost::math::pow<9>(make_fvar<T, m>(static_cast<T>(i))), boost::math::pow<9>(static_cast<T>(i)),
                        test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(polygamma_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    auto x = x_sampler.next();
    auto autodiff_v = boost::math::polygamma(i, make_fvar<T, m>(x));
    auto anchor_v = boost::math::polygamma(i, x);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(powm1_hpp, T, all_float_types) {
  using std::max;
  using boost::math::tools::max;
  using boost::multiprecision::max;

  using std::log;
  using boost::multiprecision::log;
  using boost::math::differentiation::detail::log;

  using std::min;
  using boost::multiprecision::min;

  using std::sqrt;
  using boost::multiprecision::sqrt;
  using boost::math::differentiation::detail::sqrt;

  using boost::math::nextafter;
  using boost::multiprecision::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = ((max))(x_sampler.next(), boost::math::nextafter(static_cast<T>(0), ((std::numeric_limits<T>::max))()));

    auto y = ((min))(x_sampler.next(), log(sqrt(((std::numeric_limits<T>::max))()) + 1) / log(x + 1));
    auto autodiff_v = boost::math::powm1(make_fvar<T, m>(x), make_fvar<T, m>(y));
    auto anchor_v = boost::math::powm1(x, y);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 2e6 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sin_pi_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::sin_pi(make_fvar<T, m>(x)), boost::math::sin_pi(x),
                        test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sinhc_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-80, 80};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    if (x != 0) {
      auto autodiff_v = boost::math::sinhc_pi(make_fvar<T, m>(x));
      auto anchor_v = boost::math::sinhc_pi(x);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50 * test_constants::pct_epsilon());
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(spherical_harmonic_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> theta_sampler{0, boost::math::constants::pi<T>()};
  test_detail::RandomSample<T> phi_sampler{0, boost::math::constants::two_pi<T>()};
  test_detail::RandomSample<int> r_sampler{0, test_constants::n_samples};
  for (auto n : boost::irange(1u, static_cast<unsigned>(test_constants::n_samples) + 1u)) {
    auto theta = theta_sampler.next();
    auto phi = phi_sampler.next();
    auto r = (std::min)(static_cast<int>(n) - 1, r_sampler.next());
    {
      auto autodiff_v = boost::math::spherical_harmonic(n, r, make_fvar<T, m>(theta), make_fvar<T, m>(phi));
      auto anchor_v = boost::math::spherical_harmonic(n, r, theta, phi);
      BOOST_REQUIRE_CLOSE(autodiff_v.real(), anchor_v.real(), 2.5e4 * test_constants::pct_epsilon());
      BOOST_REQUIRE_CLOSE(autodiff_v.imag(), anchor_v.imag(), 2.5e4 * test_constants::pct_epsilon());
    }

    {
      auto autodiff_v = boost::math::spherical_harmonic_r(n, r, make_fvar<T, m>(theta), make_fvar<T, m>(phi));
      auto anchor_v = boost::math::spherical_harmonic_r(n, r, theta, phi);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 2.5e4 * test_constants::pct_epsilon());
    }

    {
      auto autodiff_v = boost::math::spherical_harmonic_i(n, r, make_fvar<T, m>(theta), make_fvar<T, m>(phi));
      auto anchor_v = boost::math::spherical_harmonic_i(n, r, theta, phi);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 2.5e4 * test_constants::pct_epsilon());
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(sqrt1pm1_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-1, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::sqrt1pm1(make_fvar<T, m>(x)), boost::math::sqrt1pm1(x),
                        50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(trigamma_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{0, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::trigamma(make_fvar<T, m>(x)), boost::math::trigamma(x),
                        50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(zeta_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-30, 30};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::zeta(make_fvar<T, m>(x)), boost::math::zeta(x),
                        5e3 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_SUITE_END()
