mui.MUIA_IconImage_DiskObject = -1874394368 -- I-- struct DiskObject *
mui.MUIA_IconImage_File       = -1874394367 -- I-- CONST_STRPTR

mui.MUIC_IconImage= "IconImage.mcc"

function mui.IconImageObject(...)
  return mui.new(mui.MUIC_IconImage, ...)
end
