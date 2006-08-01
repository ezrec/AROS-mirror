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

file = mui.filerequest(
  mui.ASLFR_TitleText,       "Save file requester example",
  mui.ASLFR_DoSaveMode,      true,
  mui.ASLFR_RejectIcons,     true,  -- don't show info files
  mui.ASLFR_DoPatterns,      true   -- show pattern gadget
)

print(file)
