class Color:
    def __init__( self, *args ):
        if len( args ) == 3:
            self.r = args[0]
            self.g = args[1]
            self.b = args[2]
        else:
            self.r = args[0][0]
            self.g = args[0][1]
            self.b = args[0][2]
        
    def __getitem__( self, i ):
        if   i == 0: 
            return self.r
        elif i == 1: 
            return self.g
        elif i == 2: 
            return self.b
        else:
            raise IndexError()
        
    def __str__( self ):
        return '#%02x%02x%02x' % (self.r, self.g, self.b)

    def blend( fg, bg, max, cur):
        rr = bg[0] + ((fg[0] - bg[0])*(255 * cur)/max)/255
        rg = bg[1] + ((fg[1] - bg[1])*(255 * cur)/max)/255
        rb = bg[2] + ((fg[2] - bg[2])*(255 * cur)/max)/255

        return Color( rr, rg, rb )
