-- Port of 'Pages.c' from mui38dev

require "muidefs"
require "muifuncs"

Sex = strarray.new("male", "female")
Pages = strarray.new("Race", "Class", "Armor", "Level")
Races = strarray.new("Human", "Elf", "Dwarf", "Hobbit", "Gnome")
Classes = strarray.new("Warrior", "Rogue", "Bard", "Monk", "Magician", "Archmage")

function CheckMark(label, state)
  local obj = mui.make(mui.MUIO_Checkmark, label)
  obj:setcheckmark(state)
  return obj
end

function Slider(min, max, value)
  return mui.SliderObject(
    mui.MUIA_Numeric_Min, min,
    mui.MUIA_Numeric_Max, max,
    mui.MUIA_Numeric_Value, value)
end

function main()
  window = mui.WindowObject(
    mui.MUIA_Window_Title, "Character Definition",
    mui.MUIA_Window_ID   , mui.makeid("PAGE"),
    mui.WindowContents, mui.VGroup(
      mui.Child, mui.ColGroup(
        2,
        mui.Child, mui.Label2("Name:"),
        mui.Child, mui.StringObject(
          mui.MUIA_Frame, mui.MUIV_Frame_String,
          mui.MUIA_String_Contents, "Frodo",
          mui.MUIA_String_MaxLen, 32
        ),
        mui.Child, mui.Label1("Sex:" ), mui.Child, mui.make(mui.MUIO_Cycle, 0, Sex)
      ),
      mui.Child, mui.VSpace(2),
      mui.Child, mui.RegisterGroup(
        Pages,
        mui.MUIA_Register_Frame, true,
        mui.Child, mui.HCenter(mui.make(mui.MUIO_Radio, 0, Races)),
        mui.Child, mui.HCenter(mui.make(mui.MUIO_Radio, 0, Classes)),
        mui.Child, mui.HGroup(
          mui.Child, mui.HSpace(0),
          mui.Child, mui.ColGroup(
            2,
            mui.Child, mui.Label1("Cloak:" ), mui.Child, CheckMark("", true),
            mui.Child, mui.Label1("Shield:"), mui.Child, CheckMark("", true),
            mui.Child, mui.Label1("Gloves:"), mui.Child, CheckMark("", true),
            mui.Child, mui.Label1("Helmet:"), mui.Child, CheckMark("", true)
          ),
          mui.Child, mui.HSpace(0)
        ),

        mui.Child, mui.ColGroup(
          2,
          mui.Child, mui.Label("Experience:"  ), mui.Child, Slider(0,100, 3),
          mui.Child, mui.Label("Strength:"    ), mui.Child, Slider(0,100,42),
          mui.Child, mui.Label("Dexterity:"   ), mui.Child, Slider(0,100,24),
          mui.Child, mui.Label("Condition:"   ), mui.Child, Slider(0,100,39),
          mui.Child, mui.Label("Intelligence:"), mui.Child, Slider(0,100,74)
        )
      )
    )
  )


  app = mui.ApplicationObject(
    mui.MUIA_Application_Title      , "Pages-Demo",
    mui.MUIA_Application_Version    , "$VER: Pages-Demo 0.2 (1.8.2006)",
    mui.MUIA_Application_Author     , "Stefan Stuntz, Matthias Rustler",
    mui.MUIA_Application_Description, "Show MUIs Page Groups",
    mui.MUIA_Application_Base       , "PAGESDEMO",
    mui.SubWindow, window
  )

  assert(app:check(), "Failed to create Application.")

  collectgarbage("collect")
  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)

  window:set(mui.MUIA_Window_Open, true)
  app:doint(mui.MUIM_Application_Execute)
end

--main()
_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end
Sex:dispose()
Pages:dispose()
Races:dispose()
Classes:dispose()

