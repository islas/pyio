#include "pybind11/pybind11.h"
#include "pybind11/embed.h"

PYBIND11_EMBEDDED_MODULE( static_data, m )
{
  // Place holder for adding to later
}

PYBIND11_EMBEDDED_MODULE( runtime_data, m )
{
  // Place holder for adding to later
}

PYBIND11_EMBEDDED_MODULE( helper, m )
{
  // Place holder for adding to later
}

PYBIND11_EMBEDDED_MODULE( demo, m )
{
  // Place holder for adding to later
}

#ifdef SOME_FEATURE
PYBIND11_EMBEDDED_MODULE( feature_mod, m )
{
  // Place holder for adding to later
}
#endif
