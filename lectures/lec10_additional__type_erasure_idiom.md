
# Type Erasure Idiom

### Описание проблемы

Рассмотрим множество типов, которые имеют нечто общее в публичном интерфейсе. Например, все эти типы имеют метод `execute()`. Эти типы никак не связаны между собой, они могут быть определены в сторонних библиотеках и их редактирование может быть затруднено.

Допустим, что нам нужно хранить такие объекты в одном контейнере, например в контейнере типа `std::vector<Exectutable>`.

В коде это будет выглядеть примерно так:

```c++
struct A { void execute() const { /* ... */ }};
struct B { void execute() const { /* ... */ }};

std::vector<Executable> executableObjects;
executableObjects.push_back(A{});
executableObjects.push_back(B{});

for (const auto& obj : executableObjects) {
    obj.execute();
}
```

Описанное поведение напоминает "утиную типизацию": все, что имеет метод `execute()`, может быть сохранено в объекте типа `Executable`.

Еще такое поведение часто называют `ad-hoc` полиморфизмом.

### Почему у идиомы такое название?

Для того чтобы работать с объектами, имеющими метод `execute()`, как с объектами одного типа, нужно *"забыть"* об их исходном типе, отсюда и произошло название идиомы.


### Решение

#### Класс Executable

Чтобы решить проблему, нужно разобраться, что из себя представляет тип `Executable`:
 1. Имеет метод `execute()`
 2. Имеет неявный шаблонный конструктор с одним параметром.
 3. Копирует объект, переданный в конструктор, и является его единственным владельцем. Тип объекта должен иметь константный метод `execute()`

Неявность конструктора нужна для того, чтобы в качестве аргумента `push_back` передавать `A{}`, а не `Executable{A{}}`;

Шаблонность конструктора объясняется тем, что тип аргумента может быть любым типом с константным методом `execute()`

Выразим описанное выше в коде:

```c++
class Executable {
public:
    template <class T>
    Executable(const T& t) : holder_(createHolder(t)) { }

    void execute() const { holder_.execute(); }
private:
    Holder holder_;
};
```

Осталось решить, что такое `Holder holder_;` и каким образом его нужно создавать.

#### функция `createHolder`

О функции `createHolder` известно следующее:
 1. Она создает некий объект, который владеет копией `t`
 2. Она -- шаблонная, так как тип `t` -- это параметр шаблона
 3. Тип возвращаемого значения: `Holder`

Напишем черновой вариант кода, выражающий это:

```c++
template <class T>
struct ObjectHolder {
    ObjectHolder(const T& t) : obj(t) {}
    T obj;
};

template <class T>
Holder createHolder(const T& t) {
    auto holder = ObjectHolder<T>{t};
    return ???;
}
```

#### тип `Holder`

О типе `Holder` известно следующее:
 1. Этот тип фиксирован и не зависит от `T`. (*В ином случае мы бы не смогли создать поле этого типа в классе `Executable`*)
 2. Объект этого типа каким-то образом связан с `ObjectHolder<T>{t}`: уничтожение объекта типа `Holder` должно привести к уничтожению `ObjectHolder<T>{t}`

Для того чтобы разорвать связь с типом `T`, но при этом сохранить связь с объектом типа `ObjectHolder<T>`, можно сделать `ObjectHolder<T>` наследником `ObjectHolderBase`. Тип `ObjectHolderBase` не зависит от типа `T` и указатель этого типа может указывать на объект типа `ObjectHolder<T>`.

Чтобы обеспечить уничтожение объекта типа `ObjectHolder<T>`, имея только указатель на `ObjectHolderBase`, следует воспользоваться динамическим полиморфизмом (т.е. завести виртуальный деструктор).

Реализуем классы `Holder`, `ObjectHolderBase` и исправим реализацию `ObjectHolder` и `createHolder`:

```c++
struct ObjectHolderBase {
    virtual ObjectHolderBase* clone() const = 0;
    virtual ~ObjectHolderBase() = default;
};

template <class T>
struct ObjectHolder : ObjectHolderBase {
    ObjectHolder(const T& t) : obj(t) {}
    ObjectHolder* clone() const override { return new ObjectHolder(obj); }

    T obj;
};

class Holder {
public:
    explicit Holder(ObjectHolderBase* holder) : objHolder_(holder) {}
    Holder(const Holder& other) : Holder(other.objHolder_->clone()) {}
    Holder& operator=(const Holder&) = delete;
    ~Holder() { delete objHolder_; }
private:
    const ObjectHolderBase* objHolder_;
};

template <class T>
Holder createHolder(const T& t) {
    ObjectHolderBase* holder = new ObjectHolder<T>{t};
    return Holder{holder};
}
```

Замечания:
 1. Для того чтобы объекты Executable можно было использовать в векторе, нужно обеспечить копируемость этих объектов: поэтому понадобилась реализация конструктора копирования `Holder` и метод `clone()` (*Можно было избежать лишних копирований, если использовать move-семантику, но у нас ее пока не было*)
 2. Для того чтобы компилировался код, нужно добавить метод `execute` в классы `Holder`, `ObjectHolderBase` и `ObjectHolder`.
 3. `Holder` можно заменить умным указателем, например `unique_ptr<const ObjectHolderBase>`

---

В результате мы получили неполиморфный тип `Executable`.

При создании объектов этого типа, клиенты могут пользоваться "утиной типизацией" и оборачивать объект любого типа, у которого есть метод `execute()`.

Тип `Executable` инкапсулирует динамический полиморфизм и шаблонный код, тем самым скрывая от клиента довольно нетривиальный слой абстракций.

