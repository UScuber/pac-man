#GUI作成
import tkinter as tk
import threading
import time
import sys
import gamelib as cpp


flame = 17 #ms
flip_freq = 4 #何フレームごとに画像を切り替えるか
flip = 0 #切り替わっているかどうか
canvas = None #canvas
size = 28 #画像1枚の大きさ
objects = ["pacman", "red", "blue", "orange", "pink"]
direc_name = ["up", "left", "down", "right"]
#all of images
images = [[[[None],[None]] for _ in range(4)] for _ in range(len(objects))]

#キーボードからの入力
def input_key(event):
  key_state = event.keysym
  
  if key_state == "Up":
    cpp.rotate(0)
  if key_state == "Left":
    cpp.rotate(1)
  if key_state == "Down":
    cpp.rotate(2)
  if key_state == "Right":
    cpp.rotate(3)


#画像の位置や向きなどの更新
def update_images():
  global canvas
  for i in range(len(objects)):
    x, y, r = cpp.get_xyr(i)
    canvas.itemconfig(objects[i], image= images[i][r][flip])
    canvas.moveto(objects[i],  (x / cpp.sizec + 0.5) * size,  (y / cpp.sizec + 0.5) * size)


#盤面の更新
def update():
  global canvas, flip
  cnt = 0
  while True:
    cpp.update_pos()
    
    if cnt % flip_freq == 0:  flip ^= 1
    
    update_images()

    sys.stdout.flush()
    time.sleep(flame / 1000)
    cnt += 1
  

def read_all_images():
  for i in range(len(objects)):
    for j in range(4):
      for k in range(2):
        img_name = "images/"+objects[i]+"/"+direc_name[j]+str(k) +".png"
        images[i][j][k] = tk.PhotoImage(file= img_name)

#ウィンドウの作成
def main():
  global canvas

  root = tk.Tk()
  #root.geometry("300x300")
  root.title("Pac-Man")
  canvas = tk.Canvas(root, width=550, height=630, bg="black")

  #boardに画像を取り込む
  board = [[None for _ in range(cpp.w)] for _ in range(cpp.h)]
  #盤面の描画
  for i in range(cpp.h):
    for j in range(cpp.w):
      img_name = "images/block/none.png"
      if cpp.get_field(i, j) == 1:
        img_name = "images/block/wall.png"
      board[i][j] = tk.PhotoImage(file= img_name)
      canvas.create_image((j+1)*size, (i+1)*size, image= board[i][j])


  read_all_images()

  #pacman,enemiesを描画
  for i in range(len(objects)):
    x, y, r = cpp.get_xyr(i)
    canvas.create_image((x / cpp.sizec + 0.5) * size, (y / cpp.sizec + 0.5) * size,
                        image= images[i][r][flip], tag= objects[i])



  root.bind("<KeyPress>", input_key)
  canvas.pack()


  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target= update)
  thread1.setDaemon(True)
  thread1.start()

  root.mainloop()



if __name__ == "__main__":
  main()
  sys.exit()