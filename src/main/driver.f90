! Main fortran code
program driver
  use iso_c_binding
  use EmbeddedInterpreter
  use f_c_string

  implicit none

  type( c_ptr )         :: interpreter
  real, dimension( 10 ) :: arr = [ 1:10 ]

  interpreter = EmbeddedInterpreter_ctor()

  EmbeddedInterpreter_initialize( interpreter )
  EmbeddedInterpreter_addToScope( interpreter,  f_c_string( "/glade/u/home/aislas/pythonBindings/demo/pymodules/" ) )
  
  
  ! Add embedded values
  EmbeddedInterpreter_embedFloatPtr( interpreter, f_c_string( "static_data" ), f_c_string( "arr" ), arr, 1, [ 10 ] )
  
  ! Use user module
  EmbeddedInterpreter_pymoduleLoad( interpreter,  f_c_string( "interp.euler" ) )
  EmbeddedInterpreter_pymoduleInitialize( interpreter,  f_c_string( "interp.euler" ) )

  EmbeddedInterpreter_pymoduleCall( interpreter,  f_c_string( "interp.euler" ) )


  EmbeddedInterpreter_pymoduleFinalize( interpreter,  f_c_string( "interp.euler" ) )

  ! finalize
  EmbeddedInterpreter_finalize( interpreter )
  
end program driver