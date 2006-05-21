-- gadgets window example for lua with siamiga.lib
-- 2006-01-03 Matthias Rustler

function drawstuff()
    win:setpen(2)
    win:box(70, 200, 200, 40, true) -- draw filled box
end

win = Siamiga.createwindow("Siamiga Test Window",0, 30, 400, 300, true, false)
gt="Caption"
g1 = win:addgadget(5, "button", 20, 10, 100, 15, "OK")
g2 = win:addgadget(7, "check", 100, 30, 100, 50, "Gad 2")
g3 = win:addgadget(8, "check", 100, 50, 100, 50, "Gad 3", true)
g4 = win:addgadget(9, "integer", 100, 70, 100, 20, "Gad 4", 25)
g5 = win:addgadget(10, "string", 100, 100, 100, 20, "Gad 5", "dada")
g6 = win:addgadget(11, "hslider", 100, 130, 100, 20, "Gad 6", 60, 30, 70)
g7 = win:addgadget(12, "vslider", 250, 50, 20, 100, "Gad 7", -10, -30, 50)
g8 = win:addgadget(13, "radio", 360, 50, 20, 100, "Gad 8", "tick", "trick", "track", 345, -345)
g9 = win:addgadget(14, "cycle", 300, 170, 80, 20, "Gad 9", "schni", "schna", "schnappi")
win:openwindow()
g8:set(3)
g9:set(3)
drawstuff()
done = false
while not done do
    win:waitmessage()
    while not done do
        class, code, mx, my = win:getmessage()
        if not class then
            break
        elseif class == "close" then
            print("Close")
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
        elseif class == "gadget" then
            print("Gadgetup", code )
            print("Gad2", g2:get())
            print("Gad3", g3:get())
            print("Gad4", g4:get())
            print("Gad5", g5:get())
            print("Gad6", g6:get())
            print("Gad7", g7:get())
            print("Gad8", g8:get())
            print("Gad9", g9:get())
        end
    end
end

win:closewindow()
