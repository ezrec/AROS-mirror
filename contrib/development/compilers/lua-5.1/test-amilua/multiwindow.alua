-- multi window example for lua with siamiga.lib
-- 2006-01-03 Matthias Rustler

function handleevents(win)
    local done = false
    while not done do
        class, code, mx, my = win:getmessage()
        if not class then
            return
        elseif class == "close" then
            win:closewindow()
            done = true
        elseif class == "mouse" then
            print("Mouse", code, mx, my)
        elseif class == "key" then
            print("Key", code)
        elseif class == "size" then
            win:draw()
            print("Newsize")
            print("Window dimensions", win:querywindow())
        elseif class == "refresh" then
            win:draw()
            print("Refreshwindow")
        end
    end
end

function drawstuff(win)
    local r
    win:box() -- clear window
    win:setpen(p1)
    win:line(20,30, 120, 70) -- draw line from x1,y1 to x2,y2
    win:line(180, 40) -- draw to x,y
    win:setpen(p2)
    win:box(70, 120, 40, 40) -- draw frame
    win:box(70, 200, 40, 40, true) -- draw filled box
    win:setpen(p3)
    for r = 20,50,3 do
        win:ellipse(300, 200, r, r) -- draw ellipse
    end
    win:pset(180, 200) -- plot a point
end

-- adding both functions to Siamiga table
Siamiga.handle = handleevents
Siamiga.draw = drawstuff

win1 = Siamiga.createwindow("Resizable Simple Refresh Window", 0, 30, 400, 300, true)
win1:openwindow()

win2 = Siamiga.createwindow("Resizable Smart Refresh Window", 400, 30, 400, 300, true, true)
win2:openwindow()

win3 = Siamiga.createwindow("Simple Refresh Window", 0, 330, 400, 300)
win3:openwindow()

win4 = Siamiga.createwindow("Smart Refresh Window", 400, 330, 400, 300, false, true)
win4:openwindow()

p1 = Siamiga.createpen(230,100,0)
p2 = Siamiga.createpen(0,200,150)
p3 = Siamiga.createpen(80,0,210)

win1:draw()
win2:draw()
win3:draw()
win4:draw()

while win1:waitmessage(win2, win3, win4) > 0 do -- while number of open windows > 0
    win1:handle()
    win2:handle()
    win3:handle()
    win4:handle()
end

win1:messagebox("Siamiga", "The End", "Ok")

Siamiga.releasepen(p1)
Siamiga.releasepen(p2)
Siamiga.releasepen(p3)
