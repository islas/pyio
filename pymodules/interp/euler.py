import os
filename = ""
logstr   = "{file}:{func} : "
def initialize( ) :
  filename = os.path.splitext(os.path.basename(__file__))[0]
  print( logstr.format( file=filename, func=initialize.__name__ ) )

def finalize( ) :
  print( logstr.format( file=filename, func=finalize.__name__ ) )

def main( ) :
  print( logstr.format( file=filename, func=main.__name__ ) )
  print( static_data.attr() )