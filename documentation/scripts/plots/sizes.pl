#set xlbl = "Date"
#set ylbl = "Size in MB"

#setifnotgiven yinc   = 5
#setifnotgiven xrange = ""

#proc datesettings
    format: dd.mm.yyyy

#proc getdata
    file: @data
    fieldnames: date size path
    filter: ##set SIZE = $arith(@@size/1024)
            @@date @@SIZE

#proc page
    backgroundcolor: grey(0.8)
    tightcrop: yes

#proc areadef
    rectangle: 1 1 5 3
    xscaletype: date
    #if @xrange != ""
        xrange: @xrange  
    #else
        xautorange: datafield=date nearest=day
    #endif
    yautorange: datafield=size incmult=2.0 

#proc yaxis
    stubs: inc @yinc
    label: Size in MB
    labeldetails: adjust=-0.1,0
    grid: color=grey(0.5) style=1

#proc xaxis
    label: Date
    stubs: inc 1 month
    #if @xrange != ""
        // We assume 6 months view here.
        stubformat: yyyy-mm
    #else
        stubformat: M
        autoyears: yyyy
        labeldetails: adjust=0,-0.15
    #endif
    grid: color=grey(0.5) style=1
    

#proc annotate
    #set XCENTER = $arith(@AREARIGHT-@AREALEFT/2.0)
    #set XCENTER = $arith(@AREALEFT+@XCENTER)
    #set TOP = $arith(@AREATOP+0.2)
    location: @XCENTER @TOP
    // textdetails:  
    text: @title

#proc lineplot
    xfield: date
    yfield: size
    fill: powderblue

#proc lineplot
    xfield: date
    yfield: size
    linedetails: color=blue
