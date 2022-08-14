#pragma once

#include <iosfwd>
#include <string>
#include <vector>
#include <ostream>
#include <functional>

struct big_integer {
    big_integer();
    big_integer(big_integer const& other);
    big_integer(int a);
    big_integer(unsigned int a);
    big_integer(long a);
    big_integer(unsigned long a);
    big_integer(long long a);
    big_integer(unsigned long long a);
    explicit big_integer(std::string const& str);
    ~big_integer();

    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

    //return abs of number, no exepction
    big_integer abs() const;
    big_integer& abs_no_copy();
    big_integer& negative();

private:
    uint32_t get(size_t x) const;
    //this divide by short x and return mod
    //divide only on positive short x
    uint64_t div_mod_on_short(uint32_t x);
    //clear zero on end of bigint
    big_integer& erase0();
    //this multiply short x return copy
    big_integer mul(uint32_t x) const;

    big_integer& mul_not_copy(uint32_t x);

    big_integer& mul(uint32_t x, big_integer &y);

    big_integer& bitoper(const std::function<uint32_t(uint32_t, uint32_t)>& op, big_integer const& b);

    bool negate = false;
    //addition to 2
    //the older bit in the end of vector
    //negative number ends on infinity 1
    //positive number ends on infinity 0
    std::vector <uint32_t> num;
};

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);

big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

std::string to_string(big_integer const& a);
std::ostream& operator<<(std::ostream& s, big_integer const& a);
