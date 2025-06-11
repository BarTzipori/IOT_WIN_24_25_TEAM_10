#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 11 13:57:32 2025
@author: matan
"""

from flask import Flask, request, jsonify
import os
from collections import deque

app = Flask(__name__)

UPLOAD_FOLDER = "uploads"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

lidar_points = deque(maxlen=100)

@app.route('/lidar', methods=['POST'])
def receive_lidar_data():
    try:
        data = request.get_json()
        if not isinstance(data, list):
            return {"error": "Expected a JSON array"}, 400

        for sensor_index, item in enumerate(data):
            if 'points' in item and isinstance(item['points'], list):
                for pt in item['points']:
                    x = pt.get('x')
                    y = pt.get('y')
                    if isinstance(x, (int, float)) and isinstance(y, (int, float)):
                        lidar_points.append({'x': x, 'y': y, 'sensor': sensor_index + 1})

        return {"status": "received", "count": len(lidar_points)}, 200

    except Exception as e:
        return {"error": str(e)}, 500


@app.route('/lidar_data')
def get_lidar_data():
    return jsonify(list(lidar_points))


@app.route('/lidar_graph')
def lidar_graph():
    return '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>LiDAR Live Plot</title>
        <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    </head>
    <body>
        <canvas id="scatterChart" width="800" height="500"></canvas>
        <script>
            const ctx = document.getElementById('scatterChart').getContext('2d');

            const colorMap = {
                1: 'rgb(255, 99, 132)',   // red
                2: 'rgb(54, 162, 235)',   // blue
                3: 'rgb(255, 206, 86)',   // yellow
                4: 'rgb(75, 192, 192)'    // green
            };

            const scatterChart = new Chart(ctx, {
                type: 'scatter',
                data: {
                    datasets: []
                },
                options: {
                    animation: false,
                    plugins: {
                        tooltip: {
                            callbacks: {
                                label: function(context) {
                                    const point = context.raw;
                                    return `Sensor ${point.sensor}: (x=${point.x}, y=${point.y})`;
                                }
                            }
                        }
                    },
                    scales: {
                        x: {
                            type: 'linear',
                            position: 'bottom',
                            min: 0,
                            max: 3500,
                            title: { display: true, text: 'Forward Distance (X)' }
                        },
                        y: {
                            min: 0,
                            max: 2500,
                            title: { display: true, text: 'Obstacle Height (Y)' }
                        }
                    }
                }
            });

            async function updateChart() {
                const res = await fetch('/lidar_data');
                const data = await res.json();

                const grouped = {};
                data.forEach(pt => {
                    const sensor = pt.sensor || 0;
                    if (!grouped[sensor]) grouped[sensor] = [];
                    grouped[sensor].push(pt);
                });

                scatterChart.data.datasets = Object.entries(grouped).map(([sensor, points]) => ({
                    label: `Sensor ${sensor}`,
                    data: points,
                    backgroundColor: colorMap[sensor] || 'gray',
                    pointRadius: 5
                }));

                scatterChart.update();
            }

            setInterval(updateChart, 1000);
        </script>
    </body>
    </html>
    '''

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5015, debug=True)