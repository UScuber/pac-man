#GUI作成
import tkinter as tk
import gamelib as cpp


"""メモ
canvas.move(tag_name, x, y)
-> tag_nameをx軸にx,y軸にy平行移動する
"""

canvas = 0 #canvas
#キーボードからの入力
def input_key(event):
  key_state = event.keysym
  print(key_state)
  if key_state == "Right":
    pass
  if key_state == "Left":
    pass
  if key_state == "UP":
    pass
  if key_state == "DOWN":
    pass



#ウィンドウの作成
def main():
  global canvas

  root = tk.Tk()
  #root.geometry("300x300")
  root.title("Pac-Man")
  canvas = tk.Canvas(bg="white")

  #img = tk.PhotoImage(file= "images/circle.png")
  #canvas.create_image(16,16, image=img, tag="circle")

  root.bind("<KeyPress>", input_key)
  canvas.pack()

  root.mainloop()


if __name__ == "__main__":
  main()