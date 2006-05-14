-- life.lua
-- original by Dave Bollinger <DBollinger@compuserve.com> posted to lua-l
-- modified to use Siamiga library

tilesize = 10

function delay() -- NOTE: SYSTEM-DEPENDENT, adjust as necessary
  local i=os.clock()+0.2
  while(os.clock()<i) do end
end

function ARRAY2D(w,h)
  local t = {w=w,h=h}
  for y=1,h do
    t[y] = {}
    for x=1,w do
      t[y][x]=0
    end
  end
  return t
end

_CELLS = {}

-- give birth to a "shape" within the cell array
function _CELLS:spawn(shape,left,top)
  for y=0,shape.h-1 do
    for x=0,shape.w-1 do
      self[top+y][left+x] = shape[y*shape.w+x+1]
    end
  end
end

-- run the CA and produce the next generation
function _CELLS:evolve(next)
  local ym1,y,yp1,yi=self.h-1,self.h,1,self.h
  while yi > 0 do
    local xm1,x,xp1,xi=self.w-1,self.w,1,self.w
    while xi > 0 do
      local sum = self[ym1][xm1] + self[ym1][x] + self[ym1][xp1] +
                  self[y][xm1] + self[y][xp1] +
                  self[yp1][xm1] + self[yp1][x] + self[yp1][xp1]
      next[y][x] = ((sum==2) and self[y][x]) or ((sum==3) and 1) or 0
      xm1,x,xp1,xi = x,xp1,xp1+1,xi-1
    end
    ym1,y,yp1,yi = y,yp1,yp1+1,yi-1
  end
end

-- output the array to screen
function _CELLS:draw()
  for y=1,self.h do
   for x=1,self.w do
      if self[y][x]>0 then
        win:box(x * tilesize + 5, y * tilesize + 10, tilesize - 3, tilesize - 3, true)
      end
    end
  end
end

-- constructor
function CELLS(w,h)
  local c = ARRAY2D(w,h)
  c.spawn = _CELLS.spawn
  c.evolve = _CELLS.evolve
  c.draw = _CELLS.draw
  return c
end

--
-- shapes suitable for use with spawn() above
--
HEART = { 1,0,1,1,0,1,1,1,1; w=3,h=3 }
GLIDER = { 0,0,1,1,0,1,0,1,1; w=3,h=3 }
EXPLODE = { 0,1,0,1,1,1,1,0,1,0,1,0; w=3,h=4 }
FISH = { 0,1,1,1,1,1,0,0,0,1,0,0,0,0,1,1,0,0,1,0; w=5,h=4 }
BUTTERFLY = { 1,0,0,0,1,0,1,1,1,0,1,0,0,0,1,1,0,1,0,1,1,0,0,0,1; w=5,h=5 }

-- the main routine
function LIFE(w,h)
  -- create two arrays
  local thisgen = CELLS(w,h)
  local nextgen = CELLS(w,h)

  -- create some life
  -- about 1000 generations of fun, then a glider steady-state
  thisgen:spawn(GLIDER,5,4)
  thisgen:spawn(EXPLODE,25,10)
  thisgen:spawn(FISH,4,12)

  -- run until break
  local gen=1
  while win:getmessage() ~= "close" do
    win:box()
    thisgen:evolve(nextgen)
    thisgen,nextgen = nextgen,thisgen
    thisgen:draw()
    win:text(5, h * tilesize + 60, "Life - generation " .. gen)
    gen=gen+1
    delay()
  end
end

win = Siamiga.createwindow("Life", 20, 20, 450, 300, false, false)
win:openwindow()
pen = Siamiga.createpen(0,0,200)
win:setpen(pen)
LIFE(40,20)
Siamiga.releasepen(pen)
win:closewindow()
