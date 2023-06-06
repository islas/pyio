import os
import numpy
import static_data
import demo 

filename = ""
logstr   = "{file}:{func} : "
def initialize( ) :
  filename = os.path.splitext(os.path.basename(__file__))[0]
  print( logstr.format( file=filename, func=initialize.__name__ ) )

def finalize( ) :
  print( logstr.format( file=filename, func=finalize.__name__ ) )

def main( ) :
  print( logstr.format( file=filename, func=main.__name__ ) )
  print( "Hello Fortran!" )
  print( static_data )
  print( demo )
  print( demo.__dict__ )
  print( demo.demoData() )
  print( static_data.arr() )