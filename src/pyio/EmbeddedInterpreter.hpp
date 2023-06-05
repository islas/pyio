
#ifndef EmbeddedInterpreter_hpp
#define EmbeddedInterpreter_hpp

#include <vector>
#include <string>
#include <memory>

#include "pybind11/pybind11.h"

class EmbeddedInterpreter
{
public:
  // Ctor Dtor
  EmbeddedInterpreter();
  virtual ~EmbeddedInterpreter();

  void initialize();
  void finalize();

  void addToScope( std::string directory );

  // Module handling
  void pymoduleLoad      ( std::string pymodule );
  void pymoduleInitialize( std::string pymodule );
  void pymoduleFinalize  ( std::string pymodule );

  void pymoduleCall      ( std::string pymodule );


  // Building python-accesible modules
  void embedFloatPtr( std::string pymodule, std::string attr, float *ptr, size_t numDims, size_t *pDimSize )

private:

  void createBaseEmbeddedModules( );

  static const std::string PY_INITIALIZE_METHOD;
  static const std::string PY_FINALIZE_METHOD;
  static const std::string PY_MAIN_METHOD;

  std::unique_ptr< pybind11::scoped_interpreter >       *pGuard_;            ///< Directly maintain the lifetime of this guard within this scope
  std::vector< std::string >                             userDirectories_;   ///< User supplied locations for user python modules
  std::unordered_map< std::string, pybind11::module_ >   pymodules_;         ///< Map of pymodules loaded ready to be called
  std::unordered_map< std::string, pybind11::module_ >   pymodulesEmbedded_; ///< Map of embedded pymodules available to python


  // Python modules
  pybind11::module_   sys_;
  pybind11::function  sysPathAppend_;

};

extern "C"
{

EmbeddedInterpreter * EmbeddedInterpreter_ctor      ();
void                  EmbeddedInterpreter_dtor      ( EmbeddedInterpreter *this );
void                  EmbeddedInterpreter_initialize( EmbeddedInterpreter *this );
void                  EmbeddedInterpreter_finalize  ( EmbeddedInterpreter *this );
void                  EmbeddedInterpreter_addToScope( EmbeddedInterpreter *this, char *directory );
void                  EmbeddedInterpreter_pymoduleLoad      ( EmbeddedInterpreter *this, char *pymodule );
void                  EmbeddedInterpreter_pymoduleInitialize( EmbeddedInterpreter *this, char *pymodule );
void                  EmbeddedInterpreter_pymoduleFinalize  ( EmbeddedInterpreter *this, char *pymodule );
void                  EmbeddedInterpreter_pymoduleCall      ( EmbeddedInterpreter *this, char *pymodule );
void                  EmbeddedInterpreter_embedFloatPtr     ( EmbeddedInterpreter *this, char *pymodule, char *attr, float *ptr, size_t numDims, size_t *pDimSize )

}

#endif