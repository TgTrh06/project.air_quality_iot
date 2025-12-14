import express from "express";
import cors from "cors";
import http from "http";
import { Server } from "socket.io";
import { mqttClient, latestData } from "./mqttClient.js";

const app = express();
const server = http.createServer(app);
const io = new Server(server);

app.use(cors());
app.use(express.json());
app.use(express.static("public"));

/* REST API */
app.get("/api/data", (req, res) => {
    res.json(latestData);
});

app.post("/api/control", (req, res) => {
    mqttClient.publish("esp32/control", JSON.stringify(req.body));
    res.json({ status: "sent" });
});

/* WebSocket realtime */
io.on("connection", (socket) => {
    console.log("📡 Web connected");

    socket.emit("sensor", latestData);

    setInterval(() => {
        socket.emit("sensor", latestData);
    }, 2000);
});

server.listen(3000, () => {
    console.log("🚀 Server running: http://localhost:3000");
});