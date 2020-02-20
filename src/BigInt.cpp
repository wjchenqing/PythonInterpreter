#include "BigInt.h"


BigInt::BigInt() {
    val.push_back(0);
    sgn = false;
}
BigInt::BigInt(long long a) {
    sgn = a > 0;
    if (!sgn) a *= -1;
    while (a) {
        val.push_back(a % 10);
        a /= 10;
    }
    if (val.empty()) val.push_back(0);
}
BigInt::BigInt(const string &a) {
    sgn = a[0] != '-';
    for(auto tmp = a.rbegin(); tmp < a.rend() - 1; ++tmp)
        val.push_back(*tmp - '0');
    if (sgn) val.push_back(a[0]-'0');
    while (!val.empty() && val.back() == 0) val.pop_back();
    if (val.empty()) {
        val.push_back(0); sgn = 0;
    }
}
BigInt::BigInt(const BigInt &a) {
    val = a.val;
    sgn = a.sgn;
}
BigInt &BigInt::operator=(const BigInt &a) {
    val = a.val;
    sgn = a.sgn;
    return *this;
}

BigInt operator-(const BigInt &a) {
    BigInt tmp = a;
    tmp.sgn = !a.sgn;
    return tmp;
}
BigInt operator+(const BigInt &a, const BigInt &b) {
    if (a.sgn != b.sgn) return a - (-b);
    int min_len = min(a.val.size(), b.val.size());
    int tmp = 0;
    BigInt ans;
    ans.sgn = a.sgn;
    unsigned long max_size = max(a.val.size(), b.val.size());
    ans.val.resize(max_size + 10);
    for (int i = 0; i < min_len; ++i) {
        tmp += a.val[i] + b.val[i];
        ans.val[i] = tmp % 10;
        tmp /= 10;
    }
    if (a.val.size() > b.val.size())
        for (int i = min_len; i < a.val.size(); ++i) {
            tmp += a.val[i];
            ans.val[i] = tmp % 10;
            tmp /= 10;
        }
    if (a.val.size() < b.val.size())
        for (int i = min_len; i < b.val.size(); ++i) {
            tmp += b.val[i];
            ans.val[i] = tmp % 10;
            tmp /= 10;
        }
    if (tmp == 1)  ans.val[max_size] = 1;

    while (!ans.val.empty() && ans.val.back() == 0) ans.val.pop_back();
    if (ans.val.empty()) ans.val.push_back(0);
    //cout << ans.val[0] << endl;// test
    return ans;
}
BigInt operator-(const BigInt &a, const BigInt &b) {
    if (a.sgn != b.sgn) return a + (-b);
    if (a.sgn == 0) return -((-a) - (-b));

    BigInt ans;
    unsigned long max_size = max(a.val.size(), b.val.size());
    ans.val.resize(max_size);
    int tmp = 0;
    for (unsigned long i = 0; i < max_size; ++i) {
        int _a = i < a.val.size() ? a.val[i] : 0;
        int _b = i < b.val.size() ? b.val[i] : 0;
        int x = _a - _b - tmp;
        if (x < 0) {
            x += 10;
            tmp = 1;
        } else
            tmp = 0;
        ans.val[i] = x;
    }
    if (tmp == 0) {
        ans.sgn = true;
        return ans;
    } else {
//        ans.sgn = false;
        BigInt A;
        A.val.resize(max_size + 1);
        A.val[max_size] = 1;
        A.sgn = true;
        ans.sgn = true;
        ans = -(A - ans);
        while (!ans.val.empty() && ans.val.back() == 0) ans.val.pop_back();
        if (ans.val.empty()) ans.val.push_back(0);
        return ans;
    }

//    unsigned long min_len = min (a.val.size(), b.val.size());
//    int tmp = 0;
//    BigInt ans;
//    ans.val.resize(a.val.size() + 10);
//    for (unsigned long i = 0; i < min_len; ++i) {
//        tmp += a.val[i] - b.val[i] + 10;
//        ans.val[i] = tmp % 10;
//        tmp = tmp / 10 - 1;
//    }
//    if (a.val.size() > b.val.size())
//        for (unsigned long i = min_len; i < a.val.size(); ++i) {
//            tmp += a.val[i] + 10;
//            ans.val[i] = tmp % 10;
//            tmp = tmp / 10 - 1;
//        }
//    if (a.val.size() < b.val.size())
//        for (unsigned long i = min_len; i < a.val.size(); ++i) {
//            tmp += - b.val[i] + 10;
//            ans.val[i] = tmp % 10;
//            tmp = tmp / 10 - 1;
//        }
//    ans.sgn = tmp != -1;
//    while (!ans.val.empty() && ans.val.back() == 0) ans.val.pop_back();
//    if (ans.val.empty()) ans.val.push_back(0);
//    return ans;
}
bool operator>(const BigInt &a, const BigInt &b) {
    return (a-b).sgn;
}
bool operator<(const BigInt &a, const BigInt &b) {
    BigInt tmp = b - a;
    return (tmp.sgn && (tmp.val.size()!=1 || tmp.val[0]!=0));
}
bool operator>=(const BigInt &a, const BigInt &b) {
    return !(a<b);
}
bool operator<=(const BigInt &a, const BigInt &b) {
    return !(a>b);
}
bool operator==(const BigInt &a, const BigInt &b) {
    return !(a > b) && !(a < b);
}
bool operator!=(const BigInt &a, const BigInt &b) {
    return a > b || a < b;
}
BigInt& BigInt::operator+=(const BigInt &b) {
    *this = *this + b;
    return *this;
}
BigInt& BigInt::operator-=(const BigInt &b) {
    *this = *this - b;
    return *this;
}
BigInt& BigInt::operator++() {
    *this = *this + 1;
    return *this;
}
BigInt& BigInt::operator--() {
    *this = *this - 1;
    return *this;
}
BigInt BigInt::operator++(int b) {
    BigInt tmp = *this;
    *this = *this + 1;
    return tmp;
}
BigInt BigInt::operator--(int b) {
    BigInt tmp = *this;
    *this = *this - 1;
    return tmp;
}

BigInt operator*(const BigInt &a, const BigInt &b) {
    BigInt ans;
    if (a == 0 || b == 0) return 0;
    ans.sgn = a.sgn == b.sgn;
    int total = a.val.size() + b.val.size();
    ans.val.resize(total + 10);
    for (int i = 0; i < a.val.size(); ++i)
        for (int j = 0; j < b.val.size(); ++j) {
            ans.val[i + j] += a.val[i] * b.val[j];
            ans.val[i + j + 1] += ans.val[i + j] / 10;
            ans.val[i + j] %= 10;
        }
    /*for (int i = 0; i < total; ++i) {
        ans.val[i + 1] += ans.val[i] / 10;
        ans.val[i] %= 10;
    }*/
    while (!ans.val.empty() && ans.val.back() == 0) ans.val.pop_back();
    if (ans.val.empty()) ans.val.push_back(0);
    return ans;
}
BigInt& BigInt::operator*=(const BigInt &b) {
    *this = *this * b;
    return *this;
}

bool larger(BigInt& a, const BigInt& b, unsigned long dif) {
    int s_a = dif + b.val.size(), s_b = b.val.size();
    if (a.val[s_a] != 0)
        a.val[s_a - 1] += a.val[s_a] * 10;
    for (int i = 1; i <= s_b; ++i) {
        if (a.val[s_a - i] > b.val[s_b - i]) return 1;
        if (a.val[s_a - i] < b.val[s_b - i]) return 0;
    }
    return 1;
}
BigInt operator/(const BigInt &a, const BigInt &b) {
    BigInt cur = a;
    long long a_size = a.val.size(), b_size = b.val.size();
    long long dif = a_size - b_size;
    cur.val.push_back(0);
    if (a == 0) return 0;
    if (dif < 0)
        return a.sgn == b.sgn ? 0 : -1;
    BigInt ans;
    ans.sgn = a.sgn == b.sgn;
    ans.val.resize(dif + 10);
    for (; dif >= 0; --dif) {
        int t = 0;
        while (larger(cur, b, dif)) {
            ++t;
            int tmp = 0;
            int i = 0;
            for (; i < b.val.size(); ++i) {
                tmp += cur.val[dif + i] - b.val[i] + 10;
                cur.val[dif + i] = tmp % 10;
                tmp = tmp / 10 - 1;
            }
            cur.val[dif + i] = tmp;
        }
        ans.val[dif] = t;
    }
    while (!ans.val.empty() && ans.val.back() == 0) ans.val.pop_back();
    if (ans.val.empty()) ans = 0;
    while (!cur.val.empty() && cur.val.back() == 0) cur.val.pop_back();
    if (cur.val.empty()) cur = 0;
    if (a.sgn != b.sgn && cur != 0)
        return ans - 1;
    else
        return ans;
}
BigInt& BigInt::operator/=(const BigInt &b) {
    *this = *this / b;
    return *this;
}
BigInt operator%(const BigInt &a, const BigInt &b) {
    return a - (a / b) * b;
}
BigInt& BigInt::operator%=(const BigInt &b) {
    *this = *this % b;
    return *this;
}

string BigInt::ToString() const {
    std::ostringstream st;
    if (!sgn && !(val.size() == 1 && val[0] == 0)) st << char('-');
    for (int i = val.size() - 1; i >= 0; --i) st << char(val[i] + '0');
    return st.str();
}
ostream &operator<<(ostream &os, const BigInt &a) {
    os << a.ToString();
    return os;
}
istream &operator>>(istream &is, BigInt &a) {
    string tmp;
    is >> tmp;
    a = tmp;
    return is;
}
bool BigInt::ToBool() const {
    return !(*this == 0);
}
double BigInt::ToDouble() const {
    double tmp = 0;
    double e = 1;
    for (int i : this->val) {
        tmp += (i * e);
        e *= 10;
    }
    if (!sgn) tmp *= -1;
    return tmp;
}

BigInt::~BigInt() {
}
