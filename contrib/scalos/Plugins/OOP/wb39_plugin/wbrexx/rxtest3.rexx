/* rxtest3.rexx */

OPTIONS RESULTS

ADDRESS WORKBENCH

window "Download:" open

address command "wait"

getattr object window.icons.all.count name "Download:" var IconCount

say IconCount || " Icons found."

getattr object window.icons.all name "Download:" stem icons

do n = 0 to IconCount-1
	say "Name = <" || icons.n.name || ">  Width=" || icons.n.width || "  Height=" icons.n.height
end


if rc > 0 then do
	getattr application.lasterror
	res = result
	fault code res
	say "lasterror =" result
	end
