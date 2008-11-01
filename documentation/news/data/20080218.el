====================
Ενημέρωση Κατάστασης
====================

:Συγγραφέας:   Paolo Besser
:Ημερομηνία:     2008-02-18

Τελευταία νέα
-------------

Ο Michal Schulz δουλεύει σκληρά για να μεταφέρει το AROS στην πλατφόρμα
SAM440 της Acube System, και έχει να επιδείξει μερικά ενδιαφέροντα `αποτελέσματα`__. 
Ακολουθεί ένα απόσπασμα μιας πρόσφατης δημοσίευσης του στο `blog του`__: 
"Αποφάσισα να διαχωρίσω τον πυρήνα (και τις βιβλιοθήκες που φορτώνονται μαζί του) 
από το χώρο χρηστών. Ο πυρήνας φορτώνεται κάπου μεταξύ των πρώτων 16MB της RAM και
κατόπιν επανεντοπίζεται στην εικονική διεύθυνση στην κορυφή του 32-bit χώρου διευθύνσεων. 
Ο φορτωτής bootstrap δουλεύει με τον ίδιο τρόπο όπως ο x86_64 bootstrap. Τοποθετεί
όλες τα τμήματα μόνο-για-ανάγνωση πάνω από τη βάση του πυρήνα, και όλα τα εγγράψιμα
τμήματα κάτω από τη βάση του πυρήνα. Επειδή είμαι διάβολος από τη φύση μου, το βασικό
σύστημα του SAM440 AROS μου καταλαμβάνει λαίμαργα όλη τη μνήμη *κάτω* από τη φυσική του 
τοποθεσία. Αυτή η μνήμη (μερικά megabytes) θα χρησιμοποιηθεί ως η τοπική δεξαμενή για 
τον πυρήνα και θα αφαιρεθεί από τις λειτουργίες πρόσβασης των χρηστών παντός τύπου."

Ο Nic Andrews εργάζεται πάνω στο Wanderer, με στόχο να το βελτιώσει και να διορθώσει
κάποια ενοχλητικά bugs. Αυτή τη στιγμή ο Nic "slowly hacking away at reworking the 
rendering code for wanderer iconlist class. The intermediate goal is to 
allow icon windows to buffer the icon/background rendering so, for 
instance, using the tiled rendering mode for iconlist backgrounds won't 
cause noticable icon flickering as it currently does". Περισσότερες και 
πιο λεπτομερείς πληροφορίες σχετικά με το έργο του δημοσιεύονται στο `blog του`__.


Το Internet γίνεται εύκολο
--------------------------

Ο Michael Grunditz έχει επίσημα εκδόσει τις πρώτες δοκιμαστικές εκδόσεις 0.32
του `SimpleMail`__ για το AROS Research Operating System. Το SimpleMail έχει τα
περισσότερα από τα χαρακτηριστικά που χρειάζεται ένας μοντέρνος client ηλ.ταχυδρομείου,
και ακόμα βελτιώνεται. Μπορείτε να κάνετε λήψη της μεταφοράς του στο AROS από τα `Αρχεία`__.

Ο Robert Norris έχει σημειώσει μεγάλη πρόοδο με το Traveller, τον βασισμένο στο 
Webkit περιηγητή σελίδων web για το AROS. Για να το κάνει να λειτουργήσει, χρειάζεται
ακόμα να γράψει κώδικα για κάποια υπολειπόμενα χαρακτηριστικά και βιβλιοθήκες,
εντούτοις η Cairo.library μεταφορά του είναι σε καλό σημείο και κατάφερε επιτυχώς
να εμφανίσει μερικές ιστοσελίδες σωστά. Ένα πολλά `υποσχόμενο screenshot`__ 
δημιοσιεύθηκε στο `blog του`__.


Αλλα ειδήσεις
-------------

Ο Joγo "Hardwired" Ralha έγραψε πρόσφατα μερικούς καλούς οδηγούς χρήσης του AROS.
Παρόλα αυτά, δεν είναι ολοκληρωμένοι, και αυτή τη στιγμή ζητά βοήθεια. 
Τα διαθέσιμα έγγραφα είναι ο `Οδηγός Χρήστη του AROS`__ (50% ολοκλήρωση),
ο `Οδηγός Κελύφους του AROS`__ (70%) και ο `Οδηγός Εγκατάστασης του AROS`__ (25%).
Μπορείτε να επικοινωνήσετε με το συγγραφέα μέσω της `ιστοσελίδας του`__.

Ο Alain Greppin μετέφερε το TeXlive στο AROS, ολοκληρώνοντας ένα bounty. Περισσότερες
πληροφορίες για αυτό υπάρχουν στην `ιστοσελίδα του`__.

Ο Tomek 'Error' Wiszkowski εργάζεται στο Frying Pan, μια εφαρμογή εγγραφής CD/DVD.
Δημοσίευσε `μερικά screenshots`__ στο AROS-Exec.org.
Μπορείτε να κάνετε λήψη της έκδοσης 1.3 για το AROS (shareware) από την `ιστοσελίδα εφαρμογών`__.
Για να το κάνει να δουλέψει, διόρθωσε επίσης μερικά bugs της συσκευής ATA του AROS.

...και, για όποιον έχασε αυτό το νέο: Το σύστημα αρχείων FFS του AROS απέκτησε πρόσφατα
κατάλληλη πιστοποίηση. Τέρμα πια οι άχρηστες κατατμήσεις συστήματος μόνο-για-ανάγνωση!


__ http://msaros.blogspot.com/2008/01/ive-promised-to-show-you-some.html
__ http://msaros.blogspot.com
__ http://kalamatee.blogspot.com/
__ http://simplemail.sourceforge.net/index.php?body=screenshots
__ http://archives.aros-exec.org/index.php?function=showfile&file=network/email/simplemail_beta_aros-i386.tgz
__ http://cataclysm.cx/2008/02/18/cow-launched
__ http://cataclysm.cx
__ http://archives.aros-exec.org/share/document/manual/aros_user_manual_version_0.5a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_shell_manual_version_0.7a.pdf
__ http://archives.aros-exec.org/share/document/manual/aros_install_manual_version_0.25a.pdf
__ http://aros-wandering.blogspot.com
__ http://www.chilibi.org/aros/texlive
__ http://aros-exec.org/modules/newbb/viewtopic.php?viewmode=flat&topic_id=2569&forum=2
__ http://www.tbs-software.com/fp/welcome.phtml
