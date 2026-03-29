import os
import base64
import time
from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import PlainTextResponse
from openai import OpenAI
import uvicorn

# ======================= CONFIGURATION =======================
# <-- INSERT YOUR OPENAI API KEY HERE
# Alternatively, set it as an environment variable: OPENAI_API_KEY
OPENAI_API_KEY = "sk-..." 
# =============================================================

# Initialize FastAPI app
app = FastAPI(title="AI Smart Glasses Backend")

# Initialize OpenAI Client
# It checks your environment variables first, falling back to the hardcoded string
client = OpenAI(api_key=os.environ.get("OPENAI_API_KEY", OPENAI_API_KEY))

@app.post("/vision", response_class=PlainTextResponse)
async def process_image(request: Request):
    """
    Endpoint mapping for exactly what our ESP32 is sending:
    Raw image/jpeg bytes posted to `/vision`.
    """
    # 1. Verify that the correct Content-Type was sent
    if request.headers.get("content-type") != "image/jpeg":
        raise HTTPException(status_code=400, detail="Expected Content-Type: image/jpeg")
    
    # 2. Extract JPEG binary data from the request body
    image_bytes = await request.body()
    
    if not image_bytes:
        raise HTTPException(status_code=400, detail="Empty request body. No image data received.")

    try:
        # 3. Base64 Encode the raw binary payload to conform with OpenAI API specs
        base64_image = base64.b64encode(image_bytes).decode('utf-8')
        
        # 4. Craft the prompt and make the API Call to GPT-4o Vision
        print("Sending captured frame to OpenAI GPT-4o Vision API...")
        
        response = client.chat.completions.create(
            model="gpt-4o",
            messages=[
                {
                    "role": "user",
                    "content": [
                        {
                            "type": "text",
                            "text": "Describe what you see in one short sentence. Keep it very concise because the text needs to fit on a small 128x64 display. Don't use quotes."
                        },
                        {
                            "type": "image_url",
                            "image_url": {
                                # Use the data URI scheme embedding the Base64 image
                                "url": f"data:image/jpeg;base64,{base64_image}"
                            }
                        }
                    ]
                }
            ],
            max_tokens=50 # Enforce brevity
        )
        
        # 5. Extract the AI's textual description
        description = response.choices[0].message.content.strip()
        print(f"GPT-4o Response: {description}")
        
        # --- NEW FEATURE: LOCAL LOGGING ---
        try:
            os.makedirs("history", exist_ok=True)
            timestamp = int(time.time())
            with open(f"history/capture_{timestamp}.jpg", "wb") as f:
                f.write(image_bytes)
            with open(f"history/capture_{timestamp}.txt", "w") as f:
                f.write(description)
            print(f"Saved capture to history/capture_{timestamp}")
        except Exception as log_e:
            print(f"Error saving history: {log_e}")
        # ----------------------------------
        
        # 6. Return as a plain string so the ESP32 C++ driver can simply dump it to the display
        return description

    except Exception as e:
        print(f"Error handling vision request: {e}")
        raise HTTPException(status_code=500, detail=f"Internal Server Error: {str(e)}")


if __name__ == "__main__":
    # Run the server via uvicorn programmatically.
    # Host '0.0.0.0' exposes the server on your local network (so the ESP32 can reach it)
    print("Starting AI Smart Glasses Vision Server...")
    # Adjust port as necessary
    uvicorn.run("server:app", host="0.0.0.0", port=8000, reload=True)
