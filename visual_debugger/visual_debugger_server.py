#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
LiDAR Visualizer with User Height Lines (No System Height), Auto Scale Toggle,
Smaller History and Fast Refresh without Slide Animation
"""

from flask import Flask, request, jsonify
import os
from collections import deque
import socket
import firebase_admin
from firebase_admin import credentials, db

app = Flask(__name__)
lidar_points = deque(maxlen=40)
os.makedirs("uploads", exist_ok=True)

# Initialize Firebase Admin SDK
cred = credentials.Certificate("firebase_key.json")  # Place your service account key here
firebase_admin.initialize_app(cred, {
    'databaseURL': 'https://safestep-2bc31-default-rtdb.europe-west1.firebasedatabase.app'
})

# Update IP in Firebase on server start using actual local network IP
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        s.connect(("8.8.8.8", 80))
        return s.getsockname()[0]
    finally:
        s.close()

local_ip = get_local_ip()
ref = db.reference('System_Settings/esp_target_ip')
ref.set(local_ip)
print(f"Local IP {local_ip} written to Firebase")

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

@app.route('/clear_lidar', methods=['POST'])
def clear_lidar_data():
    lidar_points.clear()
    return {"status": "cleared", "count": 0}, 200

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
        <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-datalabels"></script>
        <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-annotation@1.4.0"></script>
    </head>
    <body>
        <h2>LiDAR Visualization</h2>
        <div>
            <label>User Height (mm): <input id="userHeight" type="number" value="175"></label>
            <button onclick="updateReferenceLines()">Update</button>
            <label><input type="checkbox" id="toggleBodyLines" checked onchange="toggleReferenceLines()"> Show Reference Lines</label>
            <label><input type="checkbox" id="autoScale" checked> Auto Scale Axes</label>
        </div>
        <button onclick="clearData()">Clear Data</button>
        <button onclick="toggleFringe()">Toggle Fringe Line</button>
        <canvas id="scatterChart" width="800" height="500"></canvas>

        <script>
            let showFringe = false;

            const ctx = document.getElementById('scatterChart').getContext('2d');
            const colorMap = {
                1: 'rgb(255, 99, 132)',
                2: 'rgb(54, 162, 235)',
                3: 'rgb(255, 206, 86)',
                4: 'rgb(75, 192, 192)'
            };

            const annotationLines = {};

            const scatterChart = new Chart(ctx, {
                type: 'scatter',
                data: { datasets: [] },
                options: {
                    animation: false,
                    plugins: {
                        annotation: {
                            annotations: annotationLines
                        },
                        legend: {
                            labels: {
                                filter: item => !item.text.startsWith('latest_')
                            }
                        },
                        datalabels: {
                            formatter: value => 'S' + value.sensor,
                            anchor: 'end',
                            align: 'top',
                            font: { weight: 'bold' }
                        },
                        tooltip: {
                            callbacks: {
                                label: context => {
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
                            title: { display: true, text: 'Forward Distance (X)' }
                        },
                        y: {
                            title: { display: true, text: 'Obstacle Height (Y)' }
                        }
                    }
                },
                plugins: [ChartDataLabels]
            });

            function toggleReferenceLines() {
                const showLines = document.getElementById('toggleBodyLines').checked;
                if (!showLines) {
                    scatterChart.options.plugins.annotation.annotations = {};
                } else {
                    updateReferenceLines();
                }
                scatterChart.update();
            }

            function updateReferenceLines() {
                const userHeight = parseFloat(document.getElementById('userHeight').value);
                const showLines = document.getElementById('toggleBodyLines').checked;

                if (!showLines) return;

                scatterChart.options.plugins.annotation.annotations = {
                    chest: {
                        type: 'line',
                        yMin: 0.6 * userHeight,
                        yMax: 0.6 * userHeight,
                        borderColor: 'green',
                        borderWidth: 1,
                        label: {
                            content: 'Chest',
                            enabled: true,
                            position: 'start'
                        }
                    },
                    head: {
                        type: 'line',
                        yMin: 0.9 * userHeight,
                        yMax: 0.9 * userHeight,
                        borderColor: 'red',
                        borderWidth: 1,
                        label: {
                            content: 'Head',
                            enabled: true,
                            position: 'start'
                        }
                    },
                    fullHeight: {
                        type: 'line',
                        yMin: userHeight,
                        yMax: userHeight,
                        borderColor: 'black',
                        borderDash: [5, 5],
                        borderWidth: 1,
                        label: {
                            content: 'User Height',
                            enabled: true,
                            position: 'start'
                        }
                    }
                };

                scatterChart.update();
            }

            async function updateChart() {
                const res = await fetch('/lidar_data');
                const data = await res.json();

                const grouped = {};
                data.forEach(pt => {
                    const sensor = pt.sensor || 0;
                    if (!grouped[sensor]) grouped[sensor] = [];
                    grouped[sensor].push(pt);
                });

                const datasets = [];

                Object.entries(grouped).forEach(([sensor, points]) => {
                    const color = colorMap[sensor] || 'gray';
                    const allButLast = points.slice(0, -1);
                    const last = points[points.length - 1];

                    if (allButLast.length > 0) {
                        datasets.push({
                            label: `Sensor ${sensor}`,
                            data: allButLast,
                            backgroundColor: color,
                            pointRadius: 5,
                            datalabels: { display: false }
                        });
                    }

                    if (last) {
                        datasets.push({
                            label: `latest_${sensor}`,
                            data: [last],
                            backgroundColor: color,
                            pointRadius: 6,
                            datalabels: {
                                display: true,
                                formatter: value => 'S' + value.sensor,
                                anchor: 'end',
                                align: 'top',
                                font: { weight: 'bold' }
                            }
                        });
                    }
                });

                if (showFringe) {
                    const latestPointsOrdered = [1, 2, 3, 4]
                        .map(sensor => {
                            const points = grouped[sensor];
                            return points ? points[points.length - 1] : null;
                        })
                        .filter(p => p && p.x >= 10);

                    if (latestPointsOrdered.length === 4) {
                        datasets.push({
                            label: 'Fringe Line',
                            data: latestPointsOrdered,
                            showLine: true,
                            borderColor: 'black',
                            borderDash: [5, 5],
                            backgroundColor: 'transparent',
                            pointRadius: 0,
                            fill: false,
                            datalabels: { display: false }
                        });
                    }
                }

                const autoScale = document.getElementById('autoScale').checked;
                scatterChart.options.scales.x.min = autoScale ? undefined : 0;
                scatterChart.options.scales.x.max = autoScale ? undefined : 4500;
                scatterChart.options.scales.y.min = autoScale ? undefined : 0;
                scatterChart.options.scales.y.max = autoScale ? undefined : 4500;

                scatterChart.options.animation = false;
                scatterChart.data.datasets = datasets;
                scatterChart.update();
            }

            async function clearData() {
                await fetch('/clear_lidar', { method: 'POST' });
            }

            function toggleFringe() {
                showFringe = !showFringe;
            }

            setInterval(updateChart, 250);
            window.onload = updateReferenceLines;
        </script>
    </body>
    </html>
    '''

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5015, debug=True)