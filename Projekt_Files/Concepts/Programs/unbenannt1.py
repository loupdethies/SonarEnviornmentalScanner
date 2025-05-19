# -*- coding: utf-8 -*-
"""
Created on Thu Dec  5 10:55:22 2024

@author: X1PEWOLF
"""


import matplotlib.pyplot as plt
import matplotlib.patches as patches

# Create a figure and axis
fig, ax = plt.subplots(figsize=(12, 8))

# Define block drawing function
def draw_block(x, y, width, height, text, color="lightblue"):
    rect = patches.Rectangle((x, y), width, height, linewidth=1, edgecolor='black', facecolor=color)
    ax.add_patch(rect)
    ax.text(x + width/2, y + height/2, text, ha='center', va='center', fontsize=10, wrap=True)

# USB-C input block
draw_block(0, 8, 2, 1, "USB-C Input\n5V Supply", "lightgreen")

# ESD and polyfuse
draw_block(2.5, 8, 2, 1, "ESD Diode +\nPolyfuse", "pink")

# Buck Converter for Digital
draw_block(0, 6, 2.5, 1, "Buck Converter\n5V -> 3.3V\n(Digital Circuits)", "lightblue")

# Buck Converter for Analog
draw_block(3, 6, 2.5, 1, "Buck Converter\n5V -> 3.3V\n(Analog Circuits)", "lightblue")

# Boost Converter for 24V
draw_block(6, 8, 2.5, 1, "Boost Converter\n5V -> 24V\n(Op-Amp Power)", "lightyellow")

# Emitter Circuit
draw_block(9, 8, 3, 1, "Emitter Circuit\n(Op-Amp + Transducer)", "orange")

# Microphone Circuits
draw_block(9, 6, 3, 1, "Microphone Circuits\n(Amplifiers + Filters)", "orange")

# Microcontroller Block
draw_block(6, 4, 6, 2, "STM32F4 Microcontroller\n- Signal Processing\n- JTAG/SWD\n- SPI/I2C Expansion", "lightgrey")

# LEDs and Indicators
draw_block(3, 4, 2.5, 1, "LED Indicators\n(Power, Status, Error)", "violet")

# Peripheral Connectors
draw_block(0, 4, 2.5, 1, "Peripheral Connectors\n(SPI/I2C/UART)", "lightpink")

# Test Points
draw_block(0, 2, 2.5, 1, "Test Points\n(Voltage + Signal Lines)", "lightcoral")

# External Sensor Connection
draw_block(3, 2, 2.5, 1, "Expansion Headers\n(Sensors/Actuators)", "lightpink")

# Label axes (not shown in final)
ax.axis('off')
plt.title("Block Diagram of Sonar Environmental Scanner System", fontsize=14, weight='bold')
plt.show()
