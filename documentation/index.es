.. raw:: html

   <table style="text-align: justify; width: 100%;" background="/images/bgcolormain.png" border="0" cellpadding="1" cellspacing="1"><tr><td style="vertical-align: top;">

.. Include:: introduction/index-abstract.es

`Lee más... <introduction/index>`__

.. raw:: html

   </td></tr></tbody></table><br>


.. raw:: html

   <table style="width: 100%; text-align: justify; margin-left: auto; margin-right: auto;" background="/images/bgcolormain.png" border="0" cellpadding="1" cellspacing="1"><tr><td>

.. Include:: download-abstract.en

`Lee más... <download>`__

.. raw:: html

   </td></tr></table>


.. raw:: html

   <table style="width: 100%; text-align: justify; margin-left: auto; margin-right: auto; min-height: 300px;" background="/images/bgcolormain.png" border="0" cellpadding="1" cellspacing="1"><tr><td>

Commits
=======

.. raw:: html

   <?php include("../rsfeed/commits.php"); ?></td></tr></table>


.. raw:: html

   <table style="width: 100%; text-align: justify; margin-left: auto; margin-right: auto;" border="0" cellpadding="1" cellspacing="1"><tr><td>

.. Include:: news/index.es

.. raw:: html

   </td></tr></table><br>


.. raw:: html

   <td style="width: 243px; vertical-align: top;">
   <!-- The direct downloads block is disabled so that users go to the downloads section where they should learn about using distribution instead of nightly
   <table style="text-align: justify; width:100%;" background="/images/bgcolorright.png" border="0" cellpadding="1" cellspacing="1"><tr><td>
   <?php if ($win_ie56) { echo "<img alt=\"Direct download Icon\" src=\"/images/directdownloadicon.gif\" align=\"middle\">";} 
   else { echo "<img alt=\"Direct download Icon\" src=\"/images/directdownloadicon.png\" align=\"middle\">";} ?>Direct Downloads:<br>
   <img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"><br>
   Las nightly builds se hacen automáticamente cada noche, directamente del más 
   reciente código fuente. Si gustaras probar un LiveCD de AROS, por favor elige 
   el archivo "boot ISO" para tu plataforma (i386 or PPC).<br><br>
   <?php virtual("/cgi-bin/files2") ?> <br></td></tr></tbody></table>-->

   <table style="text-align: justify; width: 100%;" background="/images/bgcolorright.png" border="0" cellpadding="1" cellspacing="1"><tr><td>
   <?php if ($win_ie56) { echo "<img alt=\"Archive Icon\" src=\"/images/archivedownloadicon.gif\" align=\"middle\">"; } 
   else { echo "<img alt=\"Archive Icon\" src=\"/images/archivedownloadicon.png\" align=\"middle\">"; } ?>
   Las últimas propuestas al ARCHIVE:<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"><br>
   <a href=http://archives.aros-exec.org>Los archivos de AROS</a>   guardan el contenido más reciente propuesto por nuestra comunidad, y es el primer lugar para buscar aplicaciones de usuario, temas, gráficos, y documentación adicional.<br><br>
   <?php include("../rsfeed/archives.php"); ?><br></td></tr></table>

   <table style="text-align: justify; width: 100%;" background="/images/bgcolorright.png" border="0" cellpadding="1" cellspacing="1"><tr><td>
   <?php if ($win_ie56) { echo "<img alt=\"Community Icon\" src=\"/images/communityicon.gif\" align=\"middle\">"; } 
   else { echo "<img alt=\"Community Icon\" src=\"/images/communityicon.png\" align=\"middle\">"; } ?> 
   Los comentarios más recientes en el foro AROS-EXEC :<br><img style="width: 238px; height: 2px;" alt="spacer" src="/images/sidespacer.png"><br>
   <a href=http://aros-exec.org>AROS-EXEC</a> es el principal sitio de la comunidad AROS. Recibe ayuda, descubre lo que está pasando en la comunidad, y añade tus pensamientos sobre AROS.<br><br>
   <?php include("../rsfeed/aeforum.php"); ?><br></td></tr></tbody></table>

   <table style="width: 100%; text-align: justify; margin-left: auto; margin-right: auto;" background="/images/bgcolorright.png" border="0" cellpadding="1" cellspacing="1"><tr><td style="vertical-align: top;">
   <?php if ($win_ie56) { echo "<img alt=\"Syndication Icon\" src=\"/images/rssicon1.gif\" align=\"middle\">"; } else { echo "<img alt=\"Syndication Icon\" src=\"/images/rssicon1.png\" align=\"middle\">"; } ?>
   Syndication Feeds:<br><img style="width: 177px; height: 2px;" alt="spacer" src="/images/sidespacer.png"><br>
   <?php  include("../rsfeed/synfeed.php");  ?><br></td></tr></table>
   </td></tr></table>
