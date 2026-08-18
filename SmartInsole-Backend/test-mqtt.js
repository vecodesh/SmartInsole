const mqtt = require("mqtt");

console.log("Starting MQTT test...");
console.log("Connecting to mqtt://localhost:1883");

const client = mqtt.connect("mqtt://localhost:1883", {
    clientId: "NodeMQTT-Test-" + Math.random().toString(16).substring(2),
    connectTimeout: 5000,
    reconnectPeriod: 0
});

client.on("connect", () => {

    console.log("✅ MQTT CONNECTED!");

    client.subscribe("smart-insole/sensors", (error) => {

        if (error) {
            console.error("❌ Subscription failed:");
            console.error(error.message);
        } else {
            console.log("✅ Subscribed to smart-insole/sensors");
            console.log("Waiting for ESP32 data...");
        }

    });

});

client.on("message", (topic, message) => {

    console.log("📨 Message received!");
    console.log("Topic:", topic);
    console.log("Message:", message.toString());

});

client.on("error", (error) => {

    console.error("❌ MQTT ERROR:");
    console.error(error.message);

});

client.on("close", () => {

    console.log("MQTT connection closed.");

});

client.on("offline", () => {

    console.log("MQTT client offline.");

});