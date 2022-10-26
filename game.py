#GUI作成
import tkinter as tk
import threading
import time
import sys
import contextlib
with contextlib.redirect_stdout(None):
  import pygame
import gamelib as cpp


FRAME = 70 #処理の更新頻度[Hz]
IMG_FRAME = 100 #画像の切り替えの頻度[Hz]
FLIP_FREQ = 4 #何フレームごとに画像を切り替えるか
flip = 0 #切り替わっているかどうか
canvas = None #canvas
SIZE = 17 #フィールド1blockの大きさ
ADJ_X, ADJ_Y = 9, 59 #adjust_x,y
OBJECTS = ["pacman", "red", "blue", "pink", "orange"]
DIREC_NAME = ["up", "left", "down", "right"]
#all of images
#images[i]: normal, eaten, frightened
STATES_NUM = 5
NORMAL, EATEN, FRIGHTENED, SCORE, FLASH = range(STATES_NUM)
images = [[[[[None],[None]] for _ in range(len(DIREC_NAME))] for _ in range(len(OBJECTS))] for _ in range(STATES_NUM)]
ispress_key = [False] * 4
last_pressed_key = len(ispress_key)
KEY_NAME = ["Up", "Left", "Down", "Right"]
game_score = 0

#キーボードからの入力
def press_key(event):
  global last_pressed_key
  key_state = event.keysym
  if key_state == "Escape":
    print("pause")
  
  for i in range(len(KEY_NAME)):
    if key_state == KEY_NAME[i]:
      if i & 1:
        cpp.start_move() #動作の開始
      ispress_key[i] = True
      last_pressed_key = i

#画像の位置や向きなどの更新
def update_images():
  global canvas
  clock = pygame.time.Clock()
  while True:
    clock.tick(FRAME)
    for i in range(len(OBJECTS)):
      x, y, r, s = cpp.get_xyrs(i)
      if cpp.get_isstop(i):
        if s == 1: #eaten スコアの表示
          x = (x + cpp.sizec/2) // cpp.sizec
          y = (y + cpp.sizec/2) // cpp.sizec
          pcx, pcy = cpp.get_xyrs(0)[:2]
          pcx = (pcx + cpp.sizec/2) // cpp.sizec
          pcy = (pcy + cpp.sizec/2) // cpp.sizec
          if x == pcx and y == pcy:
            #i,flipはどの数字でもよい
            if cpp.eat_num() == 0: print("eat_num_error")
            canvas.itemconfig(OBJECTS[i], image= images[SCORE][i][cpp.eat_num() - 1][flip])
        continue
      t = int(cpp.limit_time(i) * 4)
      if t <= 8 and not(t & 1): #flash
        canvas.itemconfig(OBJECTS[i], image= images[FLASH][i][r][flip])
      else:
        canvas.itemconfig(OBJECTS[i], image= images[s][i][r][flip])
      canvas.moveto(OBJECTS[i], x / cpp.sizec * SIZE + ADJ_X, y / cpp.sizec * SIZE + ADJ_Y)


#coinの消去
def delete_coin(t):
  if t != -1:
    canvas.delete("coin" + str(t))

#盤面の更新
def update():
  global canvas, flip, game_score
  cnt = 0
  start = time.time()
  thread1 = threading.Thread(target= update_images)
  thread1.setDaemon(True)
  thread1.start()
  clock = pygame.time.Clock()
  while True:
    clock.tick(FRAME)
    res = cpp.update_pos(time.time() - start, last_pressed_key)
    if res != -1:
      game_score += 10
      write_score()
    delete_coin(res)
    if cnt % FLIP_FREQ == 0:  flip ^= 1

    sys.stdout.flush()
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
        #flash
        #残り数秒になった時に白く点滅する画像
        img_name = "images/frightened/"+"1"+str(k)+".png"
        images[FLASH][i][j][k] = tk.PhotoImage(file= img_name)

def draw_all_coins(coins):
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
      canvas.create_image((j+1)*SIZE + 7, (i+1)*SIZE +56, image= coins[-1], tag= tag)

def write_score():
  global lbl_score
  lbl_score["text"] = str(game_score).zfill(7)

#ウィンドウの作成
def main():
  global canvas, lbl_score, lbl_up

  root = tk.Tk()
  #root.geometry("300x300")
  root.title("Pac-Man")
  canvas = tk.Canvas(root, width=500, height=630, bg="black")

  cpp.reset()

  #boardに画像を取り込む
  img_name = "images/stage.png"
  board = tk.PhotoImage(file= img_name)
  canvas.create_image(250, 318, image= board)
  lbl_score = tk.Label(text="0000000", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_score.place(x=470, y=28, anchor=tk.NE)
  lbl_up = tk.Label(text="00", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_up.place(x=70, y=28, anchor=tk.NW)
  #lbl_score.after(100, write_score)

  read_all_images()

  #coinを描画
  coins = [] #一時保存用
  draw_all_coins(coins)

  #pacman,enemiesを描画
  for i in [0,4,3,2,1]: #画像の奥行を設定
    x, y, r, s = cpp.get_xyrs(i)
    canvas.create_image(x / cpp.sizec * SIZE + 22, y / cpp.sizec * SIZE + 72,
                        image= images[s][i][r][flip], tag= OBJECTS[i])


  root.bind("<KeyPress>", press_key)
  canvas.pack()


  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target= update)
  thread1.setDaemon(True)
  thread1.start()

  root.mainloop()



if __name__ == "__main__":
  main()
  sys.exit()