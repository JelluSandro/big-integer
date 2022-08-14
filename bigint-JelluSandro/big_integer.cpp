#include "big_integer.h"
#include <cstddef>
#include <stdexcept>
#include <ostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>

constexpr uint64_t MOD32 = static_cast<uint64_t>(UINT32_MAX) + 1;

big_integer::big_integer() : negate(false), num(1, 0) {
}

constexpr uint64_t UINT32_64 = static_cast<uint64_t>(UINT32_MAX);

uint32_t mod32(uint64_t x) {
    return static_cast<uint32_t> (x & UINT32_64);
}

uint64_t parse(std::string const &s, size_t l, size_t r) {
    uint64_t ans = 0;
    for (size_t i = l; i < r; i++) {
        if (s[i] >= '0' && s[i] <= '9') {
            ans = ans * 10 + (s[i] - '0');
        } else {
            throw std::invalid_argument("found not number");
        }
    }
    return ans;
}

big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int a) : negate(a < 0), num(1, a) {}

big_integer::big_integer(unsigned int a) : negate(false), num(1, a) {}

big_integer::big_integer(long a) : big_integer(static_cast<long long>(a)) {}

big_integer::big_integer(unsigned long a) : big_integer(static_cast<unsigned long long>(a)) {}

big_integer::big_integer(unsigned long long a) : negate(false) {
    while (a != 0) {
        num.push_back(mod32(a));
        a /= MOD32;
    }
}

big_integer::big_integer(long long a) : negate(a < 0) {
    unsigned long long x = a;
    while (x != 0) {
        num.push_back(mod32(x));
        x /= MOD32;
    }
    this->erase0();
}


const big_integer BIG_ONE = big_integer(1);

const big_integer BIG_MINUS_ONE = big_integer(-1);

uint32_t big_integer::get(size_t x) const {
    if (num.size() < x + 1) {
        if (negate) {
            return UINT32_MAX;
        }
        return 0;
    }
    if (num.size() == x + 1) {
        if (negate) {
            return (UINT32_MAX & num[x]);
        }
    }
    return num[x];
}

uint64_t POW10[10] = {1,
                      10,
                      100,
                      1000,
                      10000,
                      100000,
                      1000000,
                      10000000,
                      100000000,
                      1000000000};

big_integer big_integer::mul(uint32_t input) const {
    big_integer ans = *this;
    return ans.mul_not_copy(input);
}


big_integer &big_integer::mul_not_copy(uint32_t input) {
    uint64_t x = input;
    uint64_t y = 0;
    for (size_t j = 0; j < num.size(); j++) {
        y += x * static_cast<uint64_t>(num[j]);
        num[j] = mod32(y);
        y /= MOD32;
    }
    if (y != 0) {
        num.push_back(y % MOD32);
    }
    return *this;
}


big_integer &big_integer::mul(uint32_t input, big_integer &z) {
    uint64_t x = input;
    uint64_t second = 0;
    size_t n = num.size();
    size_t m = z.num.size();
    for (size_t j = 0; j < n + m + 3; j++) {
        second += x * static_cast<uint64_t>(this->get(j));
        uint64_t pr = mod32(second);
        second /= MOD32;
        uint64_t pp = pr + static_cast<uint64_t>(z.get(j));
        if (j < z.num.size()) {
            z.num[j] = mod32(pp);
        } else {
            z.num.push_back(mod32(pp));
        }
        second = (second + pp / MOD32);
    }
    return *this;
}

uint64_t big_integer::div_mod_on_short(uint32_t x) {
    uint64_t p = 0;
    for (size_t i = num.size(); i > 0; i--) {
        uint64_t b = ((p << 32) | static_cast<uint64_t>(this->get(i - 1)));
        num[i - 1] = mod32(b / x);
        p = b % x;
    }
    this->erase0();
    return p;
}


big_integer::big_integer(std::string const &str) : big_integer() {
    size_t end = 0;
    if (str[0] == '-' || str[0] == '+') {
        end++;
    }
    if (str.size() == end) {
        throw std::invalid_argument("empty number");
    }
    uint64_t x;
    uint32_t k;
    size_t len = str.length();
    for (size_t i = end; i < len; i += 9) {
        if (i + 9 >= len) {
            k = str.size() - i;
        } else {
            k = 9;
        }
        x = parse(str, i, i + k);
        this->mul_not_copy(POW10[k]);
        *this += x;
    }
    if (str[0] == '-') {
        this->negative();
    }

}

big_integer::~big_integer() = default;

big_integer &big_integer::operator=(big_integer const &other) = default;

big_integer &big_integer::erase0() {
    while (num.size() > 1 && ((num.back() == 0 && !negate) || (num.back() == UINT32_MAX && negate))) {
        num.pop_back();
    }
    if (num.size() == 0) {
        negate = false;
        num.push_back(0);
    }
    return *this;
}

big_integer &big_integer::operator+=(big_integer const &rhs) {
    uint32_t p = 0;
    size_t n = rhs.num.size();
    size_t m = num.size();
    size_t mn = std::max(n, m);
    uint32_t pp = 0;
    for (size_t i = 0; i < mn + 2; i++) {
        uint64_t x = p;
        x += rhs.get(i);
        x += this->get(i);
        pp = mod32(x);
        if (m > i) {
            num[i] = pp;
        } else {
            num.push_back(pp);
        }
        p = x / MOD32;
    }
    if (pp == 0) {
        negate = false;
    } else {
        negate = true;
    }
    this->erase0();
    return *this;
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    this->negative();
    *this += rhs;
    this->negative();
    return *this;
}

big_integer big_integer::abs() const {
    big_integer ans = *this;
    return ans.abs_no_copy();
}

big_integer &big_integer::abs_no_copy() {
    if (negate) {
        this->negative();
    }
    return *this;
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    bool flag = negate ^rhs.negate;
    this->abs_no_copy();
    big_integer b = rhs.abs();
    size_t n = b.num.size();
    big_integer ans;
    ans.num.resize(n + num.size() + 3, 0);
    for (size_t i = n; i > 0; i--) {
        if (i != n + 1) {
            ans <<= 32;
        }
        this->mul(b.num[i - 1], ans);
    }
    *this = ans;
    if (flag) {
        this->negative();
    }
    this->erase0();
    return *this;
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    bool neg = negate ^rhs.negate;
    this->abs_no_copy();
    big_integer x = rhs.abs();
    if (*this < x) {
        *this = 0;
        return *this;
    }
    if (x.num.size() == 1) {
        this->div_mod_on_short(x.num[0]);
    } else {
        uint32_t nor = static_cast<uint32_t>(MOD32 / (static_cast<uint64_t>(x.num.back()) + 1));
        *this = this->mul(nor);
        x = x.mul(nor);
        this->erase0();
        x.erase0();
        size_t n = num.size();
        size_t m = x.num.size();
        big_integer ans;
        ans.num.resize(n - m + 1);
        uint64_t xx = x.num[m - 1];
        for (size_t i = n - m;; i--) {
            ans.num[i] = static_cast<uint32_t>(std::min(
                    ((static_cast<uint64_t>(this->get(i + m)) << 32) +
                     static_cast<uint64_t>(this->get(i + m - 1)))
                    / xx, UINT32_64));
            *this -= x.mul(ans.num[i]) << (32 * i);
            if (*this < 0) {
                ans.num[i]--;
                *this += x << (32 * i);
            }
            if (i == 0) {
                break;
            }
        }
        *this = ans;
    }
    if (neg) {
        this->negative();
    }
    this->erase0();
    return *this;
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    *this -= (*this / rhs) * rhs;
    return *this;
}

big_integer &big_integer::bitoper(const std::function<uint32_t(uint32_t, uint32_t)> &op, big_integer const &rhs) {
    for (int i = 0; i < std::max(rhs.num.size(), num.size()); i++) {
        uint32_t x = op(rhs.get(i), this->get(i));
        if (num.size() > i) {
            num[i] = x;
        } else {
            num.push_back(x);
        };
    }
    negate = op(static_cast<uint32_t>(rhs.negate),
                static_cast<uint32_t>(negate));
    this->erase0();
    return *this;
}

uint32_t op_and(uint32_t x, uint32_t y) {
    return x & y;
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return bitoper(op_and, rhs);
}

uint32_t op_or(uint32_t x, uint32_t y) {
    return x | y;
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return bitoper(op_or, rhs);
}

uint32_t op_xor(uint32_t x, uint32_t y) {
    return x ^ y;
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return bitoper(op_xor, rhs);
}

big_integer &big_integer::operator<<=(int rhs) {
    int i = 0;
    num.insert(num.begin(), rhs / 32, 0);
    i = rhs % 32;
    if (i != 0) {
        uint64_t y = 0;
        for (size_t j = 0; j < num.size(); j++) {
            y |= (static_cast<uint64_t>(num[j]) << i); //(flag << 32));
            num[j] = static_cast<uint32_t>(y); //md = 2^32
            y >>= 32;
        }
        if (y != 0) {
            if (negate) {
                y |= (UINT32_64 << i);
            }
            num.push_back(mod32(y));
        }
    }
    this->erase0();
    return *this;
}

big_integer &big_integer::operator>>=(int rhs) {
    int i = 0;
    if (negate) {
        num.push_back(UINT32_MAX);
    } else {
        num.push_back(0);
    }
    num.erase(num.begin(), num.begin() + rhs / 32);
    i = rhs % 32;
    if (i != 0) {
        for (size_t j = 0; j < num.size(); j++) {
            if (j + 1 < num.size()) {
                num[j] = (num[j] >> i);
                num[j] |= ((num[j + 1] % (1 << i)) << (32 - i));
            }
        }
    }
    this->erase0();
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}


big_integer &big_integer::negative() {
    --*this;
    negate ^= 1;
    for (size_t i = 0; i < num.size(); i++) {
        num[i] = ~num[i];
    }
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer new_b = *this;
    return new_b.negative();
}

big_integer big_integer::operator~() const {
    big_integer new_b = *this;
    new_b.negate ^= 1;
    for (size_t i = 0; i < new_b.num.size(); i++) {
        new_b.num[i] = ~new_b.num[i];
    }
    return new_b;
}

big_integer &big_integer::operator++() {
    return *this += BIG_ONE;
}

big_integer big_integer::operator++(int) {
    big_integer new_b = *this;
    *this += BIG_ONE;
    return new_b;
}

big_integer &big_integer::operator--() {
    return *this += BIG_MINUS_ONE;
}

big_integer big_integer::operator--(int) {
    big_integer new_b = *this;
    *this += BIG_MINUS_ONE;
    return new_b;
}

big_integer operator+(big_integer a, big_integer const &b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const &b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const &b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const &b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const &b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const &b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const &b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const &b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const &a, big_integer const &b) {
    return a.num == b.num;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return a.num != b.num;
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.negate == b.negate) {
        if (a.num.size() < b.num.size()) {
            return !a.negate;
        }
        if (a.num.size() > b.num.size()) {
            return a.negate;
        }
        if (a.num.size() > 0) {
            for (size_t i = a.num.size(); i > 0; i--) {
                if (a.num[i - 1] < b.num[i - 1]) {
                    return true;
                }
                if (a.num[i - 1] > b.num[i - 1]) {
                    return false;
                }
            }
        }
        return false;
    }
    return !b.negate;
}

bool operator>(big_integer const &a, big_integer const &b) {
    return operator<(b, a);
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return (operator<(a, b) | operator==(a, b));
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return (operator<(b, a) | operator==(a, b));
}

std::string to_string(big_integer const &a) {
    std::string s = "";
    big_integer x = a.abs();
    x.erase0();
    if (x == 0) {
        s = "0";
    }
    while (x != 0) {
        std::string t = std::to_string(x.div_mod_on_short(1000000000));
        std::reverse(t.begin(), t.end());
        while (x != 0 && t.size() < 9) {
            t = t + '0';
        }
        s = s + t;
    }
    if (a.negate) {
        s = s + "-";
    }
    std::reverse(s.begin(), s.end());
    return s;
}

std::ostream &operator<<(std::ostream &s, big_integer const &a) {
    return s << to_string(a);
}
