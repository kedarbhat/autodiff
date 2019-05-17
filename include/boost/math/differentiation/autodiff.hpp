//           Copyright Matthew Pulver 2018 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE_1_0.txt or copy at
//           https://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_MATH_DIFFERENTIATION_AUTODIFF_HPP
#define BOOST_MATH_DIFFERENTIATION_AUTODIFF_HPP

#include <boost/math/tools/config.hpp>

#include <boost/cstdfloat.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/math/tools/promotion.hpp>
#include <boost/mp11/integer_sequence.hpp>
#include <boost/multiprecision/rational_adaptor.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <numeric>
#include <ostream>
#include <tuple>
#include <type_traits>

// Automatic Differentiation v1
namespace boost { namespace math { namespace differentiation { inline namespace autodiff_v1 {

namespace detail {

template<typename RealType, typename... RealTypes>
struct promote_args_n { using type = typename tools::promote_args_2<RealType, typename promote_args_n<RealTypes...>::type>::type; };

template<typename RealType>
struct promote_args_n<RealType> { using type = typename tools::promote_arg<RealType>::type; };

} // namespace detail

template<typename RealType, typename... RealTypes>
using promote = typename detail::promote_args_n<RealType,RealTypes...>::type;

namespace detail {

template<typename RealType, size_t Order>
class fvar;

// Compile-time test for fvar<> type.
template<typename T>
struct is_fvar_t : std::false_type{};

template<typename RealType, size_t Order>
struct is_fvar_t<fvar<RealType,Order>> : std::true_type {};

template<typename T>
using is_fvar = is_fvar_t<decay_t<T>>;

template<typename RealType, size_t Order, size_t... Orders> // specialized for fvar<> below.
struct nest_fvar { using type = fvar<typename nest_fvar<RealType,Orders...>::type,Order>; };

template<typename RealType, size_t Order>
struct nest_fvar<RealType,Order> { using type = fvar<RealType,Order>; };


template <typename>
struct get_depth_t : std::integral_constant<size_t, 0> {};

template <typename RealType, size_t Order>
struct get_depth_t<fvar<RealType,Order>> : std::integral_constant<size_t, get_depth_t<RealType>::value+1> {};

template<typename T>
using get_depth = get_depth_t<decay_t<T>>;

template <typename>
struct get_order_sum_t : std::integral_constant<size_t, 0> {};

template <typename RealType, size_t Order>
struct get_order_sum_t<fvar<RealType,Order>> : std::integral_constant<size_t, get_order_sum_t<RealType>::value+Order> {};

template<typename T>
using get_order_sum = get_order_sum_t<decay_t<T>>;

// Get non-fvar<> root type T of autodiff_fvar<T,O0,O1,O2,...>.
template<typename RealType>
struct get_root_type { using type = RealType; };

template<typename RealType, size_t Order>
struct get_root_type<fvar<RealType,Order>> { using type = typename get_root_type<RealType>::type; };

// Get type from descending Depth levels into fvar<>.
template<typename RealType, size_t Depth>
struct type_at { using type = RealType; };

template<typename RealType, size_t Order, size_t Depth>
struct type_at<fvar<RealType,Order>,Depth> { using type =
	typename conditional<Depth == 0, fvar<RealType, Order>, typename type_at<RealType, Depth - 1>::type>::type;
};

template<typename RealType, size_t Depth>
using get_type_at = typename type_at<RealType,Depth>::type;

// Satisfies Boost's Conceptual Requirements for Real Number Types.
// https://www.boost.org/libs/math/doc/html/math_toolkit/real_concepts.html
template<typename RealType, size_t Order>
class fvar
{
  std::array<RealType,Order+1> v;

 public:

  using root_type = typename get_root_type<RealType>::type; // RealType in the root fvar<RealType,Order>.

  fvar() = default;

  // Initialize a variable or constant.
  fvar(const root_type&, const bool is_variable);

  // RealType(cr) | RealType | RealType is copy constructible.
  fvar(const fvar&) = default;

  // Be aware of implicit casting from one fvar<> type to another by this copy constructor.
  template<typename RealType2, size_t Order2>
  fvar(const fvar<RealType2,Order2>&);

  // RealType(ca) | RealType | RealType is copy constructible from the arithmetic types.
  explicit fvar(const root_type&); // Initialize a constant. (No epsilon terms.)

  template<typename RealType2>
  fvar(const RealType2& ca); // Supports any RealType2 for which static_cast<root_type>(ca) compiles.

  explicit fvar(const char* ca); // Converts a const char* string to RealType by boost::lexical_cast

  // r = cr | RealType& | Assignment operator.
  fvar& operator=(const fvar&) = default;

  // r = ca | RealType& | Assignment operator from the arithmetic types.
  // Handled by constructor that takes a single parameter of generic type.
  //fvar& operator=(const root_type&); // Set a constant.

  // r += cr | RealType& | Adds cr to r.
  template<typename RealType2, size_t Order2>
  fvar& operator+=(const fvar<RealType2,Order2>&);

  // r += ca | RealType& | Adds ar to r.
  fvar& operator+=(const root_type&);

  // r -= cr | RealType& | Subtracts cr from r.
  template<typename RealType2, size_t Order2>
  fvar& operator-=(const fvar<RealType2,Order2>&);

  // r -= ca | RealType& | Subtracts ca from r.
  fvar& operator-=(const root_type&);

  // r *= cr | RealType& | Multiplies r by cr.
  template<typename RealType2, size_t Order2>
  fvar& operator*=(const fvar<RealType2,Order2>&);

  // r *= ca | RealType& | Multiplies r by ca.
  fvar& operator*=(const root_type&);

  // r /= cr | RealType& | Divides r by cr.
  template<typename RealType2, size_t Order2>
  fvar& operator/=(const fvar<RealType2,Order2>&);

  // r /= ca | RealType& | Divides r by ca.
  fvar& operator/=(const root_type&);

  // -r | RealType | Unary Negation.
  fvar operator-() const;

  // +r | RealType& | Identity Operation.
  const fvar& operator+() const;

  // cr + cr2 | RealType | Binary Addition
  template<typename RealType2, size_t Order2>
  promote<fvar,fvar<RealType2,Order2>> operator+(const fvar<RealType2,Order2>&) const;

  // cr + ca | RealType | Binary Addition
  fvar operator+(const root_type&) const;

  // ca + cr | RealType | Binary Addition
  template<typename RealType2, size_t Order2>
  friend fvar<RealType2,Order2>
  operator+(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr - cr2 | RealType | Binary Subtraction
  template<typename RealType2, size_t Order2>
  promote<fvar,fvar<RealType2,Order2>> operator-(const fvar<RealType2,Order2>&) const;

  // cr - ca | RealType | Binary Subtraction
  fvar operator-(const root_type&) const;

  // ca - cr | RealType | Binary Subtraction
  template<typename RealType2, size_t Order2>
  friend fvar<RealType2,Order2>
  operator-(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr * cr2 | RealType | Binary Multiplication
  template<typename RealType2, size_t Order2>
  promote<fvar,fvar<RealType2,Order2>> operator*(const fvar<RealType2,Order2>&) const;

  // cr * ca | RealType | Binary Multiplication
  fvar operator*(const root_type&) const;

  // ca * cr | RealType | Binary Multiplication
  template<typename RealType2, size_t Order2>
  friend fvar<RealType2,Order2>
  operator*(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr / cr2 | RealType | Binary Subtraction
  template<typename RealType2, size_t Order2>
  promote<fvar,fvar<RealType2,Order2>> operator/(const fvar<RealType2,Order2>&) const;

  // cr / ca | RealType | Binary Subtraction
  fvar operator/(const root_type&) const;

  // ca / cr | RealType | Binary Subtraction
  template<typename RealType2, size_t Order2>
  friend fvar<RealType2,Order2>
  operator/(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr == cr2 | bool | Equality Comparison
  template<typename RealType2, size_t Order2> // This only compares the root term. All other terms are ignored.
  bool operator==(const fvar<RealType2,Order2>&) const;

  // cr == ca | bool | Equality Comparison
  bool operator==(const root_type&) const;

  // ca == cr | bool | Equality Comparison
  template<typename RealType2, size_t Order2> // This only compares the root term. All other terms are ignored.
  friend bool operator==(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr != cr2 | bool | Inequality Comparison
  template<typename RealType2, size_t Order2>
  bool operator!=(const fvar<RealType2,Order2>&) const;

  // cr != ca | bool | Inequality Comparison
  bool operator!=(const root_type&) const;

  // ca != cr | bool | Inequality Comparison
  template<typename RealType2, size_t Order2>
  friend bool operator!=(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr <= cr2 | bool | Less than equal to.
  template<typename RealType2, size_t Order2>
  bool operator<=(const fvar<RealType2,Order2>&) const;

  // cr <= ca | bool | Less than equal to.
  bool operator<=(const root_type&) const;

  // ca <= cr | bool | Less than equal to.
  template<typename RealType2, size_t Order2>
  friend bool operator<=(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr >= cr2 | bool | Greater than equal to.
  template<typename RealType2, size_t Order2>
  bool operator>=(const fvar<RealType2,Order2>&) const;

  // cr >= ca | bool | Greater than equal to.
  bool operator>=(const root_type&) const;

  // ca >= cr | bool | Greater than equal to.
  template<typename RealType2, size_t Order2>
  friend bool operator>=(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr < cr2 | bool | Less than comparison.
  template<typename RealType2, size_t Order2>
  bool operator<(const fvar<RealType2,Order2>&) const;

  // cr < ca | bool | Less than comparison.
  bool operator<(const root_type&) const;

  // ca < cr | bool | Less than comparison.
  template<typename RealType2, size_t Order2>
  friend bool operator<(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // cr > cr2 | bool | Greater than comparison.
  template<typename RealType2, size_t Order2>
  bool operator>(const fvar<RealType2,Order2>&) const;

  // cr > ca | bool | Greater than comparison.
  bool operator>(const root_type&) const;

  // ca > cr | bool | Greater than comparison.
  template<typename RealType2, size_t Order2>
  friend bool operator>(const typename fvar<RealType2,Order2>::root_type&, const fvar<RealType2,Order2>&);

  // Will throw std::out_of_range if Order < order.
  template<typename... Orders>
  get_type_at<RealType, sizeof...(Orders)> at(size_t order, Orders... orders) const;

  template<typename... Orders>
  get_type_at<fvar, sizeof...(Orders)> derivative(Orders... orders) const;

  fvar inverse() const; // Multiplicative inverse.

  fvar& negate(); // Negate and return reference to *this.

  static constexpr size_t depth = get_depth<fvar>::value; // Number of nested std::array<RealType,Order>.

  static constexpr size_t order_sum = get_order_sum<fvar>::value;

  explicit operator root_type() const; // Must be explicit, otherwise overloaded operators are ambiguous.

  template<typename T, typename = typename boost::enable_if<boost::is_arithmetic<decay_t<T>>>::type>
  explicit operator T() const; // Must be explicit; multiprecision has trouble without the std::enable_if

  fvar& set_root(const root_type&);

  // Apply coefficients using horner method.
  template<typename Func, typename Fvar, typename... Fvars>
  promote<fvar<RealType,Order>,Fvar,Fvars...> apply_coefficients(const size_t order,
                                                                 const Func& f, const Fvar& cr, Fvars&&... fvars) const;

  template<typename Func>
  fvar apply_coefficients(const size_t order, const Func& f) const;

  // Use when function returns derivative(i)/factorial(i) and may have some infinite derivatives.
  fvar apply_coefficients_nonhorner(const std::function<root_type(size_t)>&) const;

  // Apply derivatives using horner method.
  template<typename Func, typename Fvar, typename... Fvars>
  promote<fvar<RealType,Order>,Fvar,Fvars...> apply_derivatives(const size_t order,
                                                                 const Func& f, const Fvar& cr, Fvars&&... fvars) const;

  template<typename Func>
  fvar apply_derivatives(const size_t order, const Func& f) const;

  // Apply derivatives using horner method.
  template<typename Func, typename Fvar, typename... Fvars>
  promote<fvar<RealType,Order>,Fvar,Fvars...> apply_derivatives_nonhorner(const size_t order,
                                                                 const Func& f, const Fvar& cr, Fvars&&... fvars) const;

  template<typename Func>
  fvar apply_derivatives_nonhorner(const size_t order, const Func& f) const;

 private:

  RealType epsilon_inner_product(size_t z0, size_t isum0, size_t m0,
                                 const fvar& cr, size_t z1, size_t isum1, size_t m1, size_t j) const;

  fvar epsilon_multiply(size_t z0, size_t isum0, const fvar& cr, size_t z1, size_t isum1) const;

  fvar epsilon_multiply(size_t z0, size_t isum0, const root_type& ca) const;

  fvar inverse_apply() const;

  fvar& multiply_assign_by_root_type(bool is_root, const root_type&);

  template<typename RealType2, size_t Orders2>
  friend class fvar;

  template<typename RealType2, size_t Order2>
  friend std::ostream& operator<<(std::ostream&, const fvar<RealType2,Order2>&);

  // C++11 Compatibility
#ifdef BOOST_NO_CXX17_IF_CONSTEXPR
  template<typename RootType>
  void fvar_cpp11(std::true_type, const RootType& ca, const bool is_variable);

  template<typename RootType>
  void fvar_cpp11(std::false_type, const RootType& ca, const bool is_variable);

  template<typename... Orders>
  get_type_at<RealType, sizeof...(Orders)> at_cpp11(std::true_type, size_t order, Orders... orders) const;

  template<typename... Orders>
  get_type_at<RealType, sizeof...(Orders)> at_cpp11(std::false_type, size_t order, Orders... orders) const;

  template<typename SizeType>
  fvar epsilon_multiply_cpp11(std::true_type,
                              SizeType z0, size_t isum0, const fvar& cr, size_t z1, size_t isum1) const;

  template<typename SizeType>
  fvar epsilon_multiply_cpp11(std::false_type,
                              SizeType z0, size_t isum0, const fvar& cr, size_t z1, size_t isum1) const;

  template<typename SizeType>
  fvar epsilon_multiply_cpp11(std::true_type, SizeType z0, size_t isum0, const root_type& ca) const;

  template<typename SizeType>
  fvar epsilon_multiply_cpp11(std::false_type, SizeType z0, size_t isum0, const root_type& ca) const;

  template<typename RootType>
  fvar& multiply_assign_by_root_type_cpp11(std::true_type, bool is_root, const RootType& ca);

  template<typename RootType>
  fvar& multiply_assign_by_root_type_cpp11(std::false_type, bool is_root, const RootType& ca);

  template<typename RootType>
  fvar& negate_cpp11(std::true_type, const RootType&);

  template<typename RootType>
  fvar& negate_cpp11(std::false_type, const RootType&);

  template<typename RootType>
  fvar& set_root_cpp11(std::true_type, const RootType& root);

  template<typename RootType>
  fvar& set_root_cpp11(std::false_type, const RootType& root);
#endif
};

// C++11 compatibility
#ifdef BOOST_NO_CXX17_IF_CONSTEXPR
#  define BOOST_AUTODIFF_IF_CONSTEXPR
#else
#  define BOOST_AUTODIFF_IF_CONSTEXPR constexpr
#endif

// Standard Library Support Requirements

// fabs(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> fabs(const fvar<RealType,Order>&);

// abs(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> abs(const fvar<RealType,Order>&);

// ceil(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> ceil(const fvar<RealType,Order>&);

// floor(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> floor(const fvar<RealType,Order>&);

// exp(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> exp(const fvar<RealType,Order>&);

// pow(cr, ca) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> pow(const fvar<RealType,Order>&,const typename fvar<RealType,Order>::root_type&);

// pow(ca, cr) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> pow(const typename fvar<RealType,Order>::root_type&,const fvar<RealType,Order>&);

// pow(cr1, cr2) | RealType
template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>
pow(const fvar<RealType1,Order1>&, const fvar<RealType2,Order2>&);

// sqrt(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> sqrt(const fvar<RealType,Order>&);

// log(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> log(const fvar<RealType,Order>&);

// frexp(cr1, &i) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> frexp(const fvar<RealType,Order>&, int*);

// ldexp(cr1, i) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> ldexp(const fvar<RealType,Order>&, int);

// cos(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> cos(const fvar<RealType,Order>&);

// sin(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> sin(const fvar<RealType,Order>&);

// asin(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> asin(const fvar<RealType,Order>&);

// tan(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> tan(const fvar<RealType,Order>&);

// atan(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> atan(const fvar<RealType,Order>&);

// atan2(cr, ca) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> atan2(const fvar<RealType,Order>&, const typename fvar<RealType,Order>::root_type&);

// atan2(ca, cr) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> atan2(const typename fvar<RealType,Order>::root_type&, const fvar<RealType,Order>&);

// atan2(cr1, cr2) | RealType
template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>, fvar<RealType2,Order2>>
atan2(const fvar<RealType1,Order1>&, const fvar<RealType2,Order2>&);

// fmod(cr1,cr2) | RealType
template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>
fmod(const fvar<RealType1,Order1>&, const fvar<RealType2,Order2>&);

// round(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> round(const fvar<RealType,Order>&);

// iround(cr1) | int
template<typename RealType, size_t Order>
int iround(const fvar<RealType,Order>&);

// trunc(cr1) | RealType
template<typename RealType, size_t Order>
fvar<RealType,Order> trunc(const fvar<RealType,Order>&);

// itrunc(cr1) | int
template<typename RealType, size_t Order>
int itrunc(const fvar<RealType,Order>&);

// Additional functions
template<typename RealType, size_t Order>
fvar<RealType,Order> acos(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> acosh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> asinh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> atanh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> cosh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> erf(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> erfc(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> lambert_w0(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> sinc(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> sinh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
fvar<RealType,Order> tanh(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
long lround(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
long long llround(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
long long lltrunc(const fvar<RealType,Order>&);

template<typename RealType, size_t Order>
long double truncl(const fvar<RealType,Order>&);

} // namespace detail

template<typename RealType, size_t Order, size_t... Orders>
using autodiff_fvar = typename detail::nest_fvar<RealType,Order,Orders...>::type;

template<typename RealType, size_t Order, size_t... Orders>
autodiff_fvar<RealType,Order,Orders...> make_fvar(const RealType& ca) {
  return autodiff_fvar<RealType,Order,Orders...>(ca, true);
}

namespace detail {

template <size_t>
struct zero : std::integral_constant<size_t,0> {};

template<typename RealType, size_t Order, size_t... Is>
auto make_fvar_for_tuple(mp11::index_sequence<Is...>, const RealType& ca) -> decltype(make_fvar<RealType,zero<Is>::value...,Order>(ca)) {
  return make_fvar<RealType,zero<Is>::value...,Order>(ca);
}

template<typename RealType, size_t... Orders, size_t... Is, typename... RealTypes>
auto make_ftuple_impl(mp11::index_sequence<Is...>, const RealTypes&... ca) -> decltype(std::make_tuple(make_fvar_for_tuple<RealType,Orders>(mp11::make_index_sequence<Is>{},ca)...)) {
  return std::make_tuple(make_fvar_for_tuple<RealType,Orders>(mp11::make_index_sequence<Is>{},ca)...);
}

} // namespace detail

template<typename RealType, size_t... Orders, typename... RealTypes>
auto make_ftuple(const RealTypes&... ca) -> decltype(detail::make_ftuple_impl<RealType,Orders...>(mp11::index_sequence_for<RealTypes...>{}, ca...)) {
  static_assert(sizeof...(Orders) == sizeof...(RealTypes),
    "Number of Orders must match number of function parameters.");
  return detail::make_ftuple_impl<RealType,Orders...>(mp11::index_sequence_for<RealTypes...>{}, ca...);
}

namespace detail {

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
template<typename RealType, size_t Order>
fvar<RealType,Order>::fvar(const root_type& ca, const bool is_variable)
{
    if constexpr (is_fvar<RealType>::value)
    {
        v.front() = RealType(ca, is_variable);
        if constexpr (0 < Order)
            std::fill(v.begin()+1, v.end(), static_cast<RealType>(0));
    }
    else
    {
        v.front() = ca;
        if constexpr (0 < Order)
            v[1] = static_cast<root_type>(static_cast<int>(is_variable));
        if constexpr (1 < Order)
            std::fill(v.begin()+2, v.end(), static_cast<RealType>(0));
    }
}
#endif

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
fvar<RealType,Order>::fvar(const fvar<RealType2,Order2>& cr)
{
  for (size_t i=0 ; i<=(std::min)(Order,Order2) ; ++i)
    v[i] = static_cast<RealType>(cr.v[i]);
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order2 < Order)
    std::fill(v.begin()+(Order2+1), v.end(), static_cast<RealType>(0));
}

template<typename RealType, size_t Order>
fvar<RealType,Order>::fvar(const root_type& ca)
    :    v{{static_cast<RealType>(ca)}}
{
}

template<typename RealType, size_t Order>
template<typename RealType2>
fvar<RealType,Order>::fvar(const RealType2& ca)
    :    v{{static_cast<RealType>(ca)}} // Can cause compiler error if RealType2 cannot be cast to root_type.
{
}

template<typename RealType, size_t Order>
fvar<RealType,Order>::fvar(const char* ca_str)
    :    v{{static_cast<RealType>(boost::lexical_cast<promote<root_type, double>>(ca_str))}}
{
}

/*
template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::operator=(const root_type& ca)
{
    v.front() = static_cast<RealType>(ca);
    if BOOST_AUTODIFF_IF_CONSTEXPR (0 < Order)
        std::fill(v.begin()+1, v.end(), static_cast<RealType>(0));
    return *this;
}
*/

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
fvar<RealType,Order>& fvar<RealType,Order>::operator+=(const fvar<RealType2,Order2>& cr)
{
  for (size_t i=0 ; i<=(std::min)(Order,Order2) ; ++i)
    v[i] += cr.v[i];
  return *this;
}

template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::operator+=(const root_type& ca)
{
  v.front() += ca;
  return *this;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
fvar<RealType,Order>& fvar<RealType,Order>::operator-=(const fvar<RealType2,Order2>& cr)
{
  for (size_t i=0 ; i<=Order ; ++i)
    v[i] -= cr.v[i];
  return *this;
}

template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::operator-=(const root_type& ca)
{
  v.front() -= ca;
  return *this;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
fvar<RealType,Order>& fvar<RealType,Order>::operator*=(const fvar<RealType2,Order2>& cr)
{
  using diff_t = typename std::array<RealType, Order+1>::difference_type;
  const promote<RealType,RealType2> zero(0);
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order <= Order2)
    for (size_t i=0, j=Order ; i<=Order ; ++i, --j)
      v[j] = std::inner_product(v.cbegin(), v.cend()-diff_t(i), cr.v.crbegin()+diff_t(i), zero);
  else
  {
    for (size_t i=0, j=Order ; i<=Order-Order2 ; ++i, --j)
      v[j] = std::inner_product(cr.v.cbegin(), cr.v.cend(), v.crbegin()+diff_t(i), zero);
    for (size_t i=Order-Order2+1, j=Order2-1 ; i<=Order ; ++i, --j)
      v[j] = std::inner_product(cr.v.cbegin(), cr.v.cbegin()+diff_t(j+1), v.crbegin()+diff_t(i), zero);
  }
  return *this;
}

template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::operator*=(const root_type& ca)
{
  return multiply_assign_by_root_type(true, ca);
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
fvar<RealType,Order>& fvar<RealType,Order>::operator/=(const fvar<RealType2,Order2>& cr)
{
  using diff_t = typename std::array<RealType, Order+1>::difference_type;
  const RealType zero(0);
  v.front() /= cr.v.front();
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order < Order2)
    for (size_t i=1, j=Order2-1, k=Order ; i<=Order ; ++i, --j, --k)
      (v[i] -= std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), v.crbegin()+diff_t(k), zero)) /= cr.v.front();
  else if BOOST_AUTODIFF_IF_CONSTEXPR (0 < Order2)
    for (size_t i=1, j=Order2-1, k=Order ; i<=Order ; ++i, j&&--j, --k)
      (v[i] -= std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), v.crbegin()+diff_t(k), zero)) /= cr.v.front();
  else
    for (size_t i=1 ; i<=Order ; ++i)
      v[i] /= cr.v.front();
  return *this;
}

template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::operator/=(const root_type& ca)
{
  std::for_each(v.begin(), v.end(), [&ca](RealType& x) { x /= ca; });
  return *this;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::operator-() const
{
  fvar<RealType,Order> retval(*this);
  retval.negate();
  return retval;
}

template<typename RealType, size_t Order>
const fvar<RealType,Order>& fvar<RealType,Order>::operator+() const
{
  return *this;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
promote<fvar<RealType,Order>,fvar<RealType2,Order2>>
fvar<RealType,Order>::operator+(const fvar<RealType2,Order2>& cr) const
{
  promote<fvar<RealType,Order>,fvar<RealType2,Order2>> retval;
  for (size_t i=0 ; i<=(std::min)(Order,Order2) ; ++i)
    retval.v[i] = v[i] + cr.v[i];
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order < Order2)
    for (size_t i=Order+1 ; i<=Order2 ; ++i)
      retval.v[i] = cr.v[i];
  else if BOOST_AUTODIFF_IF_CONSTEXPR (Order2 < Order)
    for (size_t i=Order2+1 ; i<=Order ; ++i)
      retval.v[i] = v[i];
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::operator+(const root_type& ca) const
{
  fvar<RealType,Order> retval(*this);
  retval.v.front() += ca;
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> operator+(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return cr + ca;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
promote<fvar<RealType,Order>,fvar<RealType2,Order2>>
fvar<RealType,Order>::operator-(const fvar<RealType2,Order2>& cr) const
{
  promote<fvar<RealType,Order>,fvar<RealType2,Order2>> retval;
  for (size_t i=0 ; i<=(std::min)(Order,Order2) ; ++i)
    retval.v[i] = v[i] - cr.v[i];
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order < Order2)
    for (auto i=Order+1 ; i<=Order2 ; ++i)
      retval.v[i] = -cr.v[i];
  else if BOOST_AUTODIFF_IF_CONSTEXPR (Order2 < Order)
    for (auto i=Order2+1 ; i<=Order ; ++i)
      retval.v[i] = v[i];
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::operator-(const root_type& ca) const
{
  fvar<RealType,Order> retval(*this);
  retval.v.front() -= ca;
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> operator-(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  fvar<RealType,Order> mcr = -cr; // Has same address as retval in operator-() due to NRVO.
  mcr += ca;
  return mcr; // <-- This allows for NRVO. The following does not. --> return mcr += ca;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
promote<fvar<RealType,Order>,fvar<RealType2,Order2>>
fvar<RealType,Order>::operator*(const fvar<RealType2,Order2>& cr) const
{
  using diff_t = typename std::array<RealType, Order+1>::difference_type;
  const promote<RealType,RealType2> zero(0);
  promote<fvar<RealType,Order>,fvar<RealType2,Order2>> retval;
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order < Order2)
    for (size_t i=0, j=Order, k=Order2 ; i<=Order2 ; ++i, j&&--j, --k)
      retval.v[i] = std::inner_product(v.cbegin(), v.cend()-diff_t(j), cr.v.crbegin()+diff_t(k), zero);
  else
    for (size_t i=0, j=Order2, k=Order ; i<=Order ; ++i, j&&--j, --k)
      retval.v[i] = std::inner_product(cr.v.cbegin(), cr.v.cend()-diff_t(j), v.crbegin()+diff_t(k), zero);
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::operator*(const root_type& ca) const
{
  fvar<RealType,Order> retval(*this);
  retval *= ca;
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> operator*(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return cr * ca;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
promote<fvar<RealType,Order>,fvar<RealType2,Order2>>
fvar<RealType,Order>::operator/(const fvar<RealType2,Order2>& cr) const
{
  using diff_t = typename std::array<RealType, Order+1>::difference_type;
  const promote<RealType,RealType2> zero(0);
  promote<fvar<RealType,Order>,fvar<RealType2,Order2>> retval;
  retval.v.front() = v.front() / cr.v.front();
  if BOOST_AUTODIFF_IF_CONSTEXPR (Order < Order2)
  {
    for (size_t i=1, j=Order2-1 ; i<=Order ; ++i, --j)
      retval.v[i] = (v[i] -
          std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), retval.v.crbegin()+diff_t(j+1), zero)) / cr.v.front();
    for (size_t i=Order+1, j=Order2-Order-1 ; i<=Order2 ; ++i, --j)
      retval.v[i] =
          -std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), retval.v.crbegin()+diff_t(j+1), zero) / cr.v.front();
  }
  else if BOOST_AUTODIFF_IF_CONSTEXPR (0 < Order2)
    for (size_t i=1, j=Order2-1, k=Order ; i<=Order ; ++i, j&&--j, --k)
      retval.v[i] =
          (v[i] - std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), retval.v.crbegin()+diff_t(k), zero)) / cr.v.front();
  else
    for (size_t i=1 ; i<=Order ; ++i)
      retval.v[i] = v[i] / cr.v.front();
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::operator/(const root_type& ca) const
{
  fvar<RealType,Order> retval(*this);
  retval /= ca;
  return retval;
}

template<typename RealType, size_t Order>
fvar<RealType,Order> operator/(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  using diff_t = typename std::array<RealType, Order+1>::difference_type;
  fvar<RealType,Order> retval;
  retval.v.front() = ca / cr.v.front();
  if BOOST_AUTODIFF_IF_CONSTEXPR (0 < Order)
  {
    const RealType zero(0);
    for (size_t i=1, j=Order-1 ; i<=Order ; ++i, --j)
      retval.v[i] = -std::inner_product(cr.v.cbegin()+1, cr.v.cend()-diff_t(j), retval.v.crbegin()+diff_t(j+1), zero)
          / cr.v.front();
  }
  return retval;
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator==(const fvar<RealType2,Order2>& cr) const
{
  return v.front() == cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator==(const root_type& ca) const
{
  return v.front() == ca;
}

template<typename RealType, size_t Order>
bool operator==(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca == cr.v.front();
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator!=(const fvar<RealType2,Order2>& cr) const
{
  return v.front() != cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator!=(const root_type& ca) const
{
  return v.front() != ca;
}

template<typename RealType, size_t Order>
bool operator!=(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca != cr.v.front();
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator<=(const fvar<RealType2,Order2>& cr) const
{
  return v.front() <= cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator<=(const root_type& ca) const
{
  return v.front() <= ca;
}

template<typename RealType, size_t Order>
bool operator<=(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca <= cr.v.front();
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator>=(const fvar<RealType2,Order2>& cr) const
{
  return v.front() >= cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator>=(const root_type& ca) const
{
  return v.front() >= ca;
}

template<typename RealType, size_t Order>
bool operator>=(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca >= cr.v.front();
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator<(const fvar<RealType2,Order2>& cr) const
{
  return v.front() < cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator<(const root_type& ca) const
{
  return v.front() < ca;
}

template<typename RealType, size_t Order>
bool operator<(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca < cr.v.front();
}

template<typename RealType, size_t Order>
template<typename RealType2, size_t Order2>
bool fvar<RealType,Order>::operator>(const fvar<RealType2,Order2>& cr) const
{
  return v.front() > cr.v.front();
}

template<typename RealType, size_t Order>
bool fvar<RealType,Order>::operator>(const root_type& ca) const
{
  return v.front() > ca;
}

template<typename RealType, size_t Order>
bool operator>(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  return ca > cr.v.front();
}

/*** Other methods and functions ***/

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// f : order -> derivative(order)/factorial(order)
// Use this when you have the polynomial coefficients, rather than just the derivatives. E.g. See atan2().
template<typename RealType, size_t Order>
template<typename Func, typename Fvar, typename... Fvars>
promote<fvar<RealType,Order>,Fvar,Fvars...> fvar<RealType,Order>::apply_coefficients(
    const size_t order, const Func& f, const Fvar& cr, Fvars&&... fvars) const
{
    const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
    size_t i = (std::min)(order, order_sum);
    promote<fvar<RealType,Order>,Fvar,Fvars...> accumulator = cr.apply_coefficients(
        order-i, [&f,i](auto... indices) { return f(i,indices...); }, std::forward<Fvars>(fvars)...);
    while (i--)
        (accumulator *= epsilon) += cr.apply_coefficients(
            order-i, [&f,i](auto... indices) { return f(i,indices...); }, std::forward<Fvars>(fvars)...);
    return accumulator;
}
#endif

// f : order -> derivative(order)/factorial(order)
// Use this when you have the polynomial coefficients, rather than just the derivatives. E.g. See atan().
template<typename RealType, size_t Order>
template<typename Func>
fvar<RealType,Order> fvar<RealType,Order>::apply_coefficients(const size_t order, const Func& f) const
{
  const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
  size_t i = (std::min)(order, order_sum);
#else // ODR-use of static constexpr
  size_t i = order < order_sum ? order : order_sum;
#endif
  fvar<RealType,Order> accumulator = f(i);
  while (i--)
    (accumulator *= epsilon) += f(i);
  return accumulator;
}

// f : order -> derivative(order)/factorial(order)
// Use this when you have the polynomial coefficients, rather than just the derivatives. E.g. See atan().
template<typename RealType, size_t Order>
fvar<RealType,Order>
fvar<RealType,Order>::apply_coefficients_nonhorner(const std::function<root_type(size_t)>& f) const
{
  const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
  fvar<RealType,Order> epsilon_i = fvar<RealType,Order>(1); // epsilon to the power of i
  fvar<RealType,Order> accumulator = fvar<RealType,Order>(f(0));
  for (size_t i=1 ; i<=order_sum ; ++i)
  {   // accumulator += (epsilon_i *= epsilon) * f(i);
    epsilon_i = epsilon_i.epsilon_multiply(i-1, 0, epsilon, 1, 0);
    accumulator += epsilon_i.epsilon_multiply(i, 0, f(i));
  }
  return accumulator;
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// f : order -> derivative(order)
template<typename RealType, size_t Order>
template<typename Func, typename Fvar, typename... Fvars>
promote<fvar<RealType,Order>,Fvar,Fvars...> fvar<RealType,Order>::apply_derivatives(
    const size_t order, const Func& f, const Fvar& cr, Fvars&&... fvars) const
{
    const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
    size_t i = (std::min)(order, order_sum);
    promote<fvar<RealType,Order>,Fvar,Fvars...> accumulator = cr.apply_derivatives(
        order-i, [&f,i](auto... indices) { return f(i,indices...); }, std::forward<Fvars>(fvars)...)
        / factorial<root_type>(static_cast<unsigned>(i));
    while (i--)
        (accumulator *= epsilon) += cr.apply_derivatives(
            order-i, [&f,i](auto... indices) { return f(i,indices...); }, std::forward<Fvars>(fvars)...)
            / factorial<root_type>(static_cast<unsigned>(i));
    return accumulator;
}
#endif

// f : order -> derivative(order)
template<typename RealType, size_t Order>
template<typename Func>
fvar<RealType,Order> fvar<RealType,Order>::apply_derivatives(const size_t order, const Func& f) const
{
  const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
  size_t i = (std::min)(order, order_sum);
#else // ODR-use of static constexpr
  size_t i = order < order_sum ? order : order_sum;
#endif
  fvar<RealType,Order> accumulator = f(i) / factorial<root_type>(static_cast<unsigned>(i));
  while (i--)
    (accumulator *= epsilon) += f(i) / factorial<root_type>(static_cast<unsigned>(i));
  return accumulator;
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// f : order -> derivative(order)
template<typename RealType, size_t Order>
template<typename Func, typename Fvar, typename... Fvars>
promote<fvar<RealType,Order>,Fvar,Fvars...> fvar<RealType,Order>::apply_derivatives_nonhorner(
    const size_t order, const Func& f, const Fvar& cr, Fvars&&... fvars) const
{
    const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
    fvar<RealType,Order> epsilon_i = fvar<RealType,Order>(1); // epsilon to the power of i
    promote<fvar<RealType,Order>,Fvar,Fvars...> accumulator = cr.apply_derivatives_nonhorner(
        order, [&f](auto... indices) { return f(0,static_cast<std::size_t>(indices)...); }, std::forward<Fvars>(fvars)...);
    const size_t i_max = (std::min)(order, order_sum);
    for (size_t i=1 ; i<=i_max ; ++i)
    { // accumulator += (epsilon_i *= epsilon) * (f(i) / factorial<root_type>(i));
      epsilon_i = epsilon_i.epsilon_multiply(i-1, 0, epsilon, 1, 0);
      accumulator += epsilon_i.epsilon_multiply(i, 0, cr.apply_derivatives_nonhorner(
            order-i, [&f,i](auto... indices) { return f(i,static_cast<std::size_t>(indices)...); }, std::forward<Fvars>(fvars)...)
            / factorial<root_type>(static_cast<unsigned>(i)), 0, 0);
    }
    return accumulator;
}
#endif

// f : order -> derivative(order)
template<typename RealType, size_t Order>
template<typename Func>
fvar<RealType,Order> fvar<RealType,Order>::apply_derivatives_nonhorner(const size_t /*order*/, const Func& f) const
{
  const fvar<RealType,Order> epsilon = fvar<RealType,Order>(*this).set_root(0);
  fvar<RealType,Order> epsilon_i = fvar<RealType,Order>(1); // epsilon to the power of i
  fvar<RealType,Order> accumulator = fvar<RealType,Order>(f(0u));
  for (size_t i=1 ; i<=order_sum ; ++i)
  { // accumulator += (epsilon_i *= epsilon) * (f(i) / factorial<root_type>(i));
    epsilon_i = epsilon_i.epsilon_multiply(i-1, 0, epsilon, 1, 0);
    accumulator += epsilon_i.epsilon_multiply(i, 0, f(i) / factorial<root_type>(static_cast<unsigned>(i)));
  }
  return accumulator;
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// Can throw "std::out_of_range: array::at: __n (which is 7) >= _Nm (which is 7)"
template<typename RealType, size_t Order>
template<typename... Orders>
get_type_at<RealType,sizeof...(Orders)> fvar<RealType,Order>::at(size_t order, Orders... orders) const
{
    if constexpr (0 < sizeof...(Orders))
        return v.at(order).at(static_cast<std::size_t>(orders)...);
    else
        return v.at(order);
}
#endif

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// Can throw "std::out_of_range: array::at: __n (which is 7) >= _Nm (which is 7)"
template<typename RealType, size_t Order>
template<typename... Orders>
get_type_at<fvar<RealType,Order>,sizeof...(Orders)> fvar<RealType,Order>::derivative(Orders... orders) const
{
  static_assert(sizeof...(Orders) <= depth, "Number of parameters to derivative(...) cannot exceed fvar::depth.");
  return at(static_cast<std::size_t>(orders)...) * (... * factorial<root_type>(static_cast<unsigned>(orders)));
}
#endif

template<typename RealType, size_t Order>
RealType fvar<RealType,Order>::epsilon_inner_product(size_t z0, const size_t isum0, const size_t m0,
                                                     const fvar<RealType,Order>& cr, size_t z1, const size_t isum1, const size_t m1, const size_t j) const
{
  static_assert(is_fvar<RealType>::value, "epsilon_inner_product() must have 1 < depth.");
  RealType accumulator = RealType();
  const auto i0_max = m1 < j ? j-m1 : 0;
  for (auto i0=m0, i1=j-m0 ; i0<=i0_max ; ++i0, --i1)
    accumulator += v.at(i0).epsilon_multiply(z0, isum0+i0, cr.v.at(i1), z1, isum1+i1);
  return accumulator;
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::epsilon_multiply(size_t z0, size_t isum0,
    const fvar<RealType,Order>& cr, size_t z1, size_t isum1) const
{
    using diff_t = typename std::array<RealType, Order+1>::difference_type;
    const RealType zero(0);
    const size_t m0 = order_sum + isum0 < Order + z0 ? Order + z0 - (order_sum + isum0) : 0;
    const size_t m1 = order_sum + isum1 < Order + z1 ? Order + z1 - (order_sum + isum1) : 0;
    const size_t i_max = m0 + m1 < Order ? Order - (m0 + m1) : 0;
    fvar<RealType,Order> retval = fvar<RealType,Order>();
    if constexpr (is_fvar<RealType>::value)
        for (size_t i=0, j=Order ; i<=i_max ; ++i, --j)
            retval.v[j] = epsilon_inner_product(z0, isum0, m0, cr, z1, isum1, m1, j);
    else
        for (size_t i=0, j=Order ; i<=i_max ; ++i, --j)
            retval.v[j] = std::inner_product(v.cbegin()+diff_t(m0), v.cend()-diff_t(i+m1), cr.v.crbegin()+diff_t(i+m0), zero);
    return retval;
}
#endif

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
// When called from outside this method, z0 should be non-zero. Otherwise if z0=0 then it will give an
// incorrect result of 0 when the root value is 0 and ca=inf, when instead the correct product is nan.
// If z0=0 then use the regular multiply operator*() instead.
template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::epsilon_multiply(size_t z0, size_t isum0,
    const root_type& ca) const
{
    fvar<RealType,Order> retval(*this);
    const size_t m0 = order_sum + isum0 < Order + z0 ? Order + z0 - (order_sum + isum0) : 0;
    if constexpr (is_fvar<RealType>::value)
        for (size_t i=m0 ; i<=Order ; ++i)
            retval.v[i] = retval.v[i].epsilon_multiply(z0, isum0+i, ca);
    else
        for (size_t i=m0 ; i<=Order ; ++i)
            if (retval.v[i] != static_cast<RealType>(0))
                retval.v[i] *= ca;
    return retval;
}
#endif

template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::inverse() const
{
  return static_cast<root_type>(*this) == 0 ? inverse_apply() : 1 / *this;
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::negate()
{
    if constexpr (is_fvar<RealType>::value)
        std::for_each(v.begin(), v.end(), [](RealType& r) { r.negate(); });
    else
        std::for_each(v.begin(), v.end(), [](RealType& a) { a = -a; });
    return *this;
}
#endif

// This gives log(0.0) = depth(1)(-inf,inf,-inf,inf,-inf,inf)
// 1 / *this: log(0.0) = depth(1)(-inf,inf,-inf,-nan,-nan,-nan)
template<typename RealType, size_t Order>
fvar<RealType,Order> fvar<RealType,Order>::inverse_apply() const
{
  root_type derivatives[order_sum+1]; // LCOV_EXCL_LINE This causes a false negative on lcov coverage test.
  const root_type x0 = static_cast<root_type>(*this);
  *derivatives = 1 / x0;
  for (size_t i=1 ; i<=order_sum ; ++i)
    derivatives[i] = -derivatives[i-1] * i / x0;
  return apply_derivatives_nonhorner(order_sum, [&derivatives](size_t j) { return derivatives[j]; });
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::multiply_assign_by_root_type(bool is_root, const root_type& ca)
{
    auto itr = v.begin();
    if constexpr (is_fvar<RealType>::value)
    {
        itr->multiply_assign_by_root_type(is_root, ca);
        for (++itr ; itr!=v.end() ; ++itr)
            itr->multiply_assign_by_root_type(false, ca);
    }
    else
    {
        if (is_root || *itr != 0)
            *itr *= ca; // Skip multiplication of 0 by ca=inf to avoid nan. Exception: root value is always multiplied.
        for (++itr ; itr!=v.end() ; ++itr)
            if (*itr != 0)
                *itr *= ca;
    }
    return *this;
}
#endif

template<typename RealType, size_t Order>
fvar<RealType, Order>::operator root_type() const
{
  return static_cast<root_type>(v.front());
}

template<typename RealType, size_t Order>
template<typename T, typename /*unused*/>
fvar<RealType,Order>::operator T() const
{
  return static_cast<T>(static_cast<root_type>(v.front()));
}

#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
template<typename RealType, size_t Order>
fvar<RealType,Order>& fvar<RealType,Order>::set_root(const root_type& root)
{
    if constexpr (is_fvar<RealType>::value)
        v.front().set_root(root);
    else
        v.front() = root;
    return *this;
}
#endif

// Standard Library Support Requirements

template<typename RealType, size_t Order>
fvar<RealType,Order> fabs(const fvar<RealType,Order>& cr)
{
  const typename fvar<RealType,Order>::root_type zero(0);
  return cr < zero ? -cr
                   : cr == zero ? fvar<RealType,Order>() // Canonical fabs'(0) = 0.
                                : cr; // Propagate NaN.
}

template<typename RealType, size_t Order>
fvar<RealType,Order> abs(const fvar<RealType,Order>& cr)
{
  return fabs(cr);
}

template<typename RealType, size_t Order>
fvar<RealType,Order> ceil(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  return fvar<RealType,Order>(ceil(static_cast<typename fvar<RealType,Order>::root_type>(cr)));
}

template<typename RealType, size_t Order>
fvar<RealType,Order> floor(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  return fvar<RealType,Order>(floor(static_cast<typename fvar<RealType,Order>::root_type>(cr)));
}

template<typename RealType, size_t Order>
fvar<RealType,Order> exp(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  using root_type = typename fvar<RealType,Order>::root_type;
  const root_type d0 = exp(static_cast<root_type>(cr));
  return cr.apply_derivatives(order, [&d0](size_t) { return d0; });
}

template<typename RealType, size_t Order>
fvar<RealType,Order> pow(const fvar<RealType,Order>& x, const typename fvar<RealType,Order>::root_type& y)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type x0 = static_cast<root_type>(x);
  root_type derivatives[order+1] { pow(x0, y) };
  for (size_t i=0 ; i<order && y-i!=0 ; ++i)
    derivatives[i+1] = (y-i) * derivatives[i] / x0;
  if (fabs(x0) < std::numeric_limits<root_type>::epsilon()) {
  	return x.apply_derivatives_nonhorner(order, [&derivatives](size_t i) { return derivatives[i]; });
  } else {
    return x.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> pow(const typename fvar<RealType,Order>::root_type& x, const fvar<RealType,Order>& y)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type logx = log(x);
  const root_type y0 = static_cast<root_type>(y);
  root_type derivatives[order+1];
  *derivatives = pow(x, y0);
  for (size_t i=0 ; i<order ; ++i)
    derivatives[i+1] = derivatives[i] * logx;
  if (fabs(x) < std::numeric_limits<root_type>::epsilon()) {
  	return y.apply_derivatives_nonhorner(order, [&derivatives](size_t i) { return derivatives[i]; });
  }
  return y.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i]; });
}

template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>
    pow(const fvar<RealType1,Order1>& x, const fvar<RealType2,Order2>& y)
{
  BOOST_MATH_STD_USING
  using return_type = promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>;
  using root_type = typename return_type::root_type;
  constexpr size_t order = return_type::order_sum;
  const root_type x0 = static_cast<root_type>(x);
  const root_type y0 = static_cast<root_type>(y);
  root_type dxydx[order+1] { pow(x0, y0) };
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0) {
  	return return_type(*dxydx);
  }
  else
  {
    for (size_t i=0 ; i<order && y0-i!=0 ; ++i)
      dxydx[i+1] = (y0-i) * dxydx[i] / x0;
    std::array<fvar<root_type,order>,order+1> lognx;
    lognx.front() = fvar<root_type,order>(1);
#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
    lognx[1] = log(make_fvar<root_type,order>(x0));
#else // for compilers that compile this branch when order=0.
    lognx[(std::min)(size_t(1),order)] = log(make_fvar<root_type,order>(x0));
#endif
    for (size_t i=1 ; i<order ; ++i)
      lognx[i+1] = lognx[i] * lognx[1];
    const auto f = [&dxydx,&lognx](size_t i, size_t j) {
      size_t binomial = 1;
      root_type sum = dxydx[i] * static_cast<root_type>(lognx[j]);
      for (size_t k=1 ; k<=i ; ++k)
      {
        (binomial *= (i-k+1)) /= k; // binomial_coefficient(i,k)
        sum += binomial * dxydx[i-k] * lognx[j].derivative(k);
      }
      return sum; };
	if (fabs(x0) < std::numeric_limits<root_type>::epsilon()) {
		return x.apply_derivatives_nonhorner(order, f, y);
	}
  	return x.apply_derivatives(order, f, y);
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> sqrt(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  root_type derivatives[order+1];
  const root_type x = static_cast<root_type>(cr);
  *derivatives = sqrt(x);
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0) {
  	return fvar<RealType, Order>(*derivatives);
  }
  else
  {
    root_type numerator = 0.5;
    root_type powers = 1;
#ifndef BOOST_NO_CXX17_IF_CONSTEXPR
    derivatives[1] = numerator / *derivatives;
#else // for compilers that compile this branch when order=0.
    derivatives[(std::min)(size_t(1),order)] = numerator / *derivatives;
#endif
    using diff_t = typename std::array<RealType, Order+1>::difference_type;
    for (size_t i=2 ; i<=order ; ++i)
    {
      numerator *= root_type(-0.5) * ((diff_t(i)<<1)-3);
      powers *= x;
      derivatives[i] = numerator / (powers * *derivatives);
    }
    const auto f = [&derivatives](size_t i) { return derivatives[i]; };
	if (cr < std::numeric_limits<root_type>::epsilon()) {
		return cr.apply_derivatives_nonhorner(order, f);
	}
    return cr.apply_derivatives(order, f);
  }
}

// Natural logarithm. If cr==0 then derivative(i) may have nans due to nans from inverse().
template<typename RealType, size_t Order>
fvar<RealType, Order> log(const fvar<RealType, Order>& cr)
{
    BOOST_MATH_STD_USING
    using root_type = typename fvar<RealType, Order>::root_type;
    constexpr size_t order = fvar<RealType, Order>::order_sum;
    const root_type d0 = log(static_cast<root_type>(cr));
    if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
      return fvar<RealType, Order>(d0);
    else
    {
      const auto d1 = make_fvar<root_type, order - 1>(static_cast<root_type>(cr)).inverse(); // log'(x) = 1 / x
      return cr.apply_coefficients_nonhorner([&d0, &d1](size_t i) { return i ? d1.at(i - 1) / i : d0; });
    }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> frexp(const fvar<RealType,Order>& cr, int* exp)
{
  using std::exp2;
  using std::frexp;
  using multiprecision::exp2;
  

  using root_type = typename fvar<RealType,Order>::root_type;
  frexp(static_cast<root_type>(cr), exp);
  return cr * static_cast<root_type>(exp2(-*exp));
}

template<typename RealType, size_t Order>
fvar<RealType,Order> ldexp(const fvar<RealType,Order>& cr, int exp)
{
  // argument to std::exp2 must be casted to root_type, otherwise std::exp2 returns double (always)
  using std::exp2;
  using multiprecision::exp2;
  return cr * exp2(static_cast<typename fvar<RealType, Order>::root_type>(exp));
}

template<typename RealType, size_t Order>
fvar<RealType,Order> cos(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = cos(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    const root_type d1 = -sin(static_cast<root_type>(cr));
    const root_type derivatives[4] { d0, d1, -d0, -d1 };
    return cr.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i&3]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> sin(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = sin(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    const root_type d1 = cos(static_cast<root_type>(cr));
    const root_type derivatives[4] { d0, d1, -d0, -d1 };
    return cr.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i&3]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> asin(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = asin(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = sqrt((x*=x).negate()+=1).inverse(); // asin'(x) = 1 / sqrt(1-x*x).
    return cr.apply_coefficients_nonhorner([&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> tan(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = tan(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto c = cos(make_fvar<root_type,order-1>(static_cast<root_type>(cr)));
    const auto d1 = (c*=c).inverse(); // tan'(x) = 1 / cos(x)^2
    return cr.apply_coefficients_nonhorner([&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> atan(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = atan(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = ((x*=x)+=1).inverse(); // atan'(x) = 1 / (x*x+1).
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> atan2(const fvar<RealType,Order>& cr, const typename fvar<RealType,Order>::root_type& ca)
{
  BOOST_MATH_STD_USING
  using multiprecision::atan2;
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = atan2(static_cast<root_type>(cr), ca);
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto y = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = ca / ((y*=y)+=(ca*ca)); // (d/dy)atan2(y,x) = x / (y*y+x*x)
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> atan2(const typename fvar<RealType,Order>::root_type& ca, const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using multiprecision::atan2;
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = atan2(ca, static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = -ca / ((x*=x)+=(ca*ca)); // (d/dx)atan2(y,x) = -y / (x*x+y*y)
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

// Calculate via horner. No change to accuracies.
// Mathematically should be 0:
// z.derivative(4,5) = -0.00274658  (float)
// z.derivative(4,5) = -1.66267e-11 (double)
// z.derivative(4,5) = 6.245e-16    (long double)
template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>
atan2(const fvar<RealType1,Order1>& cr1, const fvar<RealType2,Order2>& cr2)
{
  BOOST_MATH_STD_USING
  using multiprecision::atan2;
  using return_type = promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>;
  using root_type = typename return_type::root_type;
  constexpr size_t order = return_type::order_sum;
  const root_type y = static_cast<root_type>(cr1);
  const root_type x = static_cast<root_type>(cr2);
  const root_type d00 = atan2(y, x);
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return return_type(d00);
  else
  {
    constexpr size_t order1 = fvar<RealType1,Order1>::order_sum;
    constexpr size_t order2 = fvar<RealType2,Order2>::order_sum;
    auto x01 = make_fvar<typename fvar<RealType2,Order2>::root_type,order2-1>(x);
    const auto d01 = -y / ((x01*=x01)+=(y*y));
    auto y10 = make_fvar<typename fvar<RealType1,Order1>::root_type,order1-1>(y);
    auto x10 = make_fvar<typename fvar<RealType2,Order2>::root_type,0,order2>(x);
    const auto d10 = x10 / ((x10*x10)+(y10*=y10));
    const auto f = [&d00,&d01,&d10](size_t i, size_t j) { return i ? d10.at(i-1,j)/i : j ? d01.at(j-1)/j : d00; };
    return cr1.apply_coefficients(order, f, cr2);
  }
}

template<typename RealType1, size_t Order1, typename RealType2, size_t Order2>
promote<fvar<RealType1,Order1>,fvar<RealType2,Order2>>
fmod(const fvar<RealType1,Order1>& cr1, const fvar<RealType2,Order2>& cr2)
{
  using math::trunc;
  
  const auto numer = static_cast<typename fvar<RealType1,Order1>::root_type>(cr1);
  const auto denom = static_cast<typename fvar<RealType2,Order2>::root_type>(cr2);
  return cr1 - cr2 * trunc(numer/denom);
}

template<typename RealType, size_t Order>
fvar<RealType,Order> round(const fvar<RealType,Order>& cr)
{
  using math::round;
  
  return fvar<RealType,Order>(round(static_cast<typename fvar<RealType,Order>::root_type>(cr)));
}

template<typename RealType, size_t Order>
int iround(const fvar<RealType,Order>& cr)
{
  using math::iround;
  
  return iround(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

template<typename RealType, size_t Order>
fvar<RealType,Order> trunc(const fvar<RealType,Order>& cr)
{
  using math::trunc;
  
  return fvar<RealType,Order>(trunc(static_cast<typename fvar<RealType,Order>::root_type>(cr)));
}

template<typename RealType, size_t Order>
int itrunc(const fvar<RealType,Order>& cr)
{
  using math::itrunc;
  
  return itrunc(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

template<typename RealType, size_t Order>
std::ostream& operator<<(std::ostream& out, const fvar<RealType,Order>& cr)
{
  out << "depth(" << cr.depth << ')';
  for (size_t i=0 ; i<cr.v.size() ; ++i)
    out << (i?',':'(') << cr.v[i];
  return out << ')';
}

// Additional functions

template<typename RealType, size_t Order>
fvar<RealType,Order> acos(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = acos(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = sqrt((x*=x).negate()+=1).inverse().negate(); // acos'(x) = -1 / sqrt(1-x*x).
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> acosh(const fvar<RealType,Order>& cr)
{
  using math::acosh;
  
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = acosh(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = sqrt((x*=x)-=1).inverse(); // acosh'(x) = 1 / sqrt(x*x-1).
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> asinh(const fvar<RealType,Order>& cr)
{
  using math::asinh;
  
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = asinh(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = sqrt((x*=x)+=1).inverse(); // asinh'(x) = 1 / sqrt(x*x+1).
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> atanh(const fvar<RealType,Order>& cr)
{
  using math::atanh;
  
  using root_type = typename fvar<RealType, Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = atanh(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr));
    const auto d1 = ((x*=x).negate()+=1).inverse(); // atanh'(x) = 1 / (1-x*x)
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> cosh(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = cosh(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    const root_type derivatives[2] { d0, sinh(static_cast<root_type>(cr)) };
    return cr.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i&1]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> erf(const fvar<RealType,Order>& cr)
{
  using math::erf;
  
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = erf(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr)); // d1 = 2/sqrt(pi)*exp(-x*x)
    const auto d1 = 2*constants::one_div_root_pi<root_type>()*exp((x*=x).negate());
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> erfc(const fvar<RealType,Order>& cr)
{
  using math::erfc;
  
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = erfc(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    auto x = make_fvar<root_type,order-1>(static_cast<root_type>(cr)); // erfc'(x) = -erf'(x)
    const auto d1 = -2*constants::one_div_root_pi<root_type>()*exp((x*=x).negate());
    return cr.apply_coefficients(order, [&d0,&d1](size_t i) { return i ? d1.at(i-1)/i : d0; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> lambert_w0(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using math::lambert_w0;
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  root_type derivatives[order+1];
  *derivatives = lambert_w0(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(*derivatives);
  else
  {
    const root_type expw = exp(*derivatives);
    derivatives[1] = 1 / (static_cast<root_type>(cr) + expw);
    if BOOST_AUTODIFF_IF_CONSTEXPR (order == 1)
      return cr.apply_derivatives_nonhorner(order, [&derivatives](size_t i) { return derivatives[i]; });
    else
    {
      using diff_t = typename std::array<RealType, Order+1>::difference_type;
      root_type d1powers = derivatives[1] * derivatives[1];
      const root_type x = derivatives[1] * expw;
      derivatives[2] = d1powers * (-1 - x);
      std::array<root_type,order> coef {{ -1, -1 }}; // as in derivatives[2].
      for (size_t n=3 ; n<=order ; ++n)
      {
        coef[n-1] = coef[n-2] * -static_cast<root_type>(2*n-3);
        for (size_t j=n-2 ; j!=0 ; --j)
          (coef[j] *= -static_cast<root_type>(n-1)) -= (n+j-2) * coef[j-1];
        coef[0] *= -static_cast<root_type>(n-1);
        d1powers *= derivatives[1];
        derivatives[n] = d1powers * std::accumulate(coef.crend()-diff_t(n-1), coef.crend(), coef[n-1],
                                                    [&x](const root_type& a, const root_type& b) { return a*x + b; });
      }
      return cr.apply_derivatives_nonhorner(order, [&derivatives](size_t i) { return derivatives[i]; });
    }
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> sinc(const fvar<RealType,Order>& cr)
{
  if (cr != 0)
    return sin(cr) / cr;
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  root_type taylor[order+1] { 1 }; // sinc(0) = 1
  if BOOST_AUTODIFF_IF_CONSTEXPR (order == 0)
    return fvar<RealType,Order>(*taylor);
  else
  {
    for (size_t n=2 ; n<=order ; n+=2)
      taylor[n] = (1-static_cast<int>(n&2)) / factorial<root_type>(static_cast<unsigned>(n+1));
    return cr.apply_coefficients_nonhorner([&taylor](size_t i) { return taylor[i]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> sinh(const fvar<RealType,Order>& cr)
{
  BOOST_MATH_STD_USING
  using root_type = typename fvar<RealType,Order>::root_type;
  constexpr size_t order = fvar<RealType,Order>::order_sum;
  const root_type d0 = sinh(static_cast<root_type>(cr));
  if BOOST_AUTODIFF_IF_CONSTEXPR (fvar<RealType,Order>::order_sum == 0)
    return fvar<RealType,Order>(d0);
  else
  {
    const root_type derivatives[2] { d0, cosh(static_cast<root_type>(cr)) };
    return cr.apply_derivatives(order, [&derivatives](size_t i) { return derivatives[i&1]; });
  }
}

template<typename RealType, size_t Order>
fvar<RealType,Order> tanh(const fvar<RealType,Order>& cr)
{
  fvar<RealType,Order> retval = exp(cr*2);
  const fvar<RealType,Order> denom = retval + 1;
  (retval -= 1) /= denom;
  return retval;
}

template<typename RealType, size_t Order>
long lround(const fvar<RealType,Order>& cr)
{
  using math::lround;
  
  return lround(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

template<typename RealType, size_t Order>
long long llround(const fvar<RealType,Order>& cr)
{
  using math::llround;
  
  return llround(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

template<typename RealType, size_t Order>
long long lltrunc(const fvar<RealType,Order>& cr)
{
  using math::lltrunc;
  
  return lltrunc(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

template<typename RealType, size_t Order>
long double truncl(const fvar<RealType,Order>& cr)
{
  using std::truncl;
  return truncl(static_cast<typename fvar<RealType,Order>::root_type>(cr));
}

} } } } } // namespace boost::math::differentiation::autodiff_v1::detail

namespace std {

/// boost::math::tools::digits<RealType>() is handled by this std::numeric_limits<> specialization,
/// and similarly for max_value, min_value, log_max_value, log_min_value, and epsilon.
template <typename RealType, size_t Order>
class numeric_limits<boost::math::differentiation::detail::fvar<RealType,Order>>
    : public numeric_limits<typename boost::math::differentiation::detail::fvar<RealType,Order>::root_type>
{ };

} // namespace std

namespace boost { namespace math { namespace tools {
namespace detail {
template<typename RealType, std::size_t Order>
using autodiff_fvar_type = differentiation::detail::fvar<RealType, Order>;

template<typename RealType, std::size_t Order>
using autodiff_root_type = typename autodiff_fvar_type<RealType, Order>::root_type;
} // namespace detail

// See boost/math/tools/promotion.hpp
template<typename RealType0, size_t Order0, typename RealType1, size_t Order1>
struct promote_args_2<detail::autodiff_fvar_type<RealType0, Order0>,
                      detail::autodiff_fvar_type<RealType1, Order1>>
{
  using type = detail::autodiff_fvar_type<typename promote_args_2<RealType0, RealType1>::type,
#ifndef BOOST_NO_CXX14_CONSTEXPR
           (std::max)(Order0,Order1)>;
#else
          Order0 < Order1 ? Order1 : Order0>;
#endif
};

template<typename RealType, size_t Order>
struct promote_args<detail::autodiff_fvar_type<RealType, Order>>
{
  using type = detail::autodiff_fvar_type<typename promote_args<RealType>::type, Order>;
};

template<typename RealType0, size_t Order0, typename RealType1>
struct promote_args_2<detail::autodiff_fvar_type<RealType0, Order0>, RealType1>
{
  using type = detail::autodiff_fvar_type<typename promote_args_2<RealType0, RealType1>::type, Order0>;
};

template<typename RealType0, typename RealType1, size_t Order1>
struct promote_args_2<RealType0, detail::autodiff_fvar_type<RealType1, Order1>>
{
      using type = detail::autodiff_fvar_type<typename promote_args_2<RealType0, RealType1>::type, Order1>;
};

template<typename destination_t, typename RealType, std::size_t Order>
inline BOOST_MATH_CONSTEXPR destination_t real_cast(const detail::autodiff_fvar_type<RealType, Order>& from_v) BOOST_NOEXCEPT_IF(BOOST_MATH_IS_FLOAT(destination_t) && BOOST_MATH_IS_FLOAT(RealType))
{
      return real_cast<destination_t>(static_cast<detail::autodiff_root_type<RealType, Order>>(from_v));
}

} // namespace tools

namespace policies {

template <class Policy, std::size_t Order>
using fvar_t = differentiation::detail::fvar<Policy, Order>;
template <class Policy, std::size_t Order>
struct evaluation<fvar_t<float, Order>, Policy> {
  using type = fvar_t<typename conditional<Policy::promote_float_type::value, double, float>::type, Order>;
};

template <class Policy, std::size_t Order>
struct evaluation<fvar_t<double, Order>, Policy> {
  using type =
  fvar_t<typename conditional<Policy::promote_double_type::value, long double, double>::type, Order>;
};

} } } // namespace boost::math::policies

#ifdef BOOST_NO_CXX17_IF_CONSTEXPR
#include "autodiff_cpp11.hpp"
#endif

#endif // BOOST_MATH_DIFFERENTIATION_AUTODIFF_HPP
