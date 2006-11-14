require "muidefs"
require "muifuncs"

function creategui()
  local button = mui.SimpleButton("_Ok")
  local text = mui.TextObject(mui.MUIA_Text_Contents, "\27cHello world!\nHow are you?")
  local window = mui.WindowObject(
    mui.MUIA_Window_Title, "Hello world!",
    mui.MUIA_Window_RootObject, mui.VGroup(
      mui.Child, text,
      mui.Child, button
    )
  )
  app = mui.ApplicationObject(
    mui.MUIA_Application_Window, window
  )
  assert(app:check(), "Cant create application")
  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)
  button:doint(mui.MUIM_Notify, mui.MUIA_Pressed, false,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)
  window:set(mui.MUIA_Window_Open, true)
end

function main()
  creategui()
  app:doint(mui.MUIM_Application_Execute)
end

_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end
