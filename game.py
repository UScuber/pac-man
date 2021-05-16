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
  canvas = tk.Canvas(bg="black")

  img = tk.PhotoImage(file= "images/pacman/cirright.png")
  canvas.create_image(16,16, image=img, tag="circle")

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