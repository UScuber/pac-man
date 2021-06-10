#GUI作成
import tkinter as tk
import threading
import time
import sys
import gamelib as cpp


FRAME = 18 #ms 処理の更新頻度
IMG_FRAME = 9 #ms 画像の切り替えの頻度
FLIP_FREQ = 4 #何フレームごとに画像を切り替えるか
flip = 0 #切り替わっているかどうか
canvas = None #canvas
SIZE = 17 #フィールド1blockの大きさ
ADJ_X, ADJ_Y = 8, 12 #adjust_x,y
OBJECTS = ["pacman", "red", "blue", "orange", "pink"]
DIREC_NAME = ["up", "left", "down", "right"]
#all of images
#images[i]: normal, eaten, frightened
STATES_NUM = 4
NORMAL, EATEN, FRIGHTENED, SCORE, = range(STATES_NUM)
images = [[[[[None],[None]] for _ in range(len(DIREC_NAME))] for _ in range(len(OBJECTS))] for _ in range(STATES_NUM)]
ispress_key = [False] * 4
KEY_NAME = ["Right", "Down", "Left", "Up"] #逆向きにする

#キーボードからの入力
def press_key(event):
  key_state = event.keysym
  for i in range(len(KEY_NAME)):
    if key_state == KEY_NAME[i]:
      ispress_key[i] = True

def release_key(event):
  key_state = event.keysym
  for i in range(len(KEY_NAME)):
    if key_state == KEY_NAME[i]:
      ispress_key[i] = False



#画像の位置や向きなどの更新
def update_images():
  global canvas
  while True:
    for i in range(len(OBJECTS)):
      x, y, r, s = cpp.get_xyrs(i)
      if cpp.get_isstop(i):
        if s == 1: #eaten スコアの表示
          #i,flipはどの数字でもよい
          if cpp.eat_num() == 0: print("error")
          canvas.itemconfig(OBJECTS[i], image= images[SCORE][i][cpp.eat_num() - 1][flip])
        continue
      canvas.itemconfig(OBJECTS[i], image= images[s][i][r][flip])
      canvas.moveto(OBJECTS[i], x / cpp.sizec * SIZE + ADJ_X, y / cpp.sizec * SIZE + ADJ_Y)
    
    time.sleep(IMG_FRAME / 1000)

#coinの消去
def delete_coin(t):
  if t != -1:
    canvas.delete("coin" + str(t))

#盤面の更新
def update():
  global canvas, flip
  cnt = 0
  start = time.time()
  thread1 = threading.Thread(target= update_images)
  thread1.setDaemon(True)
  thread1.start()

  while True:
    res = cpp.update_pos(time.time() - start)
    delete_coin(res)
    if cnt % FLIP_FREQ == 0:  flip ^= 1

    for i in range(len(ispress_key)):
      if ispress_key[i] == True:
        cpp.rotate(3 - i)

    sys.stdout.flush()
    time.sleep(FRAME / 1000)
    cnt += 1


def read_all_images():
  #coinはmain関数の中で画像を読み込む
  for i in range(len(OBJECTS)): #object
    for j in range(4): #direction
      for k in range(2): #flip
        #normal
        img_name = "images/"+OBJECTS[i]+"/"+DIREC_NAME[j]+str(k) +".png"
        images[NORMAL][i][j][k] = tk.PhotoImage(file= img_name)
        #eaten
        img_name = "images/eaten/"+DIREC_NAME[j]+".png"
        images[EATEN][i][j][k] = tk.PhotoImage(file= img_name)
        #frightened
        #とりあえず白く点滅するやつはなしにする
        img_name = "images/frightened/"+"0"+str(k)+".png"
        images[FRIGHTENED][i][j][k] = tk.PhotoImage(file= img_name)
        #score
        #食べられた時の表示する200,400,800,1600の画像
        img_name = "images/eaten/"+str(1<<(j+1))+"00.png"
        images[SCORE][i][j][k] = tk.PhotoImage(file= img_name)

#ウィンドウの作成
def main():
  global canvas

  root = tk.Tk()
  #root.geometry("300x300")
  root.title("Pac-Man")
  canvas = tk.Canvas(root, width=500, height=560, bg="black")

  #boardに画像を取り込む
  img_name = "images/board.png"
  board = tk.PhotoImage(file= img_name)
  canvas.create_image(250, 280, image= board)


  read_all_images()

  #coinを描画
  coins = [] #一時保存用
  for i in range(cpp.h):
    for j in range(cpp.w):
      t = cpp.get_field(i, j)
      file_name = "images/items/"

      """ C++の定数coin,COINの値に注意 """
      if t == 7: #coin
        file_name += "small.png"
      elif t == 8: #COIN
        file_name += "big.png"
      else: continue
      coins.append(tk.PhotoImage(file= file_name))
      tag = "coin" + str(i*cpp.w + j)
      canvas.create_image((j+1)*SIZE + 5, (i+1)*SIZE + 9, image= coins[-1], tag= tag)

  #pacman,enemiesを描画
  for i in [0,4,3,2,1]: #画像の奥行を設定
    x, y, r, s = cpp.get_xyrs(i)
    canvas.create_image(0,0, image= images[s][i][r][flip], tag= OBJECTS[i])


  root.bind("<KeyPress>", press_key)
  root.bind("<KeyRelease>", release_key)
  canvas.pack()


  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target= update)
  thread1.setDaemon(True)
  thread1.start()

  root.mainloop()



if __name__ == "__main__":
  main()
  sys.exit()