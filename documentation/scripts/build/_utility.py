class Borg:
    __hive_mind = {}

    def __init__( self ):
        self.__dict__ = self.__hive_mind


