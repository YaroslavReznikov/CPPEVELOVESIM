#include <iostream>
#include <vector>
#include <variant>
#include <random>
#include <cstdlib>
#include <optional>
#include <array>
#include <stdexcept>
#include <chrono>
#include <thread>
class food{
public:
    int getValue() const {
        return this->value;
    }
    food() {
        std::uniform_int_distribution<> dis(1, 4);
        std::random_device rd;
        std::mt19937 gen(rd());
        this -> value = dis(gen);
    }
protected:
    int value;
};

class cell : public food{
public:
cell() {
    std::uniform_int_distribution<> dis(2, 5);
    std::uniform_int_distribution<> movingDis(1, 8);
    std::random_device rd;
    std::mt19937 gen(rd());
    this->hungerBar = dis(gen);
    if (movingDis(gen) > 3){
        this->canMove = true;
    }
    if (movingDis(gen) > 4){
        this->hunter = true;
    }
    if (movingDis(gen) > 1){
        this->canCreateOtherCells = true;
    }
}

cell(const cell& father) {
    std::uniform_int_distribution<> dis(2, 5);
    std::uniform_int_distribution<> movingDis(1, 8);
    std::random_device rd;
    std::mt19937 gen(rd());
    this->hungerBar = 1;

    if (movingDis(gen) + (father.getCanMove() ? 1 : 0) > 2) {
        this->canMove = true;
    }
    
    if (movingDis(gen) + (father.getHunter() ? 1 : 0) > 2) {  // assuming this is what you intended
        this->hunter = true;
    }
    
    if (movingDis(gen) > 1){
        this->canCreateOtherCells = true;
    }
}
    void feed(std::variant<cell, food, std::nullptr_t> booty){
        if (std::get_if<cell>(&booty)){
            auto cellPtr = std::get_if<cell>(&booty);
            this -> hungerBar += cellPtr -> getHungerBar()-1;
        }
        else if (std::get_if<food>(&booty)){
            auto foodPtr = std::get_if<food>(&booty);
            this -> hungerBar += foodPtr -> getValue();
        }
    }

    void newStage(){
        this -> hungerBar -= 1;
    }
    [[nodiscard]] int getHungerBar() const{
        return this -> hungerBar;
    }
    void setCanCreateOtherCells(bool value) {
        canCreateOtherCells = value;
    }

    [[nodiscard]] bool getCanCreateOtherCells() const {
        return canCreateOtherCells;
    }

    void setCanMove(bool value) {
        canMove = value;
    }

    [[nodiscard]] bool getCanMove() const {
        return canMove;
    }

    void setHunter(bool value) {
        hunter = value;
    }

    [[nodiscard]] bool getHunter() const {
        return hunter;
    }

private:
    bool hunter = false;
    bool canMove = false;
    bool canCreateOtherCells = true;
    int hungerBar;
};
template<typename T>
T getRandomElement(const std::vector<T>& vec) {

    if(vec.empty()) {
        throw std::runtime_error("The vector is empty");
    }

    // Random number generator setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, vec.size() - 1);

    int randomIndex = dis(gen);
    return vec[randomIndex];
}
void draw(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>> gamePole){
    std::cout << '\n' ;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (std::get_if<food>(&gamePole[i][j])){
                std::cout << "F ";
            }
            else if (std::get_if<cell>(&gamePole[i][j])){
                std::cout << "C ";
            }
            else {
                std::cout << "N ";
            }
        }
        std::cout << '\n';
    }
}
bool cellCheck(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++ ){
            if (std::get_if<cell>(&gamePole[i][j])){
                return true;
            }
        }
    }
    return false;
}
std::vector<std::vector<int>> getAllAllowedMoves(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole, int row, int collum){
    std::vector<std::vector<int>> result;
    for (int i = -1; i < 2; i++){
        std::vector<int> localResult;
        for (int j = -1; j < 2; j++){
            if (row + i < 8 && collum + j < 8 && row + i >= 0 && collum + j >= 0 && !std::get_if<cell>(&gamePole[row + i][collum + j])){
                localResult.push_back(row + i);
                localResult.push_back(collum + j);
                result.push_back(localResult);
            }
        }
    }
    return result;
}
std::vector<std::array<int, 2>> getAllFreeTiles(
    std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole,
    int row, int collum) {

    std::vector<std::array<int, 2>> result;

    for (int i = -1; i < 2; i++){ 
        for(int j = -1; j < 2; j++){
            if(row + i >= 0 && row + i < gamePole.size() && collum + j >= 0 && collum + j < gamePole[0].size()) {
                if(std::get_if<std::nullptr_t>(&gamePole[row + i][collum + j]) && !(i == 0 && j == 0)){
                    std::array<int, 2> coordinates = {row + i, collum + j};
                    result.push_back(coordinates);
                }
            }
        }
    }

    return result;
}
std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>> gamePoleGenerator(){
    std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>> gamePole;
    std::uniform_int_distribution<> dis(1, 100);
    std::random_device rd;
    std::mt19937 gen(rd());
    int randomNum;
    std::cout << '\n';
    for (int i = 0; i < 8; i++) {
        std::vector<std::variant<cell, food, std::nullptr_t>> Row;
        for (int j = 0; j < 8; j++) {
            randomNum = dis(gen);
            if (randomNum <= 20)
                Row.push_back(cell());
            else if (randomNum <= 40)
                Row.push_back(food());
            else
                Row.push_back(nullptr);
        }
        gamePole.push_back(Row);
    }
    return gamePole;
}
std::vector<int> checkIfFoodIsNear(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole, int row, int collum){
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if (row + i < 8 && collum + j < 8 && row + i >= 0 && collum + j >= 0 &&
                std::get_if<food>(&gamePole[row+i][collum+j])){
                std::vector<int> result;
                result.push_back(row+i);
                result.push_back(collum+j);
                return result;
            }
        }
    }
    std::vector<int> result;
    return result;
}
void moveOfCell(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole
, int i, int j, cell* &cellPtr){
    std::vector<int> nearFood;
    std::random_device rd;
    nearFood = checkIfFoodIsNear(gamePole, i, j);
    if (!nearFood.empty()) {
        cellPtr->feed(gamePole[nearFood[0]][nearFood[1]]);
        gamePole[nearFood[0]][nearFood[1]] = *cellPtr;
        gamePole[i][j] = nullptr;
    }
    else {
        std::vector<std::vector<int>> possibleMoves = getAllAllowedMoves(gamePole, i, j);
        possibleMoves.push_back({i, j});
        std::uniform_int_distribution<> dis(0, possibleMoves.size()-1) ;
        gamePole[i][j] = nullptr;
        std::vector<int> chosen = possibleMoves[dis(rd)];
        gamePole[chosen[0]][chosen[1]] = *cellPtr;

    }
}
bool eatingCellsLogic(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole,
int row, int colum, cell* cellPtr){
    std::vector<std::vector<int>> result;
    for (int i = -1; i < 2; i++){
        for (int j = -1; j < 2; j++){
            if(!(i == 0 && j ==0)
            && row + i < 8 && colum + j < 8 && row + i >= 0 && colum + j >= 0
             && std::get_if<cell>(&gamePole[row+i][colum+j])){
                std::vector<int> my = {row+i, colum+j};
                result.push_back(my);
            }
        }
    }
    if (result.size() > 0){
        std::vector<int> best = result[0];
        for (int i = 1; i < result.size(); i++){
            if (auto cellPtr1 = std::get_if<cell>(&gamePole[best[0]][best[1]]);
                auto cellPtr2 = std::get_if<cell>(&gamePole[result[i][0]][result[i][1]])) {
            if (cellPtr1->getHungerBar() < cellPtr2->getHungerBar()){
                            best = result[i];
                }
            }
        }
        cellPtr->feed(gamePole[best[0]][best[1]]);
        gamePole[row][colum] = nullptr;
        gamePole[best[0]][best[1]] = *cellPtr;
        return true;
    }
    return false;
}
void logic(std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>>& gamePole, int turn) {
        std::random_device rd;

    for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                auto cellPtr = std::get_if<cell>(&gamePole[i][j]);
                std::array<int, 2> randomElem;
                if (std::get_if<std::nullptr_t>(&gamePole[i][j])) {
                }
                else if
                (std::get_if<food>(&gamePole[i][j])) {

                } else {
                    cellPtr->newStage();
                    if (cellPtr->getHungerBar() >= 0) {
                        bool moved = false;
                        if(cellPtr->getCanCreateOtherCells() && cellPtr->getHungerBar() >= 2){
                            try {
                            std::vector<std::array<int, 2>> freeTiles;
                            freeTiles = getAllFreeTiles(gamePole, i, j);
                            randomElem = getRandomElement(freeTiles);
                            gamePole[randomElem[0]][randomElem[1]] = cell(*cellPtr);
                            cellPtr->newStage();
                            }
                            catch(const std::runtime_error& e){

                            }
                        }
                        if (cellPtr->getHunter()){
                            moved = eatingCellsLogic(gamePole, i, j, cellPtr);
                        }
                        if (cellPtr->getCanMove() && !moved) {
                            moveOfCell(gamePole, i, j, cellPtr);
                        }
                    } else {
                        gamePole[i][j] = nullptr;
                    }
                }
            }
        }
}
int main() {
    std::vector<std::vector<std::variant<cell, food, std::nullptr_t>>> gamePole = gamePoleGenerator();
    int turn = 1;
    while (cellCheck(gamePole)) {
        draw(gamePole);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        logic(gamePole,turn);
        turn ++;
    }
    draw(gamePole);
    std::cout << "all cells are dead";
    std::cout << '\n' << turn;
    return 0;
}