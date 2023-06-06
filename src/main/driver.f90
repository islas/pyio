! Main fortran code
program driver
  use iso_c_binding
  use EmbeddedInterpreter
  use f_c_helpers

  implicit none

  type( c_ptr )         :: interpreter = c_null_ptr
  real, dimension( 10 ) :: arr = [ 1:10 ]
  integer( c_size_t ), dimension(1) :: dims = [ 10 ]
  integer( c_size_t )                :: numDims = 1

  call EmbeddedInterpreter_ctor( interpreter )
  

  call EmbeddedInterpreter_initialize( interpreter )
  call EmbeddedInterpreter_addToScope( interpreter,  f_c_string( "/glade/u/home/aislas/pythonBindings/demo/pymodules/" ) )
  
  
  ! Add embedded values
  call EmbeddedInterpreter_embedFloatPtr( interpreter, f_c_string( "static_data" ), f_c_string( "arr" ), arr, numDims, dims )
  
  ! Use user module
  call EmbeddedInterpreter_pymoduleLoad( interpreter,  f_c_string( "interp.euler" ) )
  call EmbeddedInterpreter_pymoduleInitialize( interpreter,  f_c_string( "interp.euler" ) )

  call EmbeddedInterpreter_pymoduleCall( interpreter,  f_c_string( "interp.euler" ) )


  call EmbeddedInterpreter_pymoduleFinalize( interpreter,  f_c_string( "interp.euler" ) )

  ! finalize
  call EmbeddedInterpreter_finalize( interpreter )

  ! delete
  call EmbeddedInterpreter_dtor( interpreter )
  
end program driver