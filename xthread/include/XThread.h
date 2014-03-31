#pragma once

#include <boost/thread.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/lexical_cast.hpp>


namespace xthread {


/**
* ������������� �������� ������������� ������ ���������� ������������
* ����. ��������.
*
* @template T  ��� ���������, ������� ������ ���������.
*/
template< typename T >
class XThread {
public:
    /**
    * ������������� ������.
    */
    typedef boost::thread::id  uid_t;


    /**
    * ���������� ����������.
    * ������������ ����� UID ������ (������� ������� �������) � ��� �������.
    */
    typedef std::pair< uid_t, T >  content_t;


    /**
    * ���������.
    */
    typedef std::vector< content_t >  container_t;




public:
    /**
    * @param count  ���������� �������.
    * @param limit  ������������ ���-�� ��������� � ����������.
    * @param pause  ������� ������� ����� ����� ����� �� ������
    *               ������ ����. ����� ������ � �����������.
    *               ����������� � ��.
    */
    XThread( size_t count, size_t limit, size_t pause );




    virtual ~XThread();




    /**
    * ��������� ������� ��������� ������ ��������.
    *
    * @see ����. � ���� ������.
    */
    void start();




    /**
    * ������������� ������� ��������� ������ ��������.
    */
    void stop();



    
    inline container_t const& container() const {
        return mContainer;
    }




    /**
    * @return ���������� ��������� ����������.
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
    * ��������� ������� �� ��������� ������ � ����� ������.
    */
    void push( uid_t thread, const T& );




    /**
    * @return ���������� ���� < true, ���������� >. ��� ���������� ������
    *         ��������� �� UID ������. ���� ������, ������� �� ������ ���������.
    *         ���� < false, ������ ���������� > ������������, ���� ������� �
    *         ������ �� ������.
    */
    std::pair< bool, typename XThread< T >::content_t >  pop( uid_t thread );




    /**
    * ������� ��������� ������� �� ����������.
    */
    void eraseLast();




    /**
    * @return ����� ���������� ���������� ����������.
    *
    * # ��������� ������ ��������� ���� �� ���� �������.
    */
    content_t last() const;




    /**
    * ���������� (�� ����������� ����������) ������� ������ ������
    * � ����������.
    */
    void operator()();




protected:
    /**
    * ������� ������ ������ � ����������.
    * ����� ��������� ����������.
    */
    void behaviour();




public:
    /**
    * ��� �������� �� ������ get-������.
    */
    const size_t  count;
    const size_t  limit;
    const size_t  pause;




private:
    container_t  mContainer;

    /**
    * ���������� ������ � �����������.
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
* �������� � ������� ���������� � ���������� ��������� � ����������
* �� ������� ������.
*/
template< typename T >
std::ostream& operator<<( std::ostream&, const xthread::XThread< T >& );

} // std








#include "XThread.inl"
