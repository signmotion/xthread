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
    // ��������� ������
    for (size_t k = 0; k < count; ++k) {
        std::unique_ptr< boost::thread >  th(
            new boost::thread( boost::ref( *this ) )
        );
#ifdef _DEBUG
        const auto uid = th->get_id();
        std::cout << "\n����� " << (k + 1) <<
            " (" << uid << ") ������." << std::endl;
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
        std::cout << "\n����� " << ( *itr )->get_id() << " ����." << std::endl;
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

    // ����������
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

    // ���� ��������� �������, ����������� ��������� �������
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

    // ����������
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

    // ����������
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
        // # ����� ��� ���������� ������� ��������� boost::mutex::scoped_lock.

    } catch ( const std::exception& ex ) {
        // @todo fine ������� �� ������� � ������� � ������ �������.
        std::cout << std::endl << "(!) ���������� � ������ " <<
            boost::this_thread::get_id() << ". " <<
            ex.what() <<
            std::endl;

    } catch ( ... ) {
        // ���������� ������������� ����������
    }

    // @see ���� ����� ���������� ���������� ���� >
    //      http://boost.org/doc/libs/1_53_0/libs/exception/doc/tutorial_exception_ptr.html
}




template< typename T >
inline void
XThread< T >::behaviour() {

    const auto uid = boost::this_thread::get_id();

    // ���� ��� ����������� ��������� ����. �����
    std::stringstream ss;
    ss << uid;
    boost::mt19937  gen( boost::hash_value( ss.str() ) );
    boost::uniform_int<>  uni( 0, INT_MAX );
    boost::variate_generator< boost::mt19937&, boost::uniform_int<> >
        rg( gen, uni );


    // ����������� ������ ������ � �����������
    for ( ; ; ) {
#ifdef _DEBUG
        //std::cout << "\n" << mContainer.size() << "\t";
        std::cout << ".";
#endif
        // ��������� �������
        // # ���������� ����������� �� ���������.
        const T value = T();

        {
            boost::mutex::scoped_lock  lock( mtxContainer );

            // ����� (�� ������� ������) ������� ��������� �� ����������
            if ( !mContainer.empty() ) {
                const content_t lastContent = last();
                if (lastContent.first != uid) {
                    pop( lastContent.first );
                }
            }

            // ��������� �� � ���� ������ �� ������ ���� ����������
            if (mContainer.size() > limit) {
                pop( uid );
            }

            // ��������� ���� �������
            push( uid, value );
        }


        // �������
        const auto random = rg();
        const auto wait = boost::posix_time::millisec( random % pause + 1 );
        boost::this_thread::sleep( wait );


        // ��������� ����������
        if ((random % 5000) == 1) {
            //throw std::exception( "�������� ����������." );
        }

    } // for ( ; ; )
}


} // xthread








namespace std {


template< typename T >
std::ostream&
operator<<( std::ostream& out,  const xthread::XThread< T >& th ) {
    using namespace xthread;

    // # ���� �� ������ � ���������� ��� �� ������ ��������,
    //   ����� �� ����������.

    // ��������
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

    // ��������
    std::cout << "\n���������� ����������\n";
    for (auto itr = statistics.cbegin(); itr != statistics.cend(); ++itr) {
        std::cout << "\t����� " << itr->first << "\t" <<
            itr->second << " ����." <<
            std::endl;
    }
    std::cout << "����. ������������������ ����������" <<
        " ��������� � ����������: " << th.container().size() << std::endl <<
        "�������� POP:\t"   << th.countPop() << std::endl <<
        "�������� PUSH:\t"  << th.countPush() << std::endl <<
        "�������� ERASE:\t" << th.countErase() <<
        std::endl;

    return out;
}


} // std
