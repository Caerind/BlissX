#include "Value.hpp"

#define ALLOCATE_OBJ(type, objType) \
    (type*)Obj::allocateObj(sizeof(type), objType)

Obj* Obj::allocateObj(std::size_t size, Obj::Type type)
{
    Obj* object = (Obj*)Memory::reallocate(NULL, 0, size);
    object->type = type;
    return object;
}

ObjString* ObjString::copyString(const char* chars, int length)
{
    char* heapChars = MEMORY_ALLOCATE(char, length + 1);
    memcpy(heapChars, chars, length);
    heapChars[length] = '\0';
    return allocateString(heapChars, length);
}

ObjString* ObjString::takeString(char* chars, int length)
{
    return allocateString(chars, length);
}

ObjString* ObjString::allocateString(char* chars, int length)
{
    ObjString* string = ALLOCATE_OBJ(ObjString, Obj::Type::String);
    string->length = length;
    string->chars = chars;
    return string;
}

Value::Value()
    : mType(Value::Type::Null)
{
    mAs.number = 0;
}

Value::Value(bool value)
    : mType(Value::Type::Bool)
{
    mAs.boolean = value;
}

Value::Value(double value)
    : mType(Value::Type::Number)
{
    mAs.number = value;
}

Value::Value(Obj* object)
    : mType(Value::Type::Object)
{
    mAs.object = object;
}

Value::Type Value::getType() const
{
    return mType;
}

Obj::Type Value::getObjectType() const
{
    return mAs.object->type;
}

bool Value::isNull() const
{
    return mType == Value::Type::Null;
}

bool Value::isBool() const
{
    return mType == Value::Type::Bool;
}

bool Value::isNumber() const
{
    return mType == Value::Type::Number;
}

bool Value::isObject() const
{
    return mType == Value::Type::Object;
}

bool Value::isString() const
{
    return isObject() && asObject()->type == Obj::Type::String;
}

bool Value::isFalsey() const
{
    return isNull() || (isBool() && !asBool());
}

bool Value::isEquals(const Value& value) const
{
    if (mType != value.mType) return false;
    switch (mType)
    {
        case Value::Type::Bool: return asBool() == value.asBool();
        case Value::Type::Null: return true;
        case Value::Type::Number: return asNumber() == value.asNumber();
        case Value::Type::Object:
        {
            ObjString* aString = asString();
            ObjString* bString = value.asString();
            return aString->length == bString->length && memcmp(aString->chars, bString->chars, aString->length) == 0;
        }
    }
    return false;
}

bool Value::asBool() const
{
    return mAs.boolean;
}

double Value::asNumber() const
{
    return mAs.number;
}

Obj* Value::asObject() const
{
    return mAs.object;
}

ObjString* Value::asString() const
{
    return (ObjString*)mAs.object;
}

char* Value::asCString() const
{
    return ((ObjString*)mAs.object)->chars;
}

ValueArray::ValueArray()
    : mCount(0)
    , mCapacity(0)
    , mValues(nullptr)
{
}

ValueArray::~ValueArray()
{
    clear();
}

void ValueArray::clear()
{
    MEMORY_FREE_ARRAY(Value, mValues, mCapacity);
    mCount = 0;
    mCapacity = 0;
    mValues = nullptr;
}

void ValueArray::push(Value value)
{
    if (mCapacity < mCount + 1)
    {
        reserve(MEMORY_GROW_CAPACITY(mCapacity));
    }

    mValues[mCount] = value;
    mCount++;
}

void ValueArray::reserve(std::size_t size)
{
    if (mCapacity < size)
    {
        mValues = MEMORY_GROW_ARRAY(mValues, Value, mCapacity, size);
        mCapacity = size;
    }
}

std::size_t ValueArray::size() const
{
    return mCount;
}

std::size_t ValueArray::capacity() const
{
    return mCapacity;
}

Value& ValueArray::operator[](std::size_t index)
{
    // TODO : Throw exception ?
    return mValues[index];
}

const Value& ValueArray::operator[](std::size_t index) const
{
    // TODO : Throw exception ?
    return mValues[index];
}
