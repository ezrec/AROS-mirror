{
This one formats input and puts each line into a 4 columns wide row in an 
HTML-table. (Nice for automatically generated web-pages.)

Steinar Knutsen <sk@nvg.unit.no>
}

"<table>
"["<td>"[^$1_=~$[%$10=~]?][,]#%"</td>"]p:[^$1_=~]["<tr>
<td>",[^$1_=~$[%$10=~]?][,]#%"</td>"p;!p;!p;!"
"]#%"</table>
"
