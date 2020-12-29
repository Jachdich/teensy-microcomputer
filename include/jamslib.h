#ifndef __JAMSLIB_H
#define __JAMSLIB_H
#include <malloc.h>
typedef unsigned int uint32_t;
enum class ErrorCode {
    OK,
    NOT_FOUND,
};

template<class T>
class Vector {
public:
    T * cont;
    uint32_t capacity;
    uint32_t size;
    Vector(uint32_t n);

    Vector();

    ~Vector();
    
    void push_back(T value);

    T& operator[](uint32_t pos);
};

template<class A, class B> 
class Pair {
public:
    A a;
    B b;
    Pair(A a, B b);
};

template<class K, class V>
class Map {
public:
    Vector<Pair<K, V>> cont;
    Map();
    
    void put(K key, V value);
    
    V get(K key, ErrorCode& code);

    void extend(Map<K, V>& other);
};

uint32_t strlen(const char * data);

void strcpy(const char * a, char * b);

void memcpy(void * from, void * to, uint32_t size);

class String {
public:
    char * cont;
    uint32_t size;
    String();
    String(const char * data);
    String(const String &str);

    bool operator==(const String& other);
    
    ~String();

    String operator+(const String& other);

    String operator+(const char * other);
};

String operator+(const char * a, String& b);

void CALError(String message);
#endif
