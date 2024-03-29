Многопоточное консольное приложение С++.

Основной поток запускает X вспомогательных потоков.
X - первый параметр командной строки.

Каждый из вспомогательных потоков бесконечно  (с некоторой изменяющейся
задержкой) добавляет в некий общий контейнер свой элемент.

При этом он удаляет самый старый элемент в случае, если тот был добавлен
другим потоком или если количество элементов в контейнере > Y.
Y - второй параметр командной строки.

Основной поток ожидает нажатия на Enter.

После получения ввода из командной строки основной поток печатает статистику
вида '<поток> - <количество элементов для этого потока в контейнере>' для всех
потоков (вспомогательные потоки больше использоваться не будут) и максимальное
зарегистрированное количество элементов в контейнере.

После этого приложение завершается.


Основные условия и требования:

1. Формат командной строки 'test_app  X  Y'.

2. Y - целое число (size_t) на величину которого ограничение не накладывается
(в рамках ограничения типа переменной).

3. Код должен безопасно обрабатывать возможные исключения, т.е предполагается
что любой из потоков может формировать С++ исключение в любой точке кода.
При этом общая работоспособность приложения должна сохраняться, либо приложение
должно корректно завершиться если исключение произошло в основном потоке.

4. Код должен быть объектно ориентирован.

5. Приложение должно обеспечивать оптимальную скорость (в рамках С++, т.е. без
использования ассемблерных вставок e.t.c.) как для накопления, так и для
отображения статистики.

6. Используется Visual Studio.

7. Без использования CLR.

8. Приветствуется использование библиотеки boost.
