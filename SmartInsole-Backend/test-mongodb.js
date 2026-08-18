const { MongoClient } = require("mongodb");
require("dotenv").config();

const uri = process.env.MONGODB_URI;
const dbName = process.env.MONGODB_DATABASE;

const client = new MongoClient(uri);

async function testMongoDB() {
    try {
        console.log("Connecting to MongoDB Atlas...");

        await client.connect();

        console.log("✅ MongoDB Atlas connected successfully!");

        const db = client.db(dbName);

        // Test database access
        await db.command({ ping: 1 });

        console.log(`✅ Database "${dbName}" is accessible!`);
    } catch (error) {
        console.error("❌ MongoDB connection failed:");
        console.error(error.message);
    } finally {
        await client.close();
        console.log("MongoDB connection closed.");
    }
}

testMongoDB();