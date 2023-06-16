import os
import numpy
import runtime_data
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

  arr = runtime_data.arr()

  # print( type( arr ) )
  # print( arr.flags )

  # print( demo.demoData() )
  # print( arr )
  

  # arr[5] = 999
  # arr[2] = static_data.getDemo1()
  # arr[1] = static_data.getDemo2()
  # arr[0] = static_data.getDemo3()
  id = runtime_data.omp_id()

  if id < arr.size :
    print( "Writing from thread {}".format( id ) )
    arr[id] = id
 

  # print( runtime_data.arr() )
  

