NAME = your_class_name
PATH = Catalogs/Français/Feelin

Feelin:$(PATH)/FC_$(NAME).catalog : enums.h table.h
	flexcat Catalog.cd Français.ct CATALOG Feelin:LOCALE/$(PATH)/FC_$(NAME).catalog
	copy Feelin:LOCALE/$(PATH)/FC_$(NAME).catalog TO LOCALE:$(PATH)/FC_$(NAME).catalog

enums.h : catalog.cd
	flexcat Catalog.cd enums.h=Feelin:Sources/_locale/Enums.sd

table.h : catalog.cd
	flexcat Catalog.cd table.h=Feelin:Sources/_locale/Table.sd

