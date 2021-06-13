#include <iostream>
#include <cassert>
#include <set>
#include <tuple>
#include <time.h>

//ブロックの数（縦、横）
constexpr int f_height = 31;
constexpr int f_width = 28;

constexpr int size = 6000; //1blockの大きさ
constexpr int normal_spd = 880; //初期状態の速さ90%
constexpr int slow_spd = 440; //低速時の速さ
constexpr int high_spd = 2200; //高速時の速さ(巣に戻るとき)
//ピクセル
constexpr int height = (f_height - 1) * size + 1;
constexpr int width = (f_width - 1) * size + 1;

constexpr int pac_pos_y = 23, pac_pos_x = 13;
constexpr int red_pos_y = 11, red_pos_x = 13;
constexpr int blue_pos_y = 11, blue_pos_x = 13;
constexpr int oran_pos_y = 11, oran_pos_x = 13;
constexpr int pink_pos_y = 11, pink_pos_x = 13;
//食べられた時に戻る場所
constexpr int nest_pos_y = 11, nest_pos_x = 13;

//frightened_modeの制限時間[s]
constexpr double frightened_time = 10;
constexpr int eat_cnt = 30; //食べたときに止まるフレーム数
//frightened_modeが終了する何秒[s]前から点滅させるか
constexpr double frightened_limit_time = 3;

constexpr int dots_all_num = 246;

constexpr int inf = 1000000000;
enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink, //enemies
  dots, DOTS
};
//敵の状態
enum{
  normal,
  eaten,
  frightened
};
bool chase_mode = true;
bool gameover = false;

int wait_cnt = 0;
int eat_num = 0; //食べた数
double adjust_time = 0; //frightenedの時の時間を引く
double frightened_start_time = 0;
double current_time = 0;

int dots_remain_num = dots_all_num;

const int dy[] = {-1,0,1,0};
const int dx[] = {0,-1,0,1};
//pink target
const int pty[] = {-4,0,4,0};
const int ptx[] = {-4,-4,0,4};
//blue center
const int bcy[] = {-2,0,2,0};
const int bcx[] = {-2,-2,0,2};


int cur_table_pos = 0;
const int time_table[] = { //chase,scatter modeを変える時間[s]
  0, //changed to scatter_mode
  7, //when 7[s], changes to chase_mode
  20 +7, //when 20+7[s], changes to scatter_mode
  7 +20+7, //chase
  20 +7+20+7, //scatter
  5 +20+7+20+7, //chase
  20 +5+20+7+20+7, //scatter
  5 +20+5+20+7+20+7, //chase
  inf //won't change to scatter
};

//フィールドの初期状態
int field[f_height][f_width] = {
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall},
  {wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,DOTS,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,DOTS,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,none,none,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {none,none,none,none,none,none,dots,none,none,none,wall,none,none,none,none,none,none,wall,none,none,none,dots,none,none,none,none,none,none},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall},
  {wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall},
  {wall,DOTS,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,DOTS,wall},
  {wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall},
  {wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall},
  {wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall},
  {wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall},
  {wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall},
  {wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall},
  {wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall}
};

//enemyが入れないところ{y, x, r}
std::set<std::tuple<int,int,int>> isgate{
  {12,13,2},{12,14,2}, //敵の出入り口
  {10,12,0},{10,15,0}, //上
  {22,12,0},{22,15,0}, //下
};

//fieldの値を取得
int get_field_val(int y, int x){
  if(y == 14 && (x < 0 || x >= f_width)) return none; //14はワープのところ
  if(y < 0 || y >= f_height || x < 0 || x >= f_width) return wall;
  return field[y][x];
}
void set_field_val(int y, int x, int t){
  if(y < 0 || y >= f_height || x < 0 || x >= f_width);
  else field[y][x] = t;
}
int round(const int &a){
  return (a+size/2)/size;
}

//direction: 0,1,2,3 = up,left,down,right
//方向は小さいほうから優先度高め
struct position {
  position(int y = 0, int x = 0, int r = 0): y(y), x(x), rot(r){}
  int get_y() const{ return y; }
  int get_x() const{ return x; }
  int get_r() const{ return rot; }
  int get_spd() const{ return spd; }
  int round_y() const{ return round(y); }
  int round_x() const{ return round(x); }
  bool is_stop() const{ return Stop; }
  int get_state() const{ return state; }
  bool check_state(const int &i) const{ return state == i; }
  //方向をrにセット
  void rotate(const int &r){ rot = r; }
  void reverse(){ rot = (rot+2) % 4; }
  void slow_down(){ spd = slow_spd; }
  void set_normal(){ spd = normal_spd; }
  void speed_up(){ spd = high_spd; }
  //最大速度のt%の速度に設定
  void set_speed(int t){ spd = 11 * t; }
  void start(){ Stop = false; }
  void stop(){ Stop = true; }
  void set_state(const int &t){ state = t; }
  void warp(){
    //(y,x) = (14, -2), (14, f_width + 2)
    if(y == 14*size){
      if(x < -2*size) x = (f_width+2)*size - (-2*size - x);
      else if(x > (f_width+2)*size) x = -2*size + (x - (f_width+2)*size);
    }
  }
  void change_speed(bool ok = true){
    if(!ison_block()) return;
    if(is_intunnel() && ok) slow_down();
    else if(state == frightened) slow_down();
    else if(state == eaten) speed_up();
    else if(state == normal) set_normal();
    else printf("error");
  }
  //壁に当たった場合、残りの移動量を返す
  int move(){
    if(is_stop()) return 0;
    int ry = round(y);
    int rx = round(x);
    int ty = (ry*size - y) * dy[rot];
    int tx = (rx*size - x) * dx[rot];
    
    //現在の方向と同じだった場合
    if(0 <= ty && ty < spd && 0 <= tx && tx < spd){
      int res = ty + tx; //動く量
      y += dy[rot] * res;
      x += dx[rot] * res;
      //if(get_field_val(ry+dy[rot], rx+dx[rot]) == wall) return 0;
      return spd - res; //残りの量
    }else{
      warp();
      y += dy[rot] * spd;
      x += dx[rot] * spd;
      return 0;
    }
    return 0;
  }
  //thisとaとの距離
  int dist(const position &a) const{
    int ay = a.round_y(), ax = a.round_x();
    int ty = round_y(), tx = round_x();
    return (ay-ty)*(ay-ty) + (ax-tx)*(ax-tx);
  }
  //thisと(y*size,x*size)との距離
  int dist(const int &sy, const int &sx) const{
    int ty = round_y(), tx = round_x();
    return (sy-ty)*(sy-ty) + (sx-tx)*(sx-tx);
  }
  bool isopposite(const int &r) const{ return (rot + 2) % 4 == r; }
  bool ison_block() const{ return !(y % size || x % size); }
  //ワープする所の通路にいるかどうか
  bool is_intunnel() const{
    if(y != 14*size) return false;
    int d = abs(14*size - x); //フィールドの中心からのx軸方向の距離
    return 9*size <= d;
  }
  //パックマンと触れたか判定する
  bool is_touch();
  void change_direction(const position &, int);
  private:
  int y,x,rot;
  int spd = normal_spd;
  int state = normal;
  bool Stop = false;
};
position pacman(pac_pos_y*size, pac_pos_x*size, 1);
position red_enemy(red_pos_y*size, red_pos_x*size, 0);
position blue_enemy(blue_pos_y*size, blue_pos_x*size, 0);
position oran_enemy(oran_pos_y*size, oran_pos_x*size, 0);
position pink_enemy(pink_pos_y*size, pink_pos_x*size, 0);

position *enemies[] = { &red_enemy, &blue_enemy, &oran_enemy, &pink_enemy };
//方向転換、次に移動すべき回転場所を返す
void position::change_direction(const position &target, int dir = -1){
  int move_num = move(); //動ける量

  if(!ison_block()) return;

  int ry = round_y(), rx = round_x();

  //pac-manからの方向の入力を確かめる
  if(dir != -1){
    //入力なしの場合
    if(dir >= 4) dir = rot;

    int ny = ry + dy[dir];
    int nx = rx + dx[dir];
    if(get_field_val(ny, nx) == wall){
      if(get_field_val(ry+dy[rot], rx+dx[rot]) != wall) dir = rot;
      else return;
    }
    if(dir == 2 && ny == 12 && (nx==13||nx==14)) //敵の出入り口
      return;
  }
  //frightened_modeの時はランダム
  else if(check_state(frightened)){
    while(true){
      dir = rand() % 4;
      int ny = ry + dy[dir];
      int nx = rx + dx[dir];

      if(isopposite(dir)) continue;
      if(get_field_val(ny, nx) == wall) continue;
      if(isgate.count({ny,nx, dir})) continue;
      break;
    }
  }
  //eaten_mode 巣に戻った時
  else if(check_state(eaten) && ry == nest_pos_y && rx == nest_pos_x){
    rotate(2);
    set_state(normal);
    printf("returned\n");
    return;
  }
  else{
    int dist = inf;
    for(int i = 0; i < 4; i++){
      int ny = ry + dy[i];
      int nx = rx + dx[i];

      if(isopposite(i)) continue;
      if(get_field_val(ny, nx) == wall) continue;
      if(isgate.count({ny,nx, i})) continue;
      int d = target.dist(ny, nx);
      if(dist > d){
        dist = d;
        dir = i;
      }
    }
  }

  if(dir == -1) printf("dir_error ");

  //進行方向がwall
  if(get_field_val(ry+dy[dir], rx+dx[dir]) == wall){
    return;
  }
  rotate(dir);

  //残りの分を動かす
  y += dy[rot] * move_num;
  x += dx[rot] * move_num;
}

void change_to_eaten(){
  pacman.stop();
  //戻る途中でなければ動作を一時的に止める
  for(auto enem : enemies){
    if(!enem->check_state(eaten)) enem->stop();
  }
  eat_num++;
}

void change_all_speed(bool is_ate_dots){
  //enemies
  bool changed[] = {false};

  if(dots_remain_num <= 10){ //elroy2 dots left
    for(int i = 0; i <= 1; i++){
      enemies[i]->set_speed(85);
      changed[i] = true;
    }
  }else if(dots_remain_num <= 20){ //elroy1 dots left
    for(int i = 0; i <= 1; i++){
      enemies[i]->set_speed(80);
      changed[i] = true;
    }
  }

  for(int i = 0; i < 4; i++){
    if(changed[i]) continue;
    if(enemies[i]->check_state(eaten)) enemies[i]->set_speed(250); //check
    else if(enemies[i]->is_intunnel()) enemies[i]->set_speed(40);
    else if(enemies[i]->check_state(frightened)) enemies[i]->set_speed(50);
    else if(enemies[i]->check_state(normal)) enemies[i]->set_speed(75);
  }

  //pacman
  //frightened_modeの場合
  if(frightened_start_time != 0){
    if(is_ate_dots) pacman.set_speed(79);
    else pacman.set_speed(90);
  }else{
    if(is_ate_dots) pacman.set_speed(71);
    else pacman.set_speed(80);
  }
}

bool position::is_touch(){
  if(dist(pacman) || check_state(eaten)) return false;
  if(check_state(normal)){
    gameover = true;
    printf("gameover!\n");
  }else if(check_state(frightened)){
    wait_cnt = eat_cnt;
    //stop, eaten_modeにする
    stop(); set_state(eaten);
    change_to_eaten();
  }
  return true;
}

void set_state_enemies(int st){
  for(auto enem : enemies){
    if(!enem->check_state(eaten)) enem->set_state(st);
  }
}

//モード切替時に敵の進行方向を逆にする
void reverse_enemies(){
  for(auto enem : enemies){
    enem->reverse();
  }
}

void red_move(){
  position target(-4*size, (f_width-3)*size); //scatter
  if(red_enemy.check_state(eaten)) //eaten
    target = position(nest_pos_y*size, nest_pos_x*size);
  else if(chase_mode) //chase
    target = pacman;

  red_enemy.change_direction(target);
}

void blue_move(){
  position target((f_height+1)*size, f_width*size); //scatter
  if(blue_enemy.check_state(eaten)) //eaten
    target = position(nest_pos_y*size, nest_pos_x*size);
  else if(chase_mode){ //chase
    int r = pacman.get_r();
    int py = pacman.round_y() + bcy[r];
    int px = pacman.round_x() + bcx[r];

    int ty = 2*py - red_enemy.round_y();
    int tx = 2*px - red_enemy.round_x();
    target = position(ty*size, tx*size);
  }
  blue_enemy.change_direction(target);
}

void oran_move(){
  constexpr int max_dist = 8 * 8; //最大距離の2乗
  position target((f_height+1)*size, 0); //scatter
  int d = pacman.dist(oran_enemy);

  if(oran_enemy.check_state(eaten)) //eaten
    target = position(nest_pos_y*size, nest_pos_x*size);
  else if(d >= max_dist && chase_mode) //chase
    target = pacman;
  
  oran_enemy.change_direction(target);
}

void pink_move(){
  position target(-4*size, 2*size); //scatter
  if(pink_enemy.check_state(eaten)) //eaten
    target = position(nest_pos_y*size, nest_pos_x*size);
  else if(chase_mode){ //chase
    int r = pacman.get_r();
    int py = pacman.round_y() + pty[r];
    int px = pacman.round_x() + ptx[r];
    target = position(py*size, px*size);
  }
  pink_enemy.change_direction(target);
}

//scatter <=> chase
void change_scmode(){
  chase_mode ^= true;
  if(chase_mode) printf("changed to chase mode\n");
  else printf("changed to scatter mode\n");
  reverse_enemies();
  cur_table_pos++;
}

void start_frightened_mode(double time){
  set_state_enemies(frightened);
  frightened_start_time = time - adjust_time;
  reverse_enemies();
  printf("changed to frightened mode\n");
}

void end_frightened_mode(){
  frightened_start_time = 0;
  set_state_enemies(normal);
  adjust_time += frightened_time;
  eat_num = 0;
  printf("return to normal mode\n");
}
//1フレームだけ進める
void move_all(int r){
  red_move();
  blue_move();
  oran_move();
  pink_move();

  pacman.change_direction(position(), r);
}


//Pythonから毎フレーム呼び出される
//rはキーボードから受け付けた方向
int update(double time, int r){
  current_time = time;
  if(frightened_start_time != 0){
    //frightened_modeが終わったときor全部食べた時
    if(time - adjust_time - frightened_start_time >= frightened_time){
      end_frightened_mode();
    }
  }
  //時間になったらモードの変更をする
  else if(time - adjust_time >= time_table[cur_table_pos]){
    change_scmode();
  }

  move_all(r);

  bool is_ate_dots = false;
  int res = -1;
  int y = pacman.round_y();
  int x = pacman.round_x();
  //dotsを取った時の処理
  const int v = get_field_val(y, x);
  if(v == dots || v == DOTS){
    set_field_val(y, x, none);
    res = y*f_width + x;
    dots_remain_num--;
    is_ate_dots = true;
    if(v == DOTS) start_frightened_mode(time);
  }

  if(wait_cnt) wait_cnt--;

  if(!wait_cnt){
    pacman.start();
    for(auto enem : enemies){
      enem->start();
    }
    
    if(frightened_start_time != 0 && eat_num == 4){
      //adjust_timeを調節してfrightened_modeを終了させる
      adjust_time = time - frightened_start_time - frightened_time;
    }

    for(auto enem : enemies){
      if(enem->is_touch()) return res;
    }
  }
  change_all_speed(is_ate_dots);

  return res;
}

#include <Windows.h>

namespace python {
  //pacman, red,blue,orange,pink = 0,1,2,3,4
  //現在の位置を出力する
  int get_posy(int i){
    if(!i) return pacman.get_y();
    return enemies[i - 1]->get_y();
  }
  int get_posx(int i){
    if(!i) return pacman.get_x();
    return enemies[i - 1]->get_x();
  }
  int get_rot(int i){
    if(!i) return pacman.get_r();
    return enemies[i - 1]->get_r();
  }
  int get_state(int i){
    if(!i) return pacman.get_state();
    return enemies[i - 1]->get_state();
  }
  bool get_is_stop(int i){
    if(!i) return pacman.is_stop();
    return enemies[i - 1]->is_stop();
  }
  //frightened_modeが時間制限になりそうかどうか
  bool get_is_limit(int i){
    if(!i) return false;
    if(frightened_time - (current_time - adjust_time - frightened_start_time) <= frightened_limit_time){
      if(enemies[i - 1]->check_state(frightened)) return true;
    }
    return false;
  }
  int get_eat_num(){
    return eat_num;
  }
  
};
