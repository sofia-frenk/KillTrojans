//
//  main.cpp
//  KillTrojans
//
//  Created by Sofia Frenk Knaul on 7/31/23.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <cstring> // For strlen
using namespace std;

constexpr int MAX_N = 100;

struct Tile {
    int t;
    int v;
};

struct X {
    int value;
    string route ;
};

void load_input_file(const std::string& file_name, int& n, int& H, std::vector<std::vector<Tile>>& tiles) {
    std::ifstream file(file_name);
    file >> n >> H;
    
    // Initialize the grid
    tiles.resize(n, std::vector<Tile>(n));

    for (int i = 0; i < n * n; i++) {
        if (i == 0) {
            continue; // The initial tile is zero type with zero value
        }
        Tile tile;
        int x,y;
        file >> x >> y >> tile.t >> tile.v;
        tiles[x][y] = tile;
    }
    
    file.close();
}

struct X MaxForStructX(struct X x1, struct X x2){
    if (x1.value > x2.value)
        return x1;
    else
        return x2;
}

void print_tile_data(int n, const std::vector<std::vector<Tile>>& tiles) {
    std::cout << "Tile Types:\n";
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            std::cout << "[" << tiles[x][y].t << "] ";
        }
        std::cout << '\n';
    }

    std::cout << "\nTile Values:\n";
    for (int x = 0; x < n; x++) {
        for (int y = 0; y < n; y++) {
            std::cout << "[" << tiles[x][y].v << "] ";
        }
        std::cout << '\n';
    }
}

int**** InitializeMemo(int n)
{
    int**** memo = (int****)malloc(n*sizeof(int***));
    for(int i=0; i<n; i++)
    {
        memo[i] = (int***)malloc(n*sizeof(int**));
        for(int j=0; j<n; j++)
        {
            memo[i][j] = (int**)malloc(2*sizeof(int*));
            for(int k=0; k<2; k++)
            {
                memo[i][j][k] = (int*)malloc(2*sizeof(int));
                for(int l=0; l<2; l++)
                    memo[i][j][k][l] = INT_MIN;
            }
        }
    }
    return memo;
}

int MyMaxFxn(int* decisionArray, int arraySize)
{
    int max = INT_MIN;

    for(int i = 0; i < arraySize; i++)
    {
        if(decisionArray[i] >= max)
            max = decisionArray[i];
    }
    return max;
}

struct X  DPHelper(int i, int j, bool holdingProt, bool holdingMult, int n, int H, const std::vector<std::vector<Tile>>& tiles, int**** memo, string route)
{
    
    route += "(" + std::to_string(i) + ", " + std::to_string(j) + ") ";
    //tautology: go right or go down
    //subproblems: index i and index j, quarterary varible for landing

    //base cases
//    if((i >= n || j >= n) || H<0)
//    {
//        return -INT_MAX;
//    }
    //define return value res
    struct X res = X{H, route};
    
    int currentType = tiles[i][j].t;
    int currentValue = tiles[i][j].v;
    
    if(H < 0)
    {
//        res = INT_MIN;
//        return res;
        return X{0, route};
    }
    
    //Base Case:  At last tile
    if(i == n-1 && j == n-1)
    {
        if(currentType == 1)
            return X{currentValue, route};
        else if(currentType == 0)
            return X{-currentValue, route};
        else if (currentType == 2)
            return X{-currentValue, route};
        else if (currentType == 3)
            return X{currentValue, route};
    }
        
    //base case for memo
    //      3  4      true        false
    //      3  4      true        true
    if(memo[i][j][holdingProt][holdingMult] != INT_MIN)
        return X{memo[i][j][holdingProt][holdingMult], route};

    
    //define variables for recursion capture
    struct X right_ = X {INT_MIN, route};
    struct X down_ = X {INT_MIN, route}, right_use= X {INT_MIN, route}, down_use = X {INT_MIN, route};
    
    
    if (i == 0 && j == 0){ // initial tile
        right_ = DPHelper(i, j+1, holdingProt, holdingMult, n, H, tiles, memo, route);
        down_ = DPHelper(i+1, j, holdingProt, holdingMult, n, H, tiles, memo, route);
        res = MaxForStructX(right_, down_);
        memo[i][j][holdingProt][holdingMult] = res.value;
        return res;
    }
    //define tile values and types
    
    if(currentType == 0) //land on damage
    {
        // at this tile, we can either:
        // 1)  don't use prot and lose the value of the tile
        // 2)  use prot mult and lose/gain nothing
        
        if (holdingProt){
            // use the prot power up for going right and down
            
            //if(right_use == INT_MIN) don't subtract
            
            // moving right
            if (j<n-1)
                right_use = DPHelper(i, j+1, false, holdingMult, n, H, tiles, memo, route+"P ");
            
            // moving down
            if(i<n-1)
                down_use = DPHelper(i+1, j, false, holdingMult, n, H, tiles, memo, route+"P ");
            
            // DON'T use the prot power up for going right and down
            // moving right
            if (j<n-1){
                right_ = DPHelper(i, j+1, holdingProt, holdingMult, n, H-currentValue, tiles, memo, route);
                right_.value -= currentValue;
            }
            
            // moving down
            if(i<n-1){
                down_ = DPHelper(i+1, j, holdingProt, holdingMult, n, H-currentValue, tiles, memo, route);
                down_.value -= currentValue;
                
            }
            
        }
        else {
            // DON'T use the mult power up for going right and down
            // moving right
            if (j<n-1){
                right_ = DPHelper(i, j+1, holdingProt, holdingMult, n, H-currentValue, tiles, memo, route);
                right_.value -= currentValue;
                
            }
            
            // moving down
            if(i<n-1){
                down_ = DPHelper(i+1, j, holdingProt, holdingMult, n, H-currentValue, tiles, memo, route);
                down_.value -= currentValue;
                
            }
            
        }
        
        struct X res1 = MaxForStructX(right_, down_);
        struct X res2 = MaxForStructX(right_use,down_use);
        res = MaxForStructX(res1, res2);
        memo[i][j][holdingProt][holdingMult] = res.value;
        return res;
    }
    else if(currentType == 1) //land on healing
    {
        // at this tile, we can either:
        // 1)  don't use mult and gain a # of points equal tovalue of the tile
        // 2)  use a mult and get 2x the value of the tile
        
        if (holdingMult){
            // use the mult power up for going right and down
            // moving right
            if (j<n-1){
                right_use = DPHelper(i, j+1, holdingProt, false, n, H+2*currentValue, tiles, memo, route+"M ");
                right_use.value += 2*currentValue;
                
            }
            
            
            // moving down
            if(i<n-1){
                down_use = DPHelper(i+1, j, holdingProt, false, n, H+2*currentValue, tiles, memo, route+"M ");
                down_use.value += 2*currentValue;
                
            }
            
            
            // DON'T use the mult power up for going right and down
            // moving right
            if (j<n-1){
                right_ = DPHelper(i, j+1, holdingProt, holdingMult, n, H+currentValue, tiles, memo, route);
                right_.value += currentValue;
                
            }
            
            // moving down
            if(i<n-1){
                down_ = DPHelper(i+1, j, holdingProt, holdingMult, n, H+currentValue, tiles, memo, route);
                down_.value += currentValue;
                
            }
            
        }
        else {
            // DON'T use the mult power up for going right and down
            // moving right
            if (j<n-1){
                right_ = DPHelper(i, j+1, holdingProt, holdingMult, n, H+currentValue, tiles, memo, route);
                right_.value += currentValue;
                
            }
            
            // moving down
            if(i<n-1){
                down_ = DPHelper(i+1, j, holdingProt, holdingMult, n, H+currentValue, tiles, memo, route);
                down_.value += currentValue;
                
            }
            
        }
        
        struct X res1 = MaxForStructX(right_, down_);
        struct X res2 = MaxForStructX(right_use,down_use);
        res = MaxForStructX(res1, res2);
        memo[i][j][holdingProt][holdingMult] = res.value;
        return res;
    }
 
    else if (currentType == 2) //land on protection
    {
        // At this tile, all that happens is we pick up the powerup
        // and then move on
        // moving right
        if (j<n-1)
            right_ = DPHelper(i, j+1, true, holdingMult, n, H, tiles, memo, route+"+p ");
        
        // moving down
        if(i<n-1)
            down_ = DPHelper(i+1, j, true, holdingMult, n, H, tiles, memo, route+"+p") ;
        
        res = MaxForStructX(right_, down_);
        memo[i][j][holdingProt][holdingMult] = res.value;
        return res;
    }

    else if (currentType == 3) //land on multiplier
    {
        // At this tile, all that happens is we pick up the mult powerup
        // and then move on
        
        // moving right
        if (j<n-1)
            right_ = DPHelper(i, j+1, holdingProt, true, n, H, tiles, memo, route+"+m ");
        
        // moving down
        if(i<n-1)
            down_ = DPHelper(i+1, j, holdingProt, true, n, H, tiles, memo, route+"+m ") ;
        
        res = MaxForStructX(right_, down_);
        memo[i][j][holdingProt][holdingMult] = res.value;
        return res;
    }

    return res;
}

bool DP(int n, int H, const std::vector<std::vector<Tile>>& tiles) {
    // TODO
    // Placeholder function - implement your logic here
    // Your code to check whether it is possible to reach the bottom-right
    // corner without running out of HP should go here.
    // You should use dynamic programming to solve the problem.
    // Return true if possible, false otherwise.
    
    // By default, we return false
    // TODO you should change this


    //define return value res
    bool res = false;
    if (n > MAX_N)
        return false;

    //fill memo with INT_MIN
    int**** memo = InitializeMemo(n);
    bool useProtTile = false;
    bool useMultTile = false;
    int i = 0;
    int j = 0;
    //declare variables that will store returned values from recursion
    //four possibilities: down & use special token, down and dont use, rght and use token, right and dont use
    
    string emptyRoute = "";
    
    struct X changeInHealth = DPHelper(i, j, useProtTile, useMultTile, n, H, tiles, memo, emptyRoute);
    
    if (H + changeInHealth.value >= 0)
        res = true;
    else
        return false;
    return res;
}
//route    std::string    "(0, 0) (0, 1) +p (0, 2) +m (0, 3) M (1, 3) P (2, 3) (3, 3) "
void write_output_file(const std::string& output_file_name, bool result) {
    std::ofstream file(output_file_name);
    file << result;
    file.close();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " a_file_name.txt\n";
        return 1;
    }

    int n, H;
    vector<vector<Tile> > tiles;
    load_input_file(argv[1], n, H, tiles);
    print_tile_data(n, tiles);
    bool result = DP(n, H, tiles);
    cout << "Result: " << result << endl;
    
    // Construct the output file name
    std::string output_file_name(argv[1]);
    output_file_name.insert(output_file_name.find_last_of('.'), "_out");
    std::ofstream outfile(output_file_name);
    outfile << result;
    outfile.close();

    return 0;
}
