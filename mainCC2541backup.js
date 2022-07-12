'use strict';
const { SerialPort } = require("serialport");
const fs = require('fs');

const baudRate = 250000;/*caution: BAUD 250k*/
const comName = "com3";

// console.log(process.argv);
let backupFilePath = process.argv[2];
if (!backupFilePath) {
    backupFilePath = "cc2541FlashBackup.bin";
}
let baseName = backupFilePath.substr(0, backupFilePath.lastIndexOf("."));
let extName = backupFilePath.substr(backupFilePath.lastIndexOf("."), backupFilePath.length);
let suffix = 0;
console.log(baseName, extName);

while (fs.existsSync(backupFilePath)) {
    suffix++;
    backupFilePath = `${baseName}(${suffix})${extName}`;
}



const com = new SerialPort({ path: comName, baudRate });

com.on("error", (err) => console.error("Serial Error: ", err));
com.on("pause", () => console.log("com pause!"));
com.on("end", () => console.log("com end!"));
com.on("close", () => console.log("com closed!"));
com.on("resume", () => console.log("com resume"));
com.addListener("drain", () => {
    console.log("drain");
});

const cmdList = [
    "/d\r\n",
    "/gs\r\n",
    "/fr\r\n",
];

let cmdInd = 0;
let recvBuf = [];
let state = 0;
let flashSize = 0;
let buf = null;
let bufInd = 0;

/*com 打开时会触发uno重置 */
com.on("data", (data) => {

    switch (state) {
        case 0:
            for (let i = 0; i < data.length; i++) {
                const b = data[i];
                if (b !== 0) {
                    recvBuf.push(String.fromCharCode(b));
                } else {
                    const s = recvBuf.join('');
                    recvBuf = [];
                    console.log(s);
                    if (cmdList[cmdInd - 1] === "/gs\r\n") {
                        const m = /DEBUG_LOCKED:.*(\d)/i.exec(s);/*判断芯片是不是允许debug */
                        if (m && m[1] != '0') {
                            console.warn("Caution: cc2541 debug locked is set! Maybe cannot make a backup!");
                        }
                    }
                    com.write(cmdList[cmdInd]);
                    cmdInd++;
                    if (cmdInd == cmdList.length) {
                        state = 1;
                    }
                }
            }
            break;
        case 1:

            for (let i = 0; i < data.length; i++) {
                recvBuf.push(data[i]);
            }

            if (flashSize === 0 && recvBuf.length >= 4) {
                flashSize = Buffer.from(recvBuf).readUInt32LE(0);
                console.log("flash size: ", flashSize);
                flashSize += 4;/*add last 4 bytes are checkSum */
                buf = Buffer.alloc(flashSize);
                for (let i = 4; i < recvBuf.length; i++) {
                    buf[bufInd] = recvBuf[i];
                    bufInd++;
                }
                recvBuf = [];
                state = 2;
            }
            break;
        case 2:
            for (let i = 0; i < data.length; i++) {
                buf[bufInd] = data[i];
                bufInd++;
            }
            renderProgressBar(bufInd, flashSize, "reading: ");
            if (bufInd >= flashSize) {
                const checkSum0 = buf.readUInt32LE(flashSize - 4);
                flashSize -= 4;/*remove last 4 bytes are checkSum*/
                console.log("");
                console.log("Evaluating check sum...");
                let checkSum1 = 0;
                for (let i = 0; i < flashSize; i++) {
                    checkSum1 += buf[i];
                }
                checkSum1 &= 0xFFFFFFFF;
                if (checkSum1 === checkSum0) {
                    const fd = fs.openSync(backupFilePath, 'w');
                    fs.writeSync(fd, buf, 0, flashSize);
                    console.log("backup file created: ", backupFilePath);
                } else {
                    console.log("check sum failed: ", checkSum0, checkSum1);
                }
                com.write("/r\r\n");/*cc2541 exit debug mode */
                setTimeout(() => {
                    process.exit();
                }, 500);
            }
            break;

        default:
            break;
    }

});


function renderProgressBar(count, totalCount, title) {
    process.stdout.clearLine();
    process.stdout.moveCursor(-200, 0);
    const percent = parseInt((count / totalCount) * 100);
    const c = parseInt(percent / 2);
    process.stdout.write(title);
    // process.stdout.write("▮".repeat(c));
    // process.stdout.write("▯".repeat(50 - c));
    process.stdout.write("  " + percent.toString() + "% ");
    process.stdout.write(count + "/" + totalCount);
}