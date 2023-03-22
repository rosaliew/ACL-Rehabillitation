from tkinter import *

ws = Tk()
ws.geometry('600x600')
ws.title('PythonGuides')
ws['bg'] = '#5d8a82'

f = ("Times bold", 14)


def nextPage():
    ws.destroy()
    import SLST_pg


def prevPage():
    ws.destroy()
    import SEBT_pg


Label(
    ws,
    text="ACL Rehab Home",
    padx=20,
    pady=20,
    bg='#5d8a82',
    font=f
).pack(expand=True, fill=BOTH)

Button(
    ws,
    text="Single Leg Stance Test",
    font=f,
    command=nextPage
).pack(fill=X, expand=TRUE, side=LEFT)

Button(
    ws,
    text="Star Excursion Balance Test",
    font=f,
    command=prevPage
).pack(fill=X, expand=TRUE, side=LEFT)

ws.mainloop()