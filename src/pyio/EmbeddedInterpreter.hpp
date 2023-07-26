
#ifndef EmbeddedInterpreter_hpp
#define EmbeddedInterpreter_hpp

#include <vector>
#include <string>
#include <memory>
#include <map>

#include "pybind11/pybind11.h"
#include "pybind11/embed.h"


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
  void pymoduleCall      ( std::string pymodule );

  // Embedded module loading
  void embeddedPymoduleLoad( std::string pymodule );

  // Building python-accesible modules - only operable on pymodules loaded from embedPymoduleLoad
  void embedDoublePtr ( std::string pymodule, std::string attr, double *ptr, size_t numDims, size_t *pDimSize );
  void embedFloatPtr  ( std::string pymodule, std::string attr, float  *ptr, size_t numDims, size_t *pDimSize );
  void embedFloatValueCase( std::string pymodule, std::string attr, std::string attrCase, float(*func)(const char*) );
  void embedInt32ValueCase( std::string pymodule, std::string attr, std::string attrCase, int32_t(*func)(const char*) );
  void embedFloatValue    ( std::string pymodule, std::string attr, float(*func)(void) );
  void embedInt32Value    ( std::string pymodule, std::string attr, int32_t(*func)(void) );

private:

  pybind11::scoped_interpreter        guard_;            ///< Directly maintain the lifetime of this guard within this scope
  std::vector< std::string >                             userDirectories_;   ///< User supplied locations for user python modules
  std::unordered_map< std::string, pybind11::module_ >   pymodules_;         ///< Map of pymodules loaded ready to be called
  std::unordered_map< std::string, pybind11::module_ >   pymodulesEmbedded_; ///< Map of embedded pymodules available to python

  // std::vector< PyThreadState * >  threadStates_;
  std::vector< PyGILState_STATE > gilStates_;
  PyThreadState                  *pMainThreadState_;

  // Python modules
  pybind11::module_   sys_;
  pybind11::function  sysPathAppend_;

};

extern "C"
{

void                  EmbeddedInterpreter_ctor      ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_dtor      ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_initialize( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_finalize  ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_threadingInit    ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_threadingStart   ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_threadingStop    ( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_threadingFinalize( EmbeddedInterpreter **ppObj );
void                  EmbeddedInterpreter_addToScope( EmbeddedInterpreter **ppObj, char *directory );
void                  EmbeddedInterpreter_pymoduleLoad        ( EmbeddedInterpreter **ppObj, char *pymodule );
void                  EmbeddedInterpreter_pymoduleCall        ( EmbeddedInterpreter **ppObj, char *pymodule );
void                  EmbeddedInterpreter_embeddedPymoduleLoad( EmbeddedInterpreter **ppObj, char *pymodule );
void                  EmbeddedInterpreter_embedDoublePtr      ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, double *ptr, size_t numDims, size_t *pDimSize );
void                  EmbeddedInterpreter_embedFloatPtr       ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float  *ptr, size_t numDims, size_t *pDimSize );
void                  EmbeddedInterpreter_embedFloatValueCase ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, char  *attrCase, float(*func)(const char*) );
void                  EmbeddedInterpreter_embedInt32ValueCase ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, char  *attrCase, int32_t(*func)(const char*) );
void                  EmbeddedInterpreter_embedFloatValue     ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, float(*func)(void) );
void                  EmbeddedInterpreter_embedInt32Value     ( EmbeddedInterpreter **ppObj, char *pymodule, char *attr, int32_t(*func)(void) );

}

#endif