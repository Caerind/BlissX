#ifndef VALUE_HPP
#define VALUE_HPP

#include "Memory.hpp"

struct Obj
{
    enum Type
    {
        String,
    };

    Type type;

    static Obj* allocateObj(std::size_t size, Obj::Type type);
};

struct ObjString
{
    Obj obj;
    int length;
    char* chars;

    static ObjString* copyString(const char* chars, int length);
    static ObjString* takeString(char* chars, int length);
    static ObjString* allocateString(char* chars, int length);
};

class Value
{
    public:
        enum Type
        {
            Null,
            Bool,
            Number,
            Object
        };

        Value();
        Value(bool value);
        Value(double value);
        Value(Obj* object);

        Type getType() const;
        Obj::Type getObjectType() const;

        bool isNull() const;
        bool isBool() const;
        bool isNumber() const;
        bool isObject() const;
        bool isString() const;

        bool isFalsey() const;
        bool isEquals(const Value& value) const;

        bool asBool() const;
        double asNumber() const;
        Obj* asObject() const;
        ObjString* asString() const;
        char* asCString() const;

    private:
        Type mType;
        union As
        {
            bool boolean;
            double number;
            Obj* object;
        } mAs;
};

class ValueArray
{
    public:
        ValueArray();
        ~ValueArray();

        void clear();

        void push(Value value);
        void reserve(std::size_t size);

        std::size_t size() const;
        std::size_t capacity() const;

        Value& operator[](std::size_t index);
        const Value& operator[](std::size_t index) const;

    private:
        std::size_t mCount;
        std::size_t mCapacity;
        Value* mValues;
};

#endif // VALUE_HPP
