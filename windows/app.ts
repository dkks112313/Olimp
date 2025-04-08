const addon = require('../build/Release/screen_capture');

const capture = new addon.SimpleCapture();
capture.startCapture();

setTimeout(() => {
    capture.startCapture();
}, 50000);