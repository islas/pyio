! Fortran bindings
module EmbeddedInterpreter
  use, intrinsic :: iso_c_binding
  type( c_ptr ), public :: eimod_pEmbeddedInterpreter = c_null_ptr

  contains

  interface
    function EmbeddedInterpreter_ctor              ( ) result( ptr )    bind( c, name="EmbeddedInterpreter_ctor"               )
      ! return EmbeddedInterpreter *
      type( c_ptr ), intent( out ) :: ptr
    end function EmbeddedInterpreter_ctor

    function EmbeddedInterpreter_dtor              ( eiPtr )              bind( c, name="EmbeddedInterpreter_dtor"               )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end function EmbeddedInterpreter_dtor

    function EmbeddedInterpreter_initialize        ( eiPtr )              bind( c, name="EmbeddedInterpreter_initialize"         )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end function EmbeddedInterpreter_initialize

    function EmbeddedInterpreter_finalize          ( eiPtr )             bind( c, name="EmbeddedInterpreter_finalize"           )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      ! return void
    end function EmbeddedInterpreter_finalize

    function EmbeddedInterpreter_addToScope        ( eiPtr, directory )  bind( c, name="EmbeddedInterpreter_addToScope"         )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: directory
      ! return void
    end function EmbeddedInterpreter_addToScope

    function EmbeddedInterpreter_pymoduleLoad      ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleLoad"       )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end function EmbeddedInterpreter_pymoduleLoad

    function EmbeddedInterpreter_pymoduleInitialize( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleInitialize" )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end function EmbeddedInterpreter_pymoduleInitialize

    function EmbeddedInterpreter_pymoduleFinalize  ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleFinalize"   )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end function EmbeddedInterpreter_pymoduleFinalize

    function EmbeddedInterpreter_pymoduleCall      ( eiPtr, pymodule )   bind( c, name="EmbeddedInterpreter_pymoduleCall"       )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end function EmbeddedInterpreter_pymoduleCall

    function EmbeddedInterpreter_embedFloatPtr     ( eiPtr, pymodule, attr, ptr, numDims, dimSize ) bind( c, name="EmbeddedInterpreter_embedFloatPtr"      )
      implicit none
      type( c_ptr ), intent( in ) :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_float ),     dimension(*), intent( in ) :: ptr
      integer( c_size_t ),               intent( in ) :: numDims
      integer( c_size_t ), dimension(*), intent( in ) :: dimSize
      ! return void
    end function EmbeddedInterpreter_embedFloatPtr

  end interface
