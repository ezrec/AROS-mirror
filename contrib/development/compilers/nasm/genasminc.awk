BEGIN {
    print "LIBVECTSIZE EQU 4"
}

/#define/ {
  line = $0

  if (match(line, /['(']/) == 0)
    next

  # collect all of the line, that may be separated by backslahes
  is_match = match(line, /[\\]/);
  while (is_match > 0)
  {
    # remove that last backslash
    line = substr(line, 0, is_match-1);

    getline
    line = line $0
    is_match = match(line, /[\\]/);
  }

  # replace the newly modified line
  gsub(/[ \t]+/," ",line);
  $0 = line

  # print line

  lib_name = substr($NF, 0, match($NF, /[')']/)-1)
  fn_name  = substr($2, 0, match($2, /['(']/)-1)
  fn_vec   = substr($(NF-1), 0, match($(NF-1), /[',']/)-1)

  print "LVO" lib_name "_" fn_name "\tEQU\t(" fn_vec " * LIBVECTSIZE)"
}

END {
  print "\n"
}

