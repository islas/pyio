import os
import numpy
import runtime_data
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

  print( type( runtime_data.arr() ) )
  print( runtime_data.arr().flags )

  print( demo.demoData() )
  print( runtime_data.arr() )

  runtime_data.arr()[5] = 999

  print( runtime_data.arr() )
  

