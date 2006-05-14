win = Siamiga.createwindow("Picture test", 20, 30, 200, 200, true, true)
win:openwindow()

--pic = Sipicture.load("system:prefs/presets/patterns/misc/amoeba.png")
pic = Sipicture.load("test.iff")


function drawstuff()
    pic:put(win, 30, 30)
end

drawstuff()
pic2 = Sipicture.get(win, 50, 30 , 40, 40)
pic2:put(win, 100, 100)

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
