
#include "EmbeddedInterpreter.hpp"

#include <vector>
#include <string>
#include <list>

#include "pybind11/pybind11.h"

EmbeddedInterpreter::PY_INITIALIZE_METHOD   = "initialize"
EmbeddedInterpreter::PY_FINALIZE_METHOD     = "finalize"
EmbeddedInterpreter::PY_MAIN_METHOD         = "main"

////////////////////////////////////////////////////////////////////////////////
/// \brief Ctor
////////////////////////////////////////////////////////////////////////////////
EmbeddedInterpreter::EmbeddedInterpreter()
{
  // Set the ptr
  pGuard_.reset( new pybind11::scoped_interpreter() );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Dtor
////////////////////////////////////////////////////////////////////////////////
EmbeddedInterpreter::~EmbeddedInterpreter()
{
  // Dump the ptr
  pGuard_.reset();
}

////////////////////////////////////////////////////////////////////////////////
/// \brief Initialize the embedded python interpreter
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::initialize()
{

  // Import sys
  sys_ = pybind11::module_::import( "sys" );
  sysPathAppend_ = sys.attr( "path" ).attr( "append" );

  // Generate embedded modules
  createBaseEmbeddedModules();

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
/// \brief "Creates" imports of the base modules provided
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::createBaseEmbeddedModules( )
{
  // No good way of gathering list of all embdedded modules dynamically
  // so we "just have to know" somehow

  std::list< std::string > modulesToLoad { 
                                            "static_data",
                                            "runtime_data",
                                            "helper",
#ifdef SOME_FEATURE
                                            "feature_mod",
#endif
                                          };

  for ( std::string &module : modulesToLoad )
  {
    pymodulesEmbedded_[ module ] = pybind11::module_::import( module );
  }
}


////////////////////////////////////////////////////////////////////////////////
/// \brief Builds into a module a float ptr to use
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter::embedFloatPtr( std::string pymodule, std::string attr, float *ptr, size_t numDims, size_t *pDimSize )
{

  // Get embedded module
  pybind11::module_ mod = pymodulesEmbedded_[ std::string( pymodule ) ];

  
  // Add attribute to it
  // mod.def(
  //         "value_ptr",
  //         // Lambda
  //         [](mat4& m) {
  //                       pybind11::str dummyDataOwner;
  //                       return pybind11::array_t< float >( 
  //                           pybind11::buffer_info(
  //                             ptr,                                                         // pointer to memory buffer
  //                             sizeof( ptr[0] )                                             // size of underlying scalar type
  //                             pybind11::format_descriptor< decltype( ptr[0] ) >::format(), // python struct-style format descriptor
  //                             numDims,                                                     // number of dimensions
  //                             { dims },                                                    // buffer dimensions
  //                             { striding }                                                 // strides (in bytes) for each index
  //                           )
  //         pybind11::return_value_policy::reference
  //         )

  mod.def(
          std::string( cstr ),
          // Lambda
          []() {
                // Create a Python object that will free the allocated
                // memory when destroyed:
                pybind11::capsule dummyDataOwner( foo, []( void *f )
                                                  {
                                                    float *foo = reinterpret_cast<float *>( f );
                                                    std::cerr << "Element [0] = " << foo[0] << "\n";
                                                    std::cerr << "!!!freeing memory @ " << f << "\n";
                                                    // delete[] foo; do nothing
                                                  }
                                                );
                return pybind11::array_t< decltype( ptr[0] ), pybind11::array::f_style | pybind11::array::forcecast >(
                          std::vector< decltype( ptr[0] ) >( pDimSize, pDimSize + numDims ), // shape
                          ptr,
                          dummyDataOwner
                        )
          },
          pybind11::return_value_policy::reference
          )


//   // C API Buffer protocol
//   .def_buffer([](Matrix &m) -> py::array_info {
//     return py::array_info(
//         m.data(),                                /* Pointer to buffer */
//         sizeof(Scalar),                          /* Size of one scalar */
//         py::format_descriptor<Scalar>::format(), /* Python struct-style format descriptor */
//         2,                                       /* Number of dimensions */
//         { m.rows(), m.cols() },                  /* Buffer dimensions */
//         { sizeof(Scalar) * (rowMajor ? m.cols() : 1),
//           sizeof(Scalar) * (rowMajor ? 1 : m.rows()) }
//                                                  /* Strides (in bytes) for each index */
//     );
//  })
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
EmbeddedInterpreter_addToScope( EmbeddedInterpreter *this, char *directory )
{
  this->addToScope( std::string( directory ) );
}


////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleLoad
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleLoad( EmbeddedInterpreter *this, char *pymodule )
{
  this->pymoduleLoad( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleInitialize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleInitialize( EmbeddedInterpreter *this, char *pymodule )
{
  this->pymoduleInitialize( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleFinalize
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleFinalize( EmbeddedInterpreter *this, char *pymodule )
{
  this->pymoduleFinalize( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for pymoduleCall
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_pymoduleCall( EmbeddedInterpreter *this, char *pymodule )
{
  this->pymoduleCall( std::string( pymodule ) );
}

////////////////////////////////////////////////////////////////////////////////
/// \brief C binding for embedFloatPtr
////////////////////////////////////////////////////////////////////////////////
void
EmbeddedInterpreter_embedFloatPtr( EmbeddedInterpreter *this, char *pymodule, char *attr, float *ptr, size_t numDims, size_t *pDimSize )
{
  this->embedFloatPtr( std::string( pymodule ), std::string( attr ), ptr, numDims, pDimSize );
}