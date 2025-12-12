#include <iostream>
#include <Windows.h>
#include <string>
#include <cstdlib>

using namespace std;

struct Cell // координаты символа в двумерном массиве символов
{
    int x;
    int y;
};

struct Node // узел, который хранит состояние клетки
{
    int x;
    int y;
    int g; // стомость от начала клетки
    int f; // полная стоимость (g + h), где h - расстояние до конца
    Node* parent; // предыдущий узел
};

int DistanceToCell(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// A* алгоритм, который возвращает указатель на клетку, которую мы ищем
Cell* AStarSearch(string* map, int height, int width, Cell start, Cell end, int &pathLenght)
{
    // массив указателей на доступность узла
    bool** closed = new bool* [height]; // указываем строками на столбцы
    // массив указателей на стоимость узла
    int** gcost = new int* [height];
    Node** nodes = new Node * [height];

    // Инициализация поля и значений узлов
    for (int i = 0; i < height; i++)
    {
        // Выделяем строки
        closed[i] = new bool[width];
        gcost[i] = new int[width];
        nodes[i] = new Node[width];

        // Заполнение значений
        for (int j = 0; j < width; j++)
        {
            closed[i][j] = false;
            gcost[i][j] = 1000000000;
            nodes[i][j].x = i;
            nodes[i][j].y = j;
            nodes[i][j].g = 1000000000;
            nodes[i][j].f = 1000000000;
            nodes[i][j].parent = NULL;
        }
    }

    int maxOpen = height + width; // максимальное количество элементов в таблице
    int* openX = new int[maxOpen]; // массив X координат в открытых узлах
    int* openY = new int[maxOpen]; // массив Y координат в открытых узлах
    int* openG = new int[maxOpen]; // массив значений стоимости
    int* openF = new int[maxOpen]; // массив значений полной стоимости
    int openCount = 0; // текущее количество элементов 

    // Стартовая клетка
    gcost[start.x][start.y] = 0;
    nodes[start.x][start.y].g = 0;
    nodes[start.x][start.y].f = DistanceToCell(start.x, start.y, end.x, end.y);
    nodes[start.x][start.y].parent = NULL;

    // Открытые списки 
    openX[openCount] = start.x;
    openY[openCount] = start.y;
    openF[openCount] = nodes[start.x][start.y].f;
    openG[openCount] = 0;
    openCount++;

    //                влево, вправо, вниз, вверх
    int directionsX[4] = {-1, 1, 0, 0}; // Направления, куда может идти x
    int directionsY[4] = {0, 0, -1, 1}; // Направления, куда может идти x

    bool endFound = false;
    Cell* path;

    // Пока есть открытые узлы
    while (openCount > 0)
    {
        int bestId = 0;
        for (int i = 0; i < openCount; i++)
        {
            // Если текущий узел полной стоимости меньше лучшего узла полной стоимости ИЛИ текущий узел полной стоимости равен лучшему узлу полной стоимости И текущая стоимость от предыдущей клетки меньше лучше стоимости предыдущей клетки
            if (openF[i] < openF[bestId] || (openF[i] == openF[bestId] && openG[i] < openG[bestId]))
            {
                bestId = i;
            }
        }

        // Создаем переменные для работы с текущим узлом
        int currentX = openX[bestId];
        int currentY = openY[bestId];

        // Перемещаем последний элемент на место лучшего, затем присваиваем ему значения
        openCount--;
        openX[bestId] = openX[openCount];
        openY[bestId] = openY[openCount];
        openF[bestId] = openF[openCount];
        openG[bestId] = openG[openCount];

        // Если узел уже закрыт, пропускаем условие, либо помечаем текущий узел, как закрытый
        if (closed[currentX][currentY]) continue;
        closed[currentX][currentY] = true;

        // Если достигли целевого узла, отмечаем, что путь найден и выходим из цикла поиска
        if (currentX == end.x && currentY == end.y)
        {
            endFound = true;
            break;
        }

        // Для каждого соседа необходимо:                 
        for (int i = 0; i < 4; i++)
        {
            // 1. вычислять координаты
            int neighbourX = currentX + directionsX[i];
            int neighbourY = currentY + directionsY[i];
            // 2. проверять границы карты
            if (neighbourX < 0 || neighbourX >= height || neighbourY < 0 || neighbourY >= width) continue;
            // 3. проверять является ли сосед стеной
            if (map[neighbourX][neighbourY] == '+') continue;
            // 4. проверять закрыт ли сосед
            if (closed[neighbourX][neighbourY]) continue;

            int currentG = gcost[currentX][currentY] + 1;

            // Если найдена меньшая стоимость к соседу
            if (currentG < gcost[neighbourX][neighbourY])
            {
                // Обновляем значения у соседа
                gcost[neighbourX][neighbourY] = currentG;
                nodes[neighbourX][neighbourY].g = currentG;
                nodes[neighbourX][neighbourY].f = currentG + DistanceToCell(neighbourX, neighbourY, end.x, end.y);
                nodes[neighbourX][neighbourY].parent = &nodes[currentX][currentY];

                // Обновляем открытые списки
                openX[openCount] = neighbourX;
                openY[openCount] = neighbourY;
                openF[openCount] = nodes[neighbourX][neighbourY].f;
                openG[openCount] = nodes[neighbourX][neighbourY].g;
                openCount++;
            }
        }
    }

    //Если путь найден, восстанавливаем его по предыдущим узлам
    if (endFound)
    {
        cout << "Путь построен!" << endl;

        Node* currentNode = &nodes[end.x][end.y]; // Текущий узел на момент нахождения конца

        // Считаем путь 
        int pathLenght = 0;
        while (currentNode != NULL)
        {
            currentNode = currentNode->parent;
            pathLenght++;
        }

        // Создаём массив клеток с размером пути
        path = new Cell[pathLenght];
        currentNode = &nodes[end.x][end.y];

        // Построение массива клеток обратного порядка
        for (int i = pathLenght - 1; i >= 0; i--)
        {
            path[i].x = currentNode->x;
            path[i].y = currentNode->y;
            currentNode = currentNode->parent;
        }
    }

    // Восвобождение памяти под конец
    for (int i = 0; i < height; i++)
    {
        // Поскольку мы работаем с массивом указателей на указатели, сперва необходимо очистить помять у строк, а затем и у самого массива
        delete[] closed[i];
        delete[] gcost[i];
        delete[] nodes[i];
    }
    delete[] closed;
    delete[] gcost;
    delete[] nodes;

    delete[] openX;
    delete[] openY;
    delete[] openG;
    delete[] openF;

    return path;
}

void PrintMap(string* map, int height, int width, Cell* path, Cell start, Cell end, int pathLenght)
{
    string* mapCopy = new string[height];

    for (int i = 0; i < height; i++) mapCopy[i] = map[i];

    for (int i = 1; i + 1 < pathLenght; i++) mapCopy[path[i].x][path[i].y] = '*';

    map[start.x][start.y] = 'S';
    map[end.x][end.y] = 'F';

    for (int i = 0; i < mapCopy->size(); i++) cout << mapCopy[i] << endl;

    delete[] mapCopy;
}

int main()
{
    setlocale(LC_ALL, "");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string map[] =
    {
        // · - пустая клетка
        // + - стена
        // * - путь
        // S - начало
        // F - финиш
        "···+·",
        "++·+·",
        "·····",
        "·+++·",
        "·····"
    };

    int height = 5;
    int width = 5;

    Cell start; // структура клетки (символа в двумерном массиве)
    start.x = 0;
    start.y = 0;

    Cell end;
    end.x = 4;
    end.y = 4;

    int pathLength = 0;
    Cell* path = AStarSearch(map, height, width, start, end, pathLength);

    if (path != NULL)
    {
        PrintMap(map, height, width, path, start, end, pathLength);
        cout << path->x << " - " << path->y << endl;
        delete[] path;
    }
    else cout << "Не удалось построить пути к цели!\n";
}