/* Test der shutdown.library */

/* Wird möchten über Funktionsergebnisse informiert werden. */

options results

/* Fall noch nicht aktiv, wird versucht die shutdown.library einzubinden. */

if ~show('l',"shutdown.library") then do
   call addlib 'shutdown.library',0,-30,0
end

/* Die einzige zur Verfügung stehende Funktion wird aufgerufen und
 * wenn möglich das Ergebnis ausgegeben.
 */

say shutdown(immediately)
