namespace xthread {


template< typename T >
inline
XThread< T >::XThread(
    size_t count,
    size_t limit,
    size_t pause
) :
    count( count ),
    limit( limit ),
    pause( pause ),
    mCountPop( 0 ),
    mCountPush( 0 ),
    mCountErase( 0 )
{
}




template< typename T >
inline
XThread< T >::~XThread() {
    stop();
}




template< typename T >
inline void
XThread< T >::start() {
    // формируем потоки
    for (size_t k = 0; k < count; ++k) {
        std::unique_ptr< boost::thread >  th(
            new boost::thread( boost::ref( *this ) )
        );
#ifdef _DEBUG
        const auto uid = th->get_id();
        std::cout << "\nПоток " << (k + 1) <<
            " (" << uid << ") создан." << std::endl;
#endif
        mThreadList.push_back( std::move( th ) );

    } // for (size_t k = 0; ...
}




template< typename T >
inline void
XThread< T >::stop() {
    for (auto itr = mThreadList.begin(); itr != mThreadList.end(); ++itr) {
        ( *itr )->interrupt();
#ifdef _DEBUG
        std::cout << "\nПоток " << ( *itr )->get_id() << " снят." << std::endl;
#endif
    }
}




template< typename T >
inline void
XThread< T >::push( uid_t thread,  const T& element ) {
#ifdef _DEBUG
    //std::cout << "+" << thread << "\t";
#endif

    mContainer.push_back( std::make_pair( thread, element ) );

    // статистика
    boost::mutex::scoped_lock  lock( mtxCountPush );
    ++mCountPush;
}




template< typename T >
inline std::pair< bool,  typename XThread< T >::content_t >
XThread< T >::pop( uid_t thread ) {
#ifdef _DEBUG
    //std::cout << "-" << thread << "\t";
#endif

    std::pair< bool, content_t >  r =
        std::make_pair( false, std::make_pair( uid_t(), T() ) );

    // ищем последний элемент, добавленный указанным потоком
    container_t::iterator  last = mContainer.end();
    for (auto itr = mContainer.begin(); itr != mContainer.end(); ++itr) {
        const content_t& content = *itr;
        if (content.first == thread) {
            last = itr;
        }
    }
    if (last != mContainer.end()) {
        r = std::make_pair( true, *last );
        eraseLast();
    }

    // статистика
    boost::mutex::scoped_lock  lock( mtxCountPop );
    ++mCountPop;

    return r;
}




template< typename T >
inline void
XThread< T >::eraseLast() {
    const auto last = mContainer.end() - 1;
    if (last > mContainer.begin()) {
        mContainer.erase( last );
    }

    // статистика
    boost::mutex::scoped_lock  lock( mtxCountErase );
    ++mCountErase;
}




template< typename T >
inline typename XThread< T >::content_t
XThread< T >::last() const {
    const content_t content = mContainer.back();
    return content;
}




template< typename T >
inline void
XThread< T >::operator()() {
    try {
        behaviour();
        // # Поток сам осовободит ресурсы благодаря boost::mutex::scoped_lock.

    } catch ( const std::exception& ex ) {
        // @todo fine Неплохо бы держать в порядке и список потоков.
        std::cout << std::endl << "(!) Исключение в потоке " <<
            boost::this_thread::get_id() << ". " <<
            ex.what() <<
            std::endl;

    } catch ( ... ) {
        // игнорируем нестандартные исключения
    }

    // @see Если нужно передавать исключения выше >
    //      http://boost.org/doc/libs/1_53_0/libs/exception/doc/tutorial_exception_ptr.html
}




template< typename T >
inline void
XThread< T >::behaviour() {

    const auto uid = boost::this_thread::get_id();

    // ниже нам понадобится генератор случ. чисел
    std::stringstream ss;
    ss << uid;
    boost::mt19937  gen( boost::hash_value( ss.str() ) );
    boost::uniform_int<>  uni( 0, INT_MAX );
    boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
        rg( gen, uni );


    // бесконечная работа потока с контейнером
    for ( ; ; ) {
#ifdef _DEBUG
        //std::cout << "\n" << mContainer.size() << "\t";
        std::cout << ".";
#endif
        // добавляем элемент
        // # Используем конструктор по умолчанию.
        const T value = T();

        {
            boost::mutex::scoped_lock  lock( mtxContainer );

            // чужой (от другого потока) элемент удаляется из контейнера
            if ( !mContainer.empty() ) {
                const content_t lastContent = last();
                if (lastContent.first != uid) {
                    pop( lastContent.first );
                }
            }

            // контейнер ни в коем случае не должен быть переполнен
            if (mContainer.size() > limit) {
                pop( uid );
            }

            // добавляем свой элемент
            push( uid, value );
        }


        // ожидаем
        const auto random = rg();
        const auto wait = boost::posix_time::millisec( random % pause + 1 );
        boost::this_thread::sleep( wait );


        // эмулируем исключение
        if ((random % 5000) == 1) {
            //throw std::exception( "Эмуляция исключения." );
        }

    } // for ( ; ; )
}


} // xthread








namespace std {


template< typename T >
std::ostream&
operator<<( std::ostream& out,  const xthread::XThread< T >& th ) {
    using namespace xthread;

    // # Если от потока в контейнере нет ни одного элемента,
    //   поток не печатается.

    // собираем
    std::map< XThread< T >::uid_t, size_t >  statistics;
    for (auto itr = th.container().cbegin(); itr != th.container().cend(); ++itr) {
        const auto& uid = itr->first;
        auto ftr = statistics.find( uid );
        if (ftr == statistics.end()) {
            statistics.insert( std::make_pair( uid, 1 ) );
        } else {
            ftr->second++;
        }
    }

    // печатаем
    std::cout << "\nСодержимое контейнера\n";
    for (auto itr = statistics.cbegin(); itr != statistics.cend(); ++itr) {
        std::cout << "\tПоток " << itr->first << "\t" <<
            itr->second << " элем." <<
            std::endl;
    }
    std::cout << "Макс. зарегистрированное количество" <<
        " элементов в контейнере: " << th.container().size() << std::endl <<
        "Операций POP:\t"   << th.countPop() << std::endl <<
        "Операций PUSH:\t"  << th.countPush() << std::endl <<
        "Операций ERASE:\t" << th.countErase() <<
        std::endl;

    return out;
}


} // std
