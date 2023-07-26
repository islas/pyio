
#include "EmbeddedInterpreter.hpp"

#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <type_traits>

#include <fenv.h>

#include <omp.h>

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"
#include "pybind11/numpy.h"

// https://github.com/numpy/numpy/issues/20504
#define  FPE_GUARD_START( stash ) fenv_t stash; feholdexcept( &stash )
#define  FPE_GUARD_STOP ( stash ) fesetenv( &stash )

const std::string EmbeddedInterpreter::PY_INITIALIZE_METHOD   = "initialize";
const std::string EmbeddedInterpreter::PY_FINALIZE_METHOD     = "finalize";
const std::string EmbeddedInterpreter::PY_MAIN_METHOD         = "main";

////////////////////////////////////////////////////////////////////////////////
/// \brief Ctor
////////////////////////////////////////////////////////////////////////////////
EmbeddedInterpreter::EmbeddedInterpreter()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Dtor
////////////////////////////////////////////////////////////////////////////////
EmbeddedInterpreter::~EmbeddedInterpreter()
{
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the embedded python interpreter
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::initialize()
{
  if ( Py_IsInitialized() )
  {
    
#ifdef _OPENMP
    pMainThreadState_ = PyThreadState_Get();
    #pragma omp parallel
    {
      #pragma omp single
      gilStates_.resize( omp_get_num_threads() + 1 );
    }
#endif
  }

  // Import sys
  sys_ = pybind11::module_::import( "sys" );
  sysPathAppend_ = sys_.attr( "path" ).attr( "append" );

}

////////////////////////////////////////////////////////////////////////////////
/// \brief Clean up embedded python interpreter
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::finalize()
{
  // Clear containers
  {
    userDirectories_.clear();
    pymodules_.clear();
  }

}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Initializes" threading capabilities of python
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::threadingInit()
{
#ifdef _OPENMP
  if ( PyGILState_Check() ) 
  {
    // Prep and release GIL
    pMainThreadState_ = PyEval_SaveThread();
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Starts" threading capabilities of python
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::threadingStart()
{
#ifdef _OPENMP
  if ( !PyGILState_Check() ) 
  {
    std::cout << "Acquiring GIL" << std::endl;
    // swap your python thread state
    gilStates_[ omp_get_thread_num() ] = PyGILState_Ensure();
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Stops" threading capabilities of python
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::threadingStop()
{
#ifdef _OPENMP
  if ( PyGILState_Check() ) 
  {
    // Let it gooooo
    std::cout << "Releasing GIL" << std::endl;
    // clean the thread state before leaving
    PyGILState_Release( gilStates_[ omp_get_thread_num() ] );
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Finalizes" threading capabilities of python
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::threadingFinalize()
{
#ifdef _OPENMP
  if ( !PyGILState_Check() ) 
  {
    // We are back on the main thread, reacquire the GIL
    PyEval_RestoreThread( pMainThreadState_ );
  }
#endif
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Adds module search directories to the python interpreter
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::addToScope( std::string directory )
{
  userDirectories_.push_back( directory );
  sysPathAppend_( directory );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Creates" imports of the base modules provided
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embeddedPymoduleLoad( std::string pymodule )
{
  FPE_GUARD_START();
  pymodulesEmbedded_[ pymodule ] = pybind11::module_::import( pymodule.c_str() );
  FPE_GUARD_STOP();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Loads a pymodule into C++ scope to be used by C/C++/Fortran
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleLoad( std::string pymodule )
{
  FPE_GUARD_START();
  pybind11::module_ loaded = pybind11::module_::import( pymodule.c_str() );
  FPE_GUARD_STOP();

  pymodules_[ pymodule ] = loaded;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calls a pymodule's void function
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleCall( std::string pymodule, std::string function )
{
  FPE_GUARD_START();
  if ( pybind11::hasattr( pymodules_[ pymodule ], function.c_str() ) )
  {
    pymodules_[ pymodule ].attr( function.c_str() )();
  }
  FPE_GUARD_STOP();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a double ptr to use
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedDoublePtr( std::string pymodule, std::string attr, double *ptr, size_t numDims, size_t *pDimSize )
{
  FPE_GUARD_START();
  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

  // We are okay to make copies of these since they should be "small"
  pybind11::array::ShapeContainer dims = pybind11::array::ShapeContainer( std::vector< ssize_t >( pDimSize, pDimSize + numDims ) );
  pybind11::str dummyDataOwner;

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // // Lambda
          [=]() {
                return 
                  pybind11::array_t< std::remove_reference< decltype( ptr[0] ) >::type, pybind11::array::f_style | pybind11::array::forcecast >( 
                    dims,  // buffer dimensions
                    static_cast< const std::remove_reference< decltype( ptr[0] ) >::type * >( ptr ),
                    dummyDataOwner
                    );
          },
          pybind11::return_value_policy::automatic_reference
          );
  FPE_GUARD_STOP();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a float ptr to use
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedFloatPtr( std::string pymodule, std::string attr, float *ptr, size_t numDims, size_t *pDimSize )
{
  FPE_GUARD_START();
  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

  // We are okay to make copies of these since they should be "small"
  pybind11::array::ShapeContainer dims = pybind11::array::ShapeContainer( std::vector< ssize_t >( pDimSize, pDimSize + numDims ) );
  pybind11::str dummyDataOwner;

  // Add attribute to it
  mod.def(
          attr.c_str(),
          // // Lambda
          [=]() {
                return 
                  pybind11::array_t< std::remove_reference< decltype( ptr[0] ) >::type, pybind11::array::f_style | pybind11::array::forcecast >( 
                    dims,  // buffer dimensions
                    static_cast< const std::remove_reference< decltype( ptr[0] ) >::type * >( ptr ),
                    dummyDataOwner
                    );
          },
          pybind11::return_value_policy::automatic_reference
          );
  FPE_GUARD_STOP();
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a float value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedFloatValueCase( std::string pymodule, std::string attr, std::string attrCase, float(*func)(const char*) )
{


#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
            std::cout << __func__ << ": " <<  attrCase << std::endl;
#endif

  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

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


////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a int32 value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedInt32ValueCase( std::string pymodule, std::string attr, std::string attrCase, int32_t(*func)(const char*) )
{

#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
            std::cout << __func__ << ": " <<  attrCase << std::endl;
#endif

  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

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

////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a float value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedFloatValue( std::string pymodule, std::string attr, float(*func)(void) )
{


#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
            std::cout << __func__ << ": " <<  attrCase << std::endl;
#endif

  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

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
/// \brief Builds into a module a int32 value returned from a function pointer
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedInt32Value( std::string pymodule, std::string attr, int32_t(*func)(void) )
{

#ifndef NDEBUG
            std::cout << __func__ << ": " <<  reinterpret_cast< void * >( func ) << std::endl;
            std::cout << __func__ << ": " <<  attrCase << std::endl;
#endif

  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

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
////////////////////////////////////////////////////////////////////////////////
///
/// C bindings below
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for "ctor"
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_ctor( EmbeddedInterpreter **ppObj )
{
  EmbeddedInterpreter *pInterp = new EmbeddedInterpreter();
#ifndef NDEBUG
  std::cout << __func__ << ": " << "Returning " << static_cast< void * >( pInterp ) << std::endl;
#endif
  
  *ppObj = pInterp;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for "dtor"
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_dtor( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  "Deleting " << static_cast< void * >( *ppObj ) << std::endl;
#endif
  delete (*ppObj);
  (*ppObj) = 0;
  ppObj    = 0;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for initialize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_initialize( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->initialize();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for finalize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_finalize  ( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->finalize();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for threadingInit
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_threadingInit  ( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->threadingInit();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for threadingStart
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_threadingStart  ( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->threadingStart();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for threadingStop
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_threadingStop  ( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->threadingStop();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for threadingFinalize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_threadingFinalize  ( EmbeddedInterpreter **ppObj )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->threadingFinalize();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for addToScope
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_addToScope( EmbeddedInterpreter **ppObj, char *directory )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->addToScope( std::string( directory ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleLoad
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleLoad( EmbeddedInterpreter **ppObj, char *pymodule )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->pymoduleLoad( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleCall
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleCall( EmbeddedInterpreter **ppObj, char *pymodule, char *function )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->pymoduleCall( std::string( pymodule ), std::string( function ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embeddedPymoduleLoad
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embeddedPymoduleLoad( EmbeddedInterpreter **ppObj, char *pymodule )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embeddedPymoduleLoad( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoublePtr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoublePtr( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double *ptr, size_t numDims, size_t *pDimSize )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedDoublePtr( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatPtr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatPtr( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float *ptr, size_t numDims, size_t *pDimSize )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedFloatPtr( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatValueCase
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatValueCase( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, char *attrCase, float(*func)(const char*) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedFloatValueCase( std::string( pymodule ), std::string( attr ), std::string( attrCase ), func );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32ValueCase
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32ValueCase( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, char *attrCase, int32_t(*func)(const char*) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedInt32ValueCase( std::string( pymodule ), std::string( attr ), std::string( attrCase ), func );
}


////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatValue
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatValue( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float(*func)(void) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedFloatValue( std::string( pymodule ), std::string( attr ), func );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32Value
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32Value( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t(*func)(void) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedInt32Value( std::string( pymodule ), std::string( attr ), func );
}