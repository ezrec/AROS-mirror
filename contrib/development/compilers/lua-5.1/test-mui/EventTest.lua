require "muidefs"
require "muifuncs"

ok_id = 1
cancel_id = 2

function creategui()
  local okbutton = mui.SimpleButton("_Ok")
  local cancelbutton = mui.SimpleButton("_Cancel")
  local text = mui.TextObject(mui.MUIA_Text_Contents, "\27cHello world!\nHow are you?")
  window = mui.WindowObject(
    mui.MUIA_Window_Title, "Hello world!",
    mui.MUIA_Window_RootObject, mui.VGroup(
      mui.Child, text,
      mui.Child, mui.HGroup(
        mui.Child, okbutton,
        mui.Child, cancelbutton
      )
    )
  )
  app = mui.ApplicationObject(
    mui.MUIA_Application_Window, window
  )
  assert(app:check(), "Cant create application")
  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)

  okbutton:doint(mui.MUIM_Notify, mui.MUIA_Pressed, false, app, 2, mui.MUIM_Application_ReturnID, ok_id)
  cancelbutton:doint(mui.MUIM_Notify, mui.MUIA_Pressed, false, app, 2, mui.MUIM_Application_ReturnID, cancel_id)

  window:set(mui.MUIA_Window_Open, true)
end


function main()
  creategui()
  running = true
  while running do
    id, signals = app:input()
    if id == mui.MUIV_Application_ReturnID_Quit then
      running = false
    elseif id == ok_id then
      app:request(window, 0, "Hint", "OK", "OK gadget has been clicked")
    elseif id == cancel_id then
      app:request(window, 0, "Hint", "OK", "Cancel gadget has been clicked")
    end
    if running then mui.wait(signals) end
  end
end


_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end

