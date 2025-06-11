import json
import threading
import time
from datetime import datetime
from http.server import HTTPServer, BaseHTTPRequestHandler
from collections import deque
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np

# Data storage
data_buffer = deque(maxlen=1000)  # Store last 1000 data points
data_lock = threading.Lock()

class DistanceDataHandler(BaseHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/distance_data':
            # Get content length
            content_length = int(self.headers['Content-Length'])
            
            # Read POST data
            post_data = self.rfile.read(content_length)
            
            try:
                # Parse JSON data - expecting array of sensor data
                sensor_data = json.loads(post_data.decode('utf-8'))
                
                # Add timestamp for plotting
                data_point = {
                    'timestamp': time.time(),
                    'sensors': sensor_data
                }
                
                # Thread-safe data storage
                with data_lock:
                    data_buffer.append(data_point)
                
                print(f"Received data from {len(sensor_data)} sensors")
                for i, sensor in enumerate(sensor_data):
                    point = sensor['points'][0]
                    print(f"  Sensor {i+1}: x={point['x']}mm, y={point['y']}mm")
                
                # Send response
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({"status": "success"}).encode())
                
            except json.JSONDecodeError:
                self.send_response(400)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                self.wfile.write(json.dumps({"error": "Invalid JSON"}).encode())
        else:
            self.send_response(404)
            self.end_headers()
    
    def log_message(self, format, *args):
        # Suppress default logging to keep console clean
        pass

def run_server():
    """Run HTTP server in separate thread"""
    server_address = ('', 8000)  # Listen on all interfaces, port 8000
    httpd = HTTPServer(server_address, DistanceDataHandler)
    print("HTTP Server running on port 8000...")
    print("Make sure to update ESP32 code with this PC's IP address")
    print(server_address)
    httpd.serve_forever()

def animate_graph(frame):
    """Animation function for real-time graph"""
    with data_lock:
        if len(data_buffer) == 0:
            return
        
        # Get the latest data point
        latest_data = data_buffer[-1]
    
    # Extract sensor data
    sensors = latest_data['sensors']
    
    if len(sensors) == 0:
        return
    
    # Clear previous plots
    plt.clf()
    
    # Create subplots
    fig = plt.gcf()
    fig.suptitle('ESP32 4-Sensor Distance Visualization', fontsize=16)
    
    # Plot 1: 2D scatter plot (top view - x vs z)
    plt.subplot(2, 2, 1)
    colors = ['red', 'blue', 'green', 'orange']
    
    for i, sensor in enumerate(sensors):
        point = sensor['points'][0]
        x = point['x']  # forward distance
        y = point['y']  # height
        
        plt.scatter(x, 0, c=colors[i % len(colors)], s=100, label=f'Sensor {i+1}', alpha=0.8)
        plt.text(x, 0.1, f'S{i+1}\n{x}mm', ha='center', va='bottom', fontsize=8)
    
    plt.xlabel('Forward Distance (mm)')
    plt.ylabel('Position')
    plt.title('Sensor Distances (Top View)')
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.ylim(-0.5, 0.5)
    
    # Plot 2: Height view
    plt.subplot(2, 2, 2)
    sensor_positions = range(len(sensors))
    heights = [sensor['points'][0]['y'] for sensor in sensors]
    
    bars = plt.bar(sensor_positions, heights, color=colors[:len(sensors)], alpha=0.7)
    
    # Add value labels on bars
    for i, (bar, height) in enumerate(zip(bars, heights)):
        plt.text(bar.get_x() + bar.get_width()/2, height + 10, 
                f'{height}mm', ha='center', va='bottom', fontsize=9)
    
    plt.xlabel('Sensor Number')
    plt.ylabel('Height (mm)')
    plt.title('Sensor Heights')
    plt.xticks(sensor_positions, [f'S{i+1}' for i in sensor_positions])
    plt.grid(True, alpha=0.3, axis='y')
    
    # Plot 3: Time series of distances (if we have history)
    plt.subplot(2, 2, 3)
    with data_lock:
        if len(data_buffer) > 1:
            # Plot distance over time for each sensor
            times = []
            sensor_distances = [[] for _ in range(4)]  # Assuming 4 sensors max
            
            for data_point in list(data_buffer)[-50:]:  # Last 50 points
                times.append(data_point['timestamp'])
                for i, sensor in enumerate(data_point['sensors']):
                    if i < len(sensor_distances):
                        sensor_distances[i].append(sensor['points'][0]['x'])
            
            if len(times) > 1:
                start_time = times[0]
                relative_times = [(t - start_time) for t in times]
                
                for i, distances in enumerate(sensor_distances):
                    if distances:  # Only plot if we have data
                        plt.plot(relative_times, distances, 
                               color=colors[i % len(colors)], 
                               label=f'Sensor {i+1}', 
                               linewidth=2, alpha=0.8)
    
    plt.xlabel('Time (seconds)')
    plt.ylabel('Distance (mm)')
    plt.title('Distance History')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # Plot 4: 3D-like visualization (side view)
    plt.subplot(2, 2, 4)
    for i, sensor in enumerate(sensors):
        point = sensor['points'][0]
        x = point['x']  # forward distance  
        y = point['y']  # height
        
        plt.scatter(x, y, c=colors[i % len(colors)], s=100, label=f'Sensor {i+1}', alpha=0.8)
        plt.text(x + 20, y, f'S{i+1}', fontsize=8)
    
    plt.xlabel('Forward Distance (mm)')
    plt.ylabel('Height (mm)')
    plt.title('2D Side View (X vs Y)')
    plt.grid(True, alpha=0.3)
    plt.legend()
    
    # Adjust layout
    plt.tight_layout()

def main():
    # Start HTTP server in background thread
    server_thread = threading.Thread(target=run_server, daemon=True)
    server_thread.start()
    
    # Set up real-time plotting
    plt.ion()  # Turn on interactive mode
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Create animation
    ani = animation.FuncAnimation(fig, animate_graph, interval=100, cache_frame_data=False)
    
    print("Real-time graph started. Waiting for ESP32 data...")
    print("Close the graph window to stop the program.")
    
    try:
        plt.show()
        # Keep the main thread alive
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("\nShutting down...")

if __name__ == "__main__":
    main()