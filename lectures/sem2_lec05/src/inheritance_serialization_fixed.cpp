#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <memory>

class Animal {
    using AnimalFactory = Animal*(*)(std::istream&);
public:
    virtual ~Animal() = default;

    virtual void serialize(std::ostream&) const = 0;

    static Animal* deserialize(std::istream& in) {
        auto factory = animalFactory(className(in));
        return factory(in);
    }

protected:
    static bool registerFactory(std::string className, AnimalFactory factory) {
        return animalFactories_.try_emplace(std::move(className), factory).second;
    }

private:
    static std::string className(std::istream& in) {
        std::string name;
        in >> name;
        return name;
    }

    static AnimalFactory animalFactory(const std::string& className) {
        auto it = animalFactories_.find(className);
        if (it == animalFactories_.end()) {
            throw std::runtime_error("Unknown class: " + className);
        }
        return it->second;
    }

    static inline std::unordered_map<std::string, AnimalFactory> animalFactories_;
};

class Dog : public Animal {
public:
    Dog() = default;
    explicit Dog(std::string data)
        : dogData_(std::move(data))
    {}

    void serialize(std::ostream& out) const override {
        out << className() << " " << dogData_;
    }

private:
    static Animal* deserialize(std::istream& in) {
        std::string data;
        in >> data;
        return new Dog(std::move(data));
    }
    static std::string className() { return "Dog"; }

    std::string dogData_ = "dog";

private:
    [[maybe_unused]] static inline bool factoryRegistered
        = registerFactory(className(), deserialize);
};

class Cat :public Animal {
public:
    Cat() = default;
    explicit Cat(std::string data)
        : catData_(std::move(data))
    {}

    void serialize(std::ostream& out) const override {
        out << className() << " " << catData_;
    }
private:
    static Animal* deserialize(std::istream& in) {
        std::string data;
        in >> data;
        return new Cat(std::move(data));
    }
    static std::string className() { return "Cat"; }

    std::string catData_ = "cat";

private:
    [[maybe_unused]] static inline bool factoryRegistered
        = registerFactory(className(), deserialize);
};

int main() {
    std::vector<std::shared_ptr<Animal>> animals;
    animals.emplace_back(new Dog{"dog1"});
    animals.emplace_back(new Cat{"cat1"});
    animals.emplace_back(new Dog{"dog2"});
    animals.emplace_back(new Cat{"cat2"});

    std::stringstream ss;
    for (const auto& animal : animals) {
        animal->serialize(ss);
        ss << " ";
    }

    {
        std::vector<std::shared_ptr<Animal>> animals2;
        for (size_t i = 0; i < 4; ++i) {
            animals2.emplace_back(Animal::deserialize(ss));
        }

        assert(dynamic_cast<Dog*>(animals2[0].get()) != nullptr);
        assert(dynamic_cast<Cat*>(animals2[1].get()) != nullptr);
        assert(dynamic_cast<Dog*>(animals2[2].get()) != nullptr);
        assert(dynamic_cast<Cat*>(animals2[3].get()) != nullptr);
    }

    return 0;
}
