'use strict';
const { SerialPort } = require("serialport");
const fs = require('fs');

const baudRate = 250000;/*caution: BAUD 250k*/
const comName = "com3";


// console.log(process.argv);
const hexFilePath = process.argv[2];
if (!fs.existsSync(hexFilePath)) {
    console.error("file no exists: ", hexFilePath);
    process.exit();
}
const isBin = hexFilePath.substr(hexFilePath.lastIndexOf("."), hexFilePath.length).toLowerCase() === ".bin";

const fileBuf = fs.readFileSync(hexFilePath);
// console.log(fileBuf);

let fileBufInd = 0;
const binBuf = Buffer.alloc(514);
let isBinFileSizeSended = false;

let rowCount = getRowCount(fileBuf);
// console.log(rowCount);

let rs = rowByRow(fileBuf);


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
    // "/h\r\n",
    "/d\r\n",
    "/gi\r\n",
    isBin ? "/pb\r\n" : "/p\r\n",
];

let cmdInd = 0;
let recvBuf = [];
let rowInd = 0;
/*com 打开时会触发uno重置 */
com.on("data", (data) => {
    for (let i = 0; i < data.length; i++) {
        const b = data[i];
        if (b !== 0) {
            recvBuf.push(String.fromCharCode(b));
        } else {
            const s = recvBuf.join('');
            recvBuf = [];
            if (cmdInd < cmdList.length) {
                console.log(">>", s);

                com.write(cmdList[cmdInd]);
                console.log("<<", cmdList[cmdInd]);

                cmdInd++;
            } else {
                if (cmdInd === cmdList.length) {
                    console.log(">>");
                    console.log(s);
                    cmdInd++;
                } else if (!s.startsWith("ok")) {
                    com.write("/r\r\n");
                    console.log("");
                    console.log(">>");
                    console.log(s);
                    console.log("program failed!");
                    setTimeout(() => {
                        process.exit();
                    }, 1000);
                    return;
                }
                if (!isBin) {
                    const r = rs.next();
                    if (!r.done) {
                        com.write(r.value);
                        rowInd++;
                        renderProgressBar(rowInd, rowCount, "programing: ");
                    } else {
                        com.write("/r\r\n");
                        cmdInd = cmdList.length
                        console.log("");
                        console.log("program finish!");
                        setTimeout(() => {
                            process.exit();
                        }, 1000);
                    }
                } else {
                    if (!isBinFileSizeSended) {
                        const size = Math.ceil(fileBuf.length / 512) * 512;
                        const bufSize = Buffer.from([size & 0xFF, (size >> 8) & 0xFF, (size >> 16) & 0xFF, (size >> 24) & 0xFF]);
                        com.write(bufSize);
                        isBinFileSizeSended = true;
                    } else {/*辣鸡SerialPort模块，在一次data回调里面如果多次调用write会卡好几秒，辣鸡。。。 */
                        if (fileBufInd < fileBuf.length) {

                            let checkSum = 0;
                            if (fileBufInd + 512 <= fileBuf.length) {
                                for (let i = 0; i < 512; i++) {
                                    binBuf[i] = fileBuf[fileBufInd];
                                    checkSum += fileBuf[fileBufInd];
                                    fileBufInd++;
                                }
                            } else {
                                let remain = fileBuf.length - fileBufInd;
                                for (let i = 0; i < remain; i++) {
                                    binBuf[i] = fileBuf[fileBufInd];
                                    checkSum += fileBuf[fileBufInd];
                                    fileBufInd++;
                                }
                                for (let i = remain; i < 512; i++) {
                                    binBuf[i] = 0xFF;
                                    checkSum += 0xFF;
                                }
                            }
                            binBuf[512] = checkSum & 0xFF;
                            binBuf[513] = (checkSum >> 8) & 0xFF;
                            com.write(binBuf);
                            renderProgressBar(fileBufInd, fileBuf.length, "programing: ");
                        } else {
                            com.write("/r\r\n");
                            cmdInd = cmdList.length
                            console.log("");
                            console.log("program finish!");
                            setTimeout(() => {
                                process.exit();
                            }, 1000);
                        }
                    }
                }

            }

        }

    }
});



function getRowCount(buf) {
    const charCode = '\n'.charCodeAt(0);
    let row = 0;
    for (let i = 0; i < buf.length; i++) {
        if (buf[i] === charCode) row++;
    }
    return row;
}

function* rowByRow(buf) {
    const charCode = '\n'.charCodeAt(0);
    let row = [];
    for (let i = 0; i < buf.length; i++) {
        const b = buf[i];
        row.push(b);
        if (b === charCode) {
            yield Buffer.from(row);
            row = [];
        }
    }

}

function renderProgressBar(count, totalCount, title) {
    process.stdout.clearLine();
    process.stdout.moveCursor(-200, 0);
    const percent = parseInt((count / totalCount) * 100);
    const c = parseInt(percent / 2);
    process.stdout.write(title);
    process.stdout.write("▮".repeat(c));
    process.stdout.write("▯".repeat(50 - c));
    process.stdout.write("  " + percent.toString() + "% ");
    process.stdout.write(count + "/" + totalCount);
}