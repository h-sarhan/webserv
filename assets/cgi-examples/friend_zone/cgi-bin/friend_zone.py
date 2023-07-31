#!/usr/bin/env python3
import hashlib, time, os, sys, cgi, html
from http.cookies import SimpleCookie

friend_file = open("./friend-zone.html", "r")
for name, value in os.environ.items():
    print("{0}: {1}".format(name, value), file=sys.stderr)

try:
    friend_file = open(os.environ.get("PATH_TRANSLATED"), "r")

    #read whole file to a string
    friend_html = friend_file.read()

    #close file
    friend_file.close()

    form = cgi.FieldStorage() # instantiate only once!
    btn = form.getfirst('add', 'empty')
    btn = html.escape(btn)

    cookies = SimpleCookie(os.getenv("HTTP_COOKIE"))

    try:
        if "friend" in btn:
            friends = int(cookies["friends"].value) + 1
        else:
            friends = int(cookies["friends"].value)
    except KeyError:
        friends = 1

    cookies["friends"] = str(friends)
    cookies["friends"]["max-age"] = 20  # Expire after 20 seconds


    print(f"Content-Type: text/html\r\nContent-Length: {len(friend_html) + 4}\r\n{cookies}\r\n\r\n{friend_html}")
except:
    err_message = "wrong PATH_INFO/PATH_TRANSLATED set"
    print(f"Content-Type: text/html\r\nContent-Length: {len(err_message) + 1}\r\n\r\n{err_message}")