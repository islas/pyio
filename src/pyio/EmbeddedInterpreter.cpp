
#include "EmbeddedInterpreter.hpp"

#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <fenv.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"
#include "pybind11/numpy.h"

////////////////////////////////////////////////////////////////////////////////
/// \brief Ctor
////////////////////////////////////////////////////////////////////////////////
EmbeddedInterpreter::EmbeddedInterpreter()
  : autoLoad_( false )
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
EmbeddedInterpreter::addToScope( 
                                std::string directory ///< Directory that will be added to python module import path (sys.path)
                                )
{
  userDirectories_.push_back( directory );
  sysPathAppend_( directory );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief "Creates" imports of the base modules provided
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embeddedPymoduleLoad(
                                          std::string pymodule ///< Python module to operate on
                                          )
{
  FPE_GUARD_START( fpeTemp );
  pymodulesEmbedded_[ pymodule ] = pybind11::module_::import( pymodule.c_str() );
  FPE_GUARD_STOP( fpeTemp );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Loads a pymodule into C++ scope to be used by C/C++/Fortran
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleLoad(
                                  std::string pymodule ///< Python module to operate on
                                  )
{
  FPE_GUARD_START( fpeTemp );
  pybind11::module_ loaded = pybind11::module_::import( pymodule.c_str() );
  FPE_GUARD_STOP( fpeTemp );

  pymodules_[ pymodule ] = loaded;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calls a pymodule's void function
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleCall(
                                  std::string pymodule, ///< Python module to operate on
                                  std::string function  ///< function name to invoke within pymodule
                                  )
{
  FPE_GUARD_START( fpeTemp );
  if ( pybind11::hasattr( pymodules_[ pymodule ], function.c_str() ) )
  {
    pymodules_[ pymodule ].attr( function.c_str() )();
  }
  else
  {
    std::cout << "Warning: Python module '"      << pymodule 
              << "' does not contain function '" << function 
              << "', not executed." << std::endl;
  }
  FPE_GUARD_STOP( fpeTemp );
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Checks if the embedded python module has been loaded and reports findings
///        if necessary or simply loads
////////////////////////////////////////////////////////////////////////////////
bool
EmbeddedInterpreter::checkEmbeddedModuleLoaded(
                                                std::string pymodule ///< Python module to operate on
                                                )
{
  if ( pymodulesEmbedded_.find( pymodule ) == pymodulesEmbedded_.end() )
  {
    if ( autoLoad_ )
    {
      embeddedPymoduleLoad( pymodule );
    }
    else
    {
      std::stringstream ss;
      ss << __FILE__ << ":" << __LINE__ << " : Error: Module '" << pymodule << "' has not beed loaded" << std::endl;
      std::cerr << ss.str();
      throw std::runtime_error( ss.str() );
    }
  }
  return true;
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
//##############################################################################
///// PTR
//##############################################################################
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoublePtr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoublePtr( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double *ptr, size_t numDims, size_t *pDimSize )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatPtr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatPtr( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float *ptr, size_t numDims, size_t *pDimSize )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32Ptr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32Ptr( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t *ptr, size_t numDims, size_t *pDimSize )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
//##############################################################################
///// PTR - but actually for Fortran scalar values (single value)
//##############################################################################
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoublePtrScalar
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoublePtrScalar( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double *ptr )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  // Quickly make some stand-in args
  size_t numDims  = 1;
  size_t pDimSize[1] = { 1 };

  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatPtrScalar
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatPtrScalar( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float *ptr )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  // Quickly make some stand-in args
  size_t numDims  = 1;
  size_t pDimSize[1] = { 1 };

  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32PtrScalar
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32PtrScalar( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t *ptr )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << " embedding pointer <" << static_cast< void * >( ptr ) << ">" << std::endl;
#endif
  // Quickly make some stand-in args
  size_t numDims  = 1;
  size_t pDimSize[1] = { 1 };

  (*ppObj)->embedPtr< pybind11::array::f_style >( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}

////////////////////////////////////////////////////////////////////////////////
//##############################################################################
///// Value
//##############################################################################
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoubleValue
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoubleValue( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double val )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValue( std::string( pymodule ), std::string( attr ), val );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatValue
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatValue( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float val )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValue( std::string( pymodule ), std::string( attr ), val );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32Value
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32Value( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t val )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValue( std::string( pymodule ), std::string( attr ), val );
}

////////////////////////////////////////////////////////////////////////////////
//##############################################################################
///// Value function
//##############################################################################
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoubleValueFunc
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoubleValueFunc( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double(*func)(void) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValueFunc( std::string( pymodule ), std::string( attr ), func );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatValueFunc
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatValueFunc( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float(*func)(void) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValueFunc( std::string( pymodule ), std::string( attr ), func );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedInt32ValueFunc
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedInt32ValueFunc( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t(*func)(void) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValueFunc( std::string( pymodule ), std::string( attr ), func );
}

////////////////////////////////////////////////////////////////////////////////
//##############################################################################
///// Value function switch case
//##############################################################################
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedDoubleValueCase
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedDoubleValueCase( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, char *attrCase, double(*func)(const char*) )
{
#ifndef NDEBUG
  std::cout << __func__ << ": " <<  static_cast< void * >( *ppObj ) << std::endl;
#endif
  (*ppObj)->embedValueCase( std::string( pymodule ), std::string( attr ), std::string( attrCase ), func );
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
  (*ppObj)->embedValueCase( std::string( pymodule ), std::string( attr ), std::string( attrCase ), func );
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
  (*ppObj)->embedValueCase( std::string( pymodule ), std::string( attr ), std::string( attrCase ), func );
}

