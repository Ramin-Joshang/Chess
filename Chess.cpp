﻿#include <iostream>
#include <windows.h>

using namespace std;

// Constant
const int BOARD_SIZE = 8;
char board[BOARD_SIZE][BOARD_SIZE] = {
    {'Q', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'k', '.', '.', '.', '.', '.', '.', '.'} };

// Prototyping
void setConsoleColor(int color);
bool isPathClear(int startRow, int startCol, int endRow, int endCol);
bool isKingInCheck(char king);
void checkKingsInCheck();
void printBoard();
void addPiece(char piece, int row, int col);
void addPieceByUser();

int main()
{
    printBoard();
    checkKingsInCheck();
    addPieceByUser();
    printBoard();
    checkKingsInCheck();

    return 0;
}

// Setting Terminal Color
void setConsoleColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

// Checking if the path between points is clear
bool isPathClear(int startRow, int startCol, int endRow, int endCol)
{
    int rowDirection = (endRow - startRow) > 0 ? 1 : -1;
    int colDirection = (endCol - startCol) > 0 ? 1 : -1;

    if (startRow == endRow)
    { // Horizontal movement
        for (int col = startCol + colDirection; col != endCol; col += colDirection)
        {
            if (board[startRow][col] != '.')
                return false;
        }
    }
    else if (startCol == endCol)
    { // Vertical movement
        for (int row = startRow + rowDirection; row != endRow; row += rowDirection)
        {
            if (board[row][startCol] != '.')
                return false;
        }
    }
    else
    { // Diagonal movement
        for (int i = 1; i < abs(endRow - startRow); ++i)
        {
            if (board[startRow + i * rowDirection][startCol + i * colDirection] != '.')
                return false;
        }
    }
    return true;
}

// Checking if the white or black King is in check
bool isKingInCheck(char king)
{
    int kingRow = -1, kingCol = -1;
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            if (board[i][j] == king)
            {
                kingRow = i;
                kingCol = j;
            }
        }
    }
    // No King was found
    if (kingRow == -1 || kingCol == -1)
        return false;

    bool isWhiteKing = (king == 'K');

    // Checking Rook and Queen piece on straight lines
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        if ((isWhiteKing && (board[kingRow][i] == 'r' || board[kingRow][i] == 'q')) ||
            (!isWhiteKing && (board[kingRow][i] == 'R' || board[kingRow][i] == 'Q')))
        {
            if (isPathClear(kingRow, kingCol, kingRow, i))
                return true;
        }
        if ((isWhiteKing && (board[i][kingCol] == 'r' || board[i][kingCol] == 'q')) ||
            (!isWhiteKing && (board[i][kingCol] == 'R' || board[i][kingCol] == 'Q')))
        {
            if (isPathClear(kingRow, kingCol, i, kingCol))
                return true;
        }
    }

    // Checking Bishop and Queen pieces on the diagonals
    for (int i = -BOARD_SIZE; i < BOARD_SIZE; ++i)
    {
        if (kingRow + i >= 0 && kingRow + i < BOARD_SIZE && kingCol + i >= 0 && kingCol + i < BOARD_SIZE)
        {
            if ((isWhiteKing && (board[kingRow + i][kingCol + i] == 'b' || board[kingRow + i][kingCol + i] == 'q')) ||
                (!isWhiteKing && (board[kingRow + i][kingCol + i] == 'B' || board[kingRow + i][kingCol + i] == 'Q')))
            {
                if (isPathClear(kingRow, kingCol, kingRow + i, kingCol + i))
                    return true;
            }
        }
        if (kingRow + i >= 0 && kingRow + i < BOARD_SIZE && kingCol - i >= 0 && kingCol - i < BOARD_SIZE)
        {
            if ((isWhiteKing && (board[kingRow + i][kingCol - i] == 'b' || board[kingRow + i][kingCol - i] == 'q')) ||
                (!isWhiteKing && (board[kingRow + i][kingCol - i] == 'B' || board[kingRow + i][kingCol - i] == 'Q')))
            {
                if (isPathClear(kingRow, kingCol, kingRow + i, kingCol - i))
                    return true;
            }
        }
    }

    // Checking Knight Piece
    int knightMoves[8][2] = { {-2, 1}, {-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1} };
    for (const auto& move : knightMoves)
    {
        int newRow = kingRow + move[0], newCol = kingCol + move[1];
        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE)
        {
            if ((isWhiteKing && board[newRow][newCol] == 'n') || (!isWhiteKing && board[newRow][newCol] == 'N'))
                return true;
        }
    }

    // Checking Pawn Piece
    if (isWhiteKing)
    {
        if (kingRow > 0)
        {
            if (kingCol > 0 && board[kingRow - 1][kingCol - 1] == 'p')
                return true;
            if (kingCol < BOARD_SIZE - 1 && board[kingRow - 1][kingCol + 1] == 'p')
                return true;
        }
    }
    else
    {
        if (kingRow < BOARD_SIZE - 1)
        {
            if (kingCol > 0 && board[kingRow + 1][kingCol - 1] == 'P')
                return true;
            if (kingCol < BOARD_SIZE - 1 && board[kingRow + 1][kingCol + 1] == 'P')
                return true;
        }
    }

    // Checking the King piece (check by another King)
    int kingMoves[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1} };
    for (const auto& move : kingMoves)
    {
        int newRow = kingRow + move[0], newCol = kingCol + move[1];
        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE)
        {
            if ((isWhiteKing && board[newRow][newCol] == 'k') || (!isWhiteKing && board[newRow][newCol] == 'K'))
                return true;
        }
    }

    return false;
}

// Checking if the White or Black King is in check
void checkKingsInCheck()
{
    if (isKingInCheck('k'))
    {
        setConsoleColor(12);
        std::cout << "Black King is in check!\n\n";
    }
    else
    {
        setConsoleColor(2);
        std::cout << "Black King is not in check.\n\n";
    }
    if (isKingInCheck('K'))
    {
        setConsoleColor(12);
        std::cout << "White King is in check!\n\n";
    }
    else
    {
        setConsoleColor(2);
        std::cout << "White King is not in check.\n\n";
    }
    setConsoleColor(7);
}

// Displaying the chessboard
void printBoard()
{
    cout << "Chessboard : \n\n";
    for (int i = 0; i < BOARD_SIZE; ++i)
    {
        for (int j = 0; j < BOARD_SIZE; ++j)
        {
            cout << board[i][j] << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

// Adding a piece to the board
void addPiece(char piece, int row, int col)
{
    board[row][col] = piece;
}

// Adding a piece by the User
void addPieceByUser()
{
    char piece;
    int row, col;
    cout << "Enter the piece to add (e.g. Q, r, n, p): ";
    cin >> piece;
    cout << "Enter the row (0-7) and column (0-7) to place the piece: ";
    cin >> row >> col;
    cout << "\n";
    addPiece(piece, row, col);
}