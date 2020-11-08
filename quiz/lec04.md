
Разбор опроса по лекции 4.

# 1. Мне понятно, как бросать и ловить исключения

```c++
#include <stdexcept>
#include <iostream>

void throw_exception() {
    // Бросаем исключение
    throw std::runtime_error("error message");
}

void function() {
    try {
        throw_exception();
        // в функции throw_exception выбрасывается исключение
        // далее среди catch блоков выбирается первый подходящий
        // (в зависимости от типа объекта, который был выброшен)
    } catch (const std::logic_error& ex) {
        // этот перехват не сработает
        std::cout << "logic error";
    } catch (const std::runtime_error& ex) {
        // этот перехват сработает
        // будет выведено сообщение, сохраненное в выброшенном объекте
        std::cout << ex.what();
        // и текущее исключение пробрасывается дальше,
        // т.е. выполнение функции function заканчивается
        throw;
    } catch (...) {
        // три точки (элипсис) означает,
        // что мы можем перехватить любое исключение.
        // сообщаем в поток вывода, что исключение неизвестное
        std::cout << "unknown exception";
        // и пробрасываем его далее
        throw;
    }
}
```

# 2. Перегрузка допускается, если F в function call operator F(Arg1,Arg2) это указатель на функцию.

Нет, не допускается.

Выбор функции из нескольких перегруженных происходит, когда тип функции не фиксирован в F.
Например:

```c++
void f(int) {}
void f(double) {}

int main() {
    f(1);
}
```
в `f` тип никак не фиксирован, так как представляет собой имя функции. Из всех найденных функций `f` будет самая подходящая (в зависимости от типов передаваемых аргументов)

Рассмотрим пример с указателем:

```c++
void f(int) {}
void f(double) {}

int main() {
    void(*p)(double) = f;
    p(1);
}
```

В этом случае указатель указывает на конкретную фукнцию и при вызове будет использована именно она - перегрузка здесь не работает.

# 3. Отметьте верные утверждения

### Функцию можно передать в другую функцию по значению

```c++
void g(int) {}
using FuncType = void(int);
void f(FuncType func) {
    func(100);
}

int main() {
    f(g);
}
```

Сниппет кода, показывает, что мы вроде как передаем функцию `g` в функцию `f` по значению, но на самом деле это не так.

Здесь нужно обратить внимание на тип функции `f` и вспомнить как он формируется. Если тип параметра это тип функции, то он преобразовывается к указателю.

Чтобы это проверить, можем попробовать скомпилировать такой код:

```c++
int main() {
    f = 10;
}
```

и взглянуть на ошибку.

Текст ошибки может быть таким: _error: non-object type 'void (FuncType *)' (aka 'void (void (*)(int))') is not assignable_

Из него видно, что параметр функции имеет тип указателя.

### Функцию можно передать в другую функцию по указателю

Да, так сделать можно.

### Функцию можно передать в другую функцию по ссылке

Можно:

```c++
void g(int) {}
void f(void(&func)(int)) {
    func(100);
}

int main() {
    f(g);
}
```

### Возвращаемый тип функции может быть функцией

Такое невозможно. Код не скомпилируется:

```c++
void g(int) {}
using Func = void(int);
Func f() { // Compile-time error
    return g;
}
```

### Возвращаемый тип функции может быть указателем на функцию

А такое может быть:

```c++
void g(int) {}
using Func = void(int);
Func* f() { // OK
    return g;
}
```

# 4. Отметьте верные утверждения

### definition функции -- это и ее declaration

Объявление (свободной) функции вводит в программу информацию о ее имени и типе.
При определении функции эта информация тоже появляется в программе, поэтому определение является и объявлением тоже.

```c++
void f(); // declaration, not definition
void f()  // declaration and definition
{ }
```

### declaration [функции] может быть в любой области видимости

Да

```c++
void f(); // в глобальном пространстве имен

int g()
{
    {
        void f(); // в compound-statement
    }
}

int main() {
    void f(); // в теле функции
}

```

### `void(*ptr)(int)` - указатель на функцию, которую можно вызвать так `ptr();`

Так вызвать нельзя, так как тип функции требует хотя бы одного аргумента, и он не может быть указан по умолчанию для указателей.

### в definition типы параметров, передаваемых по значению, могут быть incomplete

Не могут. При компиляции функции важно знать размер объектов, чтобы выделить необходимый размер на стеке. У incomplete типов размер получить нельзя, поэтому нельзя и скомпилировать такой код.

Один из способов получить incomplete тип это использовать forward delcaration класса.

```c++
struct F; // Forward declaration
// now F - incomplete type
void t(F f) { // Compile-time error

}

struct F { }; // now F complete type
```

### функции `void f();` и `void f(void)` имеют один и тот же тип

Да.

```c++
#include <type_traits>
int main() {
    static_assert(std::is_same<void(void), void()>::value);
}
```

### функции `void f(int)` и `void f(const int)` имеют один и тот же тип

Да, при формировании типа функции top-level const квалификатор у типов параметров отбрасывается.

```c++
#include <type_traits>
int main() {
    static_assert(std::is_same<void(int), void(const int)>::value);
}
```

или

```c++
void f(int); // declaration

int main() {
    f(20);
}

void f(const int i) { // definition

}
```

# 5. Я знаю, как вывести в поток вывода имя функции, находясь в ее теле.

```c++
#include <iostream>

void print_my_name() {
    std::cout << __func__;
}

int main() {
    print_my_name();
}
```

# 6. Я знаю, как вызвать C++ функцию в C-коде

Рассмотрим пример:

```c++
// counter.h
#ifdef __cplusplus
extern "C" {
#endif

unsigned countOdd(int* arr, unsigned size);

#ifdef __cplusplus
}
#endif
```

В cpp-файле приведен небольшой синтетический пример, в котором используются различные возможности С++: анонимные пространства имен, structured binding, ...

```c++
// counter.cpp
#include "counter.h"

#include <cstddef>

namespace {

struct Counter {
    size_t oddCounter = 0;
    size_t evenCounter = 0;

    void operator()(int n) {
        (n % 2 == 1) ? ++oddCounter : ++evenCounter;
    }
};

} // namespace

unsigned countOdd(int* arr, unsigned size) {
    Counter counter;

    for (unsigned i = 0; i < size; ++i) {
        counter(arr[i]);
    }

    auto [odd, even] = counter;
    return odd;
}
```

```c
include "counter.h"

#include <stdio.h>

int main() {

    int nums[10];

    for (int i = 0; i < 10; ++i)
        nums[i] = i;

    printf("%d", countOdd(nums, 10));

    return 0;
}
```

Собираем и запускаем:
```bash
$ clang++ -std=c++17 -c counter.cpp -o counter.o
$ clang -c main.c -o main.o
$ clang main.o counter.o
$ ./a.out
```

_Такой способ будет работать не всегда. Если в `counter.cpp` мы воспользуемся, например `operator-new` или `std::cout`, то в `counter.o` появятся символы, которые не получится слинковать в С-шный исполныемый файл. Чтобы решить это, нужно создать библиотеку из С++ кода и использовать ее из C-кода._

# 7. Мне понятно, зачем нужен name mangling

`Name mangling` нужен для того, чтобы различать одноименные сущности.

Рассмотрим пример:

```c++
// a.cpp
void f(int) { }
void f(double) { }
```

```c++
// main.cpp
void f(double);

int main() {
    f(13.0);
}
```

```bash
$ clang++ -c a.cpp -o a.o
$ clang++ -c main.cpp -o main.o
```

У каждого объектного файла имеются символы (можно посмотреть с помощью утилиты `nm`).

Обе функции в `a.cpp` имеют внешнюю линковку и соответствующие им символы можно найти в объектом файле. `Name mangling` помогает различать эти функции (сущности), добавляя к имени информацию об используемых типах и скоупе.

Поэтому при линковке, линкер может однозначно связать функцию `f` из `main.cpp` с функцией `f` из `a.cpp`.

1. Есть полезный инструмент, чтобы получить читаемую информацию о сущности: `demangler` ([link](https://demangler.com/))
2. `extern "C"` предотвращает `name mangling`.
3. В `C` `name manlging` отсутствует, поэтому нельзя завести две функции с одинаковым именем.

# 8. Какие функции считаются жизнеспособными (viable) во время overloading resolution: `float i = 3.0; f(i);`


- `void f();` -- нет; неподходящее число параметров
- `void f(int);` -- да; `float` неявно преобразуется к `int`
- `void f(int, int);` -- нет, неверное число параметров
- `void f(int, int = 3);` -- да; см 2ой случай
- `void f(double);` -- да; `float` неявно преобразуется к `double`
- `void f(double, ...);` -- да; `ellispis` допускает отсутствие аргументов
- `void f(...);` -- да; `ellipsis` допускает любое число аргументов
- `void f(float, float);` -- нет; неверное число параметров
- `void f(char);` -- да; неявное преобразование к целочисленному типу
- `void f(int*);` -- нет; неявное преобразование к указателю не допускается
- `void f(void*);` -- нет; неявное преобразование к указателю не допускается
- `void f(int&);` -- нет; ссылка на объект типа `int` не может ссылаться на объект типа `float`
- `void f(double&);` -- нет; ссылка на объект типа `double` не может ссылаться на объект типа `float`
- `void f(const double&);` -- да; создается временный объект типа `double`
- `void f(float&);` -- да;

#### Про временный объект.

Рассмотрим такой код:

```c++
float i = 3.0;
const double& j = i;
std::cout << (&j) << " " << (&i);
```

Сравните полученные адреса.
