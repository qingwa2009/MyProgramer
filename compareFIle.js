'use strict';
const fs = require('fs');

const fileBuf0 = fs.readFileSync("cc2541FlashBackup.bin");
const fileBuf1 = fs.readFileSync("D:/desktop/cc2541/cc2541template/Projects/ble/SimpleBLEPeripheral/CC2541DB/CC2541/Exe/SimpleBLEPeripheral.bin");

function compare(buf0, buf1) {
    for (let index = 0; index < buf0.length; index++) {
        if (buf0[index] !== buf1[index]) {
            if ((buf0[index] === 0xFF || buf0[index] === 0x00) && (buf1[index] === 0xFF || buf1[index] == 0x00))
                continue;
            console.error(`index: ${index} file0: ${buf0[index]}, file1: ${buf1[index]}`);
            break;
        }
    }
}

compare(fileBuf0, fileBuf1);
// console.log(fileBuf);
