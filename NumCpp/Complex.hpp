/* Complex.hpp */
#pragma once

#include <type_traits>
#include <complex>


namespace numcpp {

template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class complex {
public:
    /* ====== Constructors ====== */

    complex() : _real(0), _imaginary(0) {}

    complex(const T& real, const T& imag = 0) : _real(real), _imaginary(imag) {}

    explicit complex(const std::complex<T> num) : _real(num.real()), _imaginary(num.imag()) {}

    complex(const complex& num) : _real(num.real()), _imaginary(num.imag()) {}


    /* ====== Helpers ====== */

    [[nodiscard]] T real() const { return _real; }

    [[nodiscard]] T imag() const { return _imaginary; }


    /* ====== Functionality ====== */

    [[nodiscard]] double abs() const {
        return std::hypot(_real, _imaginary);
    }

    [[nodiscard]] double arg() const {
        return std::atan2(_imaginary, _real);
    }

    [[nodiscard]] complex conj() const {
        return Complex(_real, -_imaginary);
    }


    /* ====== Overloads ====== */
    // Print overload
    friend std::ostream& operator<< (std::ostream& os, const complex& number) {
        os << number.real() << (number.imag() >= 0 ? " + " : " - ")
        << std::abs(number.imag()) << "j";
        return os;
    }

    /* ====== Comparison Overloads ====== */

    // operator==
    template <typename U>
    bool operator==(const complex<U>& other) const {
        return (_real == other.real()) && (_imaginary == other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator==(const U other) const {
        return (_real == other) && (_imaginary == 0);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator==(const U scalar, const complex& other) {
        return (scalar == other.real()) && (0 == other.imag());
    }


    // operator!=
    template <typename U>
    bool operator!=(const complex<U>& other) const {
        return !(*this == other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator!=(const U other) const {
        return !(*this == other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator!=(const U scalar, const complex& other) {
        return !(scalar == other);
    }


    // operator< (compares magnitudes)
    template <typename U>
    bool operator<(const complex<U>& other) const {
        return this->abs() < other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator<(const U other) const {
        return this->abs() < std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator<(const U scalar, const complex& other) {
        return std::abs(scalar) < other.abs();
    }


    // operator<= (compares magnitudes)
    template <typename U>
    bool operator<=(const complex<U>& other) const {
        return this->abs() <= other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator<=(const U other) const {
        return this->abs() <= std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator<=(const U scalar, const complex& other) {
        return std::abs(scalar) <= other.abs();
    }


    // operator> (compares magnitudes)
    template <typename U>
    bool operator>(const complex<U>& other) const {
        return this->abs() > other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator>(const U other) const {
        return this->abs() > std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator>(const U scalar, const complex& other) {
        return std::abs(scalar) > other.abs();
    }


    // operator>= (compares magnitudes)
    template <typename U>
    bool operator>=(const complex<U>& other) const {
        return this->abs() >= other.abs();
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    bool operator>=(const U other) const {
        return this->abs() >= std::abs(other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend bool operator>=(const U scalar, const complex& other) {
        return std::abs(scalar) >= other.abs();
    }

    /* ====== Arithmetic Overloads ====== */

    // operator+
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(_real+other.real(), _imaginary+other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const U other) const
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(_real+other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator+(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return complex<R>(scalar+other.real(), other.imag());
    }


    // operator-
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(_real-other.real(), _imaginary-other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const U other) const
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(_real-other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator-(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return complex<R>(scalar-other.real(), other.imag());
    }



    // operator*
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());

        T re = (_real*other.real()) - (_imaginary*other.imag());
        T im = (_real*other.imag()) + (_imaginary*other.real());
        return complex<R>(re, im);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const U other) const
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(_real*other, _imaginary*other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator*(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return complex<R>(scalar*other.real(), scalar*other.imag());
    }



    // operator/
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const complex<U>& other) const
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const complex temp = this->operator*(other.conj());
        U denominator = (other.real()*other.real()) + (other.imag()*other.imag());
        return complex<R>(temp.real()/denominator, temp.imag()/denominator);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const U other) const
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());
        return complex<R>(_real/other, _imaginary/other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator/(const U scalar, const complex& other)
        -> complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const complex temp = other.conj();
        T denominator = (other.real()*other.real()) + (other.imag()*other.imag());

        return complex<R>((temp.real()*scalar)/denominator, (temp.imag()*scalar)/denominator);
    }

private:
    T _real;
    T _imaginary;
};


/* ====== Function Wrappers ====== */

template <typename T>
T real(const complex<T>& num) { return num.real(); }

template <typename T>
T imag(const complex<T>& num) { return num.imag(); }

template <typename T>
double abs(const complex<T>& num) {
    return num.abs();
}

template <typename T>
double arg(const complex<T>& num) {
    return num.arg();
}

template <typename T>
complex<T> conj(const complex<T>& num) {
    return num.conj();
}

template <typename T>
complex<T> polar(const T& rho, const T& theta) {
    return complex<T>(rho*std::cos(theta), rho*std::sin(theta));
}


/* ====== Constants ====== */

const Complex<double> j(0,1);
// const auto i = j;


/* ====== Complex Number Type Checking ====== */

template<typename T>
struct is_complex : std::false_type {};

template<typename T>
struct is_complex<complex<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_complex_v = is_complex<T>::value;


template<typename T>
struct is_complex_floating_point : std::false_type {};

template<typename T>
struct is_complex_floating_point<complex<T>> : std::bool_constant<std::is_floating_point_v<T>> {};

template<typename T>
inline constexpr bool is_complex_floating_point_v = is_complex_floating_point<T>::value;


template<typename T>
struct is_complex_or_arithmetic : std::bool_constant<std::is_arithmetic_v<T> || is_complex_v<T>> {};

template<typename T>
inline constexpr bool is_complex_or_arithmetic_v = is_complex_or_arithmetic<T>::value;


template <typename T>
struct underlying_type {
    using type = T;
};

template <typename T>
struct underlying_type<complex<T>> {
    using type = T;
};

template <typename T>
using underlying_type_t = typename underlying_type<T>::type;

} // namespace numcpp
