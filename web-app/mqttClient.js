import mqtt from "mqtt";

const options = {
    host: "2984d72cfc7a4036be874332eae56b97.s1.eu.hivemq.cloud",
    port: 8883,
    protocol: "mqtts",
    username: "air-quality",
    password: "AirQuality123",
    rejectUnauthorized: false
};

export const mqttClient = mqtt.connect(options);

export let latestData = {};

mqttClient.on("connect", () => {
    console.log("✅ MQTT Connected");
    mqttClient.subscribe("airquality/data");
});

mqttClient.on("message", (topic, message) => {
    if (topic === "airquality/data") {
        latestData = JSON.parse(message.toString());
    }
});
