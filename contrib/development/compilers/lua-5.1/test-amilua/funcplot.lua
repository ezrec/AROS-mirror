-- simple function plotter with no clipping in y direction
-- uses Siamiga gadgets

xsl = 130
ysu = 300
xsr = 350
yso = 30

xwl = -6
ywu = -1.5
xwr = 10
ywo = 2

function world2screenX(xw)
    return (xw-xwl) / (xwr-xwl) * (xsr-xsl) + xsl
end

function world2screenY(yw)
    return (yw-ywo) / (ywu-ywo) * (ysu-yso) + yso
end

function f1(x)
    return math.sin(x)
end

function f2(x)
    return math.sin(x)/x
end

function drawfunc(func, pen)
    local xs,ys,step
    win:setpen(pen)
    first = true
    if xwr>xwl then
        step = 0.1
    else
        step = -0.1
    end
    for xw = xwl , xwr , step do
        yw = func(xw)
        xs = world2screenX(xw)
        ys = world2screenY(yw)
        if first then
            win:pset(xs,ys)
            first = false
        else
            win:line(xs,ys)
        end
    end
end

function draw(func, pen)
    local xs,ys,iw,ih

    _,_,_,_,iw,ih=win:querywindow()
    win:setpen(0)
    win:box(xsl, 0, iw, ih, true)
    xsr = iw - 15
    ysu = ih - 15

    if xwl == nil or xwr == nil or ywo == nil or ywu == nil or xsl>xsr or yso>ysu then
        -- silent exit, otherwise every refresh could cause an error
        return
    end

    xs = world2screenX(xwl)
    ys = world2screenY(ywo)
    win:setpen(1)
    win:box(xs,ys, world2screenX(xwr)-xs, world2screenY(ywu)-ys)
    xs = world2screenX(0)
    ys = world2screenY(0)
    if xs>xsl and xs<xsr then
        win:line(xs,yso,xs,ysu)
    end
    if ys>yso and ys<ysu then
        win:line(xsl,ys,xsr,ys)
    end
    drawfunc(f1, 2)
    drawfunc(f2, 3)
end

win = Siamiga.createwindow("Function Plotter", 0, 20, 450, 350, true, false)
gadxwl = win:addgadget(1, "string", 40, 30, 70, 25, "xwl", xwl)
gadywo = win:addgadget(2, "string", 40, 60, 70, 25, "ywo", ywo)
gadxwr = win:addgadget(3, "string", 40, 90, 70, 25, "xwr", xwr)
gadywu = win:addgadget(4, "string", 40, 120, 70, 25, "ywu", ywu)
gadapp = win:addgadget(6, "button", 40, 160, 70, 25, "Apply")

win:openwindow()
draw()

done = false
while not done do
    win:waitmessage()
    while not done do
        msg,gad = win:getmessage()
        if not msg then
            break       
        elseif msg=="close" then
            done = true
        elseif msg=="refresh" then
            print("refresh event")
            draw()
        elseif msg=="gadget" then
            print("gadget event")
            if gad==6 then
                xwl=tonumber(gadxwl:get())
                ywo=tonumber(gadywo:get())
                xwr=tonumber(gadxwr:get())
                ywu=tonumber(gadywu:get())
                if xwl == nil or xwr == nil or ywo == nil or ywu == nil then
                    win:messagebox("Error", "One of the gadgets doesn't contain a number", "OK")
                end
                draw()
            end
        elseif msg=="size" then
            print("size event")
            draw()
        end
    end
end
win:closewindow()
