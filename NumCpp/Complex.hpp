/* Complex.hpp */
#pragma once

#include <type_traits>
#include <complex>

namespace numcpp {

template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
class Complex {
public:
    /* ====== Constructors ====== */

    Complex(T real, T imag) : _real(real), _imaginary(imag) {}

    explicit Complex(std::complex<T> num) : _real(num.real()), _imaginary(num.imag()) {}

    explicit Complex(const Complex& num) : _real(num.real()), _imaginary(num.imag()) {}


    /* ====== Helpers ====== */

    T real() { return _real; }

    T imag() { return _imaginary; }


    /* ====== Functionality ====== */

    double abs() {
        return std::hypot(_real, _imaginary);
    }

    double arg() {
        return atan(_real/static_cast<double>(_imaginary));
    }

    Complex conj() {
        return Complex(_real, -_imaginary);
    }


    /* Overloads */
    // Print overload
    friend std::ostream& operator<< (std::ostream& os, const Complex& number) {
        os << number.real() << " + j" << number.imag();
        return os;
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


} // namespace numcpp