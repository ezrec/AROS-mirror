-- single window example for lua with siamiga.lib
-- 2006-01-03 Matthias Rustler

function drawstuff()
    local r
    win:box() --- clear window
    win:setpen(1) --- use default foreground pen
    win:text(3,14, "Press close button to quit")
    win:setpen(p1)
    win:line(20,30, 120, 70) --- draw line from x1,y1 to x2,y2
    win:line(180, 40) --- draw to x,y
    win:setpen(p2)
    win:box(70, 120, 40, 40) --- draw frame
    win:box(70, 200, 40, 40, true) --- draw filled box
    win:setpen(p3)
    for r = 20,50,3 do
        win:ellipse(300, 200, r, r) --- draw ellipse
    end
    win:pset(180, 200) --- plot a point
end

win = Siamiga.createwindow("Siamiga Test Window",0, 30, 400, 300, false)
-- title, left, top, widht, height, size gadget
win:openwindow()

print("Window dimensions", win:querywindow())
-- left, top, width, height, innerwidth, innerheight
print("Screen dimensions", Siamiga.queryscreen())

p1 = Siamiga.createpen(230,100,0)
p2 = Siamiga.createpen(0,200,150)
p3 = Siamiga.createpen(80,0,210)
drawstuff()
done = false
while not done do
    win:waitmessage()
    while not done do
        class, code, mx, my = win:getmessage()
        if not class then
            break
        elseif class == "close" then
            done = true
        elseif class == "mouse" then
            print("Mouse", code, mx, my)
        elseif class == "key" then
            print("Key", code)
        elseif class == "size" then
            drawstuff()
            print("Newsize")
            print("Window dimensions", win:querywindow())
        elseif class == "refresh" then
            drawstuff()
            print("Refreshwindow")
        end
    end
end

win:messagebox("Siamiga", "The End", "Ok")
Siamiga.releasepen(p1)
Siamiga.releasepen(p2)
Siamiga.releasepen(p3)
win:closewindow()

