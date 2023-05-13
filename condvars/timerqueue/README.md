# timerqueue

В этой задаче вам нужно реализовать примитив синхронизации `TimerQueue<T>`.

TimerQueue хранит внутри себя множество пар `(v, t)`. `v` - это значение, `t` - это
момент времени.

Операция `Add(v, t)` добавляет новую пару в множество.

Операция `Pop()` блокируется до момента `min(t) по всем элементам множества` и затем
возвращает элемент с минимальным `t` (при этом, удаляя его их множества). Если множество пустое, `Pop()` ждёт вечно.

- Для работы со временем нужно использовать `std::chrono::system_clock`.
- Решение должно использовать ожидание через условные переменные. Не создавайте никаких новых потоков, не используйте методы `sleep` и `yield`.