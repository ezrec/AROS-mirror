BEGIN {
    print "LIB_VECTSIZE EQU 4"
    print
}

# looking for the Function definition
/#define/ {

  # Make a working copy of the input
  line = $0

  # Make sure it looks like a function, i.e. has an open parenthesis
  if (match(line, /['(']/) == 0)
    next

  # collect all of the line, that may be separated by backslahes
  is_match = match(line, /[\\]/);
  while (is_match > 0)
  {
    # remove that last backslash
    line = substr(line, 0, is_match-1);

    # Append the next line
    getline
    line = line $0

    # and test that it also ends in a backslash
    is_match = match(line, /[\\]/);
  }

  # collapse whitespace
  gsub(/[ \t]+/," ",line);

  # replace the modified line, re-evaluate fields
  $0 = line

  # Now, extract the fields we are really interested in

  lib_name = substr($NF, 0, match($NF, /[')']/)-1)
  fn_name  = substr($2, 0, match($2, /['(']/)-1)

  fn_vec   = substr($(NF-1), 0, match($(NF-1), /[',']/)-1)

  lvo_name = "LVO" lib_name "_" fn_name

  printf("%-32s\tEQU\t( %3d * LIB_VECTSIZE)\n", lvo_name, fn_vec)
}

END {
  print "\n"
}
















