"""
    Convert old-style size DB to new-style (CSV) size DB.
    Reads from stdin, writes to stdout.

    Example:
        % python convertSizes.py <aros.size.old >aros.size.new
"""

import sys

for line in sys.stdin:
    words = line.split()
    sys.stdout.write( '%s;%s\n' % (words[0].strip(), words[1].strip()) )
