#! /usr/bin/env lua
-- See http://xmlsoft.org/xmlreader.html for more examples
require("xmlreader")

local example_xml = [[
<?xml version="1.0" encoding="iso-8859-1" ?>
<library>
  <book id='1'>
    <title>Green Eggs and Ham</title>
    <author>Dr. Seuss</author>
  </book>
  <book id='2'>
    <title>Where the Wild Things Are</title>
    <author>Maurice Sendak</author>
  </book>
</library>
]]

local r = assert(xmlreader.from_string(example_xml))

while (r:read()) do
  local leadingws = ('    '):rep(r:depth())
  if (r:node_type() == "element") then
    io.write(("%s%s:"):format(leadingws, r:name()))
    while (r:move_to_next_attribute()) do
      io.write((' %s=%q'):format(r:name(), r:value()))
    end
    io.write('\n')
  end
end
