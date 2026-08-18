require("dotenv").config();
const mongoose = require("mongoose");

async function testMongoDB() {
    try {
        console.log("🔄 Connecting to MongoDB Atlas with Mongoose...");
        console.log(`📊 Database: ${process.env.MONGODB_DATABASE}`);

        await mongoose.connect(process.env.MONGODB_URI);

        console.log("✅ MongoDB Atlas connected successfully!");
        console.log(`📦 Connected to database: ${mongoose.connection.db.databaseName}`);

        // Test creating a collection
        const collections = await mongoose.connection.db.listCollections().toArray();
        console.log(`📂 Existing collections: ${collections.map(c => c.name).join(', ') || 'None yet'}`);

        console.log("✅ Database is accessible and ready!");
        
    } catch (error) {
        console.error("❌ MongoDB connection failed:");
        console.error(`Error: ${error.message}`);
        
        // Helpful error messages
        if (error.message.includes('Authentication failed')) {
            console.error('🔑 Fix: Check your username and password in .env');
            console.error('   Remember to URL encode special characters like ! → %21');
        } else if (error.message.includes('ENOTFOUND')) {
            console.error('🌐 Fix: Check your cluster address in .env');
        } else if (error.message.includes('whitelist')) {
            console.error('🛡️ Fix: Add your IP to the whitelist in MongoDB Atlas');
        }
    } finally {
        await mongoose.disconnect();
        console.log("🔌 MongoDB connection closed.");
    }
}

testMongoDB();