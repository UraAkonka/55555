#ifndef PATTERNTEMPLATES_H
#define PATTERNTEMPLATES_H

#include <iostream>
#include <list>
#include <vector>

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

// Интерфейс общего контейнера
template<typename T>
class IComponentContainer {
public:
    virtual ~IComponentContainer() {}
    virtual void Add(T* item) = 0;
    virtual Iterator<T>* GetIterator() const = 0;
};

// Компонент-массив
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

// Компонент-список
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
        void First() override {} // begin уже установлен
        void Next() override { ++current; }
        bool IsDone() const override { return current == end; }
        T* GetCurrent() const override { return *current; }
    };

    Iterator<T>* GetIterator() const override {
        return new ListIterator(data);
    }
};

// Универсальный обработчик
template<typename T>
void ProcessComponents(Iterator<T>* it) {
    double total = 0;
    cout << "Список компонентов:" << endl;
    for (it->First(); !it->IsDone(); it->Next()) {
        T* comp = it->GetCurrent();
        comp->ShowInfo();
        total += comp->GetPrice();
    }
    cout << "Общая стоимость: $" << total << endl << endl;
}

// ======= ДЕКОРАТОРЫ =======

// Декоратор: логирование
template<typename T>
class LoggingIteratorDecorator : public Iterator<T> {
    Iterator<T>* inner;
public:
    LoggingIteratorDecorator(Iterator<T>* it) : inner(it) {}
    void First() override { inner->First(); }
    void Next() override { inner->Next(); }
    bool IsDone() const override { return inner->IsDone(); }
    T* GetCurrent() const override {
        T* current = inner->GetCurrent();
        cout << "[LOG] Доступ к элементу: ";
        current->ShowInfo();
        return current;
    }
    ~LoggingIteratorDecorator() { delete inner; }
};

// Декоратор: фильтр по цене
template<typename T>
class PriceFilterIteratorDecorator : public Iterator<T> {
    Iterator<T>* inner;
    double minPrice;
public:
    PriceFilterIteratorDecorator(Iterator<T>* it, double min) : inner(it), minPrice(min) {
        SkipInvalid();
    }
    void First() override {
        inner->First();
        SkipInvalid();
    }
    void Next() override {
        inner->Next();
        SkipInvalid();
    }
    bool IsDone() const override { return inner->IsDone(); }
    T* GetCurrent() const override { return inner->GetCurrent(); }

    void SkipInvalid() {
        while (!inner->IsDone() && inner->GetCurrent()->GetPrice() < minPrice) {
            inner->Next();
        }
    }

    ~PriceFilterIteratorDecorator() { delete inner; }
};

// Декоратор: подсчёт
template<typename T>
class CountingIteratorDecorator : public Iterator<T> {
    Iterator<T>* inner;
    int count;
public:
    CountingIteratorDecorator(Iterator<T>* it) : inner(it), count(0) {}
    void First() override { inner->First(); count = 0; }
    void Next() override { inner->Next(); ++count; }
    bool IsDone() const override { return inner->IsDone(); }
    T* GetCurrent() const override { return inner->GetCurrent(); }
    int GetCount() const { return count; }
    ~CountingIteratorDecorator() { delete inner; }
};

//STL

template<typename T>
class STLIteratorAdapter {
    Iterator<T>* iter;
    bool done;
public:
    STLIteratorAdapter(Iterator<T>* it) : iter(it), done(false) {
        iter->First();
        done = iter->IsDone();
    }

    T* operator*() const { return iter->GetCurrent(); }
    STLIteratorAdapter& operator++() {
        iter->Next();
        done = iter->IsDone();
        return *this;
    }

    bool operator!=(const STLIteratorAdapter& other) const {
        return done != other.done;
    }

    static STLIteratorAdapter<T> begin(Iterator<T>* it) {
        return STLIteratorAdapter<T>(it);
    }

    static STLIteratorAdapter<T> end() {
        STLIteratorAdapter<T> it(nullptr);
        it.done = true;
        return it;
    }
};

#endif
