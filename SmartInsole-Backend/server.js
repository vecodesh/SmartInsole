const mqtt = require("mqtt");
const { MongoClient } = require("mongodb");
require("dotenv").config();

// ===============================
// Configuration
// ===============================

const MQTT_BROKER = "mqtt://localhost:1883";
const MQTT_TOPIC = "smart-insole/sensors";

const MONGODB_URI = process.env.MONGODB_URI;
const DATABASE_NAME = process.env.MONGODB_DATABASE;

const COLLECTION_NAME = "sensor_data";

// ===============================
// MongoDB
// ===============================

const mongoClient = new MongoClient(MONGODB_URI);

let collection;

// ===============================
// MQTT
// ===============================

const mqttClient = mqtt.connect(MQTT_BROKER, {
    clientId: "SmartInsole-Backend-" + Math.random().toString(16).substring(2),
    reconnectPeriod: 5000
});

// ===============================
// Start Backend
// ===============================

async function startServer() {

    try {

        console.log("=================================");
        console.log("   SMART INSOLE BACKEND");
        console.log("=================================");

        // Connect MongoDB
        console.log("Connecting to MongoDB Atlas...");

        await mongoClient.connect();

        console.log("✅ MongoDB Atlas connected!");

        const db = mongoClient.db(DATABASE_NAME);

        collection = db.collection(COLLECTION_NAME);

        console.log(`📊 Database    : ${DATABASE_NAME}`);
        console.log(`📦 Collection  : ${COLLECTION_NAME}`);

        // Connect MQTT
        console.log("Connecting to MQTT broker...");

        mqttClient.on("connect", () => {

            console.log("✅ MQTT connected!");

            mqttClient.subscribe(MQTT_TOPIC, (error) => {

                if (error) {

                    console.error("❌ MQTT subscription failed:");
                    console.error(error.message);

                } else {

                    console.log(`📡 Subscribed to: ${MQTT_TOPIC}`);
                    console.log("---------------------------------");
                    console.log("Waiting for sensor data...");
                }
            });
        });

    } catch (error) {

        console.error("❌ Backend startup failed:");
        console.error(error.message);

        process.exit(1);
    }
}

// ===============================
// MQTT Message Handler
// ===============================

mqttClient.on("message", async (topic, message) => {

    try {

        console.log("\n📨 MQTT message received");

        console.log("Topic:", topic);

        const data = JSON.parse(message.toString());

        console.log("Sensor Data:");
        console.log(data);

        // Add server timestamp
        const document = {
            deviceId: "ESP32-SmartInsole-baf57630",

            timestamp: new Date(),

            sensors: data
        };

        // Save to MongoDB
        const result = await collection.insertOne(document);

        console.log("✅ Saved to MongoDB");
        console.log("Document ID:", result.insertedId);

    } catch (error) {

        console.error("❌ Error processing MQTT message:");
        console.error(error.message);
    }
});

// ===============================
// MQTT Error Handler
// ===============================

mqttClient.on("error", (error) => {

    console.error("❌ MQTT error:");
    console.error(error.message);
});

// ===============================
// Start
// ===============================

startServer();