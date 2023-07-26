! Test module
module Demo
  use iso_c_binding
  implicit none
contains
  function getRegularFloatValue( attrCase ) bind( C ) result( attr )
    use f_c_helpers
    character( kind = c_char), dimension(*), intent( in ) :: attrCase
    character( len=64 ) :: attrCaseFortran
    real( c_float ) :: attr

    call c_f_string( attrCase, attrCaseFortran )

    ! Demo selection
    select case ( attrCaseFortran )
      case ( "demo1" )
        attr = 1
      case ( "demo2" )
        attr = 2
      case ( "demo3" )
        attr = 3
      case default
        write( *, * ) "Error: Unhandled case :("
      end select
  end function getRegularFloatValue

  function getRegularInt32Value( attrCase ) bind( C ) result( attr )
    use f_c_helpers
    use omp_lib
    character( kind = c_char), dimension(*), intent( in ) :: attrCase
    character( len=64 ) :: attrCaseFortran
    real( c_int32_t ) :: attr

    call c_f_string( attrCase, attrCaseFortran )

    select case ( attrCaseFortran )
      case ( "omp" )
#ifdef _OPENMP
        attr = 1
#else
        attr = 0
#endif
      case ( "omp_get_thread_num" )
#ifdef _OPENMP
        attr = omp_get_thread_num()
#else
        attr = -1
#endif
      case default
        write( *, * ) "Error: Unhandled case :("
      end select

  end function getRegularInt32Value

end module Demo


! Main fortran code
program driver
  use iso_c_binding
  use EmbeddedInterpreter
  use f_c_helpers
  use Demo
#ifdef _OPENMP
  use omp_lib
#endif

  implicit none

  type( c_ptr )         :: interpreter = c_null_ptr
  real, dimension( 10 ) :: arr = [ 1:10 ]
  integer( c_size_t ), dimension(1) :: dims = [ 10 ]
  integer( c_size_t )               :: numDims = 1
  integer                           :: i
  integer                           :: id
#include "built_in_path.inc"

  arr(:) = 0
  

  call EmbeddedInterpreter_ctor( interpreter )
  

  call EmbeddedInterpreter_initialize( interpreter )
  call EmbeddedInterpreter_addToScope( interpreter,  f_c_string( BUILT_IN_PATH ) )

  call EmbeddedInterpreter_embeddedPymoduleLoad( "runtime_data" )
  call EmbeddedInterpreter_embeddedPymoduleLoad( "static_data"  )
  
  
  ! Add embedded values
  call EmbeddedInterpreter_embedFloatPtr( interpreter, f_c_string( "runtime_data" ), f_c_string( "arr" ), arr, numDims, dims )
  call EmbeddedInterpreter_embedFloatValue( interpreter, f_c_string( "static_data" ), f_c_string( "getDemo1" ), f_c_string( "demo1" ), c_funloc( getRegularFloatValue ) )
  call EmbeddedInterpreter_embedFloatValue( interpreter, f_c_string( "static_data" ), f_c_string( "getDemo2" ), f_c_string( "demo2" ), c_funloc( getRegularFloatValue ) )
  call EmbeddedInterpreter_embedFloatValue( interpreter, f_c_string( "static_data" ), f_c_string( "getDemo3" ), f_c_string( "demo3" ), c_funloc( getRegularFloatValue ) )


  call EmbeddedInterpreter_embedInt32Value( interpreter, f_c_string( "runtime_data" ), f_c_string( "omp_enabled" ), f_c_string( "omp" ), c_funloc( getRegularInt32Value ) )
  call EmbeddedInterpreter_embedInt32Value( interpreter, f_c_string( "runtime_data" ), f_c_string( "omp_id" ), f_c_string( "omp_get_thread_num" ), c_funloc( getRegularInt32Value ) )


  ! Use user module
  call EmbeddedInterpreter_pymoduleLoad( interpreter,  f_c_string( "interp.euler" ) )

  ! Typical steps to be done - init, then call as needed, fin
  call EmbeddedInterpreter_pymoduleCall( interpreter,  f_c_string( "interp.euler" ), f_c_string( "initialize" ) )

  call EmbeddedInterpreter_threadingInit( interpreter )
  ! Do some parallel processing
  !$OMP PARALLEL DO
  do i = 1, size( arr )
#ifdef _OPENMP
    write( *, * ) "OMP ID Fortran:  ", omp_get_thread_num()
#endif
    
    call EmbeddedInterpreter_threadingStart( interpreter )
    call EmbeddedInterpreter_pymoduleCall( interpreter,  f_c_string( "interp.euler" ), f_c_string( "main" ) )
    call EmbeddedInterpreter_threadingStop( interpreter )
  
  end do
  !$OMP END PARALLEL DO
  call EmbeddedInterpreter_threadingFinalize( interpreter )
  call sleep(5)

  call EmbeddedInterpreter_pymoduleCall( interpreter,  f_c_string( "interp.euler" ), f_c_string( "finalize" ) )

  ! finalize
  call EmbeddedInterpreter_finalize( interpreter )

  ! delete
  call EmbeddedInterpreter_dtor( interpreter )

  write( *, * ) "From Fortran : "
  ! See what happened
  do i = 1, size( arr )
    write( *, * ) arr(i)
  end do
end program driver