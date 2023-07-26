#!/usr/bin/env python3
import cgi, html, os
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

# Avoid script injection escaping the user input
btn = html.escape(btn)


if "XP" in btn:
    print(f"""\
    Content-Type: text/html\n
    {commonHTML}
    You have made a wise choice
            </div>
    </div>
        </div>
    </body>
    <script>
    function installWindowsXP()
    {{
        location.assign("https://fakeupdate.net/xp/");
    }}
    setTimeout(installWindowsXP, 1000);
    </script>
    """)
elif "Vista" in btn:
    print(f"""\
    Content-Type: text/html\n
    {commonHTML}
    You will regret this
            </div>
    </div>
        </div>
    </body>
    <script>
    function installWindowsVista()
    {{
        location.assign("https://fakeupdate.net/vista/");
    }}
    setTimeout(installWindowsVista, 1000);
    </script>
    """)
else:
    print(f"""\
    Content-Type: text/html\n
    {commonHTML}
    ERROR
            </div>
    </div>
        </div>
    </body>
    """)
