# -*- coding: utf-8 -*-
"""
Created on Wed Nov 27 21:50:37 2024

@author: X1PEWOLF
"""
# Re-importing matplotlib to ensure smooth execution
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Define a function to draw a block diagram with categories
def draw_block_diagram():
    # Create a figure and axis
    fig, ax = plt.subplots(figsize=(12, 8))
    
    # Define blocks with positions, sizes, labels, and colors
    blocks = [
        {"label": "USB-C Power Input (5V)", "pos": (0, 4), "size": (2, 1), "color": "lightcoral", "type": "Power"},
        {"label": "Buck Converter (5V to 3.3V)", "pos": (2.5, 4), "size": (3, 1), "color": "lightcoral", "type": "Power"},
        {"label": "Microcontroller (STM32/ESP32)", "pos": (6, 4), "size": (3, 1), "color": "lightgreen", "type": "Signal"},
        {"label": "DAC (20-150 kHz output)", "pos": (9.5, 5), "size": (3, 1), "color": "lightskyblue", "type": "Signal"},
        {"label": "Transducer (40 kHz)", "pos": (13, 5.5), "size": (3, 1), "color": "lightgoldenrodyellow", "type": "Output"},
        {"label": "Microphones (20-150 kHz)", "pos": (13, 3.5), "size": (3, 1), "color": "lightgoldenrodyellow", "type": "Input"},
        {"label": "Signal Processing (ADC + Filtering)", "pos": (9.5, 3), "size": (3, 1), "color": "lightskyblue", "type": "Signal"},
        {"label": "USB Data Output", "pos": (15.5, 4.5), "size": (3, 1), "color": "lightgreen", "type": "Output"},
        {"label": "Bluetooth/Wi-Fi Module (optional)", "pos": (15.5, 3), "size": (3, 1), "color": "lightgreen", "type": "Output"}
    ]
    
    # Draw the blocks
    for block in blocks:
        rect = patches.FancyBboxPatch(
            (block["pos"][0], block["pos"][1]), block["size"][0], block["size"][1],
            boxstyle="round,pad=0.1", linewidth=1, edgecolor="black", facecolor=block["color"]
        )
        ax.add_patch(rect)
        ax.text(
            block["pos"][0] + block["size"][0] / 2, block["pos"][1] + block["size"][1] / 2,
            block["label"], fontsize=10, ha="center", va="center", wrap=True
        )
    
    # Define signal flow arrows
    arrows = [
        ((2, 4.5), (2.5, 4.5)),  # USB-C to Buck Converter
        ((5.5, 4.5), (6, 4.5)),  # Buck Converter to Microcontroller
        ((8.5, 5.5), (9.5, 5.5)),  # Microcontroller to DAC
        ((12.5, 6), (13, 6)),  # DAC to Transducer
        ((12.5, 4), (13, 4)),  # Microphones to Signal Processing
        ((8.5, 4.5), (9.5, 4.5)),  # Microcontroller to Signal Processing
        ((15, 5), (15.5, 5)),  # Microcontroller to USB Data Output
        ((15, 3.5), (15.5, 3.5))  # Microcontroller to Bluetooth/Wi-Fi Module
    ]
    
    # Draw the arrows
    for arrow in arrows:
        ax.annotate(
            "", xy=arrow[1], xytext=arrow[0],
            arrowprops=dict(arrowstyle="->", lw=1.5, color="black")
        )
    
    # Set axis limits and hide axes
    ax.set_xlim(-1, 19)
    ax.set_ylim(2, 7)
    ax.axis("off")
    
    # Show the diagram
    plt.title("Sonar Environmental Scanner Block Diagram", fontsize=16)
    plt.show()

# Call the function to draw the block diagram
draw_block_diagram()
