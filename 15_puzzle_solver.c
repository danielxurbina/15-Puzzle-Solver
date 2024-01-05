#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 4
#define NxN (N*N)
#define TRUE 1
#define FALSE 0

struct node {
    int tiles[N][N];
    int f, g, h;
    short zero_row, zero_column; /* location (row and colum) of blank tile 0 */
    struct node *next;
    struct node *parent; /* used to trace back the solution */
};

int goal_rows[NxN];
int goal_columns[NxN];
struct node *start,*goal;
struct node *open = NULL, *closed = NULL;
struct node *succ_nodes[4];

// This function prints the contents of a node
void print_a_node(struct node *pnode) {
    int i,j;
    for (i=0;i<N;i++) {
        for (j=0;j<N;j++)
            printf("%2d ", pnode->tiles[i][j]);
        printf("\n");
    }
    printf("\n");
}

// This function initializes the initial state and the goal state.
struct node *initialize(char **argv){
    int i,j,k,index, tile;
    struct node *pnode;
    pnode=(struct node *) malloc(sizeof(struct node));
    index = 1;

    for (j=0;j<N;j++)
        for (k=0;k<N;k++) {
            tile=atoi(argv[index++]);
            pnode->tiles[j][k]=tile;
            if(tile==0) {
                pnode->zero_row=j;
                pnode->zero_column=k;
            }
        }
    pnode->f=0;
    pnode->g=0;
    pnode->h=0;
    pnode->next=NULL;
    pnode->parent=NULL;
    start=pnode;
    printf("initial state\n");
    print_a_node(start);
    pnode=(struct node *) malloc(sizeof(struct node));
    goal_rows[0]=3;
    goal_columns[0]=3;
    for(index=1; index<NxN; index++){
        j=(index-1)/N;
        k=(index-1)%N;
        goal_rows[index]=j;
        goal_columns[index]=k;
        pnode->tiles[j][k]=index;
    }
    pnode->tiles[N-1][N-1]=0; /* empty tile=0 */
    pnode->f=0;
    pnode->g=0;
    pnode->h=0;
    pnode->next=NULL;
    goal=pnode;
    printf("goal state\n");
    print_a_node(goal);
    return start;
}

// Helper function to create a copy of a node and return the copy
struct node* copy_node(struct node *src){
    struct node *copyNode = (struct node *) malloc(sizeof(struct node));
    memcpy(copyNode->tiles, src->tiles, NxN*sizeof(int));
    copyNode->f = src->f;
    copyNode->g = src->g;
    copyNode->h = src->h;
    copyNode->zero_row = src->zero_row;
    copyNode->zero_column = src->zero_column;
    copyNode->parent = src->parent;
    copyNode->next = NULL;
    return copyNode;
}

// This function merges the remaining nodes pointed by succ_nodes[] into the open list.
// It inserts nodes based on their f values and keeps the f values sorted.
void merge_to_open() {
    int i;
    for(i = 0; i < N; i++){
        if(succ_nodes[i] == NULL) continue;
        struct node *newNode = copy_node(succ_nodes[i]);
        if(open == NULL || newNode->f < open->f){
            newNode->next = open;
            open = newNode;
        } 
        else {
            struct node *temp = open;
            while(temp->next != NULL && newNode->f >= temp->next->f){
                temp = temp->next;
            }
            newNode->next = temp->next;
            temp->next = newNode;
        }
    }
}

// This function swaps two tiles in a node
void swap(int row1,int column1,int row2,int column2, struct node * pnode){
    int temp = pnode->tiles[row1][column1];
    pnode->tiles[row1][column1] = pnode->tiles[row2][column2];
    pnode->tiles[row2][column2] = temp;
}

// This function finds the goal position of a number in the goal state
void findGoalPos(int number, int *goalRow, int *goalCol){
    int i, j;
    if(number == 0){
        *goalRow = *goalCol = 0;
        return;
    }

    for(i = 0; i < NxN; i++){
        for(j = 0; j < NxN; j++){
            if(goal->tiles[i][j] == number){
                *goalRow = i;
                *goalCol = j;
                return;
            }
        }
    }
}

// This function calculates the manhattan distance
int manhattanDist(int number, int row, int col){
    int goalRow, goalCol;
    findGoalPos(number, &goalRow, &goalCol);
    return abs(row - goalRow) + abs(col - goalCol);
}

/*update the f,g,h function values for a node */
// we only update f g and h when we expand a node
void update_fgh(int i) {
    struct node *pnode = succ_nodes[i];
    if(pnode == NULL) return;

    if(pnode->parent != NULL) 
        pnode->g = pnode->parent->g + 1;
    else 
        pnode->g = 1;

    int mis = 0, man = 0, correct = 0, j, k;
    for(k = 0; k < NxN; k++){
        for(j = 0; k < NxN; k++){
            correct++;
            if(pnode->tiles[j][k] != correct){
                mis++;
            }
            man += manhattanDist(pnode->tiles[j][k], j, k);
        }
    }
    
    pnode->h = mis > man ? mis : man;
    pnode->f = pnode->g + pnode->h;
}

// This function moves the blank tile either up, down, left, or right by a row or column
void move(struct node* pnode, int row_change, int col_change){
    int new_row = pnode->zero_row + row_change;
    int new_col = pnode->zero_column + col_change;

    if(new_row >= 0 && new_row < N && new_col >= 0 && new_col < N){
        swap(pnode->zero_row, pnode->zero_column, new_row, new_col, pnode);
        pnode->zero_row = new_row;
        pnode->zero_column = new_col;
    }
    else{
        pnode = NULL;
    }
}

/* 0 goes down by a row */
void move_down(struct node * pnode){
    move(pnode, 1, 0);
}

/* 0 goes right by a column */
void move_right(struct node * pnode){
    move(pnode, 0, 1);
}

/* 0 goes up by a row */
void move_up(struct node * pnode){
    move(pnode, -1, 0);
}

/* 0 goes left by a column */
void move_left(struct node * pnode){
    move(pnode, 0, -1);
}

// This function creates a new node and copy the content of the selected node to the new node (selected is passed in as a parameter by the expand function)
struct node* create_node(struct node *selected){
    struct node *newNode = (struct node *)malloc(sizeof(struct node));
    memcpy(newNode->tiles, selected->tiles, sizeof(int) * NxN);
    newNode->zero_row = selected->zero_row;
    newNode->zero_column = selected->zero_column;
    newNode->parent = selected;
    return newNode;
}

// This function performs a move based on the moveType provided by the expand function
void perform_move(struct node *pnode, int moveType) {
    switch (moveType) {
        case 0: move_left(pnode); break;
        case 1: move_up(pnode); break;
        case 2: move_right(pnode); break;
        case 3: move_down(pnode); break;
    }
}

// This function expands a node, get its children nodes, and organize the children nodes using array succ_nodes.
void expand(struct node *selected) {
    for (int i = 0; i < N; i++) {
        succ_nodes[i] = create_node(selected);
        perform_move(succ_nodes[i], i);
        update_fgh(i);
    }
}

// This function checks whether two nodes are the same.
int nodes_same(struct node *a,struct node *b) {
    int flg=FALSE;
    if (memcmp(a->tiles, b->tiles, sizeof(int)*NxN) == 0) flg=TRUE;
    return flg;
}

/* 
Filtering. Some states in the nodes pointed by succ_nodes may already be
included in either open or closed list. There is no need to check these states. 
Release the corresponding nodes and set the corresponding pointers in succ_nodes to NULL.
This is important to reduce execution time. This function checks the (i)th node pointed by succ_nodes array. 
You must call this function in a loop to check all the nodes in succ_nodes. Free the successor node and
set the corresponding pointer to NULL in succ_node[] if a matching node is found in the list.
*/
void filter(int i, struct node *pnode_list){
    if(pnode_list == NULL || succ_nodes[i] == NULL) return;
    struct node *temp = pnode_list;
    for(; temp != NULL; temp = temp->next){
        if(nodes_same(succ_nodes[i], temp)){
            succ_nodes[i] = NULL;
            return;
        }
    }
}

int main(int argc,char **argv) {
    int iter,cnt;
    struct node *copen, *cp, *solution_path;
    int ret, i, pathlen=0, index[N-1];
    
    solution_path=NULL;
    start=initialize(argv); /* init initial and goal states */
    open=start;
    
    iter=0;
    while (open!=NULL) { /* Termination cond with a solution is tested in
    expand. */
        copen=open;
        open=open->next; /* get the first node from open to expand */

        if(nodes_same(copen,goal)){ /* goal is found */
            do{ /* trace back and add the nodes on the path to a list */
                copen->next=solution_path;
                solution_path=copen;
                copen=copen->parent;
                pathlen++;
            } while(copen!=NULL);
            printf("Path (length=%d):\n", pathlen);
            copen=solution_path;
            /* print out the nodes on the list */
            while (copen != NULL) {
                print_a_node(copen);
                copen = copen->next;
            }
            break;
        }
        expand(copen); /* Find new successors */

        for(i=0;i<4;i++){
            filter(i,open);
            filter(i,closed);
            update_fgh(i);
        }

        merge_to_open(); /* New open list */
        copen->next=closed;
        closed=copen; /* New closed */
        iter++;
        if(iter %1000 == 0)
            printf("iter %d\n", iter);
    }
    return 0;
} /* end of main */