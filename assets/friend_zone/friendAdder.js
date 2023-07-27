function createFriend() {
    section = document.createElement("div");
    section.innerHTML = `<div class="window" style="width: 150px">
                                <div class="title-bar">
                                    <div class="title-bar-text">friend</div>
                                    <div class="title-bar-controls">
                                        <button aria-label="Minimize"></button>
                                        <button aria-label="Maximize"></button>
                                        <button aria-label="Close"></button>
                                    </div>
                                </div>
                                <div class="window-body friend" style="font-size: 30px;">
                                    😳
                                </div>
                            </div>`;
    document.body.appendChild(section);

    section.style.position = 'absolute';
    section.style.left = `${Math.max(200, getRandomInt(screen.availWidth) - 200)}px`;
    section.style.top = `${Math.max(200, getRandomInt(screen.availHeight) - 200)}px`;
    section.style.fontSize = 30; // Cookie stays alive for 30 seconds

    return section;
}

function getRandomInt(max) {
    return Math.floor(Math.random() * max);
}

const numFriends = document.cookie
    .split("; ")
    .find((row) => row.startsWith("friends="))
    ?.split("=")[1];

const friendCounter = document.querySelector('.friend-count');
friendCounter.innerHTML = `Friend count ${numFriends}`;
friendCounter.style.fontSize = 30;

for (var i = 0; i < numFriends; i++) {
    createFriend();
}
