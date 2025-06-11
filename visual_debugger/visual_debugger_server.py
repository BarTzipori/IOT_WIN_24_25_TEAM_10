#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#goto http://127.0.0.1:5015/lidar_graph for lidar graph
"""
Final Version — Legend Cleaned with Inline Labels Preserved
"""

from flask import Flask, request, jsonify
import os
from collections import deque

app = Flask(__name__)
lidar_points = deque(maxlen=100)
os.makedirs("uploads", exist_ok=True)

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
    </head>
    <body>
        <h2>LiDAR Visualization</h2>
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

            const scatterChart = new Chart(ctx, {
                type: 'scatter',
                data: { datasets: [] },
                options: {
                    animation: false,
                    plugins: {
                        legend: {
                            labels: {
                                filter: function(item) {
                                    // Filter out datasets like "latest_1", "latest_2", etc.
                                    return !item.text.startsWith('latest_');
                                }
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
                            min: 0,
                            max: 4500,
                            title: { display: true, text: 'Forward Distance (X)' }
                        },
                        y: {
                            min: 0,
                            max: 4500,
                            title: { display: true, text: 'Obstacle Height (Y)' }
                        }
                    }
                },
                plugins: [ChartDataLabels]
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

                const datasets = [];

                Object.entries(grouped).forEach(([sensor, points]) => {
                    const color = colorMap[sensor] || 'gray';
                    const allButLast = points.slice(0, -1);
                    const last = points[points.length - 1];

                    // Regular dataset (visible in legend)
                    if (allButLast.length > 0) {
                        datasets.push({
                            label: `Sensor ${sensor}`,
                            data: allButLast,
                            backgroundColor: color,
                            pointRadius: 5,
                            datalabels: { display: false }
                        });
                    }

                    // Latest point with inline label but no legend entry
                    if (last) {
                        datasets.push({
                            label: `latest_${sensor}`,  // Will be filtered out
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

                // Fringe line in sensor order, only if all x >= 10
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

                scatterChart.data.datasets = datasets;
                scatterChart.update();
            }

            async function clearData() {
                await fetch('/clear_lidar', { method: 'POST' });
            }

            function toggleFringe() {
                showFringe = !showFringe;
            }

            setInterval(updateChart, 1000);
        </script>
    </body>
    </html>
    '''

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5015, debug=True)