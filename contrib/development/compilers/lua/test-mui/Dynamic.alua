require "muidefs"
require "muifuncs"

exchID = 1

strings = strarray.new("one", "two", "three")

function creategui()
  button = mui.SimpleButton("Exchange")
  cycle = mui.CycleObject(mui.MUIA_Cycle_Entries, strings)

  group = mui.VGroup(
    mui.Child, cycle,
    mui.Child, button
  )

  window = mui.WindowObject(
    mui.MUIA_Window_Title, "Dynamic Cycle Test",
    mui.MUIA_Window_RootObject, group
  )

  app = mui.ApplicationObject(
    mui.MUIA_Application_Window, window
  )

  assert(app:check(), "Cant create application")

  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)

  button:doint(mui.MUIM_Notify, mui.MUIA_Pressed, false,
    app, 2, mui.MUIM_Application_ReturnID, exchID)
  
  window:set(mui.MUIA_Window_Open, true)
end

function main()
  creategui()
  running = true
  while running do
    id, signals = app:input()
    if id == mui.MUIV_Application_ReturnID_Quit then
      running = false
    elseif id == exchID then
      if mui.doint(group, mui.MUIM_Group_InitChange) then
        mui.doint(group, mui.OM_REMMEMBER, cycle) -- remove cycle object
        mui.doint(group, mui.OM_REMMEMBER, button)
        cycle:dispose()
        strings:dispose() -- dispose strarray because we have no garbage collector
        strings = strarray.new("red", "green", "blue")
        cycle = mui.CycleObject(mui.MUIA_Cycle_Entries, strings)
        mui.doint(group, mui.OM_ADDMEMBER, cycle)
        mui.doint(group, mui.MUIM_Group_ExitChange)
      end
    end
    if running then mui.wait(signals) end
  end
end

_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end
if strings then strings:dispose() end

