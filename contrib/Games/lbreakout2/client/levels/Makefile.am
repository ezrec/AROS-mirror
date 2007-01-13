EXTRA_DIST = Original AddOn-1 AddOn-2 MP_Arenas MP_Snapshots \
Afl Chaos Holidays Megadoomer Runes Wolvie \
Arcade Demons Hommage Pabelo Shimitar Zufallswelt \
BeOS-4ever HereWeGo Kevin Ph33r TheGauntlet Bombs \
HighBall LinuxFun R-World Twilight Lattsville OpenSource \
Kazan-1 Abc Fly Flower Freek GreenWall Gatinha Composers Color \
Yexter GreenWall2 Lapis SnyMaxx Pawset X IoriYagami Tricky \
Silly

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(inst_dir)/levels
	@for file in $(EXTRA_DIST); do \
	  $(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(inst_dir)/levels/$$file; \
	done

win32-install-script:
	@for file in $(EXTRA_DIST); do \
	  echo "Source: \"`pwd -W`\\$$file\"; DestDir: \"{app}\\levels\"; CopyMode: alwaysoverwrite" >> @win32_inst_script@; \
	done

