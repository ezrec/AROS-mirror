<?
/*
 * dist.php
 *
 * @(#) $Header$
 *
 */

	set_time_limit(0);
	if(($files=fopen("files","w"))==0)
	{
		echo "Error: Could not open the distribution files file for output.\n";
		exit(1);
	}
	$accessed_documentation=0;
	if(($documentation_directory=opendir("bgui_documentation/.")))
	{
		$accessed_documentation=1;
		while($entry=readdir($documentation_directory))
		{
			if($entry!="."
			&& $entry!=".."
			&& $entry!=".cvsignore"
			&& $entry!="CVS"
			&& fputs($files,"bgui_documentation/$entry\n")==0)
				$accessed_documentation=0;
		}
		closedir($documentation_directory);
	}
	if(!$accessed_documentation
	|| fputs($files,".htaccess\n")==0
	|| fputs($files,"bgui.lha\n")==0
	|| fputs($files,"bgui_src.lha\n")==0
	|| fputs($files,"bgui_dev.lha\n")==0
	|| fputs($files,"bgui_e.lha\n")==0
	|| fputs($files,"bgui_rexx.lha\n")==0
	|| fputs($files,"bgui_html_documentation.lha\n")==0
	|| fputs($files,"bgui_guide_documentation.lha\n")==0
	|| fputs($files,"bin/bgui.lha\n")==0
	|| fputs($files,"bin/bgui_src.lha\n")==0
	|| fputs($files,"bin/bgui_dev.lha\n")==0
	|| fputs($files,"bin/bgui_e.lha\n")==0
	|| fputs($files,"bin/bgui_rexx.lha\n")==0
	|| fputs($files,"bin/bgui_html_documentation.lha\n")==0
	|| fputs($files,"bin/bgui_guide_documentation.lha\n")==0
	|| fputs($files,"index.html\n")==0
	|| fputs($files,"screenshots.html\n")==0
	|| fputs($files,"manifest.html\n")==0
	|| fputs($files,"cvsweb.html\n")==0
	|| fputs($files,"cvsweb\n")==0
	|| fputs($files,"cgi-style.pl\n")==0
	|| fputs($files,"graphics/back.gif\n")==0
	|| fputs($files,"graphics/backfill.gif\n")==0
	|| fputs($files,"graphics/backfill_thumb.gif\n")==0
	|| fputs($files,"graphics/bgui_background.gif\n")==0
	|| fputs($files,"graphics/boopsi.gif\n")==0
	|| fputs($files,"graphics/boopsi_thumb.gif\n")==0
	|| fputs($files,"graphics/builder.gif\n")==0
	|| fputs($files,"graphics/builder_thumb.gif\n")==0
	|| fputs($files,"graphics/dir.gif\n")==0
	|| fputs($files,"graphics/draganddrop.gif\n")==0
	|| fputs($files,"graphics/draganddrop_thumb.gif\n")==0
	|| fputs($files,"graphics/editor.gif\n")==0
	|| fputs($files,"graphics/editor_thumb.gif\n")==0
	|| fputs($files,"graphics/groups.gif\n")==0
	|| fputs($files,"graphics/groups_thumb.gif\n")==0
	|| fputs($files,"graphics/palette.gif\n")==0
	|| fputs($files,"graphics/palette_thumb.gif\n")==0
	|| fputs($files,"graphics/preferences.gif\n")==0
	|| fputs($files,"graphics/preferences_thumb.gif\n")==0
	|| fputs($files,"graphics/text.gif\n")==0
	|| fputs($files,"graphics/textformat.gif\n")==0
	|| fputs($files,"graphics/textformat_thumb.gif\n")==0
	|| fputs($files,"graphics/tooltips.gif\n")==0
	|| fputs($files,"graphics/tooltips_thumb.gif\n")==0
	|| fputs($files,"graphics/treeview.gif\n")==0
	|| fputs($files,"graphics/treeview_thumb.gif\n")==0
	|| fputs($files,"graphics/virtual.gif\n")==0
	|| fputs($files,"graphics/virtual_thumb.gif\n")==0
	|| fputs($files,"bugs_shared.php\n")==0
	|| fputs($files,"bugs_app_config.php\n")==0
	|| fputs($files,"bugs.html\n")==0
	|| fputs($files,"bugs_statistics.html\n")==0
	|| fputs($files,"bugs_configuration.html\n")==0
	|| fputs($files,"bugs_app_locale-en.php\n")==0
	|| fputs($files,"bugs_setup.php\n")==0)
	{
		echo "Error: Could not write to the distribution files file.\n";
		exit(1);
	}
	fclose($files);
	Exec("tar -cT files | gzip >dist.tar.gz",$output,$result);
	if($result!=0)
	{
		echo "Error: Could not make the distribution archive file.\n";
		exit(1);
	}
	exit(0)
?>