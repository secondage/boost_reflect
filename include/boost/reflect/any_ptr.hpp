#ifndef _BOOST_REFLECT_ANY_PTR_HPP
#define _BOOST_REFLECT_ANY_PTR_HPP
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/reflect/vtable.hpp>
#include <boost/reflect/mirror_interface.hpp>

namespace boost { namespace reflect {

  /**
   *  @class any_ptr
   *  @brief Behaves like a smart pointer that can handle any type with the same interface.
   *
   *  If constructed from a shared pointer, then a copy of the shared pointer will go with
   *  every any_ptr.  If constructed from a regular pointer, then the pointer must be valid
   *  for the life of all copies of the any_ptr.
   *
   */
  template<typename InterfaceType, typename InterfaceDelegate = boost::reflect::mirror_interface>
  class any_ptr {
    public:
      typedef boost::reflect::vtable<InterfaceType,InterfaceDelegate> vtable_type;
      typedef InterfaceType                                           interface_type;
      typedef InterfaceDelegate                                       delegate_type;

      any_ptr()
      :m_vtable(boost::make_shared<vtable_type>()) {}

      operator bool()const  { return m_vtable; }
      bool operator!()const { return !m_vtable; }

      template<typename T>
      any_ptr( T* v )
      :m_ptr( boost::make_shared<boost::any>(v) ),m_vtable(boost::make_shared<vtable_type>()) {
        InterfaceDelegate::set_vtable(*m_vtable,*v);
      }
      template<typename T>
      any_ptr( const boost::shared_ptr<T>& v )
      :m_ptr(boost::make_shared<boost::any>(v)),m_vtable(boost::make_shared<vtable_type>()) {
        InterfaceDelegate::set_vtable(*m_vtable,*v);
      }

      /**
       *  @brief constructs an any_ptr from another any_ptr with compatible interface.
       */
      template<typename OtherInterface,typename OtherDelegate>
      any_ptr( const any_ptr<OtherInterface,OtherDelegate>& p )
      :m_ptr(p),m_vtable(boost::make_shared<vtable_type>()) {
        InterfaceDelegate::set_vtable( *m_vtable, *boost::any_cast<any_ptr<OtherInterface,OtherDelegate>&>(m_ptr) );
      }

      const vtable_type& operator*()const  { return *m_vtable;  } 
      vtable_type&       operator*()       { return *m_vtable;  } 

      const vtable_type* operator->()const { return m_vtable.get(); } 
      vtable_type*       operator->()      { return m_vtable.get(); } 
       
    protected:
      boost::shared_ptr<boost::any>       m_ptr;
      boost::shared_ptr<vtable_type>      m_vtable;
  };
  /**
   * @brief Helper function to enable automatic type deduction.
   *
   * Calls visitor with each member of the vtable of the any_ptr.
   */
  template<typename InterfaceType,typename InterfaceDelegate, typename Visitor>
  void visit( const any_ptr<InterfaceType,InterfaceDelegate>& aptr, Visitor v ) {
      boost::reflect::vtable_reflector<InterfaceType>::visit( &*aptr, v );
  }

} }

#endif
