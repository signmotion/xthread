#include "include/XThread.h"



int main( int argc, char** argv ) {

    using namespace xthread;

    setlocale( LC_ALL, "Russian" );
    setlocale( LC_NUMERIC, "C" );


    const size_t count =
        (argc > 1) ? std::atoi( argv[ 1 ] ) : 1;
    const size_t capacity =
        (argc > 2) ? std::atoi( argv[ 2 ] ) : 5;
    const size_t pause =
        (argc > 3) ? std::atoi( argv[ 3 ] ) : 100;

    std::cout <<
        "Демонстарция работы потоков с общим контейнером."
        "\n\nКоманда для запуска\t"
        "\n    xthread [count] [capacity] [pause]"
        "\nгде"
        "\n    count     количество потоков"
        "\n    capacity  размер контейнера"
        "\n    pause     время \"отдыха\" потока, мс"
        "\n"
        "\nЗапущено потоков\t" << count <<
        "\nРазмер контейнера\t" << capacity << " + 1 элемент" <<
        "\nПауза для потока\tдо " << pause << " мс" <<
        std::endl << std::endl;


    XThread< int >  xthread( count, capacity, pause );
    try {
        xthread.start();

    } catch ( const std::exception & ex ) {
        std::cerr << "(!)" << ex.what() << std::endl;

    } catch ( ... ) {
        std::cerr << "(!) Не известное исключение." << std::endl;
    }

    std::cout <<
        "\nПотоки работают с общим контейнером." <<
        "\nНажмите Enter для завершения процесса..." <<
        std::endl << std::endl;
    std::cin.ignore();
    xthread.stop();

    // результат
    std::cout << xthread;



    std::cout << "\n^\n";
    std::cin.ignore();
}
