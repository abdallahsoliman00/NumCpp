/* Complex.hpp */
#pragma once

#include <type_traits>
#include <complex>

namespace numcpp {

template <typename T = double, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class Complex {
public:
    /* ====== Constructors ====== */

    Complex(T real, T imag) : _real(real), _imaginary(imag) {}

    explicit Complex(std::complex<T> num) : _real(num.real()), _imaginary(num.imag()) {}

    Complex(const Complex& num) : _real(num.real()), _imaginary(num.imag()) {}


    /* ====== Helpers ====== */

    [[nodiscard]] T real() const { return _real; }

    [[nodiscard]] T imag() const { return _imaginary; }


    /* ====== Functionality ====== */

    [[nodiscard]] double abs() const {
        return std::hypot(_real, _imaginary);
    }

    [[nodiscard]] double arg() const {
        if (_imaginary == 0 && _real >= 0) return pi/2;
        if (_imaginary == 0 && _real < 0) return -pi/2;
        return atan(_real/static_cast<double>(_imaginary));
    }

    [[nodiscard]] Complex conj() const {
        return Complex(_real, -_imaginary);
    }


    /* ====== Overloads ====== */
    // Print overload
    friend std::ostream& operator<< (std::ostream& os, const Complex& number) {
        os << number.real() << (number.imag() >= 0 ? " + " : " - ")
        << std::abs(number.imag()) << "j";
        return os;
    }

    /* ====== Arithmetic Overloads ====== */

    // operator+
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const Complex<U>& other) const
        -> Complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return Complex<R>(_real+other.real(), _imaginary+other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator+(const U other) const
        -> Complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return Complex<R>(_real+other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator+(const U scalar, const Complex& other)
        -> Complex<decltype(std::declval<T>()+std::declval<U>())>
    {
        using R = decltype(std::declval<T>()+std::declval<U>());
        return Complex<R>(scalar+other.real(), other.imag());
    }


    // operator-
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const Complex<U>& other) const
        -> Complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return Complex<R>(_real-other.real(), _imaginary-other.imag());
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator-(const U other) const
        -> Complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return Complex<R>(_real-other, _imaginary);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator-(const U scalar, const Complex& other)
        -> Complex<decltype(std::declval<T>()-std::declval<U>())>
    {
        using R = decltype(std::declval<T>()-std::declval<U>());
        return Complex<R>(scalar-other.real(), other.imag());
    }



    // operator*
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const Complex<U>& other) const
        -> Complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());

        T re = (_real*other.real()) - (_imaginary*other.imag());
        T im = (_real*other.imag()) + (_imaginary*other.real());
        return Complex<R>(re, im);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator*(const U other) const
        -> Complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return Complex<R>(_real*other, _imaginary*other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator*(const U scalar, const Complex& other)
        -> Complex<decltype(std::declval<T>()*std::declval<U>())>
    {
        using R = decltype(std::declval<T>()*std::declval<U>());
        return Complex<R>(scalar*other.real(), scalar*other.imag());
    }



    // operator/
    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const Complex<U>& other) const
        -> Complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const Complex temp = this->operator*(other.conj());
        U denominator = (other.real()*other.real()) + (other.imag()*other.imag());
        return Complex<R>(temp.real()/denominator, temp.imag()/denominator);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    auto operator/(const U other) const
        -> Complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());
        return Complex<R>(_real/other, _imaginary/other);
    }

    template <typename U, typename = std::enable_if_t<std::is_arithmetic_v<U>>>
    friend auto operator/(const U scalar, const Complex& other)
        -> Complex<decltype(std::declval<T>()/std::declval<U>())>
    {
        using R = decltype(std::declval<T>()/std::declval<U>());

        const Complex temp = other.conj();
        T denominator = (other.real()*other.real()) + (other.imag()*other.imag());

        return Complex<R>((temp.real()*scalar)/denominator, (temp.imag()*scalar)/denominator);
    }


private:
    T _real;
    T _imaginary;
};


/* ====== Function Wrappers ====== */

template <typename T>
T real(const Complex<T>& num) { return num.real(); }

template <typename T>
T imag(const Complex<T>& num) { return num.imag(); }

template <typename T>
double abs(const Complex<T>& num) {
    return num.abs();
}

template <typename T>
double arg(const Complex<T>& num) {
    return num.arg();
}

template <typename T>
Complex<T> conj(const Complex<T>& num) {
    return num.conj();
}


/* ====== Constants ====== */

const Complex<double> i(0,1);
const auto j = i;


} // namespace numcpp