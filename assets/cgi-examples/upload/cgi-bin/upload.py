#!/usr/bin/env python3
import cgi, html, os

form = cgi.FieldStorage()

# A nested FieldStorage instance holds the file
fileitem = form['file']

# Test if the file was uploaded
if fileitem.filename:

    # strip leading path from file name
    # to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    open('files/' + fn, 'wb').write(fileitem.file.read())
    message = 'The file "' + fn + '" was uploaded successfully'

else:
    message = 'No file was uploaded'

print("""
Content-Type: text/html\n
<!DOCTYPE html>
<html>
<body>
<p>%s</p>
</body>
</html>
""" % (message,))