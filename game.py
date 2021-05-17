#GUI作成
import tkinter as tk
import threading
import time
import sys
import gamelib as cpp

"""メモ
canvas.move(tag_name, x, y)
-> tag_nameをx軸にx,y軸にy平行移動する
"""

flame = 18 #ms
flip_freq = 6 #何フレームごとに画像を切り替えるか
canvas = None #canvas
size = 28
objects = []

#キーボードからの入力
def input_key(event):
  key_state = event.keysym
  print(key_state)
  if key_state == "Right":
    pass
  if key_state == "Left":
    pass
  if key_state == "Up":
    pass
  if key_state == "Down":
    pass



#盤面の更新
def update():
  global canvas
  flip = False
  cnt = 0
  while True:
    cpp.update_pos()
    #ここに処理を書く
    for i in range(5):
      x, y = cpp.get_pos(i*2), cpp.get_pos(i*2 + 1)
      canvas.moveto(objects[i],  (x / cpp.sizec + 0.5) * size,  (y / cpp.sizec + 0.5) * size)

    
    if cnt % flip_freq == 0:
      flip ^= True
      if flip:
        img = tk.PhotoImage(file= "images/pacman/eatright.png")
        canvas.itemconfig("pacman", image= img)
      else:
        img = tk.PhotoImage(file= "images/pacman/cirright.png")
        canvas.itemconfig("pacman", image= img)
    


    sys.stdout.flush()
    time.sleep(flame / 1000)
    cnt += 1
  


#ウィンドウの作成
def main():
  global canvas

  root = tk.Tk()
  #root.geometry("300x300")
  root.title("Pac-Man")
  canvas = tk.Canvas(root, width=550, height=630, bg="black")

  #boardに画像を取り込む
  board = [[None for _ in range(cpp.w)] for _ in range(cpp.h)]
  for i in range(cpp.h):
    for j in range(cpp.w):
      img_name = "images/block/none.png"
      if cpp.get_field(i, j) == 1:
        img_name = "images/block/wall.png"
      board[i][j] = tk.PhotoImage(file= img_name)

  for i in range(cpp.h):
    for j in range(cpp.w):
      canvas.create_image((j+1)*size, (i+1)*size, image= board[i][j])
  

  img = tk.PhotoImage(file= "images/pacman/cirright.png")
  canvas.create_image((cpp.get_pos(0) / cpp.sizec + 0.5) * size,  (cpp.get_pos(1) / cpp.sizec + 0.5) * size, image=img, tag="pacman")
  objects.append("pacman")
  img2 = tk.PhotoImage(file= "images/red/up0.png")
  canvas.create_image((cpp.get_pos(2) / cpp.sizec + 0.5) * size,  (cpp.get_pos(3) / cpp.sizec + 0.5) * size, image=img2, tag="red")
  objects.append("red")
  img3 = tk.PhotoImage(file= "images/blue/up0.png")
  canvas.create_image((cpp.get_pos(4) / cpp.sizec + 0.5) * size,  (cpp.get_pos(5) / cpp.sizec + 0.5) * size, image=img3, tag="blue")
  objects.append("blue")
  img4 = tk.PhotoImage(file= "images/orange/up0.png")
  canvas.create_image((cpp.get_pos(6) / cpp.sizec + 0.5) * size,  (cpp.get_pos(7) / cpp.sizec + 0.5) * size, image=img4, tag="orange")
  objects.append("orange")
  img5 = tk.PhotoImage(file= "images/pink/up0.png")
  canvas.create_image((cpp.get_pos(8) / cpp.sizec + 0.5) * size,  (cpp.get_pos(9) / cpp.sizec + 0.5) * size, image=img5, tag="pink")
  objects.append("pink")




  root.bind("<KeyPress>", input_key)
  canvas.pack()


  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target = update)
  thread1.setDaemon(True)
  thread1.start()

  root.mainloop()



if __name__ == "__main__":
  main()
  sys.exit()