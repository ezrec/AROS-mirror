/* here it is a bug of interpret */
say "Calling PROCEDURE with INTERPRET"
interpret 'call proc'
say 'Normal CALL of the PROCEDURE'
call proc
say 'Procedure Finished'
exit

proc:
   say 'we are doing INTERPRET "return"'
   interpret 'return'

proc2:
   say 'It should not come here!!'
   return
