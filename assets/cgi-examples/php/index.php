
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>counter</title>
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
            justify-content: center;
            padding: 1rem;
            font-size: 20px;
        }

        .demo {
            display: flex;
            justify-content: center;
            align-items: center;
            text-align: center;
            min-height: 100vh;
        }
    </style>
    <link rel="stylesheet" href="https://unpkg.com/xp.css">
</head>
<body>
    <div class="demo">
        <div class="window" style="width: 700px">
            <div class="title-bar">
                <div class="title-bar-text">The friend zone 😳</div>
                <div class="title-bar-controls">
                    <button aria-label="Minimize"></button>
                    <button aria-label="Maximize"></button>
                    <button aria-label="Close"></button>
                </div>
            </div>
            <div class="window-body">
                <form action="cgi-bin/friend_zone.php" method="get">
                    <div class="btns">
                        <input type="submit" name="buttonType" value="Join us"
                            style="font-size:14px; padding: 0.5rem; " />
                    </div>
                </form>
            </div>
        </div>
    </div>
</body>
</html>