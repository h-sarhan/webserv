#!/usr/bin/env python3
import cgi, html, os, sys

for name, value in os.environ.items():
    print("{0}: {1}".format(name, value), file=sys.stderr)
path = os.getcwd()
print("\ncurrent working dir = ", path, file=sys.stderr)
form = cgi.FieldStorage()
# print(form, file=sys.stderr)

# try:
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

# except:
#     message = 'Upload error'

body = """<!DOCTYPE html>
<html>
<body>
<p>%s</p>
</body>
</html>
""" % (message,)

print(f"Content-Type: text/html\r\nContent-Length: {len(body) + 1}\r\n\r\n{body}")