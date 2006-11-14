-- shows simple recursive formula
-- uses Siamiga gadgets

xsl = 150
ysu = 300
xsr = 350
yso = 50

xwl = 2
ywu = 0
xwr = 4
ywo = 1

miniter = 100
maxiter = 1000
ystart = 0.5

function world2screenX(xw)
    return (xw-xwl) / (xwr-xwl) * (xsr-xsl) + xsl
end
function screen2worldX(xs)
    return (xs-xsl) / (xsr-xsl) * (xwr-xwl) + xwl
end

function world2screenY(yw)
    return (yw-ywo) / (ywu-ywo) * (ysu-yso) + yso
end

function screen2worldY(ys)
    return (ys-yso) / (ysu-yso) * (ywu-ywo) + ywo
end

function drawfunc(func)
    local xs,ys,xw,yw
    win:setpen(1)
    for xs = xsl, xsr do
        xw = screen2worldX(xs)
        yw = ystart
        for iter = 1, maxiter do
            yw = xw * yw * (1 - yw)
            ys = world2screenY(yw)
            if iter>miniter and ys>yso and ys<ysu then
                win:pset(xs,ys)
            end
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

    win:setpen(1)
    win:text(10,14, "Draw box with left mouse button to zoom in.")
    win:text(10,30, "Click with right mouse button to reset.")
    xs = world2screenX(xwl)
    ys = world2screenY(ywo)
    win:setpen(2)
    win:box(xs,ys, world2screenX(xwr)-xs, world2screenY(ywu)-ys, true)
    xs = world2screenX(0)
    ys = world2screenY(0)
    if xs>xsl and xs<xsr then
        win:line(xs,yso,xs,ysu)
    end
    if ys>yso and ys<ysu then
        win:line(xsl,ys,xsr,ys)
    end
    drawfunc()
end

win = Siamiga.createwindow("Function Plotter", 0, 20, 450, 350, true, true)
gadxwl = win:addgadget(1, "string", 60, 50, 70, 25, "xwl", xwl)
gadywo = win:addgadget(2, "string", 60, 80, 70, 25, "ywo", ywo)
gadxwr = win:addgadget(3, "string", 60, 110, 70, 25, "xwr", xwr)
gadywu = win:addgadget(4, "string", 60, 140, 70, 25, "ywu", ywu)
gadyst = win:addgadget(5, "string", 60, 170, 70, 25, "Ystart", ystart)
gadmaxit = win:addgadget(6, "integer", 60, 200, 70, 25, "Max. Iter", maxiter)
gadminit = win:addgadget(7, "integer", 60, 230, 70, 25, "Min. Iter", miniter)

gadapp = win:addgadget(10, "button", 60, 270, 70, 25, "Apply")

win:openwindow()
draw()

done = false
while not done do
    win:waitmessage()
    while not done do
        msg,gad,mx,my = win:getmessage()
	if not msg then
            break
        elseif msg=="close" then
            done = true
        elseif msg=="refresh" then
            print("refresh event")
            draw()
        elseif msg=="gadget" then
            print("gadget event")
            if gad==10 then
                xwl=tonumber(gadxwl:get())
                ywo=tonumber(gadywo:get())
                xwr=tonumber(gadxwr:get())
                ywu=tonumber(gadywu:get())
                ystart = tonumber(gadyst:get())
                maxiter = tonumber(gadmaxit:get())
                miniter = tonumber(gadminit:get())
                if xwl == nil or xwr == nil or ywo == nil or ywu == nil or ystart == nil
                        or maxiter == nil or miniter == nil then
                    win:messagebox("Error", "One of the gadgets doesn't contain a number", "OK")
                end
                draw()
            end
        elseif msg=="mouse" then
            if gad == "ld" then
                xwlnew = screen2worldX(mx)
                ywonew = screen2worldY(my)
            elseif gad == "lu" then
                xwr = screen2worldX(mx)
                ywu = screen2worldY(my)
                xwl = xwlnew
                ywo = ywonew
                gadxwl:set(xwl)
                gadywo:set(ywo)
                gadxwr:set(xwr)
                gadywu:set(ywu)
                draw()
            elseif gad == "rd" then
                xwl = 2
                ywo = 1
                xwr = 4
                ywu = 0
                gadxwl:set(xwl)
                gadywo:set(ywo)
                gadxwr:set(xwr)
                gadywu:set(ywu)
                draw()
            end
        elseif msg=="size" then
            print("size event")
            draw()
        end
    end
end
win:closewindow()

