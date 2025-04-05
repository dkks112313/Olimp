const express = require("express");
const fs = require("fs");

const app = express();

const port = 3000;

app.use(express.static("public"));

const filePath = "./public/2024-06-18 19-49-42.mp4";

app.get("/works-in-chrome", (req: any, res: any) => {
    res.setHeader("content-type", "video/mp4");
    
    fs.stat(filePath, (err: any, stat: any) => {
        if (err) {
            console.error(`File stat error for ${filePath}.`);
            console.error(err);
            res.sendStatus(500);
            return;
        }

        res.setHeader("content-length", stat.size);

        const fileStream = fs.createReadStream(filePath);
        fileStream.on("error", (error: any) => {
            console.log(`Error reading file ${filePath}.`);
            console.log(error);
            res.sendStatus(500);
        });

        fileStream.pipe(res)
    });
});

app.listen(port, () => {
    console.log(`Open your browser and navigate to http://localhost:${port}`)
});