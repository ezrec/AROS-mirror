<?php

$feed = file_get_contents('http://archives.aros-exec.org/modules/rssfeed.php');
$rss = new SimpleXmlElement($feed);

foreach($rss->channel->item as $entry) {
echo "

link' title='$entry->title'>" . $entry->title . "
";
}

?>
