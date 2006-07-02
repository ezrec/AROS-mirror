-- Port of Balancing.c from mui38dev

require "muidefs"
require "muifuncs"

function main()
  local window = mui.WindowObject(
    mui.MUIA_Window_Title , "Balancing Groups",
    mui.MUIA_Window_ID    , mui.makeid("BALA"),
    mui.MUIA_Window_Width , mui.MUIV_Window_Width_Screen(50),
    mui.MUIA_Window_Height, mui.MUIV_Window_Height_Screen(50),
    mui.WindowContents, mui.HGroup(
    mui.Child, mui.VGroup(
      mui.MUIA_Frame, mui.MUIV_Frame_Group,
        mui.MUIA_Weight, 15,
        mui.Child, mui.RectangleObject(
          mui.MUIA_Frame, mui.MUIV_Frame_Text,
          mui.MUIA_Weight, 50
        ),
        mui.Child, mui.RectangleObject(
          mui.MUIA_Frame, mui.MUIV_Frame_Text,
          mui.MUIA_Weight, 100
        ),
        mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
        mui.Child, mui.RectangleObject(
          mui.MUIA_Frame, mui.MUIV_Frame_Text,
          mui.MUIA_Weight, 200
        )
      ),
      mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
      mui.Child, mui.VGroup(
          mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.RectangleObject(
            mui.MUIA_Frame, mui.MUIV_Frame_Text,
            mui.MUIA_ObjectID, 123
          ),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.RectangleObject(
            mui.MUIA_Frame, mui.MUIV_Frame_Text,
            mui.MUIA_ObjectID, 456
          )
        ),
        mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain),
          mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text)
        ),
        mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.HGroup(
            mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
            mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
            mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text)
          ),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.HGroup(
            mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text),
            mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
            mui.Child, mui.RectangleObject(mui.MUIA_Frame, mui.MUIV_Frame_Text)
          )
        ),
        mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.RectangleObject(
            mui.MUIA_Frame, mui.MUIV_Frame_Text,
            mui.MUIA_Weight,  50
          ),
          mui.Child, mui.RectangleObject(
            mui.MUIA_Frame, mui.MUIV_Frame_Text,
            mui.MUIA_Weight, 100
          ),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.RectangleObject(
            mui.MUIA_Frame, mui.MUIV_Frame_Text,
            mui.MUIA_Weight, 200
          )
        ),
        mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.SimpleButton("Also"),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.SimpleButton("Try"),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.SimpleButton("Sizing"),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.SimpleButton("With"),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.SimpleButton("Shift")
        ),
        mui.Child, mui.HGroup(
          mui.MUIA_Frame, mui.MUIV_Frame_Group,
          mui.Child, mui.Label("Label 1:"),
          mui.Child, mui.TextObject(mui.MUIA_Text_Contents, "data..."),
          mui.Child, mui.BalanceObject(mui.MUIA_CycleChain, 1),
          mui.Child, mui.Label("Label 2:"),
          mui.Child, mui.TextObject(mui.MUIA_Text_Contents, "more data...")
        )
      )
    )
  )

  app = mui.ApplicationObject(
    mui.MUIA_Application_Title      , "BalanceDemo",
    mui.MUIA_Application_Version    , "$VER: BalanceDemo 0.1 (1.7.2006)",
    mui.MUIA_Application_Author     , "Stefan Stuntz, Matthias Rustler",
    mui.MUIA_Application_Description, "Show balancing groups",
    mui.MUIA_Application_Base       , "BALANCEDEMO",
    mui.SubWindow, window
  )

  assert(app, "Failed to create Application.")

  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)
  collectgarbage("collect")
  window:set(mui.MUIA_Window_Open, true)
  app:doint(mui.MUIM_Application_Execute)
  window:set(mui.MUIA_Window_Open, false)
end

_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end

