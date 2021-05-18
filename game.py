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
flip_freq = 4 #何フレームごとに画像を切り替えるか
flip = 0 #切り替わっているかどうか
canvas = None #canvas
size = 28
objects = ["pacman", "red", "blue", "orange", "pink"]
direc_name = ["up", "left", "down", "right"]
images = [None] * len(objects)

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

#画像のパスを返す
def get_image_name(i):
  r = cpp.get_rot(i)
  return "images/"+objects[i]+"/"+direc_name[r]+str(flip) +".png"

#画像の位置の更新
def update_poses():
  global canvas
  for i in range(len(objects)):
    x, y = cpp.get_posx(i), cpp.get_posy(i)
    canvas.moveto(objects[i],  (x / cpp.sizec + 0.5) * size,  (y / cpp.sizec + 0.5) * size)

#画像の変更(flipやrotate)
def update_images():
  global canvas, images
  images = [None] * len(objects)
  for i in range(len(objects)):
    img_name = get_image_name(i)
    images[i] = tk.PhotoImage(file= img_name)
    canvas.itemconfig(objects[i], image= images[i])

#回転の更新
def update_rotate(i):
  global canvas
  img_name = get_image_name(i)
  images[i] = tk.PhotoImage(file= img_name)
  canvas.itemconfig(objects[i], image= images[i])

#盤面の更新
def update():
  global canvas, flip
  cnt = 0
  while True:
    cpp.update_pos()
    #ここに処理を書く
    update_poses()

    
    if cnt % flip_freq == 0:
      flip ^= 1
      #画像の変更
      update_images()
    


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
  #盤面の描画
  for i in range(cpp.h):
    for j in range(cpp.w):
      img_name = "images/block/none.png"
      if cpp.get_field(i, j) == 1:
        img_name = "images/block/wall.png"
      board[i][j] = tk.PhotoImage(file= img_name)
      canvas.create_image((j+1)*size, (i+1)*size, image= board[i][j])

  
  #pacman,enemiesを描画
  for i in range(len(objects)):
    img_name = get_image_name(i)
    x, y = cpp.get_posx(i), cpp.get_posy(i)
    canvas.create_image((x / cpp.sizec + 0.5) * size,  (y / cpp.sizec + 0.5) * size,  image= images[i], tag= objects[i])




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