#include <iostream>
#include <cassert>
#include <set>
#include <tuple>
#include <time.h>

//ブロックの数（縦、横）
constexpr int f_height = 31;
constexpr int f_width = 28;

constexpr int size = 120; //1blockの大きさ
constexpr int normal_spd = 24; //初期状態の速さ1/5
constexpr int slow_spd = 15; //低速時の速さ1/8
constexpr int high_spd = 60; //高速時の速さ(巣に戻るとき)1/2
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

constexpr int inf = 1000000000;
enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink, //enemies
  coin, COIN //道に配置されている丸いやつ
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
  {wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,COIN,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,COIN,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,coin,coin,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,coin,coin,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,wall,wall,none,none,wall,wall,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {none,none,none,none,none,none,coin,none,none,none,wall,none,none,none,none,none,none,wall,none,none,none,coin,none,none,none,none,none,none},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,wall,wall,wall,wall,wall,coin,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,coin,wall,wall,wall,wall,wall,wall},
  {wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,coin,wall,wall,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,coin,wall,wall,wall,wall,coin,wall},
  {wall,COIN,coin,coin,wall,wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall,wall,coin,coin,COIN,wall},
  {wall,wall,wall,coin,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,coin,wall,wall,wall},
  {wall,wall,wall,coin,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,coin,wall,wall,wall},
  {wall,coin,coin,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,wall,wall,coin,coin,coin,coin,coin,coin,wall},
  {wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall},
  {wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall,wall,coin,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,coin,wall},
  {wall,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,coin,wall},
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
  void start(){ Stop = false; }
  void stop(){ Stop = true; }
  void set_state(const int &t){ state = t; }
  bool warp(){
    //(y,x) = (14, -2), (14, f_width + 2)
    if(y == 14*size){
      if(x/size == -2 && rot == 1) x = (f_width+2)*size;
      else if(x/size == f_width+2 && rot == 3) x = -2*size;
      return true;
    }
    return false;
  }
  void change_speed(bool ok = true){
    if(!ison_block()) return;
    if(is_around_warp() && ok) slow_down();
    else if(state == frightened) slow_down();
    else if(state == eaten) speed_up();
    else if(state == normal) set_normal();
    else printf("error");
  }
  bool move(){
    if(is_stop()) return false;
    if(!ison_block()){
      y += dy[rot] * spd;
      x += dx[rot] * spd;
      return true;
    }
    int ty = round(y) + dy[rot];
    int tx = round(x) + dx[rot];
    if(get_field_val(ty, tx) == wall) return false;
    warp();
    y += dy[rot] * spd;
    x += dx[rot] * spd;
    return true;
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
  bool is_around_warp() const{
    if(y != 14*size) return false;
    int d = abs(14*size - x); //フィールドの中心からのx軸方向の距離
    return 9*size <= d;
  }
  //パックマンと触れたか判定する
  bool is_touch();
  void change_direction(const position &);
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
void position::change_direction(const position &target){
  if(!ison_block()) return;
  int y = round_y(), x = round_x();
  //frightened_modeの時はランダム
  if(check_state(frightened)){
    while(true){
      int ran = rand() % 4;
      int ny = y + dy[ran];
      int nx = x + dx[ran];

      if(isopposite(ran)) continue;
      if(get_field_val(ny, nx) == wall) continue;
      if(isgate.count({ny,nx, ran})) continue;

      rotate(ran);
      return;
    }
    return;
  }
  int dir = -1, dist = inf;

  //eaten_mode 巣に戻った時
  if(check_state(eaten) && y == nest_pos_y && x == nest_pos_x){
    rotate(2);
    set_state(normal);
    printf("returned\n");
    return;
  }

  for(int i = 0; i < 4; i++){
    int ny = y + dy[i];
    int nx = x + dx[i];

    if(isopposite(i)) continue;
    if(get_field_val(ny, nx) == wall) continue;
    if(isgate.count({ny,nx, i})) continue;
    int d = target.dist(ny, nx);
    if(dist > d){
      dist = d;
      dir = i;
    }
  }

  if(dir == -1) printf("error");
  rotate(dir);
}

void change_to_eaten(){
  pacman.stop();
  //戻る途中でなければ動作を一時的に止める
  for(auto enem : enemies){
    if(!enem->check_state(eaten)) enem->stop();
  }
  eat_num++;
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

//1フレームだけ進める
void move_all(){
  pacman.move();
  for(auto enem : enemies)
    enem->move();
}

void set_state_enemies(int st){
  for(auto enem : enemies){
    if(!enem->check_state(eaten)) enem->set_state(st);
  }
}

void set_speeds(){
  pacman.change_speed(false); //例外
  for(auto enem : enemies){
    enem->change_speed();
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

void set_enemies_direction(){
  red_move();
  blue_move();
  oran_move();
  pink_move();
}

//Pythonから毎フレーム呼び出される
int update(double time){
  current_time = time;
  if(frightened_start_time != 0){
    //frightened_modeが終わったときor全部食べた時
    if(time - adjust_time - frightened_start_time >= frightened_time){
      frightened_start_time = 0;
      set_state_enemies(normal);
      adjust_time += frightened_time;
      eat_num = 0;
      printf("return to normal mode\n");
    }
  }
  //時間になったらモードの変更をする
  else if(time - adjust_time >= time_table[cur_table_pos]){
    chase_mode = true;
    if(chase_mode) printf("changed to chase mode\n");
    else printf("changed to scatter mode\n");
    reverse_enemies();
    cur_table_pos++;
  }
  set_enemies_direction();
  move_all();

  int res = -1;
  int y = pacman.round_y();
  int x = pacman.round_x();
  //coinを取った時の処理
  const int v = get_field_val(y, x);
  if(v == coin || v == COIN){
    set_field_val(y, x, none);
    res = y*f_width + x;
    if(v == COIN){ //change to frightened mode
      set_state_enemies(frightened);
      frightened_start_time = time - adjust_time;
      reverse_enemies();
      printf("changed to frightened mode\n");
    }
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

  set_speeds();

  return res;
}


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
  
  //パックマンの方向移動
  void turn(int r){
    if(!pacman.ison_block()) return;

    int y = pacman.round_y();
    int x = pacman.round_x();
    y += dy[r]; x += dx[r];
    if(get_field_val(y, x) != wall){
      if(!(r == 2 && y == 12 && (x==13||x==14))) //敵の出入り口
        pacman.rotate(r);
    }
  }

};
