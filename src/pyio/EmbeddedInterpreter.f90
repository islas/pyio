! Fortran bindings
module EmbeddedInterpreter
  use, intrinsic :: iso_c_binding
  implicit none
  type( c_ptr ), public :: eimod_pEmbeddedInterpreter = c_null_ptr

  interface
    
    subroutine EmbeddedInterpreter_ctor              ( eiPtr )              &
      bind( c, name="EmbeddedInterpreter_ctor"               )
      ! get iso_c_binding types
      import
      implicit none
      ! return EmbeddedInterpreter *
      type( c_ptr ), intent( inout ) :: eiPtr
    end subroutine EmbeddedInterpreter_ctor

    subroutine EmbeddedInterpreter_dtor              ( eiPtr )              &
      bind( c, name="EmbeddedInterpreter_dtor"               )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), intent( inout ) :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_dtor

    subroutine EmbeddedInterpreter_initialize        ( eiPtr )              &
      bind( c, name="EmbeddedInterpreter_initialize"         )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_initialize

    subroutine EmbeddedInterpreter_finalize          ( eiPtr )             &
      bind( c, name="EmbeddedInterpreter_finalize"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_finalize

    subroutine EmbeddedInterpreter_threadingInit          ( eiPtr )             &
      bind( c, name="EmbeddedInterpreter_threadingInit"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingInit

    subroutine EmbeddedInterpreter_threadingStart          ( eiPtr )             &
      bind( c, name="EmbeddedInterpreter_threadingStart"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingStart

    subroutine EmbeddedInterpreter_threadingStop          ( eiPtr )             &
      bind( c, name="EmbeddedInterpreter_threadingStop"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingStop

    subroutine EmbeddedInterpreter_threadingFinalize          ( eiPtr )             &
      bind( c, name="EmbeddedInterpreter_threadingFinalize"           )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      ! return void
    end subroutine EmbeddedInterpreter_threadingFinalize

    subroutine EmbeddedInterpreter_addToScope        ( eiPtr, directory )  &
      bind( c, name="EmbeddedInterpreter_addToScope"         )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: directory
      ! return void
    end subroutine EmbeddedInterpreter_addToScope

    subroutine EmbeddedInterpreter_pymoduleLoad      ( eiPtr, pymodule )   &
      bind( c, name="EmbeddedInterpreter_pymoduleLoad"       )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleLoad

    subroutine EmbeddedInterpreter_pymoduleCall      ( eiPtr, pymodule, func )   &
      bind( c, name="EmbeddedInterpreter_pymoduleCall"       )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_pymoduleCall

    subroutine EmbeddedInterpreter_embeddedPymoduleLoad      ( eiPtr, pymodule )   &
      bind( c, name="EmbeddedInterpreter_embeddedPymoduleLoad"       )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      ! return void
    end subroutine EmbeddedInterpreter_embeddedPymoduleLoad

    !////////////////////////////////////////////////////////////////////////////
    !//##########################################################################
    !///// Ptr
    !//##########################################################################
    !////////////////////////////////////////////////////////////////////////////
    subroutine EmbeddedInterpreter_embedDoublePtr     ( eiPtr, pymodule, attr, ptr, numDims, dimSize ) &
      bind( c, name="EmbeddedInterpreter_embedDoublePtr"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_double ),    dimension(*), intent( in ) :: ptr
      integer( c_size_t ), value,        intent( in ) :: numDims
      integer( c_size_t ), dimension(*), intent( in ) :: dimSize
      ! return void
    end subroutine EmbeddedInterpreter_embedDoublePtr

    subroutine EmbeddedInterpreter_embedFloatPtr     ( eiPtr, pymodule, attr, ptr, numDims, dimSize ) &
      bind( c, name="EmbeddedInterpreter_embedFloatPtr"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_float ),     dimension(*), intent( in ) :: ptr
      integer( c_size_t ), value,        intent( in ) :: numDims
      integer( c_size_t ), dimension(*), intent( in ) :: dimSize
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatPtr

    subroutine EmbeddedInterpreter_embedInt32Ptr     ( eiPtr, pymodule, attr, ptr, numDims, dimSize ) &
      bind( c, name="EmbeddedInterpreter_embedInt32Ptr"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      integer( c_int32_t ),dimension(*), intent( in ) :: ptr
      integer( c_size_t ), value,        intent( in ) :: numDims
      integer( c_size_t ), dimension(*), intent( in ) :: dimSize
      ! return void
    end subroutine EmbeddedInterpreter_embedInt32Ptr

    !////////////////////////////////////////////////////////////////////////////
    !//##########################################################################
    !///// Ptr but map scalars
    !//##########################################################################
    !////////////////////////////////////////////////////////////////////////////
    subroutine EmbeddedInterpreter_embedDoublePtrScalar( eiPtr, pymodule, attr, ptr ) &
      bind( c, name="EmbeddedInterpreter_embedDoublePtrScalar" )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_double ), intent( in ) :: ptr
      ! return void
    end subroutine EmbeddedInterpreter_embedDoublePtrScalar

    subroutine EmbeddedInterpreter_embedFloatPtrScalar( eiPtr, pymodule, attr, ptr ) &
      bind( c, name="EmbeddedInterpreter_embedFloatPtrScalar" )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_float ), intent( in ) :: ptr
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatPtrScalar

    subroutine EmbeddedInterpreter_embedInt32PtrScalar( eiPtr, pymodule, attr, ptr ) &
      bind( c, name="EmbeddedInterpreter_embedInt32PtrScalar" )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      integer( c_int32_t ), intent( in ) :: ptr
      ! return void
    end subroutine EmbeddedInterpreter_embedInt32PtrScalar

    !////////////////////////////////////////////////////////////////////////////
    !//##########################################################################
    !///// Value - copy
    !//##########################################################################
    !////////////////////////////////////////////////////////////////////////////
    subroutine EmbeddedInterpreter_embedDoubleValue     ( eiPtr, pymodule, attr, val ) &
      bind( c, name="EmbeddedInterpreter_embedDoubleValue"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_double ), intent( in ), value :: val
      ! return void
    end subroutine EmbeddedInterpreter_embedDoubleValue

    subroutine EmbeddedInterpreter_embedFloatValue     ( eiPtr, pymodule, attr, val ) &
      bind( c, name="EmbeddedInterpreter_embedFloatValue"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      real( c_float ), intent( in ), value :: val
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatValue

    subroutine EmbeddedInterpreter_embedInt32Value     ( eiPtr, pymodule, attr, val ) &
      bind( c, name="EmbeddedInterpreter_embedInt32Value"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      integer( c_int32_t ), intent( in ), value :: val
      ! return void
    end subroutine EmbeddedInterpreter_embedInt32Value

    !////////////////////////////////////////////////////////////////////////////
    !//##########################################################################
    !///// Value - from function
    !//##########################################################################
    !////////////////////////////////////////////////////////////////////////////
    subroutine EmbeddedInterpreter_embedDoubleValueFunc     ( eiPtr, pymodule, attr, func ) &
      bind( c, name="EmbeddedInterpreter_embedDoubleValueFunc"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedDoubleValueFunc

    subroutine EmbeddedInterpreter_embedFloatValueFunc     ( eiPtr, pymodule, attr, func ) &
      bind( c, name="EmbeddedInterpreter_embedFloatValueFunc"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatValueFunc

    subroutine EmbeddedInterpreter_embedInt32ValueFunc     ( eiPtr, pymodule, attr, func ) &
      bind( c, name="EmbeddedInterpreter_embedInt32ValueFunc"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedInt32ValueFunc

    !////////////////////////////////////////////////////////////////////////////
    !//##########################################################################
    !///// Value - from function with string case
    !//##########################################################################
    !////////////////////////////////////////////////////////////////////////////
    subroutine EmbeddedInterpreter_embedDoubleValueCase     ( eiPtr, pymodule, attr, attrCase, func ) &
      bind( c, name="EmbeddedInterpreter_embedDoubleValueCase"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      character( kind = c_char ), dimension(*), intent( in ) :: attrCase
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedDoubleValueCase

    subroutine EmbeddedInterpreter_embedFloatValueCase     ( eiPtr, pymodule, attr, attrCase, func ) &
      bind( c, name="EmbeddedInterpreter_embedFloatValueCase"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      character( kind = c_char ), dimension(*), intent( in ) :: attrCase
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedFloatValueCase

    subroutine EmbeddedInterpreter_embedInt32ValueCase     ( eiPtr, pymodule, attr, attrCase, func ) &
      bind( c, name="EmbeddedInterpreter_embedInt32ValueCase"      )
      ! get iso_c_binding types
      import
      implicit none
      type( c_ptr ), value :: eiPtr
      character( kind = c_char ), dimension(*), intent( in ) :: pymodule
      character( kind = c_char ), dimension(*), intent( in ) :: attr
      character( kind = c_char ), dimension(*), intent( in ) :: attrCase
      type( c_funptr ), value, intent( in ) :: func
      ! return void
    end subroutine EmbeddedInterpreter_embedInt32ValueCase

  end interface

  interface EmbeddedInterpreter_embedPtr
    procedure EmbeddedInterpreter_embedDoublePtr,       &
              EmbeddedInterpreter_embedFloatPtr,        &
              EmbeddedInterpreter_embedInt32Ptr,        &
              EmbeddedInterpreter_embedDoublePtrScalar, &
              EmbeddedInterpreter_embedFloatPtrScalar,  &
              EmbeddedInterpreter_embedInt32PtrScalar
  end interface EmbeddedInterpreter_embedPtr

  contains

end module EmbeddedInterpreter
