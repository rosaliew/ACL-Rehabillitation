import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

global collecting_data
collecting_data= False
saying = " "
euler_data = []
force_data = []
rom = 0;
rom_value_text = " "
rom_phrase = " "
x_data = []
y_data = []
exercise_one = False

# # -------------- to indicate which graph to show ---------------
# def exercise_one_start():
#     exercise_one = True
#
# def exercise_two_start():
#     exercise_one = False
#
# def exercise_page():
#     if exercise_one:
#         exercise_one_pg
#     elif(exercise_one != True):
#         exercise_two_pg

# # ----- global variable root
# root = tk.Tk()
# root.geometry('600x600')  # set the windows size
# root.title('Live Graphing')
# # root.configure(background = 'light blue')
# root['bg'] = '#ffbf00'
#
# # ----- create a figure plot object on GUI --------
# fig = Figure();
# ax = fig.add_subplot(111)
#
# ax.set_title('Live Range of Motion Euler Angles')
# ax.set_xlabel('Data Points')
# ax.set_ylabel('Degrees')
# ax.set_ylim(-180, 360)
# lines = ax.plot([],[])[0]  # this gives an array that is varying moved as global variable
#
# canvas = FigureCanvasTkAgg(fig, master=root)    # A tk.DrawingArea
# canvas.get_tk_widget().place(x = 100, y = 110, width = 400, height = 400)
# canvas.draw()

# Used once the start button has been pressed, it restarts global variables
def data_on():
    global collecting_data
    global euler_data
    global force_data
    global rom_value_text
    global rom_phrase

    # resetting variables back to default for new collection
    collecting_data = True
    euler_data = []
    force_data = []
    rom_value_text = " "
    rom_phrase = " "

def data_off():
    global collecting_data
    collecting_data = False
    print("Stopped data")

def data_status():
    global collecting_data
    print("data status",collecting_data)
