modules={}
function module(name)
  if modules[name] then
    current_module=modules[name]
  else
    current_module={name=name, description=""}
    modules[#modules+1]=name
    modules[name]=current_module
  end
  current_obj=current_module
end

function signature(name)
  current_obj = {name=name, params={}, returns={}}
  current_module[#current_module+1]=current_obj
end

function description(str)
  current_obj.description=str
end

function param(name, desc)
  current_obj.params[#current_obj.params+1]=name
  current_obj.params[name]=desc
end

function returns(name, desc)
  current_obj.returns[#current_obj.returns+1]=name
  current_obj.returns[name]=desc
end

local fn = arg[1]
assert(fn, "Usage: extract_doc filename")
local lines={}
local in_lua = false
for line in io.lines(fn) do
  if line:match("^%?%*/") then
    in_lua = false
  end

  lines[#lines+1] = in_lua and line or ""

  if line:match("^/%*%?lua") then
    in_lua = true
  end
end

assert(loadstring(table.concat(lines, "\n")))()
function fwrite(fmt, ...) io.write(fmt:format(...)) end

table.sort(modules)
fwrite([[
<!DOCTYPE html
    PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="en" xml:lang="en">
<head>
  <title>%s</title>
  <link rel="stylesheet" href="doc_style.css" type="text/css" />
</head>
<body>
]], lib_name)

for _,n in ipairs(modules) do
  local m = modules[n]
  fwrite("<h1>%s</h1>\n<p>%s</p>\n", m.name, m.description)
  fwrite("<dl class=\"function\">\n")
  for _,f in ipairs(m) do
    fwrite("<dt>%s</dt>\n", f.name)
    fwrite("<dd>\n%s\n", f.description)
    if (#f.params > 0) then
      fwrite("<h3>Parameters</h3>\n")
      fwrite("<ul>\n")
      for i,p in ipairs(f.params) do
        fwrite("<li>%s: %s</li>\n", p, f.params[p])
      end
      fwrite("</ul>\n")
    end
    if (#f.returns > 0) then
      fwrite("<h3>Return values</h3>\n")
      fwrite("<ol>\n")
      for i,r in ipairs(f.returns) do
        fwrite("<li>%s: %s</li>\n", r, f.returns[r])
      end
      fwrite("</ol>\n")
    end
    fwrite("</dd>\n")
  end
  fwrite("</dl>\n")
end
fwrite("</body>")
fwrite("</html>")
