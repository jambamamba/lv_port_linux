#pragma once

#include <atomic>
#include <stdlib.h>

template<class Type> class AutoFreePtr
{
public:
    AutoFreePtr() {}
    ~AutoFreePtr() {
        if(_ptr) {
            _ptr->~Type();
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
    size_t _sz = 0;
    Type *_ptr = nullptr;
};


template<class Type> class AutoFreeSharedPtr
{
public:
    AutoFreeSharedPtr() {}
    ~AutoFreeSharedPtr() {
        _rc->_ref_cnt--;
        // int rc = _rc->_ref_cnt;
        // printf("autosptr dtor, %i\n", rc);
        if(_rc->_ref_cnt > 0) {
            return;
        }
        if(_ptr) {
            _ptr->~Type();
            free(_ptr);
            _sz = 0;
            _ptr = nullptr;
        }
        delete _rc;
        _rc = nullptr;
        // printf("autosptr delete\n");
    };
    static AutoFreeSharedPtr create(size_t sz){
        AutoFreeSharedPtr ret;
        ret._rc = new RC;
        ret._sz = sizeof(Type) + sz;
        ret._ptr = (Type*) malloc(ret._sz);
        return ret;
    }
    static AutoFreeSharedPtr create(const Type &&obj, size_t sz){
        AutoFreeSharedPtr ret;
        ret._rc = new RC;
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
        _rc->_ref_cnt = 1;
        return ret;
    }
    size_t size() const {
        return _sz;
    }

    AutoFreeSharedPtr(const AutoFreeSharedPtr& rhs) {
        if(_ptr == rhs._ptr) {
            return;
        }
        _rc = rhs._rc;
        _sz = rhs._sz;
        _ptr = rhs._ptr;
        _rc->_ref_cnt++;
        // int rc = _rc->_ref_cnt;
        // printf("autosptr cctor %i\n", rc);   
    }
    const AutoFreeSharedPtr& operator=(const AutoFreeSharedPtr& rhs) {
        if(_ptr == rhs._ptr) {
            return rhs;
        }
        _rc->_ref_cnt--;
        if(_rc->_ref_cnt == 0) {
            if(_ptr) {
                _ptr->~Type();
                free(_ptr);
                _sz = 0;
                _ptr = nullptr;
            }
            delete _rc;
            _rc = nullptr;
        }
        _rc = rhs._rc;
        _sz = rhs._sz;
        _ptr = rhs._ptr;
        _rc->_ref_cnt++;
        // int rc = _rc->_ref_cnt;
        // printf("autosptr =, %i\n", rc);
        return *this;
    }

private:
    struct RC {
        std::atomic<size_t> _ref_cnt = 1;
    };
    RC *_rc = nullptr;
    size_t _sz = 0;
    Type *_ptr = nullptr;
};


