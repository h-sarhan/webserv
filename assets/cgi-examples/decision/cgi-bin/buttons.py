#!/usr/bin/env python3
import cgi, html, os, sys
commonHTML = """
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>buttons</title>
    <style>
        body {
            background-color: #ece9d8;
        }

        .title-bar-text {
            font-size: 18px;
            padding-left: 1rem;
        }

        .btns {
            display: flex;
            justify-content: space-between;
            padding: 2rem 8rem;
            font-size: 20px;
        }

        .demo {
            display: flex;
            justify-content: center;
            align-items: center;
            text-align: center;
            min-height: 100vh;
        }

        .window-body
        {
            font-size: 18px;
        }
    </style>
    <link rel="stylesheet" href="https://unpkg.com/98.css">
</head>

<body>
    <div class="demo">
        <div class="window" style="width: 700px">
            <div class="title-bar">
                <div class="title-bar-text">Upgrade Windows</div>
                <div class="title-bar-controls">
                    <button aria-label="Minimize"></button>
                    <button aria-label="Maximize"></button>
                    <button aria-label="Close"></button>
                </div>
            </div>
            <div class="window-body">
"""

form = cgi.FieldStorage() # instantiate only once!
btn = form.getfirst('buttonType', 'empty')

# Printing env variables
for name, value in os.environ.items():
    print("{0}: {1}".format(name, value), file=sys.stderr)
# while(1):
#     nothing=True
# Avoid script injection escaping the user input
btn = html.escape(btn)

wise_response = """
    You have made a wise choice
            </div>
    </div>
        </div>
    </body>
    <script>
    function installWindowsXP()
    {{
    }}
    setTimeout(installWindowsXP, 1000);
    </script>
"""

unwise_response = """
    You will regret this
            </div>
    </div>
        </div>
    </body>
    <script>
    function installWindowsVista()
    {{
    }}
    setTimeout(installWindowsVista, 1000);
    </script>
"""

error_response= """
    ERROR
            </div>
    </div>
        </div>
    </body>
"""

if "XP" in btn:
    print(f"Content-Type: text/html\r\nContent-Length: {len(commonHTML) + len(wise_response) + 1}\r\n\r\n{commonHTML}{wise_response}")
elif "Vista" in btn:
    print(f"Content-Type: text/html\r\nContent-Length: {len(commonHTML) + len(unwise_response) + 1}\r\n\r\n{commonHTML}{unwise_response}")
else:
    print(f"Content-Type: text/html\r\nContent-Length: {len(commonHTML) + len(error_response) + 1}\r\n\r\n{commonHTML}{error_response}")
