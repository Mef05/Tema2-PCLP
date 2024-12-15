import numpy as np
import argparse
import sys

import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

import tkinter as tk
from tkinter import ttk

def proccess_mat3d(mat3d):
    # transpose each plane and then reverse the order, then np.array
    return np.array([[[row[j] for row in plane] for j in range(len(plane[0]))] for plane in mat3d][::-1])

class Interactive3DFigure:
    def __init__(self, planes, rows, columns, mat3d, color='#ff00ff99'):
        self.planes = planes
        self.rows = rows
        self.columns = columns
        
        self.mat3d = proccess_mat3d(mat3d)
        self.filled = np.where(self.mat3d != 0, True, False)
        self.setTransparent()
        
        self.fig = plt.figure(figsize=(8, 6), dpi=120)
        self.ax = self.fig.add_subplot(projection='3d')
        self.ax.view_init(elev=20, azim=-30)
        
        self.fig.subplots_adjust(left=0.08, right=0.92, top=0.92, bottom=0.08, wspace=0, hspace=0)
        
        self._plot_voxels()
    
    def _plot_voxels(self):
        self.ax.clear()

        self.ax.voxels(self.filled, facecolors=self.facecolors, edgecolors=self.edgecolor)

        self.ax.set_xlim(0, self.planes)
        self.ax.set_ylim(0, self.columns)
        self.ax.set_zlim(0, self.rows)
        
        self.ax.set_xlabel('x', labelpad=-5, fontsize=14)
        self.ax.set_ylabel('z', labelpad=-5, fontsize=14)
        self.ax.set_zlabel('y', labelpad=-5, fontsize=14)

        self.ax.set_xticks(range(self.planes))
        self.ax.set_yticks(range(self.columns))
        self.ax.set_zticks(range(self.rows))

        self.ax.set_box_aspect([self.planes, self.columns, self.rows])
        self.ax.invert_xaxis()
        self.ax.invert_zaxis()
        self.ax.set_aspect('auto')

        self.ax.xaxis._axinfo['grid'].update(color='#00000020')
        self.ax.yaxis._axinfo['grid'].update(color='#00000020')
        self.ax.zaxis._axinfo['grid'].update(color='#00000020')
        
        self.ax.tick_params(labelleft=False, labelbottom=False, bottom=False)
    
    def _set_facecolors(self, colors, other='#ffffff'):
        self.facecolors = np.empty(self.mat3d.shape, dtype=object)
        for i, color in enumerate(colors):
            self.facecolors[self.mat3d == i] = color
        outside_range = np.logical_or(self.mat3d < 0, self.mat3d >= len(colors))
        self.facecolors[outside_range] = other
    
    def _set_edgecolor(self, color):
        self.edgecolor = color

    def update_plot(self):
        self._plot_voxels()
        plt.draw()
    
    def setTransparent(self):
        self._set_facecolors(['#00000000', '#64e635a0', '#c29342a0', '#87847fa0'], other='#ff1cc2a0')
        self._set_edgecolor('#00000018')
    
    def setSolid(self):
        self._set_facecolors(['#00000000', '#64e635', '#c29342', '#87847f'], other='#ff1cc2')
        self._set_edgecolor('#00000040')

    def get_figure(self):
        return self.fig

def read_file(file):
    planes = int(file.readline())
    rows = int(file.readline())
    columns = int(file.readline())

    mat3d = []

    for _ in range(planes):
        plane = []

        for _ in range(rows):
            row = []
            values = file.readline().replace('\n', ' ').split(' ')
            
            for k in range(columns):
                row.append(int(values[k]))
            plane.append(row)

        file.readline()
        mat3d.append(plane)

    return planes, rows, columns, mat3d

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='View 3D plot of a chunk')

    parser.add_argument('filename', nargs='?', help='file containing chunk data')
    parser.add_argument('-o', '--output', help='save plot to image')

    args = parser.parse_args()

    input_file = open(args.filename, 'r') if args.filename else sys.stdin

    # Create the 3D plot
    fig = Interactive3DFigure(*read_file(input_file), '#ff005aba')

    if args.output:
        fig.get_figure().savefig(args.output)
        sys.exit()

    # Tkinter application window
    root = tk.Tk()
    root.title("Vizualizare chunk")

    ttk_style = ttk.Style()
    ttk_style.theme_use('clam')

    # Create a dropdown
    dropdown = ttk.Combobox(root, values=["Transparent", "Solid"], state="readonly")
    dropdown.set("Transparent")  # Set default option
    dropdown.pack(pady=10)

    # Create a canvas to display the Matplotlib figure
    canvas = FigureCanvasTkAgg(fig.get_figure(), master=root)
    canvas.draw()

    # Add the canvas to the Tkinter window
    canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    def update_plot(event):
        selected_option = dropdown.get()

        if selected_option == 'Transparent':
            fig.setTransparent()
        elif selected_option == 'Solid':
            fig.setSolid()
        else:
            pass

        fig.update_plot()

    # Bind the dropdown selection change event to update the plot
    dropdown.bind("<<ComboboxSelected>>", update_plot)

    root.protocol("WM_DELETE_WINDOW", root.quit)

    # Tkinter event loop
    root.mainloop()

