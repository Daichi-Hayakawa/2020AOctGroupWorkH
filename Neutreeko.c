#include <stdio.h>
#include <math.h>
#include <string.h>

#define MAX_DEPTH 4
#define MAX_MOVE_N 24
#define MOVE_STR_N 5
#define BOARD_LEN 5
#define MAX_EVALUATION_V 10000
#define BLACK -1
#define WHITE 1

void getBoard(const char[MOVE_STR_N], const int inputBoard[BOARD_LEN][BOARD_LEN],
     int outputBoard[BOARD_LEN][BOARD_LEN]);
void getMove(const int board[BOARD_LEN][BOARD_LEN], char move[MOVE_STR_N]);
void getMoveList(const int board[BOARD_LEN][BOARD_LEN],
     const int player, char moveList[MAX_MOVE_N][MOVE_STR_N]);
int getEvaluationValue(const int board[BOARD_LEN][BOARD_LEN], int);
int hasWon(const int board[BOARD_LEN][BOARD_LEN]);

int max(int i, int j){ return i > j ? i: j;}
int min(int i, int j){ return i > j ? j : i;}
int abs(int x) {return (x < 0)? -x: x;}

void getBoard(const char input[5], const int board[BOARD_LEN][BOARD_LEN], int outputBoard[BOARD_LEN][BOARD_LEN])
{
	// parse the input string
	// for rows: 1 -> 4, 2 -> 3, 3 -> 2, 4 -> 1, 5 -> 0
	// for columns: A -> 0, B -> 1, C -> 2, D -> 3, E -> 4
	int currR = BOARD_LEN - 1 - input[0] + 49;
	int currC = input[1] - 65;
	int nextR = BOARD_LEN - 1 - input[2] + 49;
	int nextC = input[3] - 65;
	
	// fill new placements to outputBoard
	for (int row = 0; row < BOARD_LEN; row++)
	{
		for (int col = 0; col < BOARD_LEN; col++)
		{
			if ((row == currR) && (col == currC))
			{
				outputBoard[row][col] = 0;
			}
			else if ((row == nextR) && (col == nextC))
			{
				outputBoard[row][col] = board[currR][currC];
			}
			else
			{
				outputBoard[row][col] = board[row][col];
			}
		}
	}
	
	return;
}

int alphabeta(const int board[BOARD_LEN][BOARD_LEN], int turn, int aiColor ,int depth,
     int alpha, int beta, char outputMove[]){
    //alphabeta法に基づくAI
    //
    //board 現在の盤面
    //turn 現在の手番の人の石の色
    //aiColor AIの石の色
    //depth 現在のゲーム木の深さ。根は0
    //alpha, beta alphabeta法の引数
    //outputMove 出力する手の文字列
    if(hasWon(board))
         return MAX_EVALUATION_V * (turn * aiColor);
    if(depth == MAX_DEPTH) //ノードが葉
         return getEvaluationValue(board, aiColor);
    char moveList[MAX_MOVE_N][BOARD_LEN] = {""};
    getMoveList(board, turn, moveList);

    alpha = aiColor == turn ? -MAX_EVALUATION_V : alpha;
    beta = aiColor == turn ? beta : MAX_EVALUATION_V;

    int ansIndex = 0;
    int ansV = MAX_EVALUATION_V * turn * (-1);

    for(int i = 0; moveList[i][0] !='\0' && i < MAX_MOVE_N; i++){
        int nextBoard[BOARD_LEN][BOARD_LEN];
        getBoard(moveList[i], board, nextBoard);

        int v = alphabeta(nextBoard, -1 * turn, aiColor, depth + 1, 
                        alpha , beta, outputMove);// ノードの評価値

        if(turn == aiColor){
            if(v >= beta){//beta切り
                ansIndex = i;
                ansV = v;
                break;
            }else if(ansV < v){//より良い値が見つかったらalpha更新
                ansIndex = i;
                ansV = v;
                alpha = v;
            }
        }else{
            if(v <= alpha){//alpha切り
                ansIndex = i;
                ansV = v;
                break;
            }else if (ansV > v){//より良い値が見つかったらbeta更新
                ansIndex = i;
                ansV = v;
                beta = v;                
            }
        }
    }
    if(depth == 0) strcpy(outputMove, moveList[ansIndex]);//根なら次の手を文字化して出力
    return ansV;
}

void getMove(const int board[BOARD_LEN][BOARD_LEN], int ai, char move[MOVE_STR_N]){
    //board ... 盤面
    //player ...AIの色（黒石か白石か）
    //move ...出力する指し手
    alphabeta(board, ai, ai, 0, -MAX_EVALUATION_V, MAX_EVALUATION_V, move);
}

void getMoveList(const int board[BOARD_LEN][BOARD_LEN], const int player, char moveList[MAX_MOVE_N][MOVE_STR_N]){
    //board ...盤面
    //player どっちの手番か　黒の番なら-1, 白の番なら1
    //moveList ...可能な指し手のリストの出力、可能な手が24個以上無いときはNUll文字列で埋める
}

int getPieceN(const int board[BOARD_LEN][BOARD_LEN], int direction, int color, int index){
    //boardのある行（列, 斜め方向）にcolorの石が何個あるかを返す
    //directionは0が縦, 1が横, 2が斜め(左上から右下）, 3が斜め（右上から左下）に対応
    //斜めのindexはdirection=2のとき、(0, 4)=0 (0, 0)=4, (4, 0) = 8
    //            direction=3のとき、(0, 0)=0 (0, 4) = 4 (4, 4)=8
    int res = 0;
    if(direction ==0){
        for(int i0 = 0; i0 < BOARD_LEN; i0++){
            if(board[i0][index] == color) res++;
        }
    }else if(direction == 1){
        for(int i1 = 0; i1 < BOARD_LEN; i1++){
            if(board[index][i1] == color) res++;
        }
    }else if(direction == 2){
        int i2, j2;
        i2 = max(index - BOARD_LEN + 1, 0);
        j2 = max(BOARD_LEN -1 -index, 0);
        for(; i2 < BOARD_LEN && j2 < BOARD_LEN;){
            if(board[i2++][j2++] == color) res++;
        }
    }else{
        int i3, j3;
        i3 = max(index-BOARD_LEN+1, 0);
        j3 = min(index, BOARD_LEN-1);
        for(; i3 < BOARD_LEN && j3 >= 0;){
            if(board[i3++][j3--] == color)res++;
        }
    }
    return res;
}
int getConnectedPieceN(const int board[BOARD_LEN][BOARD_LEN], int isCorrumn, int index, int color){
    //colorの石が２個以上ある行（列）で自分の石が繋がっているほどよい値を返す
    int res = 0;
    if(isCorrumn){
        for(int i = 0; i < BOARD_LEN-1; i++){
            res += board[i][index] * board[i + 1][index];
        }
    }
    else{
        for(int i = 0; i < BOARD_LEN -1; i++){
            res += board[index][i] * board[index][i+1];
        }
    }
    return res;
}

int getEvaluationValue(const int board[BOARD_LEN][BOARD_LEN], int aiColor){
    //board ...盤面
    //出力は評価値
    int res = 0;
    for(int i = 0; i < 2; i ++){ //縦横で石がどうなっているか
        for(int j = 0; j < BOARD_LEN; j++){ 
            int ai = getPieceN(board, i, aiColor, j);
            int human = getPieceN(board, i, -1*aiColor, j);
            res += pow(10, ai) - pow(10, human);
            if(ai > 1) res += getConnectedPieceN(board, i, j, aiColor) * 10;
            if(human > 1) res -= getConnectedPieceN(board, i, j, -1*aiColor) * 10;
        }
    }       
    for(int i = 2; i < 4; i ++){ //斜めで
        for(int j = 2; j < 7; j++){ 
            int ai = getPieceN(board, i, aiColor, j);
            int human = getPieceN(board, i, -1*aiColor, j);
            res += pow(10, ai) - pow(10, human);
        }
    }    
    return res;
}


int hasWon(const int board[BOARD_LEN][BOARD_LEN])
{
	// check pieces on the edge
	int row5 = 0, row1 = 0, colA = 0, colE = 0;
	// maintain 4 sliding windows with max length 3
	// return 1 if any abs of a sum equals 3
	int leftIdx = 0, rightIdx = 0;
	while (rightIdx < BOARD_LEN)
	{
		if (rightIdx - leftIdx > 2)
		{
			row5 -= board[0][leftIdx];
			row1 -= board[BOARD_LEN - 1][leftIdx];
			colA -= board[leftIdx][0];
			colE -= board[leftIdx][BOARD_LEN - 1];
			leftIdx++;
		}
		row5 += board[0][rightIdx];
		row1 += board[BOARD_LEN - 1][rightIdx];
		colA += board[rightIdx][0];
		colE += board[rightIdx][BOARD_LEN - 1];

		if (abs(row5) == 3 || abs(row1) == 3 || abs(colA) == 3 || abs(colE) == 3) return 1;

		rightIdx++;
	}
	
	// check pieces inside
	// return 1 if any pieces accomplished the objective
	for (int row = 1; row < BOARD_LEN - 1; row++)
	{
		for (int col = 1; col < BOARD_LEN - 1; col++)
		{
			if (!board[row][col]) continue;
			// horizontally got 3 same pieces connected
			if (board[row][col] == board[row][col - 1] && board[row][col] == board[row][col + 1]) return 1;
			// vertically got 3 same pieces connected
			if (board[row][col] == board[row - 1][col] && board[row][col] == board[row + 1][col]) return 1;
			// right diagonally got 3 same pieces connected
			if (board[row][col] == board[row - 1][col - 1] && board[row][col] == board[row + 1][col + 1]) return 1;
			// left diagonally got 3 same pieces connected
			if (board[row][col] == board[row - 1][col + 1] && board[row][col] == board[row + 1][col - 1]) return 1;
		}
	}
	
	return 0;
}

int main(){

    return 0;
}