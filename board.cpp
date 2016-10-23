#include "Board.hpp"

Board::Board() {
	N = 9;
	M = 9;
	K = 10;
	init = 0;
	booom = 0;
    minesFound=0;
	board = Matrix(N, vector<int>(M));
	bombs = Matrix(N, vector<int>(M));
    //flags = DoubleMatrix(N, vector<double>(M));
    flags = Matrix(N, vector<int>(M));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			board[i][j] = -1;
			bombs[i][j] = 0;
            flags[i][j] = 0;
		}
	}
}

Board::Board(int a,int b,int c) {
    N = a;
    M = b;
    K = c;
    init = 0;
    booom = 0;
    minesFound = 0;
    board = Matrix(N, vector<int>(M));
    bombs = Matrix(N, vector<int>(M));
    //flags = DoubleMatrix(N, vector<double>(M));
    flags = Matrix(N, vector<int>(M));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            board[i][j] = -1;
            bombs[i][j] = 0;
            flags[i][j] = 0;
        }
    }
}

void Board::print() {
    string edge(2*N + 3, '=');
    cout << " |";
    for (int j = 0; j < M; j++) cout << j<<" ";
    cout << endl;
    cout << edge << endl;
    for (int i = 0; i<N; i++) {
        cout << i;
        cout << "|";
        for (int j = 0; j<M; j++) {
            if (board[i][j] == -1) { cout << "* "; }//where is unknown
            else {
                if (board[i][j] == 0) cout << "  ";//where is the interior
                else if (board[i][j] == -2) cout << "x ";//where you lose
                else cout << board[i][j]<<" ";//where is the boudary
            }
        }
        cout << "|" << endl;
    }
    cout << edge << edge << endl;
}

void Board::printFlags() {
    string edge(2*N + 3, '=');
    cout << " |";
    for (int j = 0; j < M; j++) cout << j << " ";
    cout << endl;
    cout << edge << endl;
    for (int i = 0; i<N; i++) {
        cout << i;
        cout << "|";
        for (int j = 0; j<M; j++) {
            cout<<flags[i][j]<<" ";
        }
        cout << "|" << endl;
    }
    cout << edge << endl;
}

void Board::printBomb() {
    string edge(2*N + 3, '=');
    cout << " |";
    for (int j = 0; j < M; j++) cout << j << " ";
    cout << endl;
    cout << edge << endl;
    for (int i = 0; i<N; i++) {
        cout << i;
        cout << "|";
        for (int j = 0; j<M; j++) {
            cout<<bombs[i][j]<<" ";
        }
        cout << "|" << endl;
    }
    cout << edge << endl;
}

bool Board::isEOG() {
    if (booom == 1) return 1;
    int count =0;
    for(int i=0;i<N;i++){
        for(int j=0;j<M;j++){
            if(board[i][j]==-1)count++;
        }
    }
    if (K == count) return 1;
    return 0;
}

void Board::pick(int x, int y) {
    if (!init) initialBombs(x,y);//initialize after first pick

    if (board[x][y] != -1) return;//this place has already been checked

    if (bombs[x][y] == 1) {
        booom = 1;
        board[x][y] = -2;
        return;
    }

    Point p{x,y};
    //if in one fringe ang it has no mines near it then update that neb
    for(int i=0;i<neb.size();i++){
        Neb nebnow = neb[i];
        for(int j=0;j<nebnow.fringe.size();j++){
            if(x==nebnow.fringe[j].x && y==nebnow.fringe[j].y && bombsNearby(nebnow.fringe[i])==0){
                //update boundary and fringe:never erase boundary(boundary is always boundary)
               //cout<<"updating to "<<i+1<<" neb"<<endl;
               // neb[i].fringe.erase(neb[i].fringe.begin()+j);
                nebOf(p,neb[i]);
                //erase interior part and boundary part of fringe
                for(int k=0;k<neb[i].fringe.size();k++){
                    for(int y=0;y<neb[i].boundary.size();y++){
                        if(neb[i].fringe[k].x==neb[i].boundary[y].x && neb[i].fringe[k].y==neb[i].boundary[y].y){
                            neb[i].fringe.erase(neb[i].fringe.begin()+k);
                        }
                    }
                    for(int y=0;y<neb[i].interior.size();y++){
                        if(neb[i].fringe[k].x==neb[i].interior[y].x && neb[i].fringe[k].y==neb[i].interior[y].y){
                            neb[i].fringe.erase(neb[i].fringe.begin()+k);
                        }
                    }
                }
                return;
            }
        }
    }

    //if not in any fringe; then add a new neb
    //cout<<"adding a new neb! num: "<<neb.size()+1<<endl;
    Neb newNeb;
    nebOf(p,newNeb);
    //erase interior part and boundary part of fringe
    for(int i=0;i<newNeb.fringe.size();i++){
        for(int j=0;j<newNeb.boundary.size();j++){
            if(newNeb.fringe[i].x==newNeb.boundary[j].x && newNeb.fringe[i].y==newNeb.boundary[j].y){
                newNeb.fringe.erase(newNeb.fringe.begin()+i);
            }
        }
        for(int j=0;j<newNeb.interior.size();j++){
            if(newNeb.fringe[i].x==newNeb.interior[j].x && newNeb.fringe[i].y==newNeb.interior[j].y){
                newNeb.fringe.erase(newNeb.fringe.begin()+i);
            }
        }
    }
    neb.push_back(newNeb);
    return;
}

void Board::nebOf(Point p, Neb &f) {
    if (bombs[p.x][p.y] != 0) return;//is boom

    //if this is a interior cell
    if (bombsNearby(p) == 0) {
        f.interior.push_back(p);
        flags[p.x][p.y] =2;//interior cells have been checked
        board[p.x][p.y] = 0;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) continue;
                Point np{p.x+i,p.y+j};
                if (np.x < 0 || np.x >= N || np.y < 0 || np.y >= M) continue;
                if (board[np.x][np.y] != -1)continue;
                nebOf(np,f);//recursively search its neighborhood
            }
        }
    }// if is a boundary cell
    else {
        flags[p.x][p.y] =2;//boundary cells have been checked
        f.boundary.push_back(p);
        board[p.x][p.y] = bombsNearby(p);
        //add adjacent unknown cells to fringe
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) continue;
                Point np{p.x+i,p.y+j};
                if (np.x < 0 || np.x >= N || np.y < 0 || np.y >= M) continue;
                if (board[np.x][np.y] != -1)continue;

               //unknown cells near this boundary cell, see if it is already in fringe of this neb
                bool isAlreadyFringe = false;
                for(int k=0;k<f.fringe.size();k++){
                    if(np.x==f.fringe[k].x && np.y==f.fringe[k].y){
                        isAlreadyFringe = true;
                        break;
                    }
                }
                if(!isAlreadyFringe &&  board[np.x][np.y]==-1){//not already in fringe and in unknown
                    f.fringe.push_back(np);
                }
            }
        }
    }
}

int Board::bombsNearby(Point &p) {
    int sum= 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) continue;
            Point np{ p.x + i,p.y + j };
            if (np.x < 0 || np.x >= N || np.y < 0 || np.y >= M) continue;
            if (bombs[np.x][np.y] == 1) sum++;
        }
    }
    return sum;
}

void Board::initialBombs(int x, int y) {//called only once, make sure the first pick is not a mine
    init = 1;
    srand(clock());
    int T = N*M;
    int count = 0;
    while(count<K){
        int t = rand() % T;
        int row = t/M;
        int col = t-row*M;
        if (x == row && y == col) continue;//if it is what's picked, skip
        if (bombs[row][col] == 1) continue;//if it is already a mine, skip
        bombs[row][col] = 1;
        count++;
    }
}

int Board::winOrLoss() {
    if (isEOG()==1 && booom == 1) return -1;
    if (isEOG()==1 && booom != 1) return 1;
    else return 0;
}

NebSet Board::getNeb() {
    return neb;
}

Matrix Board::getBoard() {
    return board;
}


