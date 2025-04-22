const express = require('express');
const mongoose = require('mongoose');
const cors = require('cors');
require('dotenv').config();

const app = express();
app.use(cors());
app.use(express.json());

mongoose.connect(process.env.MONGO_URI)
  .then(() => console.log('MongoDB connected'))
  .catch(err => console.error(err));

const dataSchema = new mongoose.Schema({
  temperature: Number,
  humidity: Number,
  soil: Number,
  light: Number,
  score: Number,
  createdAt: { type: Date, default: Date.now }
});

const Data = mongoose.model('Data', dataSchema);

app.post('/api/data', async (req, res) => {
  try {
    let { temperature, humidity, soil, light, score } = req.body;

    temperature = parseFloat(temperature);
    humidity = parseFloat(humidity);
    soil = parseFloat(soil);
    light = parseFloat(light);
    score = parseFloat(score);

    if (
      [temperature, humidity, soil, light, score].some(val => isNaN(val))
    ) {
      return res.status(400).json({ error: 'Invalid sensor data: must be valid numbers' });
    }

    const entry = new Data({ temperature, humidity, soil, light, score });
    await entry.save();

    res.sendStatus(201);
  } catch (err) {
    console.error('Error saving data:', err);
    res.sendStatus(500);
  }
});


app.get('/api/data/latest', async (req, res) => {
  const latest = await Data.findOne().sort({ createdAt: -1 });
  res.json(latest);
});

app.get('/api/data/history', async (req, res) => {
  try {
    const history = await Data.find().sort({ createdAt: -1 }).limit(50);
    res.json(history.reverse());
  } catch (err) {
    console.error(err);
    res.sendStatus(500);
  }
});


app.listen(3000, () => console.log('Server running on port 3000'));
