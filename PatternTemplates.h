#ifndef PATTERNTEMPLATES_H
#define PATTERNTEMPLATES_H

#include <iostream>
#include <list>
#include <vector>
#include <string>

using namespace std;

template<typename T>
class Iterator {
public:
    virtual ~Iterator() {}
    virtual void First() = 0;
    virtual void Next() = 0;
    virtual bool IsDone() const = 0;
    virtual T* GetCurrent() const = 0;
};

template<typename T>
class IComponentContainer {
public:
    virtual ~IComponentContainer() {}
    virtual void Add(T* item) = 0;
    virtual Iterator<T>* GetIterator() const = 0;
};

template<typename T>
class ComponentArray : public IComponentContainer<T> {
    T* data[100];
    size_t size;
public:
    ComponentArray() : size(0) {}
    ~ComponentArray() {
        for (size_t i = 0; i < size; ++i) delete data[i];
    }

    void Add(T* item) override { data[size++] = item; }
    T* Get(size_t index) const { return data[index]; }
    size_t Size() const { return size; }

    class ArrayIterator : public Iterator<T> {
        const ComponentArray<T>* array;
        size_t current;
    public:
        ArrayIterator(const ComponentArray<T>* arr) : array(arr), current(0) {}
        void First() override { current = 0; }
        void Next() override { current++; }
        bool IsDone() const override { return current >= array->Size(); }
        T* GetCurrent() const override { return array->Get(current); }
    };

    Iterator<T>* GetIterator() const override {
        return new ArrayIterator(this);
    }
};

template<typename T>
class ComponentList : public IComponentContainer<T> {
    list<T*> data;
public:
    ~ComponentList() {
        for (T* item : data) delete item;
    }

    void Add(T* item) override { data.push_back(item); }

    class ListIterator : public Iterator<T> {
        typename list<T*>::const_iterator current, end;
    public:
        ListIterator(const list<T*>& d) : current(d.begin()), end(d.end()) {}
        void First() override {}
        void Next() override { ++current; }
        bool IsDone() const override { return current == end; }
        T* GetCurrent() const override { return *current; }
    };

    Iterator<T>* GetIterator() const override {
        return new ListIterator(data);
    }
};

#endif
