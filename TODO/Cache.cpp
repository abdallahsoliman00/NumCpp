/* Yes, this code was mostly AI generated, yes it is only for testing purposes.*/

#include <iostream>
#include <unordered_map>
#include <cmath>
#include <string>
#include <functional>
#include <sstream>
#include <iomanip>

// Helper function to convert number to string
template <typename T>
std::string toString(const T& obj, const int float_precision = 0) {
    std::ostringstream oss;
    if(float_precision == 0)
        oss << obj;
    else
        oss << std::fixed << std::setprecision(float_precision) << obj;
    return oss.str();
}

// Cache key: combines function ID and argument
template <typename T>
struct CacheKey {
    int func_id;
    T arg;
    
    bool operator==(const CacheKey& other) const {
        return func_id == other.func_id && arg == other.arg;
    }
};

// Hash function for CacheKey
template <typename T>
struct CacheKeyHash {
    std::size_t operator()(const CacheKey<T>& key) const {
        std::size_t h1 = std::hash<int>{}(key.func_id);
        std::size_t h2 = std::hash<T>{}(key.arg);
        return h1 ^ (h2 << 1);
    }
};

// Global cache for each type
template <typename T>
std::unordered_map<CacheKey<T>, int, CacheKeyHash<T>> globalCache;

// Function IDs
enum FuncID {
    LEFT_PADDING = 1,
    RIGHT_PADDING = 2,
    EXPONENT = 3
};

// Original functions
template <typename T>
int get_left_padding(T num) {
    if (num == 0) return 1;

    if constexpr (std::is_floating_point_v<T>) {
        T integer_part = std::floor(std::abs(num));
        if (integer_part == 0)
            return 1;
        return static_cast<int>(std::log10(integer_part) + 1);
    } else {
        return static_cast<int>(std::log10(std::abs(num)) + 1);
    }
}

template <typename T>
int get_right_padding(T num) {
    if constexpr (std::is_integral_v<T>)
        return 0;

    std::string s = toString(std::abs(num));

    // Find the decimal point
    const std::size_t dot = s.find('.');
    if (dot == std::string::npos)
        return 0;

    // If the last char is '.', remove it too
    if (!s.empty() && s.back() == '.')
        return 0;

    return static_cast<int>(s.size() - dot - 1);
}

template <typename T>
int get_exponent(T num) {
    if (num == 0) return 0;
    const double lognum = std::log10(std::abs(static_cast<double>(num)));
    const int exponent = static_cast<int>(std::floor(lognum));
    return exponent;
}

// Cache wrapper class
template <typename T>
class FunctionCache {
private:
    CacheKey<T> key;
    
public:
    FunctionCache(FuncID func_id, T arg) : key{func_id, arg} {
        // Check if result is already cached
        if (globalCache<T>.find(key) == globalCache<T>.end()) {
            // Not cached, compute and store
            int result;
            switch (func_id) {
                case LEFT_PADDING:
                    result = get_left_padding(arg);
                    break;
                case RIGHT_PADDING:
                    result = get_right_padding(arg);
                    break;
                case EXPONENT:
                    result = get_exponent(arg);
                    break;
                default:
                    result = 0;
            }
            globalCache<T>[key] = result;
        }
    }
    
    // Call operator to retrieve cached result
    int operator()() const {
        auto it = globalCache<T>.find(key);
        if (it != globalCache<T>.end()) {
            return it->second;
        }
        return 0; // Should never happen if constructor worked
    }
    
    // Explicit getter
    int getResult() const {
        return (*this)();
    }
    
    // Static method to clear cache
    static void clearCache() {
        globalCache<T>.clear();
    }
    
    // Static method to get cache size
    static size_t cacheSize() {
        return globalCache<T>.size();
    }
};

int main() {
    std::cout << "=== Function Cache Demo ===" << std::endl << std::endl;
    
    // Test with integers
    std::cout << "--- Integer Tests ---" << std::endl;
    int num1 = 12345;
    FunctionCache<int> cache1(LEFT_PADDING, num1);
    std::cout << "Left padding of " << num1 << ": " << cache1() << std::endl;
    
    FunctionCache<int> cache2(RIGHT_PADDING, num1);
    std::cout << "Right padding of " << num1 << ": " << cache2() << std::endl;
    
    FunctionCache<int> cache3(EXPONENT, num1);
    std::cout << "Exponent of " << num1 << ": " << cache3() << std::endl;
    
    std::cout << "Cache size (int): " << FunctionCache<int>::cacheSize() << std::endl;
    std::cout << std::endl;
    
    // Test with doubles
    std::cout << "--- Double Tests ---" << std::endl;
    double num2 = 123.456;
    FunctionCache<double> cache4(LEFT_PADDING, num2);
    std::cout << "Left padding of " << num2 << ": " << cache4() << std::endl;
    
    FunctionCache<double> cache5(RIGHT_PADDING, num2);
    std::cout << "Right padding of " << num2 << ": " << cache5() << std::endl;
    
    FunctionCache<double> cache6(EXPONENT, num2);
    std::cout << "Exponent of " << num2 << ": " << cache6() << std::endl;
    
    std::cout << "Cache size (double): " << FunctionCache<double>::cacheSize() << std::endl;
    std::cout << std::endl;
    
    // Test caching - calling same function with same arg
    std::cout << "--- Testing Cache Reuse ---" << std::endl;
    FunctionCache<double> cache7(LEFT_PADDING, num2); // Should use cached result
    std::cout << "Left padding of " << num2 << " (cached): " << cache7() << std::endl;
    std::cout << "Cache size (double): " << FunctionCache<double>::cacheSize() 
              << " (should be same as before)" << std::endl;
    std::cout << std::endl;
    
    // Test with more values
    std::cout << "--- More Tests ---" << std::endl;
    double num3 = 0.00123;
    FunctionCache<double> cache8(LEFT_PADDING, num3);
    FunctionCache<double> cache9(RIGHT_PADDING, num3);
    FunctionCache<double> cache10(EXPONENT, num3);
    
    std::cout << "Number: " << num3 << std::endl;
    std::cout << "  Left padding: " << cache8() << std::endl;
    std::cout << "  Right padding: " << cache9() << std::endl;
    std::cout << "  Exponent: " << cache10() << std::endl;
    std::cout << "Cache size (double): " << FunctionCache<double>::cacheSize() << std::endl;
    std::cout << std::endl;
    
    // Using getResult() method
    std::cout << "--- Using getResult() ---" << std::endl;
    std::cout << "Left padding result: " << cache8.getResult() << std::endl;
    std::cout << std::endl;
    
    // Clear caches
    std::cout << "--- Clearing Caches ---" << std::endl;
    FunctionCache<int>::clearCache();
    FunctionCache<double>::clearCache();
    std::cout << "Cache size (int): " << FunctionCache<int>::cacheSize() << std::endl;
    std::cout << "Cache size (double): " << FunctionCache<double>::cacheSize() << std::endl;
    
    return 0;
}
