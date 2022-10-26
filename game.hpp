#include <set>
#include <tuple>
#include <time.h>

#define Assert(ex) if(!(ex)) puts("error!!!")

// ブロックの数(縦、横)
constexpr int height = 31;
constexpr int width = 28;

constexpr int size = 6000; // 1blockの大きさ
constexpr int frame_move = 8; // 1frameで動く量(1%)
constexpr int normal_spd = frame_move * 80; // 初期状態の速さ80%

constexpr int enemies_num = 4;

// frightened_modeが終了する何秒[s]前から点滅させるか
constexpr double frightened_limit_time = 9*0.25;
// frightened_modeの制限時間[s]
constexpr double frightened_time = 6 + frightened_limit_time;
// 食べたときに止まるフレーム数
constexpr int eat_cnt = 35;

constexpr int dots_all_num = 244;

constexpr int inf = 1000000000;
enum {
  none, wall, // 黒、壁
  pac, // Pac-Man
  red, blue, orange, pink, // enemies
  dots, DOTS
};
// 敵の状態
enum State {
  normal, // 通常
  eaten, // 食べられた
  frightened, // 青色状態
  tonest, // to nest 巣の中に入るまで(nest -> innestまで)
  innest, // in nest 入ってから待機する状態
  prepare // 出るまでの間
};
// rotate
enum { U,L,D,R };
bool chase_mode = true;
bool gameover = false;

int wait_cnt = 0;
int eat_num = 0; // 食べた数
double adjust_time = 0; // frightenedの時の時間を引く
double frightened_start_time = -1;
double current_time = 0;

int dots_remain_num = dots_all_num;

constexpr int dy[] = { -1,0,1,0 };
constexpr int dx[] = { 0,-1,0,1 };
// pink target
constexpr int pty[] = { -4,0,4,0 };
constexpr int ptx[] = { -4,-4,0,4 };
// blue center target
constexpr int bcy[] = { -2,0,2,0 };
constexpr int bcx[] = { -2,-2,0,2 };


int cur_table_pos = 0;
// chase,scatter modeを変える時間[s]
constexpr int time_table[] = {
  0, // changed to scatter_mode
  7, // when 7[s], changes to chase_mode
  20 +7, // when 20+7[s], changes to scatter_mode
  7 +20+7, // chase
  20 +7+20+7, // scatter
  5 +20+7+20+7, // chase
  20 +5+20+7+20+7, // scatter
  5 +20+5+20+7+20+7, // chase
  inf // won't change to scatter
};

// フィールドの初期状態
constexpr int first_field_board[height][width] = {
  { wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall },
  { wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,DOTS,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,DOTS,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,none,wall,wall,none,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,none,none,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { none,none,none,none,none,none,dots,none,none,none,wall,none,none,none,none,none,none,wall,none,none,none,dots,none,none,none,none,none,none },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,none,none,none,none,none,none,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,none,none,none,none,none,none,none,none,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,wall,wall,wall,wall,wall,dots,wall,wall,none,wall,wall,wall,wall,wall,wall,wall,wall,none,wall,wall,dots,wall,wall,wall,wall,wall,wall },
  { wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,dots,wall,wall,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,dots,wall,wall,wall,wall,dots,wall },
  { wall,DOTS,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,dots,none,none,dots,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,DOTS,wall },
  { wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall },
  { wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,dots,wall,wall,wall },
  { wall,dots,dots,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,wall,wall,dots,dots,dots,dots,dots,dots,wall },
  { wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall },
  { wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall,wall,dots,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,dots,wall },
  { wall,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,dots,wall },
  { wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall,wall },
};
int field[height][width];

// enemyが入れないところ{y, x, r}
const std::set<std::tuple<int,int,int>> isgate{
  { 12,13,D }, { 12,14,D }, //敵の出入り口
  { 10,12,U }, { 10,15,U }, //上
  { 22,12,U }, { 22,15,U }, //下
};

// fieldの値を取得
int get_field_val(const int y, const int x){
  if(y == 14 && (x < 0 || x >= width)) return none; // 14はワープのところ
  if(y < 0 || y >= height || x < 0 || x >= width) return wall;
  return field[y][x];
}
void set_field_val(const int y, const int x, const int t){
  if(y < 0 || y >= height || x < 0 || x >= width);
  else field[y][x] = t;
}
constexpr int round(const int a){
  return (a + size/2) / size;
}

// direction: 0,1,2,3 = up,left,down,right
// 方向は小さいほうから優先度高め
struct Position {
  Position(const int y=0, const int x=0, const int r=U): y(y), x(x), rot(r){}
  int get_y() const{ return y; }
  int get_x() const{ return x; }
  int get_r() const{ return rot; }
  int round_y() const{ return round(y); }
  int round_x() const{ return round(x); }
  bool is_stop() const{ return Stop; }
  int get_state() const{ return state; }
  // 方向をrにセット
  void rotate(const int r){ rot = r; }
  void reverse(){ rot = (rot+2) % 4; }
  // 最大速度のt%の速度に設定
  void set_speed(const int t){ spd = t * frame_move; }
  void start(){ Stop = false; }
  void stop(){ Stop = true; }
  void set_state(const State t){ state = t; }
  void check_warp(){
    // (y,x) = (14, -2), (14, width + 2)
    if(y == 14*size){
      if(x < -2*size) x = (width+2)*size - (-2*size - x);
      else if(x > (width+2)*size) x = -2*size + (x - (width+2)*size);
    }
  }
  // 壁に当たった場合、残りの移動量を返す
  int move_calc_rem(){
    if(is_stop()) return 0;
    const int ry = round(y);
    const int rx = round(x);
    const int ty = (ry*size - y) * dy[rot];
    const int tx = (rx*size - x) * dx[rot];
    
    // 現在の方向と同じだった場合
    if(0 <= ty && ty < spd && 0 <= tx && tx < spd){
      const int res = ty + tx; // 動く量
      y += dy[rot] * res;
      x += dx[rot] * res;
      //if(get_field_val(ry+dy[rot], rx+dx[rot]) == wall) return 0;
      return spd - res; // 残りの量
    }else{
      check_warp();
      y += dy[rot] * spd;
      x += dx[rot] * spd;
      return 0;
    }
    return 0;
  }
  // thisとaとの距離
  int dist(const Position &a) const{
    const int ay = a.round_y(), ax = a.round_x();
    const int ty = round_y(), tx = round_x();
    return (ay-ty)*(ay-ty) + (ax-tx)*(ax-tx);
  }
  // thisと(y*size,x*size)との距離
  int dist(const int sy, const int sx) const{
    const int ty = round_y(), tx = round_x();
    return (sy-ty)*(sy-ty) + (sx-tx)*(sx-tx);
  }
  bool isopposite(const int r) const{ return (rot + 2) % 4 == r; }
  bool ison_block() const{ return !(y % size || x % size); }
  // ワープする所の通路にいるかどうか
  bool is_intunnel() const{
    if(y != size*14) return false;
    const int d = abs(size*14 - x); // フィールドの中心からのx軸方向の距離
    return size*9 <= d;
  }
protected:
  int y,x,rot;
  int spd = normal_spd;
  State state = normal;
  bool Stop = true;
};


struct PacMan : Position {
  static constexpr int pac_pos_y = 23*size, pac_pos_x = 13*size+size/2;
  static constexpr int corner_cut = 2700;
  PacMan() : Position(pac_pos_y, pac_pos_x, L){}
  // 方向転換、次に移動すべき回転場所を返す
  void change_direction(int dir){
    int move_num = move_calc_rem(); // 動ける量

    if(!ison_block()) return;

    const int ry = round_y(), rx = round_x();
    // pac-manからの方向の入力を確かめる
    // 入力なしの場合
    if(dir >= 4) dir = rot;

    const int ny = ry + dy[dir];
    const int nx = rx + dx[dir];
    if(get_field_val(ny, nx) == wall){
      if(get_field_val(ry+dy[rot], rx+dx[rot]) != wall) dir = rot;
      else return;
    }
    if(dir == 2 && ny == 12 && (nx==13 || nx==14)){ // 敵の出入り口
      if(dir == rot) return;
      else dir = rot;
    }
    if(dir != rot) move_num += corner_cut;

    // 進行方向がwall
    if(get_field_val(ry+dy[dir], rx+dx[dir]) == wall){
      return;
    }
    rotate(dir);

    // 残りの分を動かす
    y += dy[rot] * move_num;
    x += dx[rot] * move_num;
  }
};

struct Enemy : Position {
  // 食べられた時に戻る場所
  static constexpr int nest_posy = 11, nest_posx = 13;
  Enemy(const int x, const int y, const int r, const PacMan &pm, const int n_posy, const int n_posx, const double n_wait_time) :
    Position(x, y, r), pacman(pm), innest_posy(n_posy), innest_posx(n_posx), nest_wait_time(n_wait_time){}
  // パックマンと触れたか
  bool is_touch() const{
    return !(dist(pacman) || get_state() == eaten);
  }
  // 入れないかどうか
  bool check_is_gate(const int y, const int x, const int r) const{
    if(get_state() == tonest) return false;
    if(isgate.count({ y, x, r})) return true;
    return false;
  }
  // 方向転換、次に移動すべき回転場所を返す
  void change_direction(const Position &target, int dir=-1){
    const int move_num = move_calc_rem(); // 動ける量

    if(!ison_block() || move_num <= 0) return;

    const int ry = round_y(), rx = round_x();

    if(get_state() == frightened){
      while(true){
        dir = rand() % 4;
        const int ny = ry + dy[dir];
        const int nx = rx + dx[dir];

        if(isopposite(dir)) continue;
        if(get_field_val(ny, nx) == wall) continue;
        if(isgate.count({ ny,nx, dir })) continue;
        break;
      }
    }
    // eaten_mode 巣の前まで来たとき
    else if(get_state() == eaten && ry == nest_posy && rx == nest_posx){
      rotate(D);
      set_state(tonest);
      if(wait_cnt) stop();
      printf("nest\n");
      return;
    }
    else if(get_state() == tonest && ry == innest_posy && rx == innest_posx){
      rotate(U);
      set_state(innest);
      cur_wait_time = nest_wait_time;
      return;
    }
    else if(get_state() == innest){
      if(cur_wait_time <= 0){
        set_state(prepare);
        rotate(U);
        return;
      }
      // up
      if(ry == 13) dir = D;
      // down
      else if(ry == 15) dir = U;
      else dir = get_r();
    }
    else if(get_state() == prepare && ry == nest_posy && rx == nest_posx){
      set_state(normal);
      return;
    }
    else{
      int dist = inf;
      for(int i = 0; i < 4; i++){
        const int ny = ry + dy[i];
        const int nx = rx + dx[i];

        if(isopposite(i)) continue;
        if(get_field_val(ny, nx) == wall) continue;
        if(check_is_gate(ny, nx, i)) continue;
        const int d = target.dist(ny, nx);
        if(dist > d){
          dist = d;
          dir = i;
        }
      }
    }

    Assert(dir != -1);

    // 進行方向がwall
    if(get_field_val(ry+dy[dir], rx+dx[dir]) == wall){
      return;
    }
    rotate(dir);

    // 残りの分を動かす
    y += dy[rot] * move_num;
    x += dx[rot] * move_num;
  }
  // frame更新時に呼び出される
  void update_frame(const double dt){
    cur_wait_time -= dt;
  }
  virtual void move() = 0;
protected:
  const PacMan &pacman;
  const int innest_posy, innest_posx;
  const double nest_wait_time;
  double cur_wait_time = 0;
};

struct RedEnemy : Enemy {
  static constexpr int red_posy = 11, red_posx = 13;
  static constexpr int innest_posy = 14, innest_posx = 13;
  static constexpr double nest_wait_time = 0;
  RedEnemy(const PacMan &pm) : Enemy(red_posy*size, red_posx*size, U, pm, innest_posy, innest_posx, nest_wait_time){}
  void move() override {
    Position target(-4*size, (width-3)*size); // scatter
    if(get_state() == eaten) // eaten
      target = Position(nest_posy*size, nest_posx*size);
    else if(get_state() == tonest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == innest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == prepare)
      target = Position(nest_posy*size, nest_posx*size);
    else if(chase_mode) // chase
      target = pacman;

    change_direction(target);
  }
};

struct BlueEnemy : Enemy {
  static constexpr int blue_posy = 14, blue_posx = 12;
  static constexpr int innest_posy = 14, innest_posx = 12;
  static constexpr double nest_wait_time = 1;
  const Enemy *red_enemy;
  BlueEnemy(const PacMan &pm, const Enemy *red) : Enemy(blue_posy*size, blue_posx*size, U, pm, innest_posy, innest_posx, nest_wait_time), red_enemy(red){
    cur_wait_time = nest_wait_time;
    set_state(innest);
  }
  void move() override {
    Position target((height+1)*size, width*size); // scatter
    if(get_state() == eaten) // eaten
      target = Position(nest_posy*size, nest_posx*size);
    else if(get_state() == tonest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == innest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == prepare)
      target = Position(nest_posy*size, nest_posx*size);
    else if(chase_mode){ // chase
      const int r = pacman.get_r();
      const int py = pacman.round_y() + bcy[r];
      const int px = pacman.round_x() + bcx[r];

      const int ty = 2*py - red_enemy->round_y();
      const int tx = 2*px - red_enemy->round_x();
      target = Position(ty*size, tx*size);
    }
    change_direction(target);
  }
};

struct PinkEnemy : Enemy {
  static constexpr int pink_posy = 14, pink_posx = 13;
  static constexpr int innest_posy = 14, innest_posx = 13;
  static constexpr double nest_wait_time = 5;
  PinkEnemy(const PacMan &pm) : Enemy(pink_posy*size, pink_posx*size, D, pm, innest_posy, innest_posx, nest_wait_time){
    cur_wait_time = nest_wait_time;
    set_state(innest);
  }
  void move() override {
    Position target(-4*size, 2*size); // scatter
    if(get_state() == eaten){ // eaten
      target = Position(nest_posy*size, nest_posx*size);
    }
    else if(get_state() == tonest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == innest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == prepare)
      target = Position(nest_posy*size, nest_posx*size);
    else if(chase_mode){ // chase
      const int r = pacman.get_r();
      const int py = pacman.round_y() + pty[r];
      const int px = pacman.round_x() + ptx[r];
      target = Position(py*size, px*size);
    }
    change_direction(target);
  }
};

struct OrangeEnemy : Enemy {
  static constexpr int oran_posy = 14, oran_posx = 15;
  static constexpr int innest_posy = 14, innest_posx = 15;
  static constexpr double nest_wait_time = 9;
  OrangeEnemy(const PacMan &pm) : Enemy(oran_posy*size, oran_posx*size, U, pm, innest_posy, innest_posx, nest_wait_time){
    cur_wait_time = nest_wait_time;
    set_state(innest);
  }
  void move() override {
    static constexpr int max_dist = 8 * 8; // 最大距離の2乗
    Position target((height+1)*size, 0); // scatter
    const int d = dist(pacman);

    if(get_state() == eaten) // eaten
      target = Position(nest_posy*size, nest_posx*size);
    else if(get_state() == tonest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == innest)
      target = Position(innest_posy*size, innest_posx*size);
    else if(get_state() == prepare)
      target = Position(nest_posy*size, (nest_posx+1)*size);
    else if(d >= max_dist && chase_mode) // chase
      target = pacman;
    
    change_direction(target);
  }
};

struct Game {
  Game() : pacman(){
    enemies[0] = new RedEnemy(pacman);
    enemies[1] = new BlueEnemy(pacman, enemies[0]);
    enemies[2] = new PinkEnemy(pacman);
    enemies[3] = new OrangeEnemy(pacman);
    memcpy(field, first_field_board, sizeof(field));
  }
  ~Game(){
    for(int i = 0; i < 4; i++){
      delete enemies[i];
    }
  }
  void start(){
    if(started) return;

    pacman.start();
    for(auto &enem : enemies) enem->start();
    started = true;
  }
  void reset(){
    printf("reset\n");
    this->~Game();
    pacman = PacMan();
    enemies[0] = new RedEnemy(pacman);
    enemies[1] = new BlueEnemy(pacman, enemies[0]);
    enemies[2] = new PinkEnemy(pacman);
    enemies[3] = new OrangeEnemy(pacman);
    memcpy(field, first_field_board, sizeof(field));

    chase_mode = true;
    gameover = false;

    wait_cnt = 0;
    eat_num = 0;
    adjust_time = 0;
    frightened_start_time = -1;
    current_time = 0;

    last_y = 0, last_x = 0;
    is_ate_dots = false;
    started = false;
    cur_table_pos = 0;
  }
private:
  void change_to_eaten(){
    wait_cnt = eat_cnt;
    pacman.stop();
    for(auto &enem : enemies){
      if(enem->get_state() != eaten){
        enem->stop();
        printf("stopped ");
      }
    }
    eat_num++;
  }
  void change_all_speed(){
    // enemies
    bool c[4] = {};

    if(dots_remain_num <= 10){ // elroy2 dots left
      for(int i = 0; i <= 1; i++){
        enemies[i]->set_speed(85);
        c[i] = true;
      }
    }else if(dots_remain_num <= 20){ // elroy1 dots left
      for(int i = 0; i <= 1; i++){
        enemies[i]->set_speed(80);
        c[i] = true;
      }
    }

    for(int i = 0; i < 4; i++){
      if(enemies[i]->get_state() == eaten) enemies[i]->set_speed(200);
      else if(enemies[i]->is_intunnel()) enemies[i]->set_speed(40);
      else if(enemies[i]->get_state() == frightened) enemies[i]->set_speed(50);
      else if(enemies[i]->get_state() == normal && !c[i]) enemies[i]->set_speed(75);
      else if(enemies[i]->get_state() == tonest) enemies[i]->set_speed(200);
      else if(enemies[i]->get_state() > tonest) enemies[i]->set_speed(40);
    }

    // pacman
    // frightened_modeの場合
    if(frightened_start_time != -1){
      if(is_ate_dots) pacman.set_speed(79);
      else pacman.set_speed(90);
    }else{
      if(is_ate_dots) pacman.set_speed(71);
      else pacman.set_speed(80);
    }
  }
  bool check_is_touch(){
    for(auto &enem : enemies){
      if(enem->is_touch()){
        if(enem->get_state() == normal){
          gameover = true;
          printf("gameover!\n");
        }
        else if(enem->get_state() == frightened){
          // stop, eaten_modeにする
          enem->stop();
          enem->set_state(eaten);
          change_to_eaten();
        }
        return true;
      }
    }
    return false;
  }
  void set_state_enemies(const State st) const{
    for(auto &enem : enemies){
      if(enem->get_state() != eaten && enem->get_state() < tonest){
        enem->set_state(st);
      }
    }
  }
  void reverse_enemies() const{
    for(auto &enem : enemies){
      if(enem->get_state() != eaten && enem->get_state() < tonest){
        enem->reverse();
      }
    }
  }
  // scatter <=> chase
  void change_scmode(){
    chase_mode ^= true;
    if(chase_mode) printf("changed to chase mode\n");
    else printf("changed to scatter mode\n");
    reverse_enemies();
    cur_table_pos++;
  }
  void start_frightened_mode(const double time){
    set_state_enemies(frightened);
    // adjust_timeを増やして時間を止めるため、adjust_timeを保存
    frightened_start_time = adjust_time;
    eat_num = 0;
    reverse_enemies();
    printf("changed to frightened mode\n");
  }
  void end_frightened_mode(){
    frightened_start_time = -1;
    set_state_enemies(normal);
    eat_num = 0;
    printf("return to normal mode\n");
  }
  void move_all(const int r){
    for(auto &enem : enemies){
      enem->move();
    }
    pacman.change_direction(r);
  }
public:
  // Pythonから毎フレーム呼び出される
  // rはキーボードから受け付けた方向
  int update(const double time, const int r){
    if(!started){
      current_time = time;
      adjust_time = time;
      return -1;
    }

    const double dt = time - current_time;
    current_time = time;

    if(frightened_start_time != -1){
      adjust_time += dt;
      // frightened_modeが終わったときor全部食べた時
      if(adjust_time - frightened_start_time >= frightened_time){
        end_frightened_mode();
      }
    }
    // 時間になったらモードの変更をする
    else if(time - adjust_time >= time_table[cur_table_pos]){
      change_scmode();
    }

    move_all(r);

    int res = -1;
    const int y = pacman.round_y();
    const int x = pacman.round_x();
    if(last_y != y || last_x != x) is_ate_dots = false;
    last_y = y; last_x = x;
    // dotsを取った時の処理
    const int v = get_field_val(y, x);
    if(v == dots || v == DOTS){
      set_field_val(y, x, none);
      res = y*width + x;
      dots_remain_num--;
      is_ate_dots = true;
      if(v == DOTS) start_frightened_mode(time);
      if(dots_remain_num == 0){
        // ステージクリア
        printf("cleared!!!\n");
      }
    }

    if(wait_cnt){
      wait_cnt--;
      // 時間を停止する
      frightened_start_time += dt;
    }else{
      pacman.start();
      for(auto &enem : enemies){
        enem->start();
      }
      
      if(frightened_start_time != -1 && eat_num == 4){
        end_frightened_mode();
      }

      if(check_is_touch()) return res;
    }
    change_all_speed();
    // 時間の更新
    for(auto &enem : enemies){
      enem->update_frame(dt);
    }

    return res;
  }
  
  const Enemy &get_enemy(const int i) const{ return *enemies[i]; }
  const PacMan &get_pacman() const{ return pacman; }
private:
  Enemy *enemies[enemies_num];
  PacMan pacman;
  bool started = false;
  // 最後にパックマンがいたマスの場所
  int last_y = 0, last_x = 0;
  bool is_ate_dots = false;
};

Game game;


namespace Python {

int update_frame(double time, int r){
  return game.update(time, r);
}
void start_game(){
  game.start();
}
void reset_game(){
  game.reset();
}
int get_field_value(int y, int x){
  return get_field_val(y, x);
}

// pacman, red,blue,orange,pink = 0,1,2,3,4
// 現在の位置を出力する
int get_posy(int i){
  if(!i) return game.get_pacman().get_y();
  return game.get_enemy(i-1).get_y();
}
int get_posx(int i){
  if(!i) return game.get_pacman().get_x();
  return game.get_enemy(i-1).get_x();
}
int get_rot(int i){
  if(!i) return game.get_pacman().get_r();
  return game.get_enemy(i-1).get_r();
}
int get_state(int i){
  if(!i) return game.get_pacman().get_state();
  const int st = game.get_enemy(i-1).get_state();
  if(st < tonest) return st;
  if(st == tonest) return eaten;
  return normal;
}
bool get_is_stop(int i){
  if(!i) return game.get_pacman().is_stop();
  return game.get_enemy(i-1).is_stop();
}
// frightened_modeの制限時間
double get_limit_time(int i){
  if(!i || game.get_enemy(i-1).get_state() != frightened) return inf;
  return frightened_time - (adjust_time - frightened_start_time);
}
int get_eat_num(){
  return eat_num;
}
bool get_is_game_over(){
  return gameover;
}

} // namespace Python
