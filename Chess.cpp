#include <iostream>
#include <windows.h>

using namespace std;

// Constant
const int BOARD_SIZE = 8;
char board[BOARD_SIZE][BOARD_SIZE] = {
    {'.', 'R', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', '.', '.', '.', '.', '.', '.'},
    {'.', '.', 'N', '.', '.', '.', '.', '.'},
    {'Q', '.', '.', '.', '.', '.', '.', '.'},
    {'k', '.', '.', '.', '.', '.', '.', '.'} };

bool hasKingMoved = false;
bool hasRookMoved[2] = { false, false }; // برای هر دو رخ


// Prototyping
void setConsoleColor(int color);
bool isPathClear(int startRow, int startCol, int endRow, int endCol);
bool isKingInCheck(char king);
void checkKingsInCheck();
void printBoard();
bool isCheckmate(char king);
bool canKingEscape(char king);
bool isValidPawnMove(int startRow, int startCol, int endRow, int endCol, char piece);
bool isValidKnightMove(int startRow, int startCol, int endRow, int endCol, char piece);
bool isValidRookMove(int startRow, int startCol, int endRow, int endCol, char piece);
bool isValidBishopMove(int startRow, int startCol, int endRow, int endCol, char piece);
bool isValidQueenMove(int startRow, int startCol, int endRow, int endCol, char piece);
bool canPiecesSaveKing(char king);

int main()
{
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
        if (isCheckmate('k'))
            std::cout << "Black King is in checkmate!\n\n";
        else
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
        if (isCheckmate('K'))
            std::cout << "White King is in checkmate!\n\n";
        else
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

bool isCheckmate(char king)
{
    // اگر شاه در وضعیت کیش نباشد، چک‌میت نیست
    if (!isKingInCheck(king))
        return false;

    // بررسی اینکه آیا شاه می‌تواند از کیش فرار کند یا مهره کیش‌دهنده را بزند
    if (canKingEscape(king))
        return false;

    // بررسی اینکه آیا سایر مهره‌های یار می‌توانند شاه را نجات دهند
    if (canPiecesSaveKing(king))
        return false;

    // اگر هیچکدام از موارد بالا نتوانند شاه را نجات دهند، کیش‌میت رخ داده است
    return true;
}

bool canKingEscape(char king)
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
                break;
            }
        }
    }

    int kingMoves[8][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, -1}, {1, 1}, {-1, 1}, {1, -1} };
    for (const auto& move : kingMoves)
    {
        int newRow = kingRow + move[0], newCol = kingCol + move[1];
        if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE)
        {
            char originalPiece = board[newRow][newCol];
            if (originalPiece != '.' && islower(originalPiece) == islower(king))
                continue;

            board[kingRow][kingCol] = '.';
            board[newRow][newCol] = king;
            bool isStillInCheck = isKingInCheck(king);
            board[newRow][newCol] = originalPiece;
            board[kingRow][kingCol] = king;
            if (!isStillInCheck)
                return true;
        }
    }

    // بررسی حرکت قلعه
    if (kingCol == 4)
    {
        if (canCastle(king, kingRow, kingCol, 0) || canCastle(king, kingRow, kingCol, 7))
            return true;
    }

    return false;
}


bool isValidPawnMove(int startRow, int startCol, int endRow, int endCol, char piece)
{
    int direction = (islower(piece)) ? 1 : -1; // تعیین جهت حرکت پیاده (پیاده سیاه به پایین، پیاده سفید به بالا)
    int startRowInitial = (islower(piece)) ? 1 : 6; // ردیف شروع پیاده (برای دو خانه حرکت کردن)

    // حرکت به جلو
    if (startCol == endCol)
    {
        if (board[endRow][endCol] == '.' &&
            ((startRow + direction == endRow) || (startRow == startRowInitial && startRow + 2 * direction == endRow && board[startRow + direction][startCol] == '.')))
        {
            return true;
        }
    }
    // زدن مهره حریف به صورت قطری
    else if (abs(startCol - endCol) == 1 && startRow + direction == endRow && board[endRow][endCol] != '.' && islower(board[endRow][endCol]) != islower(piece))
    {
        return true;
    }

    return false;
}

bool isValidKnightMove(int startRow, int startCol, int endRow, int endCol, char piece)
{
    // حرکت‌های اسب
    int rowDiff = abs(endRow - startRow);
    int colDiff = abs(endCol - startCol);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
}

bool isValidRookMove(int startRow, int startCol, int endRow, int endCol, char piece)
{
    // حرکت‌های رخ
    return (startRow == endRow || startCol == endCol) && isPathClear(startRow, startCol, endRow, endCol);
}

bool isValidBishopMove(int startRow, int startCol, int endRow, int endCol, char piece)
{
    // حرکت‌های فیل
    return abs(startRow - endRow) == abs(startCol - endCol) && isPathClear(startRow, startCol, endRow, endCol);
}

bool isValidQueenMove(int startRow, int startCol, int endRow, int endCol, char piece)
{
    // حرکت‌های وزیر
    return isValidRookMove(startRow, startCol, endRow, endCol, piece) || isValidBishopMove(startRow, startCol, endRow, endCol, piece);
}

bool canPiecesSaveKing(char king)
{
    for (int startRow = 0; startRow < BOARD_SIZE; ++startRow)
    {
        for (int startCol = 0; startCol < BOARD_SIZE; ++startCol)
        {
            char piece = board[startRow][startCol];
            if (piece != '.' && islower(piece) == islower(king))
            {
                for (int endRow = 0; endRow < BOARD_SIZE; ++endRow)
                {
                    for (int endCol = 0; endCol < BOARD_SIZE; ++endCol)
                    {
                        char targetPiece = board[endRow][endCol];
                        if (targetPiece == '.' || islower(targetPiece) != islower(king))
                        {
                            bool validMove = false;
                            switch (tolower(piece))
                            {
                            case 'p':
                                validMove = isValidPawnMove(startRow, startCol, endRow, endCol, piece);
                                break;
                            case 'n':
                                validMove = isValidKnightMove(startRow, startCol, endRow, endCol, piece);
                                break;
                            case 'r':
                                validMove = isValidRookMove(startRow, startCol, endRow, endCol, piece);
                                break;
                            case 'b':
                                validMove = isValidBishopMove(startRow, startCol, endRow, endCol, piece);
                                break;
                            case 'q':
                                validMove = isValidQueenMove(startRow, startCol, endRow, endCol, piece);
                                break;
                            }

                            if (validMove)
                            {
                                char temp = board[endRow][endCol];
                                board[endRow][endCol] = piece;
                                board[startRow][startCol] = '.';
                                bool isStillInCheck = isKingInCheck(king);
                                board[endRow][endCol] = temp;
                                board[startRow][startCol] = piece;
                                if (!isStillInCheck)
                                    return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool canCastle(char king, int kingRow, int kingCol, int rookCol)
{
    if (hasKingMoved || hasRookMoved[rookCol == 0 ? 0 : 1])
        return false;

    int direction = (rookCol == 0) ? -1 : 1;

    for (int i = 1; i <= 2; ++i)
    {
        int col = kingCol + i * direction;
        if (board[kingRow][col] != '.' || isKingInCheck(king))
            return false;
    }

    return true;
}

