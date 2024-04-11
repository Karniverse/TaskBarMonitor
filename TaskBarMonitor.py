import tkinter as tk
from tkinter import messagebox
import psutil
from PIL import Image, ImageDraw
import time
import io

# Create a hidden root window
root = tk.Tk()
root.withdraw()

# Create a blank image to be used as the taskbar icon
width, height = 16, 16
image = Image.new("RGB", (width, height), "black")
draw = ImageDraw.Draw(image)

# Function to update the taskbar icon with system usage graph
def update_taskbar_icon():
    try:
        # Get CPU and memory usage
        cpu_percent = psutil.cpu_percent()
        memory_percent = psutil.virtual_memory().percent
        
        # Clear previous graph
        draw.rectangle([0, 0, width, height], fill="black")
        
        # Draw CPU usage graph
        cpu_bar_height = int(height * cpu_percent / 100)
        draw.rectangle([0, height - cpu_bar_height, width // 2, height], fill="green")
        
        # Draw memory usage graph
        memory_bar_height = int(height * memory_percent / 100)
        draw.rectangle([width // 2, height - memory_bar_height, width, height], fill="blue")
        
        # Convert image to bytes
        img_bytes = io.BytesIO()
        image.save(img_bytes, format="PNG")
        img_bytes.seek(0)
        
        # Update taskbar icon
        root.iconphoto(False, tk.PhotoImage(data=img_bytes.read()))
    except Exception as e:
        messagebox.showerror("Error", f"An error occurred: {str(e)}")
    
    # Schedule next update
    root.after(1000, update_taskbar_icon)

# Start updating the taskbar icon
update_taskbar_icon()

# Run the main loop
root.mainloop()
