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
isfinished = False
canvas = None #canvas
size = 28
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
    #draw_field()
    pass



#盤面の更新
def update():
  global canvas
  flip = False

  while not isfinished:
    #ここに処理を書く
    canvas.move("circle", 1, 0)
    


    sys.stdout.flush()
    time.sleep(flame / 1000)
    flip ^= True


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
  

  #img = tk.PhotoImage(file= "images/pacman/cirright.png")
  #canvas.create_image(size, size, image=img, tag="circle")




  root.bind("<KeyPress>", input_key)
  canvas.pack()


  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target = update)
  thread1.start()

  root.mainloop()

  global isfinished
  isfinished = True
  thread1.join()



if __name__ == "__main__":
  main()
  sys.exit()