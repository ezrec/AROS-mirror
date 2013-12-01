/*                            Open Volume                      */

addlib('rexxreqtools.library',0,-30)

viewmode=2

Parse Arg name

directory=rtfilerequest(,,'Select Volume To Open',,'rtfi_volumerequest=0',)
If directory ~= "" Then
   address command "scalos:tools/opendrawer " directory 'VM=' || viewmode



