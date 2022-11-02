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
canvas, menu = None, None #canvas
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
last_pac_posx, last_pac_posy = 81000, 138000
KEY_NAME = ["Up", "Left", "Down", "Right"]
FONT_NAME = "Arial"
CANVAS_WIDTH, CANVAS_HEIGHT = 500, 630
DISPLAY_WIDTH, DISPLAY_HEIGHT = 1920, 1080 #windowの大きさに後で変更される
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
  if key_state == "q":
    print("exit")
    sys.exit()
  
  for i in range(len(KEY_NAME)):
    if key_state == KEY_NAME[i]:
      if i & 1:
        cpp.start_move() #動作の開始
        
        if is_first: #タイムのカウントを始める
          is_first = 0
          lbl_time.after(1000, count_up)
          lbl_start.destroy()

      ispress_key[i] = True
      last_pressed_key = i

#画像の位置や向きなどの更新
def update_images():
  global canvas
  global last_pac_posx, last_pac_posy
  for i in range(len(OBJECTS)):
    x, y, r, s = cpp.get_xyrs(i)
    if i == 0:
      if last_pac_posx == x and last_pac_posy == y:
        canvas.itemconfig(OBJECTS[i], image=images[s][i][r][0])
        continue
      last_pac_posx, last_pac_posy = x, y
    if cpp.get_isstop(i):
      if s == 1: #eaten スコアの表示
        x = (x + cpp.sizec/2) // cpp.sizec
        y = (y + cpp.sizec/2) // cpp.sizec
        pcx, pcy = last_pac_posx, last_pac_posy
        pcx = (pcx + cpp.sizec/2) // cpp.sizec
        pcy = (pcy + cpp.sizec/2) // cpp.sizec
        if x == pcx and y == pcy:
          #i,flipはどの数字でもよい
          if cpp.eat_num() == 0: print("eat_num_error")
          canvas.itemconfig(OBJECTS[i], image=images[SCORE][i][cpp.eat_num() - 1][flip])
      continue
    t = int(cpp.limit_time(i) * 4)
    if t <= 8 and not(t & 1): #flash
      canvas.itemconfig(OBJECTS[i], image=images[FLASH][i][r][flip])
    else:
      canvas.itemconfig(OBJECTS[i], image=images[s][i][r][flip])
    canvas.moveto(OBJECTS[i], x / cpp.sizec * SIZE + ADJ_X, y / cpp.sizec * SIZE + ADJ_Y)

#coinの消去
def delete_coin(t: int):
  if t != -1:
    canvas.delete("coin" + str(t))

#canvasのdestroy
def destroy_all():
  global canvas, menu
  if str(type(canvas)) == "<class 'tkinter.Canvas'>":
    canvas.destroy()
  if str(type(menu)) == "<class 'tkinter.Canvas'>":
    menu.destroy()

#盤面の更新
def update():
  global canvas, flip, is_end, game_score, lbl_score, lbl_mode
  cnt = 0
  start = time.time()
  clock = pygame.time.Clock()
  while is_end == 0:
    clock.tick(FRAME)
    res = cpp.update_pos(time.time() - start, last_pressed_key)
    delete_coin(res)
    if cnt % FLIP_FREQ == 0:  flip ^= 1

    sys.stdout.flush()
    cnt += 1
    game_score = cpp.get_score()
    
    update_images()

    lbl_score["text"] = str(game_score).zfill(7)
    lbl_mode["text"] = "CHASE" if cpp.get_mode() else "SCATTER"
    if cpp.is_game_over():
      time.sleep(1)
      print("####"+str(cpp.remain_num()))
      if cpp.remain_num() <= 0:
        is_end = 1
        thread3 = threading.Thread(target=display_result, args=(cpp.remain_num(),))
        thread3.setDaemon(True)
        thread3.start()
        sys.exit()
      for i in range(cpp.remain_num()):
        lbl_life[i].place_forget()
      for i in range(cpp.remain_num() - 1):
        lbl_life[i].place(x=10+35*i, y=630, anchor=tk.SW)
      cpp.restart()
      cpp.start_move()
      update_images() #初期状態を描画
      cpp.stop_move()
    elif cpp.is_game_cleared():
      time.sleep(1)
      is_end = 1
      thread3 = threading.Thread(target=display_result, args=(cpp.remain_num(),))
      thread3.setDaemon(True)
      thread3.start()

def read_all_images():
  #coinはmain関数の中で画像を読み込む
  for i in range(len(OBJECTS)): #object
    for j in range(4): #direction
      for k in range(2): #flip
        #normal
        img_name = "images/"+OBJECTS[i]+"/"+DIREC_NAME[j]+str(k) +".png"
        images[NORMAL][i][j][k] = tk.PhotoImage(file=img_name)
        #eaten
        img_name = "images/eaten/"+DIREC_NAME[j]+".png"
        images[EATEN][i][j][k] = tk.PhotoImage(file=img_name)
        #frightened
        #とりあえず白く点滅するやつはなしにする
        img_name = "images/frightened/"+"0"+str(k)+".png"
        images[FRIGHTENED][i][j][k] = tk.PhotoImage(file=img_name)
        #score
        #食べられた時の表示する200,400,800,1600の画像
        img_name = "images/eaten/"+str(1<<(j+1))+"00.png"
        images[SCORE][i][j][k] = tk.PhotoImage(file=img_name)
        #flash
        #残り数秒になった時に白く点滅する画像
        img_name = "images/frightened/"+"1"+str(k)+".png"
        images[FLASH][i][j][k] = tk.PhotoImage(file=img_name)

def draw_all_coins(coins: list):
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
      coins.append(tk.PhotoImage(file=file_name))
      tag = "coin" + str(i*cpp.w + j)
      canvas.create_image((j+1)*SIZE + 7, (i+1)*SIZE +56, image=coins[-1], tag=tag)

def startgame(event):
  global menu
  key_state = event.keysym
  if key_state == "Return":
    destroy_all()
    main()

def put_label(canvas, txt: str, font_size: int, x: int, y: int, anchor=tk.CENTER) -> tk.Label:
  label = tk.Label(canvas, text=txt, font=(FONT_NAME, font_size), fg="white", bg="black")
  label.place(x=x, y=y, anchor=anchor)
  return label

def make_label(canvas, txt: str, font_size: int) -> tk.Label:
  return tk.Label(canvas, text=txt, font=(FONT_NAME, font_size), fg="white", bg="black")

def make_canvas() -> tk.Canvas:
  global root
  return tk.Canvas(root, width=CANVAS_WIDTH, height=CANVAS_HEIGHT, bg="black")

def reset_values():
  global msg_status, is_end, game_time, is_first
  msg_status = 0
  is_end = 0
  game_time = 0
  is_first = 1

def set_menu():
  global pic_pac
  global root, menu
  reset_values()
  destroy_all()
  menu = make_canvas()
  pic_pac = ImageTk.PhotoImage(Image.open("images/pacman.png").resize((175, 181)))
  put_label(menu, "PAC-MAN", 48, x=250, y=80)
  put_label(menu, "矢印キーでパックマンを操作できます", 18, x=250, y=160)
  put_label(menu, "C++とPythonを使ってなるべく再現してみました", 15, x=250, y=450)
  put_label(menu, "PRESS ENTER", 20, x=250, y=525)
  put_label(menu, "TO START THE GAME", 20, x=250, y=565)
  lbl_pac = tk.Label(menu, text="", image=pic_pac)
  lbl_pac.place(x=250, y=315, anchor=tk.CENTER)
  menu.bind('<Key>', startgame)
  menu.focus_set()
  menu.place(x=DISPLAY_WIDTH//2, y=DISPLAY_HEIGHT//2, anchor=tk.CENTER)

def count_up():
  global game_time, lbl_time
  game_time += 1
  lbl_time["text"] = str(game_time).zfill(4)
  if is_end == 0:
    lbl_time.after(1000, count_up)


#ゲームウィンドウの作成
def main():
  global board, coins, photo_life
  global canvas, lbl_score, lbl_time, lbl_start, lbl_mode, lbl_life, thread1
  canvas = make_canvas()

  cpp.reset()

  #boardに画像を取り込む
  board = tk.PhotoImage(file="images/stage.png")
  canvas.create_image(250, 318, image=board)
  lbl_start = put_label(canvas, "READY!", 15, x=252, y=362)
  lbl_time = put_label(canvas, "0000", 15, x=40, y=28, anchor=tk.NW)
  lbl_score = put_label(canvas, "0000000", 15, x=370, y=28, anchor=tk.NW)
  lbl_mode = put_label(canvas, "SCATTER", 15, x=250, y=28, anchor=tk.N)
  photo_life = tk.PhotoImage(file="images/pacman/right0.png")
  
  lbl_life = []
  for i in range(5):
    lbl_life.append(tk.Label(canvas, text="", bg="black", image=photo_life))
  for i in range(5):
    lbl_life[i].place_forget()
  
  for i in range(cpp.remain_num()):
    lbl_life[i].place(x=10+35*i, y=630, anchor=tk.SW)

  #coinを描画
  coins = [] #一時保存用
  draw_all_coins(coins)

  #pacman,enemiesを描画
  for i in [0,4,3,2,1]: #画像の奥行を設定
    x, y, r, s = cpp.get_xyrs(i)
    canvas.create_image(x / cpp.sizec * SIZE + 22, y / cpp.sizec * SIZE + 72,
                        image=images[s][i][r][flip], tag=OBJECTS[i])

  canvas.place(x=DISPLAY_WIDTH//2, y=DISPLAY_HEIGHT//2, anchor=tk.CENTER)

  #updateを別のスレッドで動かす
  thread1 = threading.Thread(target=update)
  thread1.setDaemon(True)
  thread1.start()

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
  global canvas, lbl_msg1
  key = event.keysym
  if key == "Return":
    lbl_msg1.destroy()
    destroy_all()
    print("back to menu")
    set_menu()

def display_result(zanki: int):
  global lbl_msg1, lbl_msg2, thread1
  thread1.join()
  
  destroy_all()
  # read ranking
  with open("score.txt", "r") as f:
    ranklist = f.readlines()
  for i in range(len(ranklist)):
    ranklist[i] = int(ranklist[i].strip())
  ranklist.append(game_score)
  ranklist.sort()
  ranklist.reverse()
  print(ranklist)
  # add ranking
  with open("score.txt", "a") as f:
    print(game_score, file=f)
  
  result = make_canvas()

  if zanki == 0:
    put_label(result, "GAMEOVER", 30, x=250, y=80)
  else:
    put_label(result, "GAME CLEARED!!", 30, x=250, y=80)
  put_label(result, "SCORE", 20, x=100, y=160)
  put_label(result, game_score, 20, x=400, y=160)
  put_label(result, "TIME", 20, x=100, y=240)
  put_label(result, game_time, 20, x=400, y=240)
  put_label(result, "SCORE RANKING", 20, x=250, y=320)
  put_label(result, "1ST", 20, x=150, y=380)
  put_label(result, ranklist[0], 20, x=350, y=380)
  put_label(result, "2ND", 20, x=150, y=440)
  put_label(result, ranklist[1], 20, x=350, y=440)
  put_label(result, "3RD", 20, x=150, y=500)
  put_label(result, ranklist[2], 20, x=350, y=500)
  lbl_msg1 = put_label(result, "THX FOR PLAYING!!", 20, x=250, y=580)
  lbl_msg1.after(3000, viewmsg)
  lbl_msg2 = make_label(result, "PRESS ENTER TO GO TO MENU", 15)

  result.bind('<KeyPress>', endgame)
  result.focus_set()
  result.place(x=DISPLAY_WIDTH//2, y=DISPLAY_HEIGHT//2, anchor=tk.CENTER)


if __name__ == "__main__":
  root = tk.Tk()
  root.title("Pac-Man")
  root.config(bg="black")
  root.attributes("-fullscreen", True)
  root.update_idletasks()
  DISPLAY_WIDTH = root.winfo_width()
  DISPLAY_HEIGHT = root.winfo_height()
  set_menu()
  read_all_images() #始めに画像をすべて読み込んでおく
  root.bind('<KeyPress>', press_key)
  root.mainloop()