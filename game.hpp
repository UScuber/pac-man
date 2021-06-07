#include <iostream>
#include <cassert>
#include <set>
#include <tuple>

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
  //方向をrにセット
  void rotate(const int &r){ rot = r; }
  void reverse(){ rot = (rot+2) % 4; }
  void slow_down(){ spd = slow_spd; }
  void set_normal(){ spd = normal_spd; }
  void speed_up(){ spd = high_spd; }
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
  int dist(const position &a){
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
  void change_direction(const position &);
  private:
  int y,x,rot;
  int spd = normal_spd;
  int state = 0;
};
position pacman(pac_pos_y*size, pac_pos_x*size, 1);
position red_enemy(red_pos_y*size, red_pos_x*size, 0);
position blue_enemy(blue_pos_y*size, blue_pos_x*size, 0);
position oran_enemy(oran_pos_y*size, oran_pos_x*size, 0);
position pink_enemy(pink_pos_y*size, pink_pos_x*size, 0);

//方向転換、次に移動すべき回転場所を返す
void position::change_direction(const position &target){
  if(!ison_block()) return;
  int y = round_y(), x = round_x();
  int dir = -1, dist = inf;

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

//1フレームだけ進める
void move_all(){
  pacman.move();
  red_enemy.move();
  blue_enemy.move();
  oran_enemy.move();
  pink_enemy.move();
}

void set_speeds(){
  pacman.change_speed(false); //例外
  red_enemy.change_speed();
  blue_enemy.change_speed();
  oran_enemy.change_speed();
  pink_enemy.change_speed();
}

//モード切替時に敵の進行方向を逆にする
void reverse_enemies(){
  red_enemy.reverse();
  blue_enemy.reverse();
  oran_enemy.reverse();
  pink_enemy.reverse();
}

void red_move(){
  position target(-4*size, (f_width-3)*size); //scatter
  if(chase_mode) //chase
    target = pacman;

  red_enemy.change_direction(target);
}

void blue_move(){
  position target((f_height+1)*size, f_width*size); //scatter
  if(chase_mode){ //chase
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

  if(d >= max_dist && chase_mode) //chase
    target = pacman;
  
  oran_enemy.change_direction(target);
}

void pink_move(){
  position target(-4*size, 2*size); //scatter
  if(chase_mode){ //chase
    int r = pacman.get_r();
    int py = pacman.round_y() + pty[r];
    int px = pacman.round_x() + ptx[r];
    target = position(py*size, px*size);
  }
  pink_enemy.change_direction(target);
}

void set_enemy_direction(){
  red_move();
  blue_move();
  oran_move();
  pink_move();
}

//Pythonから毎フレーム呼び出される
int update(double time){
  //時間になったらモードの変更をする
  if(time >= time_table[cur_table_pos]){
    chase_mode ^= true;
    if(chase_mode) printf("changed to chase mode\n");
    else printf("changed to scatter mode\n");
    reverse_enemies();
    cur_table_pos++;
  }

  set_enemy_direction();
  move_all();

  int res = -1;
  int y = pacman.round_y();
  int x = pacman.round_x();
  //coinを取った時の処理
  if(get_field_val(y, x) == coin || get_field_val(y, x) == COIN){
    set_field_val(y, x, none);
    res = y*f_width + x;
  }

  set_speeds();

  return res;
}


namespace python {
  //pacman, red,blue,orange,pink = 0,1,2,3,4
  //現在の位置を出力する
  int get_posy(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_y();
      case 1: return red_enemy.get_y();
      case 2: return blue_enemy.get_y();
      case 3: return oran_enemy.get_y();
      case 4: return pink_enemy.get_y();
      default: assert(0); return -1;
    }
  }
  int get_posx(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_x();
      case 1: return red_enemy.get_x();
      case 2: return blue_enemy.get_x();
      case 3: return oran_enemy.get_x();
      case 4: return pink_enemy.get_x();
      default: assert(0); return -1;
    }
  }
  int get_rot(int i){
    assert(0 <= i && i < 5);
    switch(i){
      case 0: return pacman.get_r();
      case 1: return red_enemy.get_r();
      case 2: return blue_enemy.get_r();
      case 3: return oran_enemy.get_r();
      case 4: return pink_enemy.get_r();
      default: assert(0); return -1;
    }
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
