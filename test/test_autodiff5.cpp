//           Copyright Matthew Pulver 2018 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#include "test_autodiff.hpp"

using namespace boost::math::differentiation;

BOOST_AUTO_TEST_SUITE(test_autodiff_5)

BOOST_AUTO_TEST_CASE_TEMPLATE(bernoulli_hpp, T, all_float_types) {

  using boost::multiprecision::min;
  using std::min;
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;

  for (auto i : boost::irange(test_constants::n_samples)) {
    {
      auto autodiff_v = boost::math::bernoulli_b2n<autodiff_fvar<T, m>>(i);
      auto anchor_v = boost::math::bernoulli_b2n<T>(i);
      BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
    }
    {
      auto i_ = ((min))(19, i);
      auto autodiff_v = boost::math::tangent_t2n<autodiff_fvar<T, m>>(i_);
      auto anchor_v = boost::math::tangent_t2n<T>(i_);
      BOOST_REQUIRE_EQUAL(autodiff_v, anchor_v);
    }
  }
}

// TODO(kbhat): Something in here is very slow with boost::multiprecision
BOOST_AUTO_TEST_CASE_TEMPLATE(bessel_hpp, T, bin_float_types) {
  using boost::math::signbit;
  using boost::multiprecision::signbit;
  using boost::math::nextafter;
  using boost::math::tools::max;
  using std::max;
  using std::nextafter;

  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> v_sampler{-20, 20};
  test_detail::RandomSample<T> x_sampler{
      -boost::math::tools::log_max_value<T>() + 1,
      boost::math::tools::log_max_value<T>() - 1};
  for (auto i : boost::irange(test_constants::n_samples)) {
    auto v = v_sampler.next();
    auto x = x_sampler.next();
    v = (
        (v) ? -1 : 1) *
        (max)(v, (nextafter)(T(0), ((std::numeric_limits<T>::max))()));
    if (signbit(x)) {
      v = static_cast<T>(boost::math::itrunc(v));
    }

    try {
      auto autodiff_v =
          boost::math::cyl_bessel_i(make_fvar<T, m>(v), make_fvar<T, m>(x));
      auto anchor_v = boost::math::cyl_bessel_i(v, x);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4*test_constants::pct_epsilon());
    } catch (const std::overflow_error&) {
      BOOST_REQUIRE_THROW(boost::math::cyl_bessel_i(make_fvar<T, m>(v), make_fvar<T, m>(x)),
          boost::wrapexcept<std::overflow_error>);
      BOOST_REQUIRE_THROW(boost::math::cyl_bessel_i(v, x),
          boost::wrapexcept<std::overflow_error>);
    }

    {
      auto x_j = abs(x) + 1;
      try {
        auto autodiff_v =
            boost::math::cyl_bessel_j(make_fvar<T, m>(v), make_fvar<T, m>(x_j));
        auto anchor_v = boost::math::cyl_bessel_j(v, x_j);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4*test_constants::pct_epsilon());
      } catch (const std::overflow_error &) {
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j(make_fvar<T, m>(v), make_fvar<T, m>(x_j)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j(v, x_j),
                            boost::wrapexcept<std::overflow_error>);
      }
    }
    try {
      auto autodiff_v =
          boost::math::cyl_bessel_j_zero(make_fvar<T, m>(v), i + 1);
      auto anchor_v = boost::math::cyl_bessel_j_zero(v, i + 1);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                          1e4 * test_constants::pct_epsilon());
    } catch (const std::overflow_error&) {
      BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j_zero(make_fvar<T, m>(v), i + 1),
                          boost::wrapexcept<std::overflow_error>);
      BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j_zero(v, i + 1),
                          boost::wrapexcept<std::overflow_error>);
    }

    {
      auto x_k = abs(x) + 1;
      try {
        auto autodiff_v =
            boost::math::cyl_bessel_k(make_fvar<T, m>(v), make_fvar<T, m>(x_k));
        auto anchor_v = boost::math::cyl_bessel_k(v, x_k);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4*test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_k(make_fvar<T, m>(v), make_fvar<T, m>(x_k)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_k(v, x_k),
                            boost::wrapexcept<std::overflow_error>);
      }
    }

    {
      auto x_neumann = abs(x)+1;
      try {
        auto autodiff_v = boost::math::cyl_neumann(make_fvar<T, m>(v),
                                                   make_fvar<T, m>(x_neumann));
        auto anchor_v = boost::math::cyl_neumann(v, x_neumann);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4*test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW(boost::math::cyl_neumann(make_fvar<T, m>(v),
                                                     make_fvar<T, m>(x_neumann)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::cyl_neumann(v, x_neumann),
                            boost::wrapexcept<std::overflow_error>);
      }
    }

    {
      try {
        auto autodiff_v =
            boost::math::cyl_neumann_zero(make_fvar<T, m>(v), i + 1);
        auto anchor_v = boost::math::cyl_neumann_zero(v, i + 1);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4*test_constants::pct_epsilon());
      } catch (std::overflow_error&) {
        BOOST_REQUIRE_THROW(boost::math::cyl_neumann_zero((make_fvar<T, m>)(v), i + 1), boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::cyl_neumann_zero(v, i + 1), boost::wrapexcept<std::overflow_error>);
      }
    }

    {
      auto i_ = static_cast<unsigned>(i);
      try {
        auto autodiff_v = boost::math::sph_bessel<autodiff_fvar<T, m>>(
            i_, make_fvar<T, m>(abs(v)));
        auto anchor_v = boost::math::sph_bessel<T>(i_, abs(v));
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW((boost::math::sph_bessel<autodiff_fvar<T, m>>)(i_, (make_fvar<T, m>)(abs(v))),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::sph_bessel<T>(i_, abs(v)),
                            boost::wrapexcept<std::overflow_error>);
      }
      {
        auto v_ = (max)(T(abs(v)),
                        nextafter(T(abs(v)), 2 * (std::numeric_limits<T>::min)()));
        try {
          auto autodiff_v = boost::math::sph_neumann<autodiff_fvar<T, m>>(
              i_, make_fvar<T, m>(v_));
          auto anchor_v = boost::math::sph_neumann<T>(i_, v_);
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                              1e4 * test_constants::pct_epsilon());
        } catch (const std::overflow_error&) {
          BOOST_REQUIRE_THROW(((boost::math::sph_neumann<autodiff_fvar<T, m>>(
                                  i_, make_fvar<T, m>(v_)))),
                              boost::wrapexcept<std::overflow_error>);
          BOOST_REQUIRE_THROW(((boost::math::sph_neumann<T>(i_, v_))),
                              boost::wrapexcept<std::overflow_error>);
        }
      }

      try {
        auto autodiff_v = boost::math::cyl_bessel_i_prime(make_fvar<T, m>(v),
                                                          make_fvar<T, m>(x));
        auto anchor_v = boost::math::cyl_bessel_i_prime(v, x);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_i_prime((make_fvar<T, m>)(v), (make_fvar<T, m>)(x)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW(boost::math::cyl_bessel_i_prime(v, x),
                            boost::wrapexcept<std::overflow_error>);
      }

      {
        auto x_j = abs(x) + 1;
        try {
          auto autodiff_v = boost::math::cyl_bessel_j_prime(make_fvar<T, m>(v),
                                                            make_fvar<T, m>(x_j));
          auto anchor_v = boost::math::cyl_bessel_j_prime(v, x_j);
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                              1e4*test_constants::pct_epsilon());
        } catch (const std::overflow_error &) {
          BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j_prime((make_fvar<T, m>)(v), (make_fvar<T, m>)(x_j)),
                              boost::wrapexcept<std::overflow_error>);
          BOOST_REQUIRE_THROW(boost::math::cyl_bessel_j_prime(v, x_j),
                              boost::wrapexcept<std::overflow_error>);
        }
      }

      {
        auto x_k = abs(x) + 1;
        try {
          auto autodiff_v = boost::math::cyl_bessel_k_prime(make_fvar<T, m>(v),
                                                            make_fvar<T, m>(x_k));
          auto anchor_v = boost::math::cyl_bessel_k_prime(v, x_k);
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                              1e4*test_constants::pct_epsilon());
        } catch (const std::overflow_error &) {
          BOOST_REQUIRE_THROW(boost::math::cyl_bessel_k_prime((make_fvar<T, m>)(v), (make_fvar<T, m>)(x_k)),
                              boost::wrapexcept<std::overflow_error>);
          BOOST_REQUIRE_THROW(boost::math::cyl_bessel_k_prime(v, x_k),
                              boost::wrapexcept<std::overflow_error>);
        }
      }

      {
        auto x_neumann = abs(x) + 1;
        try {
          auto autodiff_v = boost::math::cyl_neumann_prime(
              make_fvar<T, m>(v), make_fvar<T, m>(x_neumann));
          auto anchor_v = boost::math::cyl_neumann_prime(v, x_neumann);
          BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                              1e4*test_constants::pct_epsilon());
        } catch (const std::overflow_error&) {
          BOOST_REQUIRE_THROW(boost::math::cyl_neumann_prime((make_fvar<T, m>)(v), (make_fvar<T, m>)(x_neumann)),
                              boost::wrapexcept<std::overflow_error>);
          BOOST_REQUIRE_THROW(boost::math::cyl_neumann_prime(v, x_neumann),
                              boost::wrapexcept<std::overflow_error>);
        }
      }

      try {
        auto autodiff_v = boost::math::sph_bessel_prime<autodiff_fvar<T, m>>(
            i_, make_fvar<T, m>(abs(v) + 1));
        auto anchor_v = boost::math::sph_bessel_prime<T>(i_, abs(v) + 1);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW((boost::math::sph_neumann<autodiff_fvar<T, m>>)(i_, (make_fvar<T, m>)(abs(v) + 1)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW((boost::math::sph_neumann<T>)(i_, abs(v) + 1),
                            boost::wrapexcept<std::overflow_error>);
      }

      try {
        auto autodiff_v = boost::math::sph_neumann_prime<autodiff_fvar<T, m>>(
            i_, make_fvar<T, m>(abs(v) + 1));
        auto anchor_v = boost::math::sph_neumann_prime<T>(i_, abs(v) + 1);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      } catch (const std::overflow_error&) {
        BOOST_REQUIRE_THROW((boost::math::sph_neumann_prime<autodiff_fvar<T, m>>)(i_, (make_fvar<T, m>)(abs(v) + 1)),
                            boost::wrapexcept<std::overflow_error>);
        BOOST_REQUIRE_THROW((boost::math::sph_neumann_prime<T>)(i_, abs(v) + 1),
                            boost::wrapexcept<std::overflow_error>);
      }
    }
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(beta_hpp, T, all_float_types) {
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
      {
        auto autodiff_v = boost::math::beta(
            make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
        auto anchor_v = boost::math::beta(a_, b_, z);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      }

      {
        auto autodiff_v = boost::math::betac(
            make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
        auto anchor_v = boost::math::betac(a_, b_, z);
        BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                            1e4 * test_constants::pct_epsilon());
      }

      {{auto autodiff_v =
            boost::math::ibeta(make_fvar<T, m>(a_ - 1), make_fvar<T, m>(b_ - 1),
                               make_fvar<T, m>(z));
      auto anchor_v = boost::math::ibeta(a_ - 1, b_ - 1, z);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                          1e4 * test_constants::pct_epsilon());
    }
    {
      auto autodiff_v = boost::math::ibetac(
          make_fvar<T, m>(a_ - 1), make_fvar<T, m>(b_ - 1), make_fvar<T, m>(z));
      auto anchor_v = boost::math::ibetac(a_ - 1, b_ - 1, z);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                          1e4 * test_constants::pct_epsilon());
    }
  }
  {
    auto autodiff_v = boost::math::ibeta_derivative(
        make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
    auto anchor_v = boost::math::ibeta_derivative(a_, b_, z);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
  }

  {
    {
      auto autodiff_v = boost::math::ibeta_inv(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
      auto anchor_v = boost::math::ibeta_inv<T>(a_, b_, z);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 1e4*test_constants::pct_epsilon());
    }
    {
      auto autodiff_v = boost::math::ibetac_inv(make_fvar<T, m>(a_), make_fvar<T, m>(b_), make_fvar<T, m>(z));
      auto anchor_v = boost::math::ibetac_inv<T>(a_, b_, z);
      BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 1e4 * test_constants::pct_epsilon());
    }
}
{
  auto b_norm = b_sampler.normalize(abs(b));
  {
    auto autodiff_v = boost::math::ibeta_inva(
        make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
    auto anchor_v = boost::math::ibeta_inva(a_, b_norm, z);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
  }
  {
    auto autodiff_v = boost::math::ibetac_inva(
        make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
    auto anchor_v = boost::math::ibetac_inva(a_, b_norm, z);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
  }

  {
    auto autodiff_v = boost::math::ibeta_invb(
        make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
    auto anchor_v = boost::math::ibeta_invb(a_, b_norm, z);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
  }

  {
    auto autodiff_v = boost::math::ibetac_invb(
        make_fvar<T, m>(a_), make_fvar<T, m>(b_norm), make_fvar<T, m>(z));
    auto anchor_v = boost::math::ibetac_invb(a_, b_norm, z);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
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
    auto autodiff_v = boost::math::binomial_coefficient<autodiff_fvar<T, m>>(n, r);
    auto anchor_v = boost::math::binomial_coefficient<T>(n, r);
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v, 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(cbrt_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  test_detail::RandomSample<T> x_sampler{-2000, 2000};
  for (auto i : boost::irange(test_constants::n_samples)) {
    std::ignore = i;
    auto x = x_sampler.next();
    BOOST_REQUIRE_CLOSE(boost::math::cbrt(make_fvar<T, m>(x)),
                        boost::math::cbrt(x), 50 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(chebyshev_hpp, T, all_float_types) {
  using test_constants = test_constants_t<T>;
  static constexpr auto m = test_constants::order;
  {
    test_detail::RandomSample<T> x_sampler{-2, 2};
    T t_0 = 1;
    T x = x_sampler.next();
    T t_1 = x;
    for (auto i : boost::irange(test_constants::n_samples)) {
      std::ignore = i;
      std::swap(t_0, t_1);
      auto tmp = boost::math::chebyshev_next(x, t_0, t_1);
      BOOST_REQUIRE_EQUAL(
          static_cast<T>(boost::math::chebyshev_next(make_fvar<T, m>(x), make_fvar<T, m>(t_0),
                                      make_fvar<T, m>(t_1))),
          tmp);
      t_1 = tmp;
    }
  }
  {
    test_detail::RandomSample<unsigned> n_sampler{0u, 10u};
    test_detail::RandomSample<T> x_sampler{-2, 2};
    for (auto i : boost::irange(test_constants::n_samples)) {
      std::ignore = i;
      auto n = n_sampler.next();
      auto x = x_sampler.next();
      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_t(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_t(n, x),
                          40 * test_constants::pct_epsilon());

      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_u(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_u(n, x),
                          40 * test_constants::pct_epsilon());

      BOOST_REQUIRE_CLOSE(boost::math::chebyshev_t_prime(n, make_fvar<T, m>(x)),
                          boost::math::chebyshev_t_prime(n, x),
                          40 * test_constants::pct_epsilon());

      // /usr/include/boost/math/special_functions/chebyshev.hpp:164:40: error:
      // cannot convert
      // ‘boost::boost::math::differentiation::autodiff_v1::detail::fvar<double,
      // 3>’ to ‘double’ in return
      // BOOST_REQUIRE_EQUAL(boost::math::chebyshev_clenshaw_recurrence(c.data(),c.size(),make_fvar<T,m>(0.20))
      // ,
      // boost::math::chebyshev_clenshaw_recurrence(c.data(),c.size(),static_cast<T>(0.20)));
      /*try {
        std::array<T, 4> c0{{14.2, -13.7, 82.3, 96}};
        BOOST_REQUIRE_CLOSE(boost::math::chebyshev_clenshaw_recurrence(c0.data(),
      c0.size(), make_fvar<T,m>(x)),
                                     boost::math::chebyshev_clenshaw_recurrence(c0.data(),
      c0.size(), x), 10*test_constants::pct_epsilon()); } catch (...) {
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
    BOOST_REQUIRE_CLOSE(boost::math::cos_pi(make_fvar<T, m>(x)),
                        boost::math::cos_pi(x), test_constants::pct_epsilon());
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
    BOOST_REQUIRE_CLOSE(autodiff_v, anchor_v,
                        1e4 * test_constants::pct_epsilon());
  }
}

BOOST_AUTO_TEST_SUITE_END()
