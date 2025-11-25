  

# Нахождение максимальных значений по столбцам матрицы

  

- Student: Овчинников Матвей Евгеньевич, group 3823Б1ПР2

- Technology: SEQ | MPI

- Variant: 16

  

## 1. Introduction

  

- Brief motivation: узнать про технологию Open MPI, написать собственную программу для изучения работы параллельных процессов.

- Problem context: создать программу, которая использует параллельные процессы, сложнее, чем последовательную. Однако, это окупается высокой производительностью.

- Expected outcome: получить навыки работы с параллельными процессами, научиться ускорять программы.

## 2. Problem Statement

- Formal task definition: нужно написать последовательную и параллельную, использующую средства Open MPI, программы, которые позволят найти максимальное значение в каждом столбце введенной матрицы. Сравнить скорости работы полученных реализаций, а так же проверить их валидность посредством Func и Perf тестов.

- input/output format: на вход программе подаются размеры матрицы (два целых числа), и сама матрица, которая представлена в виде одного вектора, содержащего int числа (то есть матрица хранится линейно). На выход подаётся вектор, с числами int, которые являются максимальными значениями столбцов матрицы (j-ый элемент вектора равен максимальному значению j-го столбца в данной матрице).

## 3. Baseline Algorithm (Sequential)

Если на вход подается пустая матрица, то программа вернет пустой вектор. В случае, если матрица имеет хотя бы одну ячейку, то мы проходимся внешним циклом по столбцам матрицы, а внутренними циклами по каждому столбцу отдельно и ищем в нём максимальный по значению элемент, который хранится в векторе.

## 4. Parallelization Scheme

- Data distribution:

```cpp

int base = rows / size;

int extra = rows % size;

int my_start = rank * base +  std::min(rank, extra);

int my_end = my_start + base + (rank < extra ?  1  :  0);

```

- Rank roles:

- Process 0:

- Обработка частей матрицы (base столбцов)

- Рассылка данных через MPI_Bcast

- All processes:

- Локальный поиск максимума в своём блоке

- Участие в MPI_Allreduce


  

## 5. Implementation Details

- Code structure (files, key classes/functions): функции ValidationImpl, PreProcessingImpl, PostProcessingImpl по сути не используются - всегда возвращается true. Вся логика содержится в функции:

```cpp

bool OvchinnikovMMaxValuesInMatrixRowsMPI::RunImpl() {
  int rank = 0;
  int size = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int cols = std::get<1>(GetInput());
  int rows = std::get<0>(GetInput());
  if (rows <= 0 || cols <= 0) {
    return true;
  }

  const auto &matrix = std::get<2>(GetInput());

  const int base = rows / size;
  const int extra = rows % size;
  const int my_start = (rank * base) + std::min(rank, extra);
  const int my_end = my_start + base + (rank < extra ? 1 : 0);

  std::vector<int> local_max(cols, std::numeric_limits<int>::min());

  for (int i = my_start; i < my_end; ++i) {
    for (int j = 0; j < cols; ++j) {
      local_max[j] = std::max(local_max[j], matrix[i * cols + j]);
    }
  }

  std::vector<int> global_max(cols);
  MPI_Allreduce(local_max.data(), global_max.data(), cols, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

  GetOutput() = global_max;

  return true;
}

```

- Important assumptions and corner cases: если на вход подаётся пустая матрица, то возвращается true

- Memory usage considerations: чтобы сократить время работы программы каждому процессу выделен доступ к памяти GetInput(), однако работают они только со своим блоком столбцов матрицы.

  

## 6. Experimental Setup

- Hardware/OS: CPU model, cores/threads, RAM, OS version:

- Модель ЦП: 13th Gen Intel(R) Core(TM) i7-13700H

- Архитектура: x86_64

- Ядра/потоки: 6 ядер, 12 потоков

- ОЗУ: 16 ГБ

- Версия ОС: WSL: 2.6.1.0

- Ядро: Linux 6.8.0-87-generic

- Toolchain:

- GCC 13.1.0 (Ubuntu 13.1.0-8ubuntu1~22.04)

- Clang 18.1.3

- CMake 3.28.3

- GNU Make 4.3

  

## 7. Results and Discussion

  

### 7.1 Correctness

Корректность работы была проверена с помощью комплексного модульного тестирования с использованием фреймворка Google Test. Набор тестов включает в себя 6 тестовых случаев, охватывающих различные сценарии:

```

const std::array<TestType, 6> kTestParam = {
    std::make_tuple(0, 0, std::vector<int>{}, "empty_matrix"),

    std::make_tuple(3, 3, std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9}, "matrix_3x3"),

    std::make_tuple(2, 2, std::vector<int>{-1, -5, 4, 0}, "negatives"),

    std::make_tuple(1, 1, std::vector<int>{10}, "single_element"),

    std::make_tuple(2, 3, std::vector<int>{1, 10, 3, 7, 0, 100}, "random"),

    std::make_tuple(5, 9, std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 7, 6, 3, 2, 9, 4, 2, 5, 8, 1, 2, 3, 4, 5,
                                           6, 7, 8, 9, 1, 2, 3, 4, 5, 6, 7, 8, 9, 7, 6, 3, 2, 9, 4, 2, 5, 8},
                    "large_matrix"),
};

```

Все тесты прошли успешно (6/6) со временем выполнения 0-2 мс на тест, что подтверждает правильность подсчета максимума столбцов матрицы для обеих реализаций: SEQ и MPI.

  
  

### 7.2 Performance

Present time, speedup and efficiency. Example table:

  

| Mode | Count | Time, s | Speedup | Efficiency |

|-------------|-------|---------|---------|------------|

| seq | 1 | 0.0159543991 | 1.00 | N/A |

| seq | 1 | 0.0139318466 | 1.00 | N/A |

| mpi | 4 | 0.0035918574 | 4.16 | 104% |

| mpi | 4 | 0.0074977316 | 1.99 | 49.7% |

Реализация MPI показывает ускорение более чем в четыре раза по сравнению с последовательной версией, но это видно только на pipeline. На task_run, в свою очередь, разница лишь в два раза, поскольку здесь сравнивается сам алгоритм поиска максимума в столбце, который по идее, выполняется одинаково на MPI и SEQ версии (мне кажется, что разница возникает из-за погрешности). А поскольку этот одинаковый алгоритм выполняется параллельно на нескольких процессах, то время pipeline`а в разы меньше на MPI версии.
  
  

## 8. Conclusions

Summarize findings and limitations.
Эта задача показывает хороший параллелизм, потому что максимум в каждом столбце уникальный, и я свободно могу делить матрицу на блоки из столбцов и на каждом процессе отдельно проходиться по блокам, как будто по отдельным матрицам. Однако, есть слабое место в коде: если подавать матрицу с большим кол-вом строк, чем столбцов, я не смогу нормально поделить матрицу, блоки будут большими и неудобными. В целом, это можно было бы решить, если бы я делил матрицу на блоки по площади, а не по столбцам, но я посчитал это сложным в реализации, т.к. пришлось бы как-то связывать данные из разделенных столбцов, тогда данные были бы уже зависимыми, общения между процессами было бы больше и на маленьких данных MPI версия тормозила бы процесс.
  

## 9. References

1. [Учебные материалы](https://disk.yandex.ru/d/NvHFyhOJCQU65w)

  

## Appendix (Optional)

```cpp

// Short, readable code excerpts if needed

```