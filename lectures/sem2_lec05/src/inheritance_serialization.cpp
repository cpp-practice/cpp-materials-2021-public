#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>

struct Animal {
    virtual void serialize(std::ostream&) const = 0;
    virtual ~Animal() = default;

    static Animal* deserialize(std::istream& in) {
        std::string className;
        in >> className;
        auto it = prototypes.find(className);
        if (it == prototypes.end()) {
            throw std::runtime_error("Unknown class");
        }
        Animal* animal = it->second;
        animal->deserializeImpl(in);
        return animal;
    }

    struct StaticInitHelper {
        StaticInitHelper(const std::string& className, Animal* proto) {
            Animal::prototypes[className] = proto;
        }
    };

protected:
    virtual void deserializeImpl(std::istream&) = 0;
    static inline std::unordered_map<std::string, Animal*> prototypes;
};

struct Dog : Animal {
    static StaticInitHelper init;

    void serialize(std::ostream& out) const override {
        out << "Dog" << " " << dogData;
    }
    std::string dogData = "dog";



protected:
    void deserializeImpl(std::istream& in) override {
        in >> dogData;
    }
};
Animal::StaticInitHelper Dog::init{"Dog", new Dog{}};

struct Cat : Animal {
    static Animal::StaticInitHelper init;

    void serialize(std::ostream& out) const override {
        out << "Cat" << " " << catData;
    }
    std::string catData = "cat";

protected:
    void deserializeImpl(std::istream& in) override {
        in >> catData;
    }
};
Animal::StaticInitHelper Cat::init{"Cat", new Cat{}};

int main() {
    std::vector<std::shared_ptr<Animal>> animals;
    animals.emplace_back(new Dog{});
    animals.emplace_back(new Cat{});

    std::stringstream ss;
    for (const auto& animal : animals) {
        animal->serialize(ss);
        ss << " ";
    }

    {
        std::vector<std::shared_ptr<Animal>> animals2;
        for (size_t i = 0; i < 2; ++i) {
            animals2.emplace_back(Animal::deserialize(ss));
        }

        assert(dynamic_cast<Dog*>(animals2[0].get()) != nullptr);
        assert(dynamic_cast<Cat*>(animals2[1].get()) != nullptr);
    }

    return 0;
}
