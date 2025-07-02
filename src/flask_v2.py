#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jul  2 15:49:10 2025

@author: matan
"""

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Jan 11 01:58:20 2025

@author: matan
"""

# Initialize the BLIP model and processor
from flask import Flask, request, jsonify
import os
from datetime import datetime
from PIL import Image
import torch
from transformers import BlipProcessor, BlipForConditionalGeneration
import re
processor = BlipProcessor.from_pretrained("Salesforce/blip-image-captioning-large")
model = BlipForConditionalGeneration.from_pretrained("Salesforce/blip-image-captioning-large")

app = Flask(__name__)

# Directory to save uploaded images
UPLOAD_FOLDER = "uploads"
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

TARGET_KEYWORDS = {
    "obstacles": [
        "hole", "pothole", "fence", "barrier", "wall", "step", "stairs", "curb",
        "branch", "tree", "bike", "scooter", "trash", "sign", "construction", "block",
        "pole", "chair", "table", "door", "gate", "glass", "floor", "ledge", "drop", "man", "desk", "refrigerator",
        "person", "bin", "box", "woman"
    ]
}


def clean_and_split(text):
    # Lowercase and extract words only (removes punctuation)
    return re.findall(r'\b\w+\b', text.lower())


@app.route('/upload', methods=['POST'])
def upload_image():
    # Log the received request
    print("Received a request")

    # Check if image data exists in the request
    if not request.data:
        return {"error": "No image data received"}, 400

    # Generate a readable timestamp for the filename
    timestamp = datetime.now().strftime("%Y-%m-%d_%H:%M:%S")
    filename = f"image_{timestamp}.jpg"
    file_path = os.path.join(UPLOAD_FOLDER, filename)

    # Save the image to the uploads folder
    with open(file_path, "wb") as f:
        f.write(request.data)

    print(f"Image saved as {file_path}")

    # Perform image captioning using BLIP
    raw_image = Image.open(file_path).convert('RGB')

    # Conditional image captioning (provide a prompt)
    text = "a photography of"  # You can modify this as needed
    inputs = processor(raw_image, text, return_tensors="pt")

    out = model.generate(**inputs)
    caption_conditional = processor.decode(out[0], skip_special_tokens=True)

    # Unconditional image captioning (no prompt)
    inputs = processor(raw_image, return_tensors="pt")
    out = model.generate(**inputs)
    caption_unconditional = processor.decode(out[0], skip_special_tokens=True)

    words_cond = clean_and_split(caption_conditional)
    words_uncond = clean_and_split(caption_unconditional)

    words = set(words_cond + words_uncond)
    obstacles = list(words.intersection(TARGET_KEYWORDS["obstacles"]))
    words = list(words)
    # Print the captions
    print(f"Conditional Caption: {caption_conditional}")
    print(f"Unconditional Caption: {caption_unconditional}")

    # Return the captions in the response
    return jsonify({
        "message": f"Image saved as {filename}",
        "conditional_caption": caption_conditional,
        "unconditional_caption": caption_unconditional,
        "words": words,
        "obstacles": obstacles

    }), 200


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5015, debug=True)
