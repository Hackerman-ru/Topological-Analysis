# Topological Analysis lib (Topa)

__Topa__ — библиотекой для работы в области топологического анализа. Высокооптимизированая и читаемая, она является одной из лучших сред для построения теорий и проведения экспериментов.

Реализует в себе построение комплекса Вьеториса-Рипса, вычисление устойчивых пар и гармонических представителей.

Все компоненты библиотеки хорошо заменяемы и абстрагированы от других частей, что позволяет изменять на лету алгоритмы и структуры данных, используемые для вычислений.

## Примеры

Ниже приведён пример использования библиотеки для вычисления устойчивых пар фильтрованного симплициального комплекса Вьеториса-Рипса, построенного из облака точек в формате __OFF__.

```cpp
#include <topa/topa.hpp>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

// 2-skeleton with max radius
void test_vr() {
    const float max_radius = 0.5f;
    const size_t max_dim = 2;
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = NewVR<EucledianDistance>(max_radius, max_dim);
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}

// Full 2-skeleton
void test_full() {
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = FullVR<EucledianDistance>();
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
}
```

Библиотека также поддерживает относительно быстрое вычисление гармонических представителей

```cpp
#include <topa/topa.hpp>

using namespace topa;
using namespace topa::common;
using namespace topa::fast;

void test_harmonic() {
    using Solver = ShiftSolver<25, 50, 15000, 6>;
    const float max_radius = 0.5f;
    const size_t max_dim = 2;
    std::string path = "Path-to-file.off";

    auto pointcloud = Pointcloud::Load(path).value();
    auto filtration = NewVR<EucledianDistance>(max_radius, max_dim);
    auto complex = FilteredComplex<FlatTree>::From(pointcloud, filtration);
    auto pairs = DoubleTwist<SparseMatrix, BitTreeHeap>::Compute(complex);
    auto harmonic_cycles = Harmonic<Solver, SVDSeparator, 10>::Compute(complex, pairs);
    detail::PrintCycles(harmonic_cycles);
}
```

## Архитектура

Архитектура построена на принципах Generic Programming с использованием:
-  CRTP (Curiously Recurring Template Pattern) для статического полиморфизма
- Концепций C++20 для валидации типов
- Policy-based design для замены компонентов

Большинство функций, методов и классов принимают не конкретные реализации, а модели объектов/алгоритмов, что делает код универсальным и открывает возможность добавлять эксперементальный функционал без трудностей.


<div style="text-align:center">
<img src=./docs/images/class_diagram.png width="900">
</div>

Полное описание архитектуры библиотеки можно найти [отчёте](./docs/main.pdf)

## Сравнение

По результатом бенчмарков, существующие решения ([GUDHI](https://gudhi.inria.fr/), [Dionysus-2](https://mrzv.org/software/dionysus2/), [Ripser](https://github.com/Ripser/ripser)) задач топологического анализа на C++ оказались хуже, чем данный фреймворк, который реализует новейшие алгоритмы.

<div style="text-align:center">
<img src=./docs/images/vr.png width="900">
<img src=./docs/images/full_vr.png width="900">
<img src=./docs/images/complexity.png width="900">
</div>

Важно также отметить, что __Topa__ выделяется среди них по следующим параметрам:
1. Простота использования: классы появляются для решения текущей ступени алгоритма, и данные текут между ними в ожидаемой манере (в отличие от __GUDHI__, в котором все данные сохраняются в __SimplexTree__)
2. Хорошая модульность и гибкость: можно на каждом шаге вашей задаче изменить алгоритм, добавить свою реализацию и быстро сравнить. __Ripser__ является огромной нечитаемой массой кода, которая ещё и лежит вся в одном файле
3. Превосходство в скорости: __Topa__ обгоняет даже самые лучшие реализации при вычислении устойчивых пар фильтрованного комплекса Вьеториса-Рипса

## Установка

Поддерживается установка через CMake версии не меньше 3.10. Также, можно установить библиотеку и хедеры следующими командами:

```bash
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build . --target install
```
