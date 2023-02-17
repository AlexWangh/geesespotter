#include "geesespotter_lib.h"
#include "geesespotter.h"
#include <iostream>

char *createBoard(std::size_t xdim, std::size_t ydim){
    int i;
    char *a=new char [xdim*ydim];
    for(i=0;i<xdim*ydim;i++){
        *(a+i)=0;
    } // A loop that makes the value of each box of the board to be 0.
    return a;
}

//A function that transfers the location of each box from one-dimension to two-dimension.
void one_to_two_dim(std::size_t xdim,std::size_t ydim, std::size_t one_dim, int &c, int &r){
        c=one_dim%xdim;
        r=one_dim/xdim;
}

//A function that transfers the location pf each box from two-dimension to one-dimension.
int two_to_one_dim(std::size_t xdim,std::size_t ydim, int c, int r){
        int one_dim =r*xdim+c;
        return one_dim;
}



void computeNeighbors(char *board, std::size_t xdim, std::size_t ydim){
    int i,k,c=0,r=0,rr,cc,result,neibors=0;

    /*
    Compare with the box in the middle,
    dr means the change of value of its neighbour's two-dimensional location of row.
    dc means the change of value of its neighbour's two-dimensional location of column.
    */

    int dr[]={-1,-1,-1,0,0,1,1,1};
    int dc[]={-1,0,1,-1,1,-1,0,1};

    for (i=0;i<xdim*ydim;i++){
        one_to_two_dim(xdim,ydim, i, c,r);

        if (board[i]==0){
            neibors=0;

            //This loop calculates the number of geese in its neighbours, which is the value of neighbors.

            for(int k=0;k<8;k++){

                rr=r+dr[k];
                cc=c+dc[k];

                if(rr<0 || cc<0){
                    continue;   //if its neighbor's coordinate of row or column is less than 0, it means it is out of boundary,
                }               //which will not be considered.
                if (rr>=ydim || cc>=xdim){
                    continue;   //if its neighbor's coordinate of row or column is more than ydim or xdim respectively, it means it is out of boundary,
                }               //which will not be considered.

                result=two_to_one_dim(xdim,ydim,cc,rr);
                if (board[result]==9){
                    neibors=neibors+1;
                }
            }
            board[i]=neibors;
        }
    }

}

void hideBoard(char *board, std::size_t xdim, std::size_t ydim){
    int i;
    //hide each value.
    for (i=0;i<xdim*ydim;i++){
        *(board+i)=*(board+i)+hiddenBit();
    }
}

void cleanBoard(char *board){
    delete []board;
}


void printBoard(char *board, std::size_t xdim, std::size_t ydim){
    int i;
    for (i=0;i<xdim*ydim;i++){

        if ( (*(board+i)&markedBit()) != 0){
            std::cout<<"M"; //If it is marked, print "M".
        }
        else if ((*(board+i)&hiddenBit()) !=0){
            std::cout<<"*";//If it is hidded, print "*".
        }
        else{
            std::cout<< (char) ('0'+(*(board+i)&valueMask()));
        }                   //If it is neither marked nor hidded, print the value itself.

        if ((i+1)%xdim==0){
            std::cout<<std::endl;
        }
    }

}

int reveal(char * board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
    int loc,rr,cc,k;
    loc=two_to_one_dim(xdim, ydim, xloc, yloc);

    //Same as the usages of dr and dc in the function "computeNeighbors".
    int dr[]={-1,-1,-1,0,0,1,1,1};
    int dc[]={-1,0,1,-1,1,-1,0,1};

    if (((board[loc]>>4)& 3)==3){
        return 1;
    }
    else if (((board[loc]>>4)& 3)==0){
        return 2;
    }
    else if (((board[loc]>>4)& 3)==2){
        if ((board[loc]& valueMask()) == 9){

            board[loc]=board[loc]-32;
            return 9;
        }
        else if ((board[loc]& valueMask())==0){
            board[loc]=board[loc]-32; 
            for (k=0;k<8;k++){
                rr=yloc+dr[k];//The value of the row of the neibour.
                cc=xloc+dc[k];//The value of the column of the neibour.

                if(rr<0 || cc<0){
                    continue;
                }   //if its neighbor's coordinate of row or column is less than 0, it means it is out of boundary,
                    //which will not be considered.
                

                if (rr>=ydim || cc>=xdim){
                    continue;
                }   //if its neighbor's coordinate of row or column is more than ydim or xdim respectively, it means it is out of boundary,
                    //which will not be considered.

                //For calculating the neibour's neibours, we need to call this function itself in the for loop.
                reveal(board,xdim,ydim,cc,rr);
            }
            return 0;
        }

        else{
            board[loc]=board[loc]-32;
            return 0;
        }
    }
}

int mark(char * board, std::size_t xdim, std::size_t ydim, std::size_t xloc, std::size_t yloc){
    int loc;
    loc=xdim*yloc+xloc;

    //If the box is revealed, it cannot be marked.
    if ( (board[loc]&hiddenBit()) ==0){
        return 2;
    }

    
    else if((board[loc]&markedBit())==0){
        board[loc]|=markedBit();
        return 0;
    }//If the value is unmared, mark it.
    else{
        board[loc]=board[loc]-markedBit();
        return 0;
    }//If the value is marked, unmark it.
}

bool isGameWon(char * board, std::size_t xdim, std::size_t ydim){
    int i,geese_num=0,unmarked=0;

    //If the number of geese equals to the number of boxes that is marked, also, hidded and marked respectively, the game is won.
    //Else, the game is not won.
    for (i=0;i<xdim*ydim;i++){

        if ((board[i]&valueMask())==9){
            geese_num=geese_num+1;
        }//This calculates the number of geese.

         if( (board[i]&hiddenBit())==hiddenBit()){
            unmarked=unmarked+1;
        }//This calculates the number of boxes that is unmarked.
    }

    if ( (geese_num==unmarked) ){
        return 1;
    }
    else {
        return 0;
    }
}
