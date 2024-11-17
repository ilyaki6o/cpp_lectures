#include <iostream>
#include <memory>
#include <typeinfo>
#include <stdexcept>

class Any {
private:
    // Базовый класс для хранения объекта
    struct Base {
        virtual ~Base() = default;
        virtual const std::type_info& type() const = 0;
        virtual std::unique_ptr<Base> clone() const = 0;
    };

    // Шаблонный класс для хранения конкретного типа объекта
    template<typename T>
    struct Derived : Base {
        T value;

        explicit Derived(const T& value) : value(value) {}

        const std::type_info& type() const override {
            return typeid(T);
        }

        std::unique_ptr<Base> clone() const override {
            return std::make_unique<Derived<T>>(value);
        }
    };

    std::unique_ptr<Base> ptr;

public:
    Any() = default;

    // Конструктор копирования для совместимости с любым типом
    template<typename T>
    Any(const T& value) : ptr(std::make_unique<Derived<T>>(value)) {}

    // Получение значения
    template<typename T>
    T& get() {
        if (!ptr || ptr->type() != typeid(T)) {
            throw std::bad_cast();
        }

        return static_cast<Derived<T>*>(ptr.get())->value;
    }

    Any(const Any& other) : ptr(other.ptr ? other.ptr->clone() : nullptr) {}

    Any& operator=(const Any& other) {
        if (this != &other) {
            ptr = other.ptr ? other.ptr->clone() : nullptr;
        }

        return *this;
    }
};

int main() {
    try {
        Any a(5);
        std::cout << a.get<int>() << std::endl; // 5

        a = std::string("Hello");
        std::cout << a.get<std::string>() << std::endl; // Hello

        Any b(a);
        std::cout << b.get<int>() << std::endl; // Ошибка: бросает std::bad_cast
    } catch (const std::bad_cast& e) {
        std::cerr << "Bad cast: " << e.what() << std::endl;
    }

    return 0;
}
