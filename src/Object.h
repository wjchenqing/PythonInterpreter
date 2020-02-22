//
// Created by sunshine on 2020/1/3.
//

#ifndef PYTHON_INTERPRETER_OBJECT_H
#define PYTHON_INTERPRETER_OBJECT_H

#include <utility>
#include <vector>
#include <string>
#include "BigInt.h"
#include <iostream>
#include <iomanip>

class Object;
typedef vector<Object> List;
enum type_st {
    NONE,INT,FLOAT,BOOL,STRING,LIST
};
enum flow_st {
    ATOMS,BREAK,CONTINUE,RETURN
};

class Object{

    friend std::ostream &operator<< (const std::ostream &os,const Object &b);

public:
    BigInt int_py;
    double float_py;
    bool bool_py;
    std::string string_py;
    List list_py;
//
    type_st type_py;
    flow_st flow_py;
//
    Object(): type_py(NONE), flow_py(ATOMS) {};
    Object(type_st tmp): type_py(tmp), flow_py(ATOMS) {};
    Object(BigInt tmp): type_py(INT), flow_py(ATOMS), int_py(tmp) {};
    Object(double tmp): type_py(FLOAT), flow_py(ATOMS), float_py(tmp) {};
    Object(bool tmp): type_py(BOOL), flow_py(ATOMS), bool_py(tmp) {};
    Object(string tmp): type_py(STRING), flow_py(ATOMS), string_py(tmp) {};
    Object(List tmp): type_py(LIST), flow_py(ATOMS), list_py(tmp) {};
    Object(flow_st tmp): type_py(NONE), flow_py(tmp) {};
//
    BigInt toINT() const {
        switch (type_py){
            case INT: return int_py;
            case FLOAT: return BigInt((long long) float_py);
            case BOOL: return BigInt((long long) bool_py);
            case STRING: return BigInt(string_py);
            default: return BigInt();
        }
    }
    double toFLOAT() const {
        switch(type_py){
            case INT: return int_py.ToDouble();
            case FLOAT: return float_py;
            case BOOL: return (double) bool_py;
            case STRING: return std::stod(string_py);
            default: return (double) 0;
        }
    }
    bool toBOOL() const {
        switch(type_py){
            case INT: return (int_py != 0);
            case FLOAT: return (float_py != 0);
            case BOOL: return bool_py;
            case STRING: return (string_py == std::string ("True"));
            default: return false;
        }
    }
    std::string toSTRING() const {
        switch(type_py){
            case NONE: return std::string ("None");
            case INT: return int_py.ToString();
            case FLOAT: return std::to_string(float_py);
            case BOOL: return std::string(bool_py ? "True" : "False");
            case STRING: return string_py;
            default: return std::string("");
        }
    }

//
    Object operator!() const {
        return Object(!toBOOL());
    }
    Object operator&&(const Object &b) const {
        return Object(toBOOL() && b.toBOOL());
    }
    Object operator||(const Object &b) const {
        return Object(toBOOL() || b.toBOOL());
    }
    Object operator>(const Object &b) const {
        if(type_py == STRING && b.type_py == STRING)
        {
            return Object(toSTRING() > b.toSTRING());
        }
        if(type_py != STRING && b.type_py != STRING)
        {
            double _a = (*this).toFLOAT();
            double _b = b.toFLOAT();
            return Object(_a > _b);
        }
        return Object(type_py == STRING);
    }
    Object operator<(const Object &b) const {
        return b > *this;
    }
    Object operator>=(const Object &b) const {
        return !(*this < b);
    }
    Object operator<=(const Object &b) const {
        return !(*this > b);
    }
    Object operator==(const Object &b) const {
        return (*this >= b) && (*this <= b);
    }
    Object operator!=(const Object &b) const {
        return !(*this == b);
    }
    Object operator+(const Object &b) const {
        switch (type_py) {
            case INT:
                if (b.type_py == INT)
                    return Object(toINT() + b.toINT());
                else
                    return Object(toFLOAT() + b.toFLOAT());
            case FLOAT:
                return Object(toFLOAT() + b.toFLOAT());
            case STRING:
                return Object(toSTRING() + b.toSTRING());
            default:;
        }
    }
    Object operator-(const Object &b) const {

        if (type_py == INT) {
            switch  (b.type_py){
                case INT:
                    return Object(toINT() - b.toINT());
                case FLOAT:
                    return Object(toFLOAT() - b.toFLOAT());
            }
        }
        if (type_py == FLOAT)
            return Object(toFLOAT() - b.toFLOAT());
    }
    Object operator-() const {
        if (type_py == INT)
            return Object(BigInt(0)) - *this;
        if (type_py == FLOAT)
            return Object((double)0) - *this;
    }
    Object operator*(const Object &b) const {
        switch (type_py) {
            case INT:
                if (b.type_py == INT)
                    return Object(toINT() * b.toINT());
                else if (b.type_py == FLOAT)
                    return Object(toFLOAT() * b.toFLOAT());
                break;
            case FLOAT:
                return Object(toFLOAT() * b.toFLOAT());
            default:;
        }
        if (type_py == STRING && (b.type_py == INT || b.type_py == BOOL)) {
            Object tmp(*this);
            auto num = b.toINT();
            for (BigInt i = 1; i < num; ++i)
                tmp += *this;
            return tmp;
        }
        if ((type_py == INT || type_py == BOOL) && b.type_py == STRING)
            return b * *this;
    }
    Object operator/(const Object &b) const {
        if (type_py == INT || type_py == FLOAT)
            return Object(toFLOAT() / b.toFLOAT());
    }
    Object intdiv(const Object &b) const {
        if (type_py == INT && b.type_py == INT)
            return toINT() / b.toINT();
        if (type_py == FLOAT && b.type_py == FLOAT)
            return BigInt((long long)(toFLOAT() / b.toFLOAT()));
    }
    Object intdivAugAssign(const Object &b) {
        if (type_py == INT && b.type_py == INT)
            return *this = toINT() / b.toINT();
        if (type_py == FLOAT && b.type_py == FLOAT)
            return *this = BigInt((long long)(toFLOAT() / b.toFLOAT()));
    }
    Object operator% (const Object &b) const {
        if (type_py == INT && b.type_py == INT)
            return toINT() % b.toINT();
    }
    Object operator+= (const Object &b) {
        *this = *this + b;
        return *this;
    }
    Object operator-= (const Object &b) {
        return *this = (*this - b);
    }
    Object operator*= (const Object &b) {
        return *this = (*this * b);
    }
    Object operator/= (const Object &b) {
        return *this = (*this / b);
    }
    Object operator%= (const Object &b) {
        return *this = (*this % b);
    }
};


#endif //PYTHON_INTERPRETER_OBJECT_H
