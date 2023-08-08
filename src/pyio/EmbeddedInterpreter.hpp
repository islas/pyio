
#ifndef EmbeddedInterpreter_hpp
#define EmbeddedInterpreter_hpp

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <map>

#include <fenv.h>

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"
#include "pybind11/numpy.h"


// https://github.com/numpy/numpy/issues/20504
#define  FPE_GUARD_START( stash ) fenv_t stash; feholdexcept( &stash )
#define  FPE_GUARD_STOP( stash )  fesetenv( &stash )


class EmbeddedInterpreter
{
public:
  // Ctor Dtor
  EmbeddedInterpreter();
  virtual ~EmbeddedInterpreter();

  void initialize();
  void finalize();

  void threadingInit();
  void threadingStart();
  void threadingStop();
  void threadingFinalize();

  void addToScope( std::string directory );

  // Module handling
  void pymoduleLoad      ( std::string pymodule );
  void pymoduleCall      ( std::string pymodule, std::string function );

  // Embedded module loading
  void embeddedPymoduleLoad( std::string pymodule );

  // Building python-accesible modules - only operable on pymodules loaded from embedPymoduleLoad
  template< int style, typename T >
  void embedPtr      ( std::string pymodule, std::string attr, T *ptr, size_t numDims, size_t *pDimSize );
  template< typename T >
  void embedValue    ( std::string pymodule, std::string attr, T val );
  template< typename T >
  void embedValueFunc( std::string pymodule, std::string attr, T (*func)(void) );
  template< typename T >
  void embedValueCase( std::string pymodule, std::string attr, std::string attrCase, T (*func)(const char*) );

private:
  bool checkEmbeddedModuleLoaded( std::string pymodule );

  
  pybind11::scoped_interpreter        guard_;            ///< Directly maintain the lifetime of this guard within this scope
  std::vector< std::string >                             userDirectories_;   ///< User supplied locations for user python modules
  std::unordered_map< std::string, pybind11::module_ >   pymodules_;         ///< Map of pymodules loaded ready to be called
  std::unordered_map< std::string, pybind11::module_ >   pymodulesEmbedded_; ///< Map of embedded pymodules available to python

  // OpenMP shenanigans
  std::vector< PyGILState_STATE > gilStates_;        ///< retain gil states per thread to transform POSIX original threads to "python threads"
  PyThreadState                  *pMainThreadState_; ///< retain main thread state

  // Python modules
  pybind11::module_   sys_;
  pybind11::function  sysPathAppend_;

  
  bool autoLoad_; ///< Automatic loading of any used embedded python modules during runtime

};

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a ptr to use
////////////////////////////////////////////////////////////////////////////////
template< int style = pybind11::array::c_style, typename T >
void
EmbeddedInterpreter::embedPtr(
                                    std::string  pymodule, ///< Python module to operate on
                                    std::string  attr,     ///< python attribute to associate this value with e.g. pymodule.attr()
                                    T           *ptr,      ///< pointer to respective data to map, of element size PRODUCT(pDimSize) for numDims
                                    size_t       numDims,  ///< dimensionality of the array (currently only using Fortran)
                                    size_t      *pDimSize  ///< pointer of size numDims describing the respective size of each dim
                                    )
{
  FPE_GUARD_START( fpeTemp );
  // Get embedded module
  checkEmbeddedModuleLoaded( pymodule );
  pybind11::module_ mod = pymodulesEmbedded_[ pymodule ];

  // We are okay to make copies of these since they should be "small"
  pybind11::array::ShapeContainer dims = pybind11::array::ShapeContainer( std::vector< ssize_t >( pDimSize, pDimSize + numDims ) );
  pybind11::str dummyDataOwner;

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // // Lambda
          [=]() {
                return 
                  pybind11::array_t< T, style | pybind11::array::forcecast >( 
                    dims,  // buffer dimensions
                    static_cast< const T * >( ptr ),
                    dummyDataOwner
                    );
          },
          pybind11::return_value_policy::automatic_reference
          );
  FPE_GUARD_STOP( fpeTemp );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a T value returned from a raw value
////////////////////////////////////////////////////////////////////////////////
template< typename T >
void
EmbeddedInterpreter::embedValue(
                                std::string pymodule, ///< Python module to operate on
                                std::string attr,     ///< python attribute to associate this value with e.g. pymodule.attr()
                                T           val       ///< A raw value copied from the embedding caller
                                )
{


#ifndef NDEBUG
            std::cout << __func__ << ": " << val << std::endl;
#endif

  // Get embedded module
  checkEmbeddedModuleLoaded( pymodule );
  pybind11::module_ mod = pymodulesEmbedded_[ pymodule ];

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // Lambda
          [=]() 
          { 
            return val;
          }
          );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a T value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
template< typename T >
void
EmbeddedInterpreter::embedValueFunc(
                                    std::string pymodule, ///< Python module to operate on
                                    std::string attr,     ///< python attribute to associate this value with e.g. pymodule.attr()
                                    T (*func)(void)    ///< A function pointer that takes no arguments and returns the respective value
                                    )
{


#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
#endif

  // Get embedded module
  checkEmbeddedModuleLoaded( pymodule );
  pybind11::module_ mod = pymodulesEmbedded_[ pymodule ];

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // Lambda
          [=]() 
          { 
            return func( ); 
          }
          );
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a T value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
template< typename T >
void
EmbeddedInterpreter::embedValueCase(
                                          std::string pymodule,      ///< Python module to operate on
                                          std::string attr,          ///< python attribute to associate this value with e.g. pymodule.attr()
                                          std::string attrCase,      ///< string identifier (often the same as attr) to distinguish this value
                                          T (*func)(const char*) ///< A function pointer that takes a string identifier and returns the respective value
                                          )
{


#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
            std::cout << __func__ << ": " <<  attrCase << std::endl;
#endif

  // Get embedded module
  checkEmbeddedModuleLoaded( pymodule );
  pybind11::module_ mod = pymodulesEmbedded_[ pymodule ];

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // Lambda
          [=]() 
          { 
            return func( attrCase.c_str() ); 
          }
          );
}

extern "C"
{

void                  EmbeddedInterpreter_ctor      ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_dtor      ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_initialize( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_finalize  ( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_threadingInit    ( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_threadingStart   ( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_threadingStop    ( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_threadingFinalize( EmbeddedInterpreter *pObj );
void                  EmbeddedInterpreter_addToScope( EmbeddedInterpreter *pObj, char *directory );
void                  EmbeddedInterpreter_pymoduleLoad        ( EmbeddedInterpreter *pObj, char *pymodule );
void                  EmbeddedInterpreter_pymoduleCall        ( EmbeddedInterpreter *pObj, char *pymodule, char *function );
void                  EmbeddedInterpreter_embeddedPymoduleLoad( EmbeddedInterpreter *pObj, char *pymodule );

void                  EmbeddedInterpreter_embedDoublePtr      ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, double  *ptr, size_t numDims, size_t *pDimSize );
void                  EmbeddedInterpreter_embedFloatPtr       ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, float   *ptr, size_t numDims, size_t *pDimSize );
void                  EmbeddedInterpreter_embedInt32Ptr       ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, int32_t *ptr, size_t numDims, size_t *pDimSize );

void                  EmbeddedInterpreter_embedDoublePtrScalar( EmbeddedInterpreter *pObj, char *pymodule, char *attr, double  *ptr );
void                  EmbeddedInterpreter_embedFloatPtrScalar ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, float   *ptr );
void                  EmbeddedInterpreter_embedInt32PtrScalar ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, int32_t *ptr );

void                  EmbeddedInterpreter_embedDoubleValue    ( EmbeddedInterpreter *pObj, char *pymodule, char *attr,  double val );
void                  EmbeddedInterpreter_embedFloatValue     ( EmbeddedInterpreter *pObj, char *pymodule, char *attr,   float val );
void                  EmbeddedInterpreter_embedInt32Value     ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, int32_t val );

void                  EmbeddedInterpreter_embedDoubleValueFunc( EmbeddedInterpreter *pObj, char *pymodule, char *attr,  double(*func)(void) );
void                  EmbeddedInterpreter_embedFloatValueFunc ( EmbeddedInterpreter *pObj, char *pymodule, char *attr,   float(*func)(void) );
void                  EmbeddedInterpreter_embedInt32ValueFunc ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, int32_t(*func)(void) );

void                  EmbeddedInterpreter_embedDoubleValueCase( EmbeddedInterpreter *pObj, char *pymodule, char *attr, char  *attrCase,  double(*func)(const char*) );
void                  EmbeddedInterpreter_embedFloatValueCase ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, char  *attrCase,   float(*func)(const char*) );
void                  EmbeddedInterpreter_embedInt32ValueCase ( EmbeddedInterpreter *pObj, char *pymodule, char *attr, char  *attrCase, int32_t(*func)(const char*) );



}

#endif