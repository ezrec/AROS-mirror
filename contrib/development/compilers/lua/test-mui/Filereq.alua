require "muiasl"

file = mui.filerequest(
  mui.ASLFR_TitleText,       "File requester example",
  mui.ASLFR_InitialHeight,   400,
  mui.ASLFR_InitialWidth,    320,
  mui.ASLFR_InitialLeftEdge, 40,
  mui.ASLFR_InitialTopEdge,  20,
  mui.ASLFR_PositiveText,    "O KAY",
  mui.ASLFR_NegativeText,    "not OK",
  mui.ASLFR_InitialFile,     "asl.library",
  mui.ASLFR_InitialDrawer,   "libs:"
)

print(file)


-- Multi select
-- note the curly brackets
files = {mui.filerequest(
  mui.ASLFR_TitleText,       "File requester with multi selection",
  mui.ASLFR_InitialHeight,   400,
  mui.ASLFR_InitialWidth,    320,
  mui.ASLFR_DoMultiSelect,   true
)}

count = #files
print ("Number of selected files " .. count)
for i = 1, count do
    print(files[i])
end


-- Save requester
file = mui.filerequest(
  mui.ASLFR_TitleText,       "Save file requester example",
  mui.ASLFR_DoSaveMode,      true,
  mui.ASLFR_RejectIcons,     true,  -- don't show info files
  mui.ASLFR_DoPatterns,      true   -- show pattern gadget
)

print(file)

