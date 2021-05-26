#include <iostream>
#include <cassert>
#include <set>
#include <tuple>

//ブロックの数（縦、横）
constexpr int f_height = 31;
constexpr int f_width = 28;

constexpr int size = 30; //1blockの大きさ
constexpr int init_spd = 5; //初期状態の速さ
//ピクセル
constexpr int height = (f_height - 1) * size + 1;
constexpr int width = (f_width - 1) * size + 1;

constexpr int pac_pos_y = 23, pac_pos_x = 13;
constexpr int red_pos_y = 11, red_pos_x = 13;
constexpr int blue_pos_y = 11, blue_pos_x = 13;
constexpr int oran_pos_y = 11, oran_pos_x = 13;
constexpr int pink_pos_y = 11, pink_pos_x = 13;

constexpr int inf = 1000000000;
enum{
  none, wall, //黒、壁
  pac, //Pac-Man
  red, blue, orange, pink //enemies
};
int dy[] = {-1,0,1,0};
int dx[] = {0,-1,0,1};
//フィールドの初期状態
int field[f_height][f_width] = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
  {1,0,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,0,1},
  {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,1,1,1,0,0,1,1,1,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1},
  {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0},
  {1,1,1,1,1,1,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
  {1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  {1,0,1,1,1,1,0,1,1,1,1,1,0,1,1,0,1,1,1,1,1,0,1,1,1,1,0,1},
  {1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1},
  {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
  {1,1,1,0,1,1,0,1,1,0,1,1,1,1,1,1,1,1,0,1,1,0,1,1,0,1,1,1},
  {1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1},
  {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
  {1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//enemyが入れないところ{y, x, r}
std::set<std::tuple<int,int,int>> isgate{
  {12,13,2},{12,14,2}, //敵の出入り口
  {10,12,0},{10,15,0}, //上
  {22,12,0},{22,15,0}, //下
};

//fieldの値を取得
int get_field_val(int y, int x){
  if(y < 0 || y >= f_height || x < 0 || x >= f_width) return -1;
  return field[y][x];
}

//direction: 0,1,2,3 = up,left,down,right
//方向は小さいほうから優先度高め
struct position {
  position(int y, int x, int r = 0): y(y), x(x), rot(r){}
  int get_y() const{ return y; }
  int get_x() const{ return x; }
  int get_r() const{ return rot; }
  int get_spd() const{ return spd; }
  //方向をrにセット
  void rotate(int r){
    assert(0 <= r && r < 4);
    rot = r;
  }
  bool move(){
    if(y % size || x % size){
      y += dy[rot] * spd;
      x += dx[rot] * spd;
      return true;
    }
    int ty = y / size + dy[rot];
    int tx = x / size + dx[rot];
    if(get_field_val(ty, tx) == wall) return false;
    y += dy[rot] * spd;
    x += dx[rot] * spd;
    return true;
  }
  //thisとaとの距離
  int dist(const position &a){
    int ay = a.get_y(), ax = a.get_x();
    return (ay-y)*(ay-y) + (ax-x)*(ax-x);
  }
  //thisと(y*size,x*size)との距離
  int dist(int ty, int tx) const{
    ty *= size; tx *= size;
    return (y-ty)*(y-ty) + (x-tx)*(x-tx);
  }
  bool isopposite(int r) const{
    return (rot + 2) % 4 == r;
  }
  bool ison_block() const{
    return !(y % size || x % size);
  }
  private:
  int y,x,rot;
  int spd = init_spd;
};
position pacman(pac_pos_y*size, pac_pos_x*size, 3); //初期状態は右を向いている
position red_enemy(red_pos_y*size, red_pos_x*size, 0);
position blue_enemy(blue_pos_y*size, blue_pos_x*size, 0);
position oran_enemy(oran_pos_y*size, oran_pos_x*size, 0);
position pink_enemy(pink_pos_y*size, pink_pos_x*size, 0);

//方向転換、次に移動すべき回転場所を返す
int change_direction(const position &obj, const position &target){
  if(!obj.ison_block()) return obj.get_r();
  int y = obj.get_y();
  int x = obj.get_x();
  y /= size; x /= size;

  int dir = -1, dist = inf;
  for(int i = 0; i < 4; i++){
    int ny = y + dy[i];
    int nx = x + dx[i];

    if(obj.isopposite(i)) continue;
    if(get_field_val(ny, nx) == wall) continue;
    if(isgate.count({ny,nx, i})) continue;
    int d = target.dist(ny, nx);
    if(dist > d){
      dist = d;
      dir = i;
    }
  }
  if(dir == -1) printf("error");
  return dir;
}

//pink target
int pty[] = {-4,0,4,0};
int ptx[] = {-4,-4,0,4};
//blue center
int bcy[] = {-2,0,2,0};
int bcx[] = {-2,-2,0,2};

void red_move(){
  int dir = change_direction(red_enemy, pacman);
  red_enemy.rotate(dir);
}

void blue_move(){
  int r = pacman.get_r();
  int py = pacman.get_y() + bcy[r]*size, px = pacman.get_x() + bcx[r]*size;
  position target(2*py - red_enemy.get_y(), 2*px - red_enemy.get_x());

  int dir = change_direction(blue_enemy, target);
  blue_enemy.rotate(dir);
}

void oran_move(){
  constexpr int max_dist = 8*size * 8*size; //最大距離の2乗
  int d = pacman.dist(oran_enemy);
  //円の中に入った場合
  if(d < max_dist){
    position target(height, 0);
    int dir = change_direction(oran_enemy, target);
    oran_enemy.rotate(dir);
  }else{
    int dir = change_direction(oran_enemy, pacman);
    oran_enemy.rotate(dir);
  }
}

void pink_move(){
  int r = pacman.get_r();
  position target(pacman.get_y() + pty[r]*size, pacman.get_x() + ptx[r]*size);

  int dir = change_direction(pink_enemy, target);
  pink_enemy.rotate(dir);
}
//Pythonから毎フレーム呼び出される
void update(){
  red_move();
  blue_move();
  oran_move();
  pink_move();
  
  pacman.move();
  red_enemy.move();
  blue_enemy.move();
  oran_enemy.move();
  pink_enemy.move();
}


//前回の状態からどれだけ動くのかを出力する
//それを受けてpythonで移動の処理をする
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
    int y = pacman.get_y();
    int x = pacman.get_x();
    if(y % size || x % size) return;

    y /= size; x /= size;
    y += dy[r]; x += dx[r];
    if(get_field_val(y, x) != wall){
      pacman.rotate(r);
    }
  }
};
