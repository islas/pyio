
#include "EmbeddedInterpreter.hpp"

#include <vector>
#include <stirng>

#include "pybind11/pybind11.h"

EmbeddedInterpreter::PY_INITIALIZE_METHOD   = "initialize"
EmbeddedInterpreter::PY_FINALIZE_METHOD     = "finalize"
EmbeddedInterpreter::PY_MAIN_METHOD         = "main"

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
  // Set the ptr
  pGuard_.reset( new pybind11::scoped_interpreter() );

  // Import sys
  sys_ = pybind11::module_::import( "sys" );
  sysPathAppend_ = sys.attr( "path" ).attr( "append" );

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

  // Dump the ptr
  pGuard_.reset();
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
/// \brief Loads a pymodule into C++ scope to be used by C/C++/Fortran
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleLoad( std::string pymodule )
{
  pybind11::module_ loaded = pybind11::module_::import( pymodule );

  pymodules_[ pymodule ] = loaded;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calls a pymodule's initialization function, if any
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleInitialize( std::string pymodule )
{
  if ( pybind11::hasattr( pymodules_[ pymodule ], PY_INITIALIZE_METHOD ) )
  {
    pymodules_[ pymodule ].attr( PY_INITIALIZE_METHOD )();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calls a pymodule's finalization function, if any
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleFinalize( std::string pymodule )
{
  if ( pybind11::hasattr( pymodules_[ pymodule ], PY_FINALIZE_METHOD ) )
  {
    pymodules_[ pymodule ].attr( PY_FINALIZE_METHOD )();
  }
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Calls a pymodule's primary entry function
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::pymoduleCall( std::string pymodule )
{
  if ( pybind11::hasattr( pymodules_[ pymodule ], PY_MAIN_METHOD ) )
  {
    pymodules_[ pymodule ].attr( PY_MAIN_METHOD )();
  }
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
EmbeddedInterpreter *
EmbeddedInterpreter_ctor()
{
  return new EmbeddedInterpreter();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for "dtor"
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_dtor( EmbeddedInterpreter *this )
{
  delete this;
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for initialize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_initialize( EmbeddedInterpreter *this )
{
  this->initialize();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for finalize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_finalize  ( EmbeddedInterpreter *this )
{
  this->finalize()
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for addToScope
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_addToScope( EmbeddedInterpreter *this, char *cstr )
{
  this->addToScope( std::string( str ) );
}


////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleLoad
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleLoad( EmbeddedInterpreter *this, char *cstr )
{
  this->pymoduleLoad( std::string( str ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleInitialize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleInitialize( EmbeddedInterpreter *this, char *cstr )
{
  this->pymoduleInitialize( std::string( str ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleFinalize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleFinalize( EmbeddedInterpreter *this, char *cstr )
{
  this->pymoduleFinalize( std::string( str ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleCall
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleCall( EmbeddedInterpreter *this, char *cstr )
{
  this->pymoduleCall( std::string( str ) );
}
