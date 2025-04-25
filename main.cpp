#include <iostream>
#include <cstdlib>
#include <ctime>
#include <list>
#include <vector>
#include <string>
#include "PatternTemplates.h"

using namespace std;

// ===== Стратегия =====
class ITestStrategy {
public:
    virtual ~ITestStrategy() {}
    virtual void Run() const = 0;
};

class QuickTestStrategy : public ITestStrategy {
public:
    void Run() const override { cout << "Выполняется быстрая диагностика...\n"; }
};

class PerformanceTestStrategy : public ITestStrategy {
public:
    void Run() const override { cout << "Выполняется тест производительности...\n"; }
};

class StabilityTestStrategy : public ITestStrategy {
public:
    void Run() const override { cout << "Выполняется тест стабильности...\n"; }
};

// ===== Шаблонный метод =====
class PCComponent {
protected:
    string manufacturer;
    double price;
    ITestStrategy* strategy;
public:
    PCComponent(const string& m, double p) : manufacturer(m), price(p), strategy(nullptr) {}
    virtual ~PCComponent() { delete strategy; }

    void SetTestStrategy(ITestStrategy* s) {
        if (strategy) delete strategy;
        strategy = s;
    }

    double GetPrice() const { return price; }

    virtual void ShowInfo() const = 0;
    virtual void CheckHealth() const = 0;

    void Process() const {
        ShowInfo();
        CheckHealth();
        if (strategy) strategy->Run();
        cout << endl;
    }
};

class CPU : public PCComponent {
    int cores;
    double frequency;
public:
    CPU(const string& m, double p, int c, double f)
        : PCComponent(m, p), cores(c), frequency(f) {}

    void ShowInfo() const override {
        cout << "Процессор: " << manufacturer << ", " << cores << " ядер, "
             << frequency << " ГГц, " << price << " руб.\n";
    }

    void CheckHealth() const override {
        cout << "Проверка температуры и напряжения процессора...\n";
    }
};

class GPU : public PCComponent {
    int memory;
    string memoryType;
public:
    GPU(const string& m, double p, int mem, const string& memType)
        : PCComponent(m, p), memory(mem), memoryType(memType) {}

    void ShowInfo() const override {
        cout << "Видеокарта: " << manufacturer << ", " << memory << " ГБ "
             << memoryType << ", " << price << " руб.\n";
    }

    void CheckHealth() const override {
        cout << "Проверка вентилятора и питания видеокарты...\n";
    }
};

class RAM : public PCComponent {
    int capacity;
    string type;
public:
    RAM(const string& m, double p, int cap, const string& t)
        : PCComponent(m, p), capacity(cap), type(t) {}

    void ShowInfo() const override {
        cout << "Оперативная память: " << manufacturer << ", " << capacity << " ГБ "
             << type << ", " << price << " руб.\n";
    }

    void CheckHealth() const override {
        cout << "Проверка памяти с помощью паттерн-теста...\n";
    }
};

enum class ComponentType { CPU, GPU, RAM };

PCComponent* CreateComponent(ComponentType type) {
    switch(type) {
        case ComponentType::CPU:
            return new CPU("Intel", 20000 + rand()%10000, 4 + rand()%4, 2.5 + rand()%3);
        case ComponentType::GPU:
            return new GPU("NVIDIA", 40000 + rand()%20000, 6 + rand()%6, "GDDR6");
        case ComponentType::RAM:
            return new RAM("Kingston", 5000 + rand()%5000, 8 + rand()%8, "DDR4");
        default:
            return nullptr;
    }
}

template<typename T>
void ProcessComponents(Iterator<T>* it) {
    double total = 0;
    for (it->First(); !it->IsDone(); it->Next()) {
        T* comp = it->GetCurrent();
        comp->Process();
        total += comp->GetPrice();
    }
    cout << "Общая стоимость: " << total << " руб.\n\n";
    delete it;
}

// Декоратор фильтра по цене
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

int main() {
    srand(time(nullptr));

    ComponentArray<PCComponent> arrayContainer;
    ComponentList<PCComponent> listContainer;

    for (int i = 0; i < 5; ++i) {
        PCComponent* comp = CreateComponent(static_cast<ComponentType>(rand()%3));
        comp->SetTestStrategy(new QuickTestStrategy);
        arrayContainer.Add(comp);

        comp = CreateComponent(static_cast<ComponentType>(rand()%3));
        comp->SetTestStrategy(new PerformanceTestStrategy);
        listContainer.Add(comp);
    }

    cout << "=== Обработка компонентов массива ===\n";
    ProcessComponents(arrayContainer.GetIterator());

    cout << "=== Обработка компонентов списка (только с ценой >= 20000 руб.) ===\n";
    ProcessComponents(new PriceFilterIteratorDecorator<PCComponent>(listContainer.GetIterator(), 20000));

    return 0;
}
