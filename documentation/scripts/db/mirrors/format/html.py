
def format( mirrors ):
    output  = '<form action="http://aros.sourceforge.net/tools/redirect.php" method="get">'
    output += '<select name="url">'

    for mirror in mirrors:
        output += '<option value="' + mirror[1] + '">' + mirror[0] + '</option>'

    output += '</select>'
    output += '<input type="submit" value="Go">'
    output += '</form>'

    return output
