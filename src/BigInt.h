//
// Created by sunshine on 2019/12/10.
//

#ifndef PYTHON_INTERPRETER_DATATYPE_H
#define PYTHON_INTERPRETER_DATATYPE_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

class BigInt {
    friend ostream &operator<<(ostream & os, const BigInt &a);
    friend istream &operator>>(istream &is, BigInt &a);
    friend BigInt operator+(const BigInt &a, const BigInt &b);
    friend BigInt operator-(const BigInt &a, const BigInt &b);
    friend BigInt operator*(const BigInt &a, const BigInt &b);
    friend BigInt operator/(const BigInt &a, const BigInt &b);
    friend BigInt operator%(const BigInt &a, const BigInt &b);

    friend BigInt operator-(const BigInt &a);

    friend bool operator>(const BigInt &a, const BigInt &b);
    friend bool operator<(const BigInt &a, const BigInt &b);
    friend bool operator>=(const BigInt &a, const BigInt &b);
    friend bool operator<=(const BigInt &a, const BigInt &b);
    friend bool operator==(const BigInt &a, const BigInt &b);
    friend bool operator!=(const BigInt &a, const BigInt &b);
public:
    BigInt();
    BigInt(const string &a);
    BigInt(long long a);
    BigInt(const BigInt &a);
    BigInt &operator=(const BigInt &a);

    BigInt &operator+=(const BigInt &b);
    BigInt &operator-=(const BigInt &b);
    BigInt &operator*=(const BigInt &b);
    BigInt &operator/=(const BigInt &b);
    BigInt &operator%=(const BigInt &b);

    BigInt &operator++();
    BigInt &operator--();
    BigInt operator++(int b);
    BigInt operator--(int b);

    vector<int> val;
    bool sgn;
    string ToString() const;
    bool ToBool() const;
    double ToDouble() const;
};

#endif //PYTHON_INTERPRETER_DATATYPE_H
