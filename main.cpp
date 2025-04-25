#include <iostream>
#include <cstdlib>
#include <ctime>
#include "PatternTemplates.h"

using namespace std;

class PCComponent {
protected:
    string manufacturer;
    double price;
public:
    PCComponent(const string& manuf, double pr) : manufacturer(manuf), price(pr) {}
    virtual ~PCComponent() {}
    virtual void ShowInfo() const = 0;
    double GetPrice() const { return price; }
};

class CPU : public PCComponent {
    int cores;
    double frequency;
public:
    CPU(const string& manuf, double pr, int cr, double freq)
        : PCComponent(manuf, pr), cores(cr), frequency(freq) {}

    void ShowInfo() const override {
        cout << "CPU: " << manufacturer << ", " << cores << " cores, "
             << frequency << " GHz, $" << price << endl;
    }
};

class GPU : public PCComponent {
    int memory;
    string memoryType;
public:
    GPU(const string& manuf, double pr, int mem, const string& memType)
        : PCComponent(manuf, pr), memory(mem), memoryType(memType) {}

    void ShowInfo() const override {
        cout << "GPU: " << manufacturer << ", " << memory << "GB "
             << memoryType << ", $" << price << endl;
    }
};

class RAM : public PCComponent {
    int capacity;
    string type;
public:
    RAM(const string& manuf, double pr, int cap, const string& t)
        : PCComponent(manuf, pr), capacity(cap), type(t) {}

    void ShowInfo() const override {
        cout << "RAM: " << manufacturer << ", " << capacity << "GB "
             << type << ", $" << price << endl;
    }
};

enum class ComponentType { CPU, GPU, RAM };

PCComponent* CreateComponent(ComponentType type) {
    switch(type) {
        case ComponentType::CPU:
            return new CPU("Intel", 200 + rand()%200, 4 + rand()%4, 2.5 + rand()%3);
        case ComponentType::GPU:
            return new GPU("NVIDIA", 300 + rand()%300, 6 + rand()%6, "GDDR6");
        case ComponentType::RAM:
            return new RAM("Kingston", 80 + rand()%40, 8 + rand()%8, "DDR4");
        default:
            return nullptr;
    }
}

int main() {
    srand(time(nullptr));

    ComponentArray<PCComponent> arrayContainer;
    ComponentList<PCComponent> listContainer;

    for (int i = 0; i < 5; ++i) {
        arrayContainer.Add(CreateComponent(static_cast<ComponentType>(rand()%3)));
        listContainer.Add(CreateComponent(static_cast<ComponentType>(rand()%3)));
    }

    cout << "Обычная обработка массива\n";
    ProcessComponents(arrayContainer.GetIterator());

    cout << "Обработка с логированием\n";
    ProcessComponents(new LoggingIteratorDecorator<PCComponent>(arrayContainer.GetIterator()));

    cout << "Обработка с фильтрацией (цена >= 200)\n";
    ProcessComponents(new PriceFilterIteratorDecorator<PCComponent>(listContainer.GetIterator(), 200));

    cout << "Подсчёт обработанных элементов\n";
    auto* countingIt = new CountingIteratorDecorator<PCComponent>(arrayContainer.GetIterator());
    ProcessComponents(countingIt);
    cout << "Количество элементов: " << countingIt->GetCount() << "\n";
    delete countingIt;

    cout << "STL стиль\n";
    auto rawIt = listContainer.GetIterator();
    for (auto it = STLIteratorAdapter<PCComponent>::begin(rawIt);
         it != STLIteratorAdapter<PCComponent>::end(); ++it) {
        (*it)->ShowInfo();
    }
    delete rawIt;

    return 0;
}
