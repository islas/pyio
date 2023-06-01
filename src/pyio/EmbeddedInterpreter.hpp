
#ifndef EmbeddedInterpreter_hpp
#define EmbeddedInterpreter_hpp

#include <vector>
#include <stirng>

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

private:

  static const std::string PY_INITIALIZE_METHOD;
  static const std::string PY_FINALIZE_METHOD;
  static const std::string PY_MAIN_METHOD;

  std::unique_ptr< pybind11::scoped_interpreter >       *pGuard_;           ///< Directly maintain the lifetime of this guard within this scope
  std::vector< std::string >                             userDirectories_;  ///< User supplied locations for user python modules
  std::unordered_map< std::string, pybind11::module_ >   pymodules_;        ///< Map of pymodules loaded ready to be called

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
void                  EmbeddedInterpreter_addToScope( EmbeddedInterpreter *this, char *cstr );
void                  EmbeddedInterpreter_pymoduleLoad      ( EmbeddedInterpreter *this, char *cstr );
void                  EmbeddedInterpreter_pymoduleInitialize( EmbeddedInterpreter *this, char *cstr );
void                  EmbeddedInterpreter_pymoduleFinalize  ( EmbeddedInterpreter *this, char *cstr );
void                  EmbeddedInterpreter_pymoduleCall      ( EmbeddedInterpreter *this, char *cstr );

}

#endif