#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>
#include <string.h>

#define AREA_ROW 21
#define AREA_COL 10
#define BLOCK_ROW 4
#define BLOCK_COL 4

#define CENTER 4

#define LEFT 75
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACEBAR 32

enum {
    BLACK,
    DARK_BLUE,
    DARK_GREEN,
    DARK_SKYBLUE,
    DARK_RED,
    DARK_VOILET,
    DARK_YELLOW,
    GRAY,
    DARK_GRAY,
    BLUE,
    GREEN,
    SKYBLUE,
    RED,
    VIOLET,
    YELLOW,
    WHITE,
};

int area[AREA_ROW + 1][AREA_COL + 6];
int now_area[AREA_ROW + 1][AREA_COL + 6];

int block[7][BLOCK_ROW][BLOCK_COL] = {
    {
        {0,0,0,0},  //SKYBLUE
        {1,1,1,1},
        {0,0,0,0},
        {0,0,0,0},
    } ,
    {
        {0,0,0,0},  //YELLOW
        {0,2,2,0},
        {0,2,2,0},
        {0,0,0,0},
    },
    {
        {0,0,0,0},  //RED
        {3,3,0,0},
        {0,3,3,0},
        {0,0,0,0},
    },
    {
        {0,0,0,0},  //GREEN
        {0,4,4,0},
        {4,4,0,0},
        {0,0,0,0},
    },
    {
        {0,0,0,0},  //BLUE
        {5,0,0,0},
        {5,5,5,0},
        {0,0,0,0},
    },
    {
        {0,0,0,0},  //DARK_YELLOW
        {0,0,6,0},
        {6,6,6,0},
        {0,0,0,0},
    },
    {
        {0,0,0,0},  //DARK_PURPLE
        {0,7,0,0},
        {7,7,7,0},
        {0,0,0,0},
    }
};

//점수 관련 변수
int cnt = 0;
int score = 0;


int now_block[BLOCK_ROW][BLOCK_COL];
int fast_block[BLOCK_ROW][BLOCK_COL];

int hard_y = 0, hard_x = 0;

int r;
int next_r;
struct rank
{
    char name[20];
    int sco;
}typedef rank;
rank List[11];

void rotate_block();      //블록 회전
int is_crash(int y, int x);           //내려갈 때 충돌판정
int is_FullLine();     //한줄완성판별
int down_block(int y, int x);        //블록내리기
int input();       //사용자입력
int create_block();     //랜덤으로 블록생성
void print_area(int y, int x);      //화면출력
void area_init(int y, int x);//화면 초기화
void area_change();     //옳은 값이면 과거를 현재로바꾸기
void r_area_change();      //잘못된 값이면 현재를 옳은값인 과거로 바꾸기
int block_side(int y, int x, int lr);    //블록 옆으로이동
void block_change(int r);
void block_change2();
void r_block_change();
int line_down(int d);
int is_GameOver();
void create_area();
int change_Ranking();
int print_Ranking();
int hard_drop(int y, int x);
int color(int code);
void gotoxy(int x, int y);
void print(int j, int i, int c);
void print_init(int y, int x);
void drop_point(int y, int x);
void drop_point_init(int y, int x);
void print_all();

int main()
{
    srand(time(NULL));
    next_r = rand() % 7;
    create_area();
    create_block();
}

int color(int code)
{
    switch (code)
    {
    case 1:
        return SKYBLUE;
    case 2:
        return YELLOW;
    case 3:
        return RED;
    case 4:
        return GREEN;
    case 5:
        return DARK_BLUE;
    case 6:
        return DARK_YELLOW;
    case 7:
        return VIOLET;
    }
}

void gotoxy(int x, int y) {
    COORD pos = { x,y }; //x, y 좌표 설정
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos); //커서 설정
}

void print(int j, int i, int c)
{
    if (i == 0)
        return 0;
    gotoxy(j - 2, i + 8);	// 한줄씩 시작지점을 아래로
    switch (c)
    {
    case 0:
        printf("□");
        break;
    case -1:
        break;
    default:
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color(area[i][j / 2]));
        printf("■");
        break;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
    gotoxy(AREA_COL + 20, AREA_ROW + 7);
}


void area_init(int y, int x)
{
    for (int i = y; i < BLOCK_ROW + y; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {

            if (area[i][j + CENTER + x] == now_block[i - y][j])
            {
                area[i][j + CENTER + x] = 0;
            }
        }
    }
}

void drop_point(int y, int x)
{
    for (int i = 0; i < AREA_ROW; ++i)
        for (int j = 0, j2 = 0; j < AREA_COL + 2; ++j)
        {
            if (y <= i && i <= y + BLOCK_ROW - 1 && x + CENTER <= j && j <= x + CENTER + BLOCK_COL - 1)
                if (now_block[i - y][j - CENTER - x] && now_area[i][j] == 0)
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color(r + 1));
                    print(j2, i, 0);
                }
            j2 += 2;
            // 2byte(두칸)짜리로 출력하기 때문에 x 좌표는 2칸씩 이동시켜야함
        }
}

void drop_point_init(int y, int x)
{
    for (int i = 0; i < AREA_ROW; ++i)
        for (int j = 0, j2 = 0; j < AREA_COL + 2; ++j)
        {
            if (y <= i && i <= y + BLOCK_ROW - 1 && x + CENTER <= j && j <= x + CENTER + BLOCK_COL - 1)
                if (now_block[i - y][j - CENTER - x])
                {
                    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
                    print(j2, i, 0);
                }
            j2 += 2;
            // 2byte(두칸)짜리로 출력하기 때문에 x 좌표는 2칸씩 이동시켜야함
        }
}

void print_init(int y, int x)
{
    for (int i = 0; i < AREA_ROW; ++i)
        for (int j = 0, j2 = 0; j < AREA_COL + 2; ++j)
        {
            if (y <= i && i <= y + BLOCK_ROW - 1 && x + CENTER <= j && j <= x + CENTER + BLOCK_COL - 1)
                if (now_block[i - y][j - CENTER - x])
                    print(j2, i, 0);
            j2 += 2;
            // 2byte(두칸)짜리로 출력하기 때문에 x 좌표는 2칸씩 이동시켜야함
        }
}
void print_area(int y, int x)
{
    for (int i = 0; i < AREA_ROW; ++i)
        for (int j = 0, j2 = 0; j < AREA_COL + 2; ++j)
        {
            if (y <= i && i <= y + BLOCK_ROW - 1 && x + CENTER <= j && j <= x + CENTER + BLOCK_COL - 1)
                print(j2, i, area[i][j]);
            j2 += 2;
            // 2byte(두칸)짜리로 출력하기 때문에 x 좌표는 2칸씩 이동시켜야함
        }
}
void print_all()
{
    system("cls");
    printf("score : %d\n\n\n", score);
    printf("next block\n");
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            if (block[next_r][i][j])
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color(block[next_r][i][j]));
                printf("■");
            }
            else
                printf("  ");
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 1; i < AREA_ROW; i++)
    {
        for (int j = 0; j < AREA_COL + 2; j++)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
            if (now_area[i][j] == 0)
            {
                printf("□");
            }
            else if (now_area[i][j] != -1)
            {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color(now_area[i][j]));
                printf("■");
            }
        }
        printf("\n");
    }
}

int hard_drop(int y, int x)
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            area[y + i][x + j + CENTER] += now_block[i][j];
        }
    }
    if (is_crash(y, x) == 1)
    {
        r_area_change();
        hard_y = y - 1, hard_x = x;
        drop_point(hard_y, hard_x);
        return 0;
    }
    area_init(y, x);
    hard_drop(y + 1, x);
}

void create_area()
{
    for (int i = 0; i < AREA_ROW + 1; i++)
    {
        for (int j = 0; j < AREA_COL + 2; j++)
        {
            if (j == 0 || j == AREA_COL + 1)      //양옆 맨 끝 9로 설정
            {
                area[i][j] = -1, now_area[i][j] = -1;
            }
            else if (i == AREA_ROW)     //맨 밑 1로설정 -> 충돌판정으로 블록멈추기
            {
                area[i][j] = 1, now_area[i][j] = 1;
            }
            else        //나머지 0
            {
                area[i][j] = 0, now_area[i][j] = 0;
            }
        }
    }
}
int create_block()
{
    cnt = 0;
    area_change();
    is_FullLine();
    area_change();
    r = next_r;
    next_r = (rand() % 7);   //랜덤으로 7개 블록 중 하나 결정
    while (next_r == r)
        next_r = rand() % 7;
    block_change(r);
    print_all();
    hard_drop(0, 0);
    return down_block(0, 0);     //블록내리기
}




int down_block(int y, int x)
{
    gotoxy(0, 0);
    printf("score : %d\n\n\n", score);
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            area[i+y][j + CENTER + x] += now_block[i][j];
        }
    }
    if (is_crash(y, x) == 1)   //밑에 블록이있으면
    {
        if (is_GameOver() == -1)
        {
            area_change();
            change_Ranking();
            abort();    //프로그램 종료
        }
        r_area_change();
        print_area(y - 1, x);
        return create_block();  //다음블록
    }

    //올바른값이면
    area_change();      //과거를 현재로 변경
    print_area(y, x);
    drop_point(hard_y, hard_x);
    time_t start_time = time(NULL);
    while (1)
    {
        int ans = input();      //사용자입력받기
        if (ans == 1 || ans == -1)
        {
            area_change();
            area_init(y, x);     //초기화
            int a = block_side(y, ans + x, ans);  //s값 리턴받기
            if (x != a) //위치 변할 때만 ->불필요한 출력 제거
            {
                drop_point_init(hard_y, hard_x);
                print_init(y, x);
                x = a;
                area_init(y, x);
                hard_drop(y, x);
                print_area(y, x);
            }
            drop_point(hard_y, hard_x);
        }
        else if (ans == 2)
        {
            area_change();
            area_init(y, x);
            drop_point_init(hard_y, hard_x);
            block_change2();
            rotate_block();
            for (int i = 0; i < BLOCK_ROW; i++)
            {
                for (int j = 0; j < BLOCK_COL; j++)
                {
                    area[i+y][j + CENTER + x] += now_block[i][j];
                }
            }
            if (is_crash(y, x))
            {
                r_area_change();
                r_block_change();
                drop_point(hard_y, hard_x);
            }
            else
            {
                //올바른값이면
                area_change();      //과거를 현재로 변경
                area_init(y, x);
                print_init(y, x);
                hard_drop(y, x);
                print_area(y, x);
                drop_point(hard_y, hard_x);
            }
        }
        else if (ans == 3)
        {
            score += 1;
            area_init(y, x);   //초기화
            print_init(y, x);
            return down_block(y + 1, x);     //d+1로 한칸씩 내리기
        }
        else if (ans == 4)
        {
            score += hard_y - y;
            area_init(y, x);
            print_init(y, x);
            for (int i = 0; i < BLOCK_ROW; i++)
            {
                for (int j = 0; j < BLOCK_COL; j++)
                {
                    area[hard_y + i][x + CENTER + j] += now_block[i][j];
                }
            }
            area_change();
            print_area(hard_y, hard_x);
            create_block();
        }
        time_t now_time = time(NULL);
        if (now_time - start_time >= 1)     //1초동안
        {
            break;
        }
    }
    //올바른값이면
    area_change();      //과거를 현재로 변경
    area_init(y, x);   //초기화
    print_init(y, x);
    score += 1;
    return down_block(y + 1, x);     //y+1로 한칸씩 내리기
}

int block_side(int y, int x, int lr)
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            area[i+y][j + CENTER + x] += now_block[i][j];
        }
    }

    if (is_crash(y, x))    //양옆끝 또는 블록에 닿았는지 확인
    {
        r_area_change();    //닿으면 이전값으로 변경
        return x + (lr * -1);       //(lr*-1) : 오른쪽 키 눌렀으면 -1 왼쪽키 눌렀으면 +1
    }
    else
    {
        area_change();  //아니면 과거를 현재로 바꿔주기
        print_area(y, x);   //출력
        return x;
    }
}


int is_crash(int y, int x)
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            if (now_block[i][j] == 0)   //블록이 없는 위치면 다음으로
                continue;
            if (area[i + y][j + CENTER + x] > now_block[i][j])  //블록과 충돌했을 때
            {
                return 1;
            }
            else if (area[i + y][j + CENTER + x] < now_block[i][j]) //양쪽 끝 벽에 충돌했을 때
            {
                return 2;
            }
        }
    }
    return 0;
}


int input()
{
    int nSelect = 0;
    if (kbhit()) {
        nSelect = getch();
        if (nSelect == 224) {
            nSelect = getch();
            switch (nSelect)
            {
            case LEFT:
                return -1;
            case RIGHT:
                return 1;
            case UP:
                return 2;
            case DOWN:
                return 3;
            }
        }
        else if (nSelect == SPACEBAR)
            return 4;
    }
    return 0;
}

void area_change()
{
    for (int i = 0; i < AREA_ROW + 1; i++)
    {
        for (int j = 0; j < AREA_COL + 2; j++)
        {
            now_area[i][j] = area[i][j];
        }
    }
}

void r_area_change()
{
    for (int i = 0; i < AREA_ROW + 1; i++)
    {
        for (int j = 0; j < AREA_COL + 2; j++)
        {
            area[i][j] = now_area[i][j];
        }
    }
}

void block_change(int r)
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            now_block[i][j] = 0;
            now_block[i][j] = block[r][i][j];
        }
    }
}

void block_change2()
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            fast_block[i][j] = now_block[i][j];
        }
    }
}
void r_block_change()
{
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            now_block[i][j] = fast_block[i][j];
        }
    }
}

void rotate_block()
{
    int tmp[BLOCK_ROW][BLOCK_COL];
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            tmp[i][j] = now_block[i][j];
        }
    }
    for (int i = 0; i < BLOCK_ROW; i++)
    {
        for (int j = 0; j < BLOCK_COL; j++)
        {
            now_block[i][j] = tmp[BLOCK_COL - 1 - j][i];
        }
    }
}

int is_FullLine()
{
    int j;
    int empty_cnt;
    for (int i = AREA_ROW - 1; i >= 0; i--)
    {
        empty_cnt = 0;  //빈 칸 개수
        for (j = 1; j < AREA_COL + 1; j++)
        {
            if (area[i][j] == 0)
            {
                empty_cnt++;
            }
        }
        if (empty_cnt == 0) //빈 칸 없을 때
        {
            cnt++;
            for (j = 1; j < AREA_COL + 1; j++)
            {
                score += 100 * cnt;
                return line_down(i);
            }
        }
    }
}

int line_down(int y)
{
    for (int j = 1; j < AREA_COL + 1; j++)  //채워진 줄 초기화
    {
        area[y][j] = 0
    }

    for (int i = y; i > 0; i--)
    {
        for (int j = 1; j < AREA_COL + 1; j++)
        {
            area[i][j] = area[i - 1][j];
        }
    }
    return is_FullLine();
}


int is_GameOver()
{
    for (int i = 1; i < AREA_COL + 1; i++)
    {
        if (area[1][i])
        {
            print_all();
            printf("game over\n");
            return -1;
        }
    }
}

int change_Ranking()
{
    FILE* fp = fopen("ranking.txt", "r");
    char str[100];
    int i, acc = 0, lines = 0, Lbegin[20] = { 0 };
    while (!feof(fp))
    {
        if (fgets(str, sizeof(str), fp) != NULL)
        {
            if (lines > 0)
            {
                Lbegin[lines] = acc;
            }
            acc += strlen(str) + 1;
            if (lines % 2 == 0)
            {
                strcpy(List[10 - (lines / 2)].name, str);
            }
            else
            {
                List[10 - (lines / 2)].sco = atoi(str);
            }
            lines++;
        }
    }

    List[0].sco = score;
    printf("이름을 입력하세요 : ");
    scanf(" %s", List[0].name);
    strcat(List[0].name, "\n");
    if (List[0].sco > List[1].sco)
    {
        for (int i = 0; i < 11 - 1; i++)
        {
            for (int j = 0; j < 11 - 1 - i; j++)
            {
                if (List[j].sco > List[j + 1].sco)
                {
                    int tmp;
                    tmp = List[j].sco, List[j].sco = List[j + 1].sco, List[j + 1].sco = tmp;
                    char stmp[20];
                    strcpy(stmp, List[j].name), strcpy(List[j].name, List[j + 1].name), strcpy(List[j + 1].name, stmp);
                }
            }
        }
    }
    fclose(fp);


    FILE* fw = fopen("ranking.txt", "w");
    for (int i = 10; i > 0; i--)
    {
        char ssco[100];
        sprintf(ssco, "%d", List[i].sco);
        fprintf(fw, "%s%s\n", List[i].name, ssco);
    }
    fclose(fw);
    print_Ranking();
}


int print_Ranking()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
    printf("\n\n\n");
    FILE* fp = fopen("ranking.txt", "r");
    char str[100];
    int i, acc = 0, lines = 0, Lbegin[20] = { 0 };
    while (!feof(fp))
    {
        if (fgets(str, sizeof(str), fp) != NULL)
        {
            if (lines > 0)
            {
                Lbegin[lines] = acc;
            }
            acc += strlen(str) + 1;
            if (lines % 2 == 0)
            {
                printf("\n%d위\n", lines / 2 + 1);
            }
            printf("%s", str);
            lines++;
        }
    }
    return 0;
}
