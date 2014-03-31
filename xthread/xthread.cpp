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
        "������������ ������ ������� � ����� �����������."
        "\n\n������� ��� �������\t"
        "\n    xthread [count] [capacity] [pause]"
        "\n���"
        "\n    count     ���������� �������"
        "\n    capacity  ������ ����������"
        "\n    pause     ����� \"������\" ������, ��"
        "\n"
        "\n�������� �������\t" << count <<
        "\n������ ����������\t" << capacity << " + 1 �������" <<
        "\n����� ��� ������\t�� " << pause << " ��" <<
        std::endl << std::endl;


    XThread< int >  xthread( count, capacity, pause );
    try {
        xthread.start();

    } catch ( const std::exception & ex ) {
        std::cerr << "(!)" << ex.what() << std::endl;

    } catch ( ... ) {
        std::cerr << "(!) �� ��������� ����������." << std::endl;
    }

    std::cout <<
        "\n������ �������� � ����� �����������." <<
        "\n������� Enter ��� ���������� ��������..." <<
        std::endl << std::endl;
    std::cin.ignore();
    xthread.stop();

    // ���������
    std::cout << xthread;



    std::cout << "\n^\n";
    std::cin.ignore();
}
