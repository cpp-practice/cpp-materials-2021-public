Разбор опроса по лекции 9.

# 1. Я понимаю, какой класс называется полиморфным
Класс, содержащий или наследующий одну или более виртуальных функций. Объекты этого класса содержат дополнительный указатель на vtable
# 2. Я представляю, какой оверхед мы получаем в рантайме, если пользуемся множественным наследованием
- Расчет адреса одного из базовых классов
Например, при вызове мембера:
```C++
struct A {};
struct B { void f() const {} }
struct C : public A, public B {};
int main() {
    C c;
    std::cout << &c << " ";
    c.f();
}
```
Метод `f()` в качестве аргумента неявно принимает `B* this`, поэтому для вызова `c.f()` нужно сделать неявный `upcast` в рантайме. 
- ### Во время upcast-a указателей нужно делать проверку на `nullptr` в рантайме:
```C++
struct A { int p; };
struct B { int p; };
struct C : public A, public B {};
int main() {
    C* pc = nullptr;
    B* pb = pc; // <=> pb = (pc == nullptr) ? nullptr : /* pointers offset */
}
```


# 3. Я понимаю, в чем заключается виртуальное наследование
```C++
struct A { };
struct B : *virtual* A { };
struct C : *virtual* A {};
struct D : B, C {};
```
Используя ключевое слово `virtual` мы избавляемся от двойного включения полей класса `A` в класс `D`, но, так как класс `A` теперь общий для объектов типа `B` и `C` (в `D`), то он не может быть жестко зафиксирован относительно этих классов, поэтому они используют указатель на `A`.


# 4. Я представляю какой layout имеют классы при различных вариантах множественного наследования (повторяющийся предок, виртуальный предок,...)

рассмотрим различные примеры и покомпилируем их с флагом: `clang++ -Xclang -fdump-record-layouts -c main.cpp`

- ### Simple multiple inheritance
```C++
struct A { };
struct B { };
struct C : public A, public B { };
```
```
 0 | struct C (empty)
 0 |   struct A (base) (empty)
 0 |   struct B (base) (empty)
   | [sizeof=1, dsize=0, align=1,
   |  nvsize=1, nvalign=1]
```

- ### Повторяющийся предок, нет виртуальности, публичное наследование

```C++
struct A { char a; };
struct B : A { };
struct C : A { };
struct D : B, C { int x; };
```
```
 0 | struct D
 0 |   struct B (base)
 0 |     struct A (base)
 0 |       char a
 1 |   struct C (base)
 1 |     struct A (base)
 1 |       char a
 4 |   int x
   | [sizeof=8, dsize=8, align=4,
   |  nvsize=8, nvalign=4]
```

- ### Есть виртуальные методы, публичное наследование, нет повторяющихся предков
```C++
struct A {
	virtual void f() {}
	char a;
};
struct B {
	virtual void g() {}
};
struct C : A, B { 
	void f() override {}
	void g() override {}
};
```
```
 0 | struct C
 0 |   struct A (primary base)
 0 |     (A vtable pointer)
 8 |     char a
16 |   struct B (base)
16 |     (B vtable pointer)
   | [sizeof=24, dsize=24, align=8,
   |  nvsize=24, nvalign=8]
```
- ### Виртуальное наследование
```C++
struct A { };
struct B : virtual A { };
struct C : virtual A { };
struct D : B, C { };
```
```
0 | struct D
0 |   struct B (primary base)
0 |     (B vtable pointer)
8 |   struct C (base)
8 |     (C vtable pointer)
0 |   struct A (virtual base) (empty)
  | [sizeof=16, dsize=16, align=8,
  |  nvsize=16, nvalign=8]
```

# 5. Я понимаю, для чего нужен механизм RTTI
 Это механизм, который позволяет определить тип переменной или объекта на этапе выполнения программы.

# 6. Мне понятно, в каком порядке вызываются конструкторы и деструкторы

- В начале вызываются конструкторы базовых классов, потом конструкторы производных классов.
- Деструкторы вызываются полностью в обратном порядке, сначала уничтожаются наследники, а потом базовые классы. 

# 7. (*) Мне понятно, в каком случае вычисляется выражение в typeid

В случае, когда выражение является `glvalue` и возвращает объект полиморфного типа.

