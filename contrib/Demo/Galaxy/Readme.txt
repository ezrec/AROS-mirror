                  __________________
 ________________/  the CTX files   \
|                                    |
| galaxy                             |
| 16 & 32 bpp                        |
|                                    |
| corTeX / Optimum  29 oct. 1998     |
|                                    |
|____________________________________|


[ La version francaise se trouve a la fin... ]

At last !.. After 2 months of inactivity, here comes a new 
FX...
In fact this FX was already working a long time ago, but
it was developped using my local working version of the
Optilib(R)(tm), so I had to convert it back to an independant
program... and due to the unbelievable laziness that 
caracterizes us, it took very long !..
[ perhaps others FX that are sleeping on my HD will follow...
  but we also need to keep some of them secret if we want
  to write a real demo which is more than just a compilation
  of the site's FX... ]

--Begin Interesting text -- cut here --

This FX is an extended StarField... No, dont leave now !..
This is not 16 pixels moving from the center of the screen !

This is a galaxy formed by 5000 'stars' (or nebulae..) where
we fly into; I re-used the shaded-disk routine that was in the
light-flare fx to display the zoomed 'stars'.
This routine still doesnt know about horizontal clipping, so
the result is a bit buggy...

The program can be run with a command-line parameter :
 -blur   : activates motion blur (default)
 -noblur : guess ?..

At last ! the program is timed (using the schedul_fx module), so
animation speed no more depends of your CPU. This is a very basic
module, but you're encouraged to use it, it should be useful.

This time rotations are done using matrix multiplication instead
of angle-by-angle rotation: it saves time when you have 5000 points...


--End Interesting text -- cut here --



Et maintenant, la VF...


Enfin ! Apres 2 mois d'inactivite, voila le nouvel FX...
Il etait en fait deja pret depuis pas mal de temps, mais il marchait
avec ma version locale de l'Optilib(R)(tm), donc il fallait que je
le re-convertisse en prog autonome... et notre glandouille legendaire
a fait que ca a pris bcp de temps !..
[ peut-etre que d'autres FX qui trainent sur mon DD suivront...mais
  on a aussi besoin d'en garder qq uns secrets si on veut ecrire une
  vraie demo qui soit pas une simple compil' des fx du site... ]

Bon treve de blabla, l'fx commence ici :

Cet FX est un starfield etendu... Eh partez pas !...
C'est pas 16 pixels minables qui filent droit a partir du 
centre de l'ecran!..

C'est une galaxie formee de 5000 'etoiles' (ou nebuleuses)
ou l'on virevolte; j'ai reutilise la routine de shaded-disk
de l'fx LightFlares pour afficher les 'etoiles' zoomees.
Cette routine ne connait toujours rien au clipping horizontal,
donc l'affichage est encore un peu buggue..

Le prog accepte un parametre en ligne de commande :
 -blur    : active le motion blur (par defaut)
 -noblur  : devinez ?..

Enfin! Le prog est time' (avec le module schedul_fx), donc
la vitesse d'animation ne depend plus de la puissance cpu.
C'est un module de base, mais je vous encourage a l'utiliser,
il devrait s'averer tres pratique.

Cette fois les rotations sont calculees avec des matrices
plutot que angle par angle: ca gagne du temps quand on a
5000 points a rotater...

--

corTeX / Optimum, Thu Oct 29 1998
