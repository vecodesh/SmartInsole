const express = require("express");
const { MongoClient } = require("mongodb");
const path = require("path");
require("dotenv").config();

const app = express();
app.use(express.static(path.join(__dirname, "public")));

const PORT = 3000;

const MONGODB_URI = process.env.MONGODB_URI;
const DATABASE_NAME = process.env.MONGODB_DATABASE;
const COLLECTION_NAME = "sensor_data";

const mongoClient = new MongoClient(MONGODB_URI);

let collection;

// ===============================
// Start API Server
// ===============================

async function startAPI() {
    try {
        console.log("=================================");
        console.log("   SMART INSOLE REST API");
        console.log("=================================");

        console.log("Connecting to MongoDB Atlas...");

        await mongoClient.connect();

        console.log("✅ MongoDB Atlas connected!");

        const db = mongoClient.db(DATABASE_NAME);
        collection = db.collection(COLLECTION_NAME);

        console.log(`📊 Database    : ${DATABASE_NAME}`);
        console.log(`📦 Collection  : ${COLLECTION_NAME}`);

        app.get("/", (req, res) => {
            res.json({
                message: "Smart Insole API is running",
                status: "OK"
            });
        });

        // Get latest sensor reading
        app.get("/api/sensors/latest", async (req, res) => {
            try {
                const latestData = await collection
                    .findOne({}, { sort: { timestamp: -1 } });

                if (!latestData) {
                    return res.status(404).json({
                        message: "No sensor data found"
                    });
                }

                res.json(latestData);

            } catch (error) {
                console.error(error);
                res.status(500).json({
                    message: "Failed to fetch sensor data"
                });
            }
        });

        // Get sensor history
        app.get("/api/sensors/history", async (req, res) => {
            try {
                const data = await collection
                    .find({})
                    .sort({ timestamp: -1 })
                    .limit(100)
                    .toArray();

                res.json(data);

            } catch (error) {
                console.error(error);
                res.status(500).json({
                    message: "Failed to fetch sensor history"
                });
            }
        });

        app.listen(PORT, () => {
            console.log(`🚀 API Server running at http://localhost:${PORT}`);
            console.log(`📡 Latest data: http://localhost:${PORT}/api/sensors/latest`);
            console.log(`📊 History: http://localhost:${PORT}/api/sensors/history`);
        });

    } catch (error) {
        console.error("❌ API startup failed:");
        console.error(error.message);
        process.exit(1);
    }
}

startAPI();