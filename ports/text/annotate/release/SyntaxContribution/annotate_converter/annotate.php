<?
$file="php.vim";

$fp=fopen($file, "r");
$content=fread($fp, filesize($file)); 
fclose($fp);

$keywords_find="|syn keyword (.*) contained|Uis";
preg_match_all($keywords_find, $content, $keywords_match);

foreach($keywords_match[1] as $key=>$value)
{
	$key_array=explode(" ", trim($value));

	$group=$key_array[0];
        
	array_shift($key_array);
	$keys_by_group[$group][]=$key_array;

}
var_dump($keys_by_group);


//*****************************************************************//
//    Colour link                                                  //
//*****************************************************************//

foreach($keys_by_group as $key=>$value)
{
	echo "\n";
	echo $colourlink_find="|HiLink\s+".$key."\s+([\w]+)|";
	echo "\n";
	preg_match_all($colourlink_find, $content, $colourlink_match);

	echo $colourlink_match[1][0];
	echo "\n";

	$colour_link[$key]=$colourlink_match[1][0];
}

//*****************************************************************//
//    Get Style - files in /usr/share/vim/colors/                  //
//*****************************************************************//

$style_file = "elflord.vim";

$fp=fopen($style_file, "r");
$content=fread($fp, filesize($style_file)); 
fclose($fp);

foreach($colour_link as $key=>$value)
{
	echo $stylegroup_find="|hi link ".$value."\s+(.*)|";
	echo "\n";
        preg_match_all($stylegroup_find, $content, $stylegroup_match);

	echo $stylegroup_match[1][0];
	echo "\n";

        $stylegroup[$value]=$stylegroup_match[1][0];
}


foreach($colour_link as $key=>$value)
{

	echo $style_find="|hi ".$value."\s+(.*)|";
	echo "\n";
        preg_match_all($style_find, $content, $style_match);

	if(empty($style_match[1][0]))
	{
	   echo $style_find="|hi ".$stylegroup[$value]."\s+(.*)|";
	   echo "\n";
           preg_match_all($style_find, $content, $style_match);
           
	};
	echo $style_match[1][0];
	echo "\n";
	preg_match('|term=(\w+)|',$style_match[1][0], $term_match);
	preg_match('|ctermfg=(\w+)|',$style_match[1][0], $ctermfg_match);
	preg_match('|guifg=(\w+)|',$style_match[1][0], $guifg_match);
	preg_match('|term=(\w+)|',$style_match[1][0], $gui_match);
	//var_dump($style_array);

	echo $term_match[1];
	echo "\n";
	echo $ctermfg_match[1];
	echo "\n";
	echo $guifg_match[1];
	echo "\n";
	echo $gui_match[1];
	echo "\n";

	$type_to_style[$value]["term"]=$term_match[1];
	$type_to_style[$value]["ctermfg"]=$ctermfg_match[1];
	$color_to_pen[$ctermfg_match[1]]=1;
}

$x=1;
foreach($color_to_pen as $key=>$value)
{
	$color_to_pen[$key]=$x++;
}

//*****************************************************************//
//    create xml file                                              //
//*****************************************************************//

$nb=0;
$file="Anotate_php.xml";
$fp=fopen($file, "w+");

$content='<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE Ann-SyntaxHighlighting>
<!-- There are 5 special characters that need to be codes:    -->
<!-- & - &amp;   < - &lt;   > - &gt;   " - &quot;   \' - &apos; -->
<Ann-SyntaxHighlighting>

  <Syntax>
      <Name>PHP</Name>
          <Qualifying>
	        <OrTokens min="10">ENDIF FOR WHILE DEF PROC CONST</OrTokens>
		<AndTokens>( ) , DEF</AndTokens>
		<FilePattern>#?.php</FilePattern>
          </Qualifying> '."\n";
foreach($keys_by_group as $key=>$value)
{
   $style=$type_to_style[$colour_link[$key]]["term"];
   $color=$type_to_style[$colour_link[$key]]["ctermfg"];
   
   foreach($value as $key2=>$value2)
   {
       foreach($value2 as $key3=>$value3)
       { 
	       if($value3)
	       {
		       $tmp_content.='<Token style="'.$style.'" pen="'.$color_to_pen[$color].'">'.$value3.'</Token>'."\n";
		       $nb++;
	       }
       }
   }
}
$content.="<Tokens>".$nb."</Tokens>\n";
$content.=$tmp_content;

$content.="</Syntax>\n</Ann-SyntaxHighlighting>\n";

fwrite($fp, $content);

fclose($fp);
?>
