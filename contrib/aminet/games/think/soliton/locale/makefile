# this makefile does not work with smake
# use make instead
all: translations catalogs

translations: trans
trans: trans_ca trans_da trans_de trans_hr trans_pl trans_hu

catalogs: cat
cat: cat_ca cat_da cat_de cat_hr cat_pl cat_hu

clean:
	delete #?/#?.(catalog|bak)

### català ##########################################################
trans_ca: català/Soliton.ct
cat_ca: català/Soliton.catalog

català/Soliton.catalog: català/Soliton.ct Soliton.cd
	flexcat Soliton.cd català/Soliton.ct CATALOG $@

català/Soliton.ct: Soliton.cd
	copy català/Soliton.ct català/Soliton.ct.bak
	flexcat Soliton.cd català/Soliton.ct NEWCTFILE $@

### dansk ###########################################################
trans_da: dansk/Soliton.ct
cat_da: dansk/Soliton.catalog

dansk/Soliton.catalog: dansk/Soliton.ct Soliton.cd
	flexcat Soliton.cd dansk/Soliton.ct CATALOG $@

dansk/Soliton.ct: Soliton.cd
	copy dansk/Soliton.ct dansk/Soliton.ct.bak
	flexcat Soliton.cd dansk/Soliton.ct NEWCTFILE $@

### deutsch #########################################################
trans_de: deutsch/Soliton.ct
cat_de: deutsch/Soliton.catalog

deutsch/Soliton.catalog: deutsch/Soliton.ct Soliton.cd
	flexcat Soliton.cd deutsch/Soliton.ct CATALOG $@

deutsch/Soliton.ct: Soliton.cd
	copy deutsch/Soliton.ct deutsch/Soliton.ct.bak
	flexcat Soliton.cd deutsch/Soliton.ct NEWCTFILE $@

### hrvatski ########################################################
trans_hr: hrvatski/Soliton.ct
cat_hr: hrvatski/Soliton.catalog

hrvatski/Soliton.catalog: hrvatski/Soliton.ct Soliton.cd
	flexcat Soliton.cd hrvatski/Soliton.ct CATALOG $@

hrvatski/Soliton.ct: Soliton.cd
	copy hrvatski/Soliton.ct hrvatski/Soliton.ct.bak
	flexcat Soliton.cd hrvatski/Soliton.ct NEWCTFILE $@

### polski ##########################################################
trans_pl: polski/Soliton.ct
cat_pl: polski/Soliton.catalog

polski/Soliton.catalog: polski/Soliton.ct Soliton.cd
	flexcat Soliton.cd polski/Soliton.ct CATALOG $@

polski/Soliton.ct: Soliton.cd
	copy polski/Soliton.ct polski/Soliton.ct.bak
	flexcat Soliton.cd polski/Soliton.ct NEWCTFILE $@

### maygar ##########################################################
trans_hu: maygar/Soliton.ct
cat_hu: maygar/Soliton.catalog

maygar/Soliton.catalog: maygar/Soliton.ct Soliton.cd
	flexcat Soliton.cd maygar/Soliton.ct CATALOG $@

maygar/Soliton.ct: Soliton.cd
	copy maygar/Soliton.ct maygar/Soliton.ct.bak
	flexcat Soliton.cd maygar/Soliton.ct NEWCTFILE $@

