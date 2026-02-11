import numpy as np
import matplotlib.pyplot as plt
import os

def simulate_4x3_zonal(height=12, z1_weight=0.75, z2_weight=0.75, z3_weight=0.75):
    # PCB 20x15
    res = 300
    x = np.linspace(-10, 10, res)
    y = np.linspace(-7.5, 7.5, res)
    X, Y = np.meshgrid(x, y)
    
    # 4 columns (X) and 3 rows (Y)
    x_edge = 7.5
    x_mid = 2.5
    y_edge = 5
    y_mid = 0    

    x_coords = [-x_edge, -x_mid, x_mid, x_edge]
    y_coords = [-y_edge, y_mid, y_edge]
    
    intensity = np.zeros_like(X)
    
    for lx in x_coords:
        
        for ly in y_coords:

            weight = -1
            zone = -1

            #zone 0
            if abs(lx) == x_edge and abs(ly) == y_edge:
                weight = 1.0
                zone = 0
            
            #zone 1
            if abs(lx) == x_edge and abs(ly) == y_mid:
                weight = z1_weight
                zone = 1

            #zone 2
            if abs(lx) == x_mid and abs(ly) == y_edge:
                weight = z2_weight
                zone = 2

            #zone 3
            if abs(lx) == x_mid and abs(ly) == y_mid:
                weight = z3_weight
                zone = 3
           

            print(f"zone: {zone} weight: {weight} --> lx: {lx}, ly: {ly}")

            dist_sq = (X - lx)**2 + (Y - ly)**2 + height**2
            cos_theta = height / np.sqrt(dist_sq)
            current = weight * (cos_theta**4) / dist_sq
            current[cos_theta < 0.5] = 0 # 120-deg cutoff
            intensity += current

    plt.figure(figsize=(10, 8))
    plt.contourf(X, Y, intensity / np.max(intensity), levels=20, cmap='plasma')
    plt.colorbar(label='Relative Intensity')
    plt.title(f'4x3 Zonal Dimming z1w: {z1_weight*100}% z2w:{z2_weight*100:2.0f}% z3w:{z3_weight*100}% power)')
    output_filename = f"Matilda_4x3_Zonal zw={z1_weight}_{z2_weight}_{z3_weight} x_c={x_coords} y_c={y_coords}.png" 
    plt.savefig(output_filename)
    print(f"Simulation saved: {output_filename}")

if __name__ == "__main__":
    simulate_4x3_zonal(height=12, z1_weight=0.225, z2_weight=0.225, z3_weight=0.225)

# Optional: Display the window
plt.show()

#sample console output
# zone: 0 weight: 1.0 --> lx: -7.5, ly: -5
# zone: 1 weight: 0.225 --> lx: -7.5, ly: 0
# zone: 0 weight: 1.0 --> lx: -7.5, ly: 5
# zone: 2 weight: 0.25 --> lx: -2.5, ly: -5
# zone: 3 weight: 0.2 --> lx: -2.5, ly: 0
# zone: 2 weight: 0.25 --> lx: -2.5, ly: 5
# zone: 2 weight: 0.25 --> lx: 2.5, ly: -5
# zone: 3 weight: 0.2 --> lx: 2.5, ly: 0
# zone: 2 weight: 0.25 --> lx: 2.5, ly: 5
# zone: 0 weight: 1.0 --> lx: 7.5, ly: -5
# zone: 1 weight: 0.225 --> lx: 7.5, ly: 0
# zone: 0 weight: 1.0 --> lx: 7.5, ly: 5