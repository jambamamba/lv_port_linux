#pragma once

#include <stdlib.h>

template<class Type> class AutoFreePtr
{
public:
    ~AutoFreePtr() {
        if(_ptr) {
            free(_ptr);
            _sz = 0;
            _ptr = nullptr;
        }
    };
    static AutoFreePtr create(size_t sz){
        AutoFreePtr ret;
        ret._sz = sizeof(Type) + sz;
        ret._ptr = (Type*) malloc(ret._sz);
        return ret;
    }
    static AutoFreePtr create(const Type &&obj, size_t sz){
        AutoFreePtr ret;
        ret._sz = sizeof(Type) + sz;
        ret._ptr = (Type*) malloc(ret._sz);
        memcpy(ret._ptr, &obj, sizeof(Type));
        return ret;
    }
    Type *get() const {
        return _ptr;
    }
    Type *operator->() const {
        return _ptr;
    }
    Type *release() {
        Type *ret = _ptr;
        _ptr = nullptr;
        _sz = 0;
        return ret;
    }
    size_t size() const {
        return _sz;
    }

private:
    AutoFreePtr() {}

    size_t _sz = 0;
    Type *_ptr = nullptr;
};

