for i=0,9 do
  assert(a[i]==i+1)
end

for i=1,9 do
  assert(p[i-1].y==p[i].x)
end

for i=0,9 do
  assert(M.a[i]==i+1)
end

for i=1,9 do
  assert(M.p[i-1].y==M.p[i].x)
end

for i=0,9 do
  assert(pp[i].x==M.p[i].x and p[i].y == M.pp[i].y)
end

for i=0,9 do
  assert(array.a[i] == parray.a[i])
  assert(array.p[i].x == parray.pp[i].x and array.p[i].y == parray.pp[i].y)
end

for i=0,9 do
  array.a[i] = a[9-i]
  M.a[i] = 9-i
  assert(array.a[i]==M.a[i]+1)
end

for i=1,9 do
  array.p[i] = array.pp[1]
  assert(array.p[i].x==1 and array.p[i].y==2)
end

print("Array test OK")
