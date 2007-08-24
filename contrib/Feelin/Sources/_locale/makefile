PATH = Catalogs/Français/Feelin

Feelin:$(PATH)/FC_$(NAME).catalog : Feelin:Sources/($NAME)/_locale/enums.h Feelin:Sources/($NAME)/_locale/table.h
	flexcat Feelin:Sources/($NAME)/_locale/Catalog.cd Feelin:Sources/($NAME)/_locale/Français.ct CATALOG Feelin:$(PATH)/FC_$(NAME).catalog
	copy Feelin:$(PATH)/FC_$(NAME).catalog TO LOCALE:$(PATH)/FC_$(NAME).catalog

Feelin:Sources/($NAME)/_locale/enums.h : Feelin:Sources/($NAME)/_locale/catalog.cd
	flexcat Feelin:Sources/($NAME)/_locale/Catalog.cd enums.h=Feelin:Sources/_locale/Enums.sd

Feelin:Sources/($NAME)/_locale/table.h : Feelin:Sources/($NAME)/_locale/catalog.cd
	flexcat Feelin:Sources/($NAME)/_locale/Catalog.cd table.h=Feelin:Sources/_locale/Table.sd

