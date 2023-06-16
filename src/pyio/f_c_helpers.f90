! https://fortran-lang.discourse.group/t/best-practices-for-passing-c-strings/104/15
module f_c_helpers

contains

  function f_c_string( string, asis )
    use, intrinsic :: iso_c_binding, only: c_char,c_null_char

    character( len=* ), intent( in ) :: string
    logical, intent( in ), optional  :: asis

    character( kind=c_char, len=: ), allocatable :: f_c_string
    logical :: asis_

    asis_ = .false.
    if ( present( asis ) ) asis_ = asis

    if ( asis_ ) then
      f_c_string = string//c_null_char
    else
      f_c_string = trim( string )//c_null_char
    end if

  end function

  subroutine c_f_string( string, f_string )
    use, intrinsic :: iso_c_binding, only: c_char,c_null_char
    
    character(len=1,kind=C_char), intent(in) :: string(*)
    character(len=*), intent(out) :: f_string
    integer :: i
    i=1
    do while( string(i) /= c_null_char .and. i<=len(f_string))
      f_string(i:i) = string(i)
      i=i+1
    end do
    if (i<len(f_string)) f_string(i:) = ' '
  end subroutine c_f_string

end module f_c_helpers