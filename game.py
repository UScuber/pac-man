#GUI作成
import tkinter as tk
import threading
import time
import sys
import contextlib
with contextlib.redirect_stdout(None):
  import pygame
import gamelib as cpp
from PIL import Image, ImageTk

FRAME = 70 #処理の更新頻度[Hz]
IMG_FRAME = 100 #画像の切り替えの頻度[Hz]
FLIP_FREQ = 10 #何フレームごとに画像を切り替えるか
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
msg_status = 0
is_end = 0
game_time = 0
is_first = 1

#キーボードからの入力
def press_key(event):
  global last_pressed_key, is_first
  key_state = event.keysym
  if key_state == "Escape":
    print("pause")
  
  for i in range(len(KEY_NAME)):
    if key_state == KEY_NAME[i]:
      if i & 1:
        cpp.start_move() #動作の開始
        
        if is_first:
          is_first = 0
          lbl_time.after(1000, count_up)
          lbl_start.destroy()

      ispress_key[i] = True
      last_pressed_key = i

#画像の位置や向きなどの更新
def update_images():
  global canvas
  clock = pygame.time.Clock()
  while (is_end == 0):
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
  global canvas, flip, thread1, thread2, is_end
  cnt = 0
  start = time.time()
  thread2 = threading.Thread(target= update_images)
  thread2.setDaemon(True)
  thread2.start()
  clock = pygame.time.Clock()
  while (is_end == 0):
    clock.tick(FRAME)
    res = cpp.update_pos(time.time() - start, last_pressed_key)
    delete_coin(res)
    if cnt % FLIP_FREQ == 0:  flip ^= 1

    sys.stdout.flush()
    cnt += 1
    game_score = cpp.get_score()
      
    lbl_score["text"] = str(game_score).zfill(7)
    if cpp.is_game_over():
      #time.sleep(1)
      print("####"+str(cpp.remain_num()))
      if cpp.remain_num() <= 0:
        is_end = 1
        thread3 = threading.Thread(target=failed_result)
        thread3.start()
        sys.exit()
      for i in range(cpp.remain_num()):
        lbl_life[i].place_forget()
      for i in range(cpp.remain_num() - 1):
        lbl_life[i].place(x=10+35*i, y=630, anchor=tk.SW)
      cpp.restart()
    elif cpp.is_game_cleared():
      is_end = 1
      thread3 = threading.Thread(target=cleared_result)
      thread3.start()
      sys.exit()

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

def startgame(event):
  global menu
  key_state = event.keysym
  if key_state == "Return":
    menu.destroy()
    main()

def menu():
  global root, menu
  root = tk.Tk()
  root.title("Pac-Man")
  root.resizable(False, False)
  menu = tk.Canvas(root, width=500, height=630, bg="black")
  pic_pac = ImageTk.PhotoImage(Image.open("images/pacman.png").resize((175, 181)))
  lbl_title = tk.Label(menu, text="PAC++ PERTHON", font=("4x4極小かなフォント", 30), fg="white", bg="black")
  lbl_title.place(x=250, y=80, anchor=tk.CENTER)
  lbl_manual = tk.Label(menu, text="これはポリ塩化アルミニウム(PAC)を\n食べる人を操るゲームです", font=("Arial", 15), fg="white", bg="black")
  lbl_manual.place(x=250, y=160, anchor=tk.CENTER)
  lbl_start1 = tk.Label(menu, text="PRESS ENTER", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_start1.place(x=250, y=525, anchor=tk.CENTER)
  lbl_start2 = tk.Label(menu, text="TO START THE GAME", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_start2.place(x=250, y=565, anchor=tk.CENTER)
  lbl_pac = tk.Label(menu, text="", image=pic_pac)
  lbl_pac.place(x=250, y=315, anchor=tk.CENTER)
  lbl_start1.bind('<Key>', startgame)
  lbl_start1.focus_set()
  menu.pack()
  root.mainloop()

def count_up():
  global game_time, lbl_time
  game_time += 1
  lbl_time["text"] = str(game_time).zfill(4)
  if is_end == 0:
    lbl_time.after(1000, count_up)


#ウィンドウの作成
def main():
  global canvas, lbl_score, lbl_time, lbl_start, lbl_life, thread1
  canvas = tk.Canvas(root, width=500, height=630, bg="black")

  cpp.reset()

  #boardに画像を取り込む
  img_name = "images/stage.png"
  board = tk.PhotoImage(file= img_name)
  canvas.create_image(250, 318, image= board)
  lbl_start = tk.Label(text="READY!", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_start.place(x=252, y=362, anchor=tk.CENTER)
  lbl_score = tk.Label(text="0000000", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_score.place(x=470, y=28, anchor=tk.NE)
  lbl_time = tk.Label(text="0000", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_time.place(x=70, y=28, anchor=tk.NW)
  #lbl_time.after(1000, count_up)
  photo_life = tk.PhotoImage(file="images/pacman/right0.png")
  
  lbl_life = []
  for i in range(5):
    lbl_life.append(tk.Label(text="", bg="black", image=photo_life))
  for i in range(5):
    lbl_life[i].place_forget()
  
  for i in range(cpp.remain_num()):
    lbl_life[i].place(x=10+35*i, y=630, anchor=tk.SW)


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
  thread1.setDaemon(False)
  thread1.start()

  root.mainloop()

def viewmsg():
  global lbl_msg1, lbl_msg2, msg_status
  if msg_status == 0:
    msg_status = 1
    lbl_msg1.place_forget()
    lbl_msg2.place(x=250, y=580, anchor=tk.CENTER)
  else:
    msg_status = 0
    lbl_msg2.place_forget()
    lbl_msg1.place(x=250, y=580, anchor=tk.CENTER)
  lbl_msg1.after(3000, viewmsg)

def endgame(event):
  key = event.keysym
  if key == "Return":
    sys.exit()

def failed_result():
  global lbl_msg1, lbl_msg2, thread1, thread2
  thread2.join()
  thread1.join()
  
  canvas.destroy()
  with open("time.txt", "r") as f:
    ranklist = f.readlines()
  for i in range(len(ranklist)):
    ranklist[i] = ranklist[i].replace("\n", "")
  print(ranklist)
  result = tk.Canvas(root, width=500, height=630, bg="black")
  lbl_title = tk.Label(result, text="GAMEOVER", font=("4x4極小かなフォント", 30), fg="white", bg="black")
  lbl_title.place(x=250, y=80, anchor=tk.CENTER)
  lbl_score = tk.Label(result, text="SCORE", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_score.place(x=100, y=160, anchor=tk.CENTER)
  lbl_point = tk.Label(result, text="0000000", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_point.place(x=400, y=160, anchor=tk.CENTER)
  lbl_time = tk.Label(result, text="TIME", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_time.place(x=100, y=240, anchor=tk.CENTER)
  lbl_clock = tk.Label(result, text="00:00", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_clock.place(x=400, y=240, anchor=tk.CENTER)
  lbl_ranking = tk.Label(result, text="TIME ATTACK RANKING", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_ranking.place(x=250, y=320, anchor=tk.CENTER)
  lbl_place1 = tk.Label(result, text="1ST", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place1.place(x=150, y=380, anchor=tk.CENTER)
  lbl_rank1 = tk.Label(result, text=ranklist[0], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank1.place(x=350, y=380, anchor=tk.CENTER)
  lbl_place2 = tk.Label(result, text="2ND", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place2.place(x=150, y=440, anchor=tk.CENTER)
  lbl_rank2 = tk.Label(result, text=ranklist[1], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank2.place(x=350, y=440, anchor=tk.CENTER)
  lbl_place3 = tk.Label(result, text="3RD", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place3.place(x=150, y=500, anchor=tk.CENTER)
  lbl_rank3 = tk.Label(result, text=ranklist[2], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank3.place(x=350, y=500, anchor=tk.CENTER)
  lbl_msg1 = tk.Label(result, text="THX FOR PLAYING!!", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_msg1.place(x=250, y=580, anchor=tk.CENTER)
  lbl_msg2 = tk.Label(result, text="PRESS ENTER TO GO TO MENU", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_msg1.after(3000, viewmsg)
  lbl_rank1.bind('<Key>', endgame)
  lbl_rank1.focus_set()
  result.pack()
  
def cleared_result():
  global lbl_msg1, lbl_msg2, thread1, thread2
  thread2.join()
  thread1.join()
  
  canvas.destroy()
  with open("time.txt", "r") as f:
    ranklist = f.readlines()
  for i in range(len(ranklist)):
    ranklist[i] = ranklist[i].replace("\n", "")
  print(ranklist)
  result = tk.Canvas(root, width=500, height=630, bg="black")
  lbl_title = tk.Label(result, text="GAME CLEARED", font=("4x4極小かなフォント", 30), fg="white", bg="black")
  lbl_title.place(x=250, y=80, anchor=tk.CENTER)
  lbl_score = tk.Label(result, text="SCORE", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_score.place(x=100, y=160, anchor=tk.CENTER)
  lbl_point = tk.Label(result, text="0000000", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_point.place(x=400, y=160, anchor=tk.CENTER)
  lbl_time = tk.Label(result, text="TIME", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_time.place(x=100, y=240, anchor=tk.CENTER)
  lbl_clock = tk.Label(result, text="00:00", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_clock.place(x=400, y=240, anchor=tk.CENTER)
  lbl_ranking = tk.Label(result, text="TIME ATTACK RANKING", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_ranking.place(x=250, y=320, anchor=tk.CENTER)
  lbl_place1 = tk.Label(result, text="1ST", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place1.place(x=150, y=380, anchor=tk.CENTER)
  lbl_rank1 = tk.Label(result, text=ranklist[0], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank1.place(x=350, y=380, anchor=tk.CENTER)
  lbl_place2 = tk.Label(result, text="2ND", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place2.place(x=150, y=440, anchor=tk.CENTER)
  lbl_rank2 = tk.Label(result, text=ranklist[1], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank2.place(x=350, y=440, anchor=tk.CENTER)
  lbl_place3 = tk.Label(result, text="3RD", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_place3.place(x=150, y=500, anchor=tk.CENTER)
  lbl_rank3 = tk.Label(result, text=ranklist[2], font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_rank3.place(x=350, y=500, anchor=tk.CENTER)
  lbl_msg1 = tk.Label(result, text="THX FOR PLAYING!!", font=("4x4極小かなフォント", 20), fg="white", bg="black")
  lbl_msg1.place(x=250, y=580, anchor=tk.CENTER)
  lbl_msg2 = tk.Label(result, text="PRESS ENTER TO GO TO MENU", font=("4x4極小かなフォント", 15), fg="white", bg="black")
  lbl_msg1.after(3000, viewmsg)
  lbl_rank1.bind('<Key>', endgame)
  lbl_rank1.focus_set()
  result.pack()

if __name__ == "__main__":
  menu()
  sys.exit()