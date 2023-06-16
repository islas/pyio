! Fortran bindings
module EmbeddedInterpreter
  use, intrinsic :: iso_c_binding
  implicit none
  type( c_ptr ), public :: eimod_pEmbeddedInterpreter = c_null_ptr

  interface
    
    subroutine EmbeddedInterpreter_ctor              ( eiPtr )              bind( c, name="EmbeddedInterpreter_ctor"               )
      ! get iso_c_binding types
      import
      implicit none
      ! return EmbeddedInterpreter *
      type( c_ptr ), intent( inout ) :: eiPtr
    end subroutine EmbeddedInterpreter_ctor

    subroutine EmbeddedInterpreter_dtor              ( eiPtr )              bind( c, name="EmbeddedInterpreter_dtor"               )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( inout ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_dtor

    subroutine EmbeddedInterpreter_initialize        ( eiPtr )              bind( c, name="EmbeddedInterpreter_initialize"         )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_initialize

    subroutine EmbeddedInterpreter_finalize          ( eiPtr )             bind( c, name="EmbeddedInterpreter_finalize"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_finalize

    subroutine EmbeddedInterpreter_threadingInit          ( eiPtr )             bind( c, name="EmbeddedInterpreter_threadingInit"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingInit

    subroutine EmbeddedInterpreter_threadingStart          ( eiPtr )             bind( c, name="EmbeddedInterpreter_threadingStart"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingStart

    subroutine EmbeddedInterpreter_threadingStop          ( eiPtr )             bind( c, name="EmbeddedInterpreter_threadingStop"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingStop

    subroutine EmbeddedInterpreter_threadingFinalize          ( eiPtr )             bind( c, name="EmbeddedInterpreter_threadingFinalize"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingFinalize

    subroutine EmbeddedInterpreter_addToScope        ( eiPtr, directory )  bind( c, name="EmbeddedInterpreter_addToScope"         )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: directory
      ! return void
    end subroutine EmbeddedInterpreter_addToScope

    subroutine EmbeddedInterpreter_pymoduleLoad      ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleLoad"       )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleLoad

    subroutine EmbeddedInterpreter_pymoduleInitialize( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleInitialize" )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleInitialize

    subroutine EmbeddedInterpreter_pymoduleFinalize  ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleFinalize"   )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleFinalize

    subroutine EmbeddedInterpreter_pymoduleCall      ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleCall"       )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleCall

    subroutine EmbeddedInterpreter_embedFloatPtr     ( eiPtr, pymodule, attr, ptr, numDims, dimSize ) bind( c, name="EmbeddedInterpreter_embedFloatPtr"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_float ),     dimension(*), intent( in ) :: ptr
      integer( c_size_t ), value,        intent( in ) :: numDims
      integer( c_size_t ), dimension(*), intent( in ) :: dimSize
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatPtr

  end interface


  contains

end module EmbeddedInterpreter
