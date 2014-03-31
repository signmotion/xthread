#pragma once

#include <boost/thread.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/lexical_cast.hpp>


namespace xthread {


/**
* Демонстрирует механизм использования общего контейнера одновременно
* неск. потоками.
*
* @template T  Тип элементов, которые хранит контейнер.
*/
template< typename T >
class XThread {
public:
    /**
    * Идентификатор потока.
    */
    typedef boost::thread::id  uid_t;


    /**
    * Содержимое контейнера.
    * Представляет собой UID потока (который добавил элемент) и сам элемент.
    */
    typedef std::pair< uid_t, T >  content_t;


    /**
    * Контейнер.
    */
    typedef std::vector< content_t >  container_t;




public:
    /**
    * @param count  Количество потоков.
    * @param limit  Максимальное кол-во элементов в контейнере.
    * @param pause  Сколько времени поток будет ждать до своего
    *               своего след. цикла работы с контейнером.
    *               Указывается в мс.
    */
    XThread( size_t count, size_t limit, size_t pause );




    virtual ~XThread();




    /**
    * Запускает процесс обработки списка потоками.
    *
    * @see Комм. в коде метода.
    */
    void start();




    /**
    * Останавливает процесс обработки списка потоками.
    */
    void stop();



    
    inline container_t const& container() const {
        return mContainer;
    }




    /**
    * @return Количество изменений контейнера.
    */
    size_t countPop()  const {
        return mCountPop;
    }


    size_t countPush() const {
        return mCountPush;
    }


    size_t countErase() const {
        return mCountErase;
    }




    /**
    * Добавляет элемент от заданного потока в конец списка.
    */
    void push( uid_t thread, const T& );




    /**
    * @return Возвращает пару < true, содержание >. Где содержание ищется
    *         последнее по UID потока. Если найден, элемент из списка удаляется.
    *         Пара < false, пустое содержание > возвращается, если элемент в
    *         списке не найден.
    */
    std::pair< bool, typename XThread< T >::content_t >  pop( uid_t thread );




    /**
    * Удаляет последний элемент из контейнера.
    */
    void eraseLast();




    /**
    * @return Копия последнего содержания контейнера.
    *
    * # Контейнер должен содержать хотя бы один элемент.
    */
    content_t last() const;




    /**
    * Безопасный (не выбрасывает исключений) процесс работы потока
    * с хранилищем.
    */
    void operator()();




protected:
    /**
    * Процесс работы потока с хранилищем.
    * Может выбросить исключение.
    */
    void behaviour();




public:
    /**
    * Для констант не плодим get-методы.
    */
    const size_t  count;
    const size_t  limit;
    const size_t  pause;




private:
    container_t  mContainer;

    /**
    * Статистика работы с контейнером.
    */
    size_t       mCountPop;
    size_t       mCountPush;
    size_t       mCountErase;


    typedef std::unique_ptr< boost::thread >  threadPtr;
    std::list< threadPtr >  mThreadList;

    mutable boost::mutex  mtxContainer;
    mutable boost::mutex  mtxCountPop;
    mutable boost::mutex  mtxCountPush;
    mutable boost::mutex  mtxCountErase;
};


} // xthread








namespace std {

/**
* Печатает в консоль информацию о количестве элементов в контейнере
* от каждого потока.
*/
template< typename T >
std::ostream& operator<<( std::ostream&, const xthread::XThread< T >& );

} // std








#include "XThread.inl"
