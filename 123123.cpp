#include <iostream>
#include <vector>

// Структура компонента
struct Component {
    std::string name;
    std::string type;
    float price;
};

int main() {
    // Список компонентов
    std::vector<Component> components = {
        {"CPU: Intel, 5 cores, 2.5 GHz", "CPU", 271},
        {"CPU: Intel, 6 cores, 2.5 GHz", "CPU", 264},
        {"GPU: NVIDIA, 7GB GDDR6", "GPU", 460},
        {"CPU: Intel, 6 cores, 4.5 GHz", "CPU", 256},
        {"CPU: Intel, 5 cores, 4.5 GHz", "CPU", 239}
    };

    // Обработка с фильтрацией (цена >= 200)
    float total = 0;
    int count = 0;

    std::cout << "Список компонентов (фильтрация: цена >= 200):" << std::endl;
    for (const auto& component : components) {
        if (component.price >= 200) {
            std::cout << component.name << ", $" << component.price << std::endl;
            total += component.price;
            count++;
        }
    }

    std::cout << "Общая стоимость: $" << total << std::endl;
    std::cout << "Количество элементов: " << count << std::endl;

    return 0;
}
