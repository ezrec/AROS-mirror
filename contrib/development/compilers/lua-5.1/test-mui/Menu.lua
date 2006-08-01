require "muidefs"
require "muifuncs"

-- until Zulu has callback functions we have to use return ID's
new_menu_ID = 50
open_menu_ID = 51
save_menu_ID = 52
saveas_menu_ID = 53
about_menu_ID = 54
toggle_menu_ID = 55

function menuentry(title, short)
  -- creates a single menu entry
  return mui.MenuitemObject(
    mui.MUIA_Menuitem_Title, title,
    mui.MUIA_Menuitem_Shortcut, short
  )
end

function set_menu_id(object, id)
  -- adds a return ID to a menu selection event
  if id == 0 then
    error("Menu ID must not be 0")
  end
  object:doint(mui.MUIM_Notify, mui.MUIA_Menuitem_Trigger, mui.MUIV_EveryTime,
    app, 2, mui.MUIM_Application_ReturnID, id)
end

  
function main()
  new_menu = menuentry("New", "N")
  open_menu = menuentry("Open", "O")
  save_menu = menuentry("Save", "S")
  saveas_menu = menuentry("Save As", 0)
  about_menu = menuentry("About", "?")
  aboutzune_menu = menuentry("About Zune", 0)
  quit_menu = menuentry("Quit", "Q")

  red_menu = menuentry("Red", 0)
  green_menu = menuentry("Green", 0)
  blue_menu = menuentry("Blue", 0)

  toggle_menu = menuentry("Toggle", 0)
  checkit_menu = menuentry("Checkit", 0)

  -- toggle and checkit must be set before the menu is used
  toggle_menu:set(mui.MUIA_Menuitem_Checkit, true)
  toggle_menu:set(mui.MUIA_Menuitem_Toggle, true)
  checkit_menu:set(mui.MUIA_Menuitem_Checkit, true)
 
  window = mui.WindowObject(
    mui.MUIA_Window_Title, "Menu demonstration",
    mui.WindowContents, mui.VGroup(
      mui.Child, mui.TextObject(mui.MUIA_Text_Contents, "Menu demonstration")
    )
  )

  app = mui.ApplicationObject(
    mui.MUIA_Application_Title      , "Menu.lua",
    mui.MUIA_Application_Version    , "$VER: Menu.lua 0.1 (31.7.2006)",
    mui.MUIA_Application_Author     , "Matthias Rustler",
    mui.MUIA_Application_Description, "MUI menubar demonstration",
    mui.MUIA_Application_Base       , "MENULUA",

    mui.MUIA_Application_Menustrip, mui.MenustripObject(
      -- adding a menu to the application means that all windows get the same menu
      mui.MUIA_Family_Child, mui.MenuObject(
        mui.MUIA_Menu_Title, "Project",
        mui.MUIA_Family_Child, new_menu,
        mui.MUIA_Family_Child, open_menu,
        mui.MUIA_Family_Child, save_menu,
        mui.MUIA_Family_Child, saveas_menu,
        mui.MUIA_Family_Child, menuentry(-1,0), -- separator bar
        mui.MUIA_Family_Child, about_menu,
        mui.MUIA_Family_Child, aboutzune_menu,
        mui.MUIA_Family_Child, menuentry(-1,0),
        mui.MUIA_Family_Child, quit_menu
      ),
      mui.MUIA_Family_Child, mui.MenuObject(
	mui.MUIA_Menu_Title, "Setup",
	mui.MUIA_Family_Child, mui.MenuitemObject(
	  mui.MUIA_Menuitem_Title, "Color",
	  -- a sub menu
	  mui.MUIA_Family_Child, red_menu,
	  mui.MUIA_Family_Child, green_menu,
	  mui.MUIA_Family_Child, blue_menu
	),
	mui.MUIA_Family_Child, toggle_menu,
	mui.MUIA_Family_Child, checkit_menu
      )
    ),
    mui.MUIA_Application_Window, window
  )
  assert(app:check(), "Cant create applicaton")

  window:doint(mui.MUIM_Notify, mui.MUIA_Window_CloseRequest, true,
    app, 2, mui.MUIM_Application_ReturnID, mui.MUIV_Application_ReturnID_Quit)

  set_menu_id(new_menu, new_menu_ID)
  set_menu_id(open_menu, open_menu_ID)
  set_menu_id(save_menu, save_menu_ID)
  set_menu_id(saveas_menu, saveas_menu_ID)
  set_menu_id(about_menu, about_menu_ID)
  set_menu_id(quit_menu, mui.MUIV_Application_ReturnID_Quit)
  set_menu_id(toggle_menu, toggle_menu_ID)
 
  -- Zune handles the about dialog itself
  aboutzune_menu:doint(mui.MUIM_Notify, mui.MUIA_Menuitem_Trigger, mui.MUIV_EveryTime,
    app, 2, mui.MUIM_Application_AboutMUI, window)

  window:set(mui.MUIA_Window_Open, true) 
  
  running = true
  while running do
    id, signals = app:input()
    if id == mui.MUIV_Application_ReturnID_Quit then
      running = false
    elseif id == new_menu_ID then
      print("Menu 'New' selected")
    elseif id == open_menu_ID then
      print("Menu 'Open' selected")
    elseif id == save_menu_ID then
      print("Menu 'Save' selected")
    elseif id == saveas_menu_ID then
      print("Menu 'Save As' selected")
    elseif id == about_menu_ID then
      app:request(window, 0, "About", "OK", "Menu demonstration")
    elseif id == toggle_menu_ID then
      state = toggle_menu:getint(mui.MUIA_Menuitem_Checked)
      print("State of toggle menu", state)
    end
    if running then mui.wait(signals) end
  end
end

print("") -- open console window
--main()
_, err = pcall(main)
if err then print("Error: " .. err) end
if app then app:dispose() end

